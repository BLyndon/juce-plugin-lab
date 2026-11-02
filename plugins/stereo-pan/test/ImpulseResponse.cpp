#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

constexpr double sampleRate = 48000.0;
constexpr int blockSize = 512;

// Absolute tolerance from spec.md
constexpr double tolerance = 1e-6;

enum Channel {
    left = 0,
    right = 1
};

static void setParameter (PluginProcessor& plugin, const char* id, float value)
{
    auto* param = plugin.parameters.getParameter (id);
    REQUIRE (param != nullptr);
    param->setValueNotifyingHost (param->convertTo0to1 (value));
}

static void setWidthAndPosition (PluginProcessor& plugin, float width, float position)
{
    setParameter (plugin, Parameters::Width::id, width);
    setParameter (plugin, Parameters::Position::id, position);
}

// Processes numSamples of ones on one input and zeros on the other, in blocks of blockLength.
// The output is (a[n], c[n]) for the left input and (b[n], d[n]) for the right input.
static juce::AudioBuffer<float> processOnesOn (PluginProcessor& plugin, Channel input, int numSamples, int blockLength = blockSize)
{
    juce::AudioBuffer<float> output (2, numSamples);
    output.clear();
    juce::FloatVectorOperations::fill (output.getWritePointer (input), 1.0f, numSamples);

    juce::MidiBuffer midi;
    for (int start = 0; start < numSamples; start += blockLength)
    {
        juce::AudioBuffer<float> block (output.getArrayOfWritePointers(), 2, start, juce::jmin (blockLength, numSamples - start));
        plugin.processBlock (block, midi);
    }
    return output;
}

struct ExpectedGains
{
    float width;
    float position;
    double a, b, c, d;
};

// Expected values from spec.md, written out here instead of computed from the gain mapping
// so that an error in the mapping fails the tests
TEST_CASE ("Impulse response", "[dsp]")
{
    const auto expected = GENERATE (
        ExpectedGains { 1.0f, 0.0f, 1.0, 0.0, 0.0, 1.0 },
        ExpectedGains { 0.5f, 0.0f, 0.9238795, 0.3826834, 0.3826834, 0.9238795 },
        ExpectedGains { 0.0f, 0.0f, 0.7071068, 0.7071068, 0.7071068, 0.7071068 },
        ExpectedGains { 1.0f, 0.5f, 0.7071068, 0.0, 0.7071068, 1.0 },
        ExpectedGains { 1.0f, 1.0f, 0.0, 0.0, 1.0, 1.0 },
        ExpectedGains { 1.0f, -1.0f, 1.0, 1.0, 0.0, 0.0 });
    CAPTURE (expected.width, expected.position);

    PluginProcessor plugin;
    setWidthAndPosition (plugin, expected.width, expected.position);
    // Settled gains: prepare sets them to their targets
    plugin.prepareToPlay (sampleRate, blockSize);

    // Impulse into one input: y_L[0] and y_R[0] are that input's two gains, all other samples 0
    auto checkImpulseOn = [&plugin] (Channel input, double expectedLeft, double expectedRight) {
        juce::AudioBuffer<float> buffer (2, blockSize);
        buffer.clear();
        buffer.setSample (input, 0, 1.0f);

        juce::MidiBuffer midi;
        plugin.processBlock (buffer, midi);

        CHECK_THAT (buffer.getSample (left, 0), Catch::Matchers::WithinAbs (expectedLeft, tolerance));
        CHECK_THAT (buffer.getSample (right, 0), Catch::Matchers::WithinAbs (expectedRight, tolerance));
        for (int channel = 0; channel < 2; ++channel)
            for (int i = 1; i < blockSize; ++i)
                CHECK (buffer.getSample (channel, i) == 0.0f);
    };

    SECTION ("left input gives a and c")
    {
        checkImpulseOn (left, expected.a, expected.c);
    }

    SECTION ("right input gives b and d")
    {
        checkImpulseOn (right, expected.b, expected.d);
    }

    plugin.releaseResources();
}

TEST_CASE ("Gain smoothing", "[dsp]")
{
    constexpr int rampLength = 960; // 20 ms at 48 kHz

    // Fully left: a = b = 1, c = d = 0
    PluginProcessor plugin;
    setWidthAndPosition (plugin, 1.0f, -1.0f);
    plugin.prepareToPlay (sampleRate, blockSize);

    // Linear ramp from start to target over length samples, landing on the target at sample length - 1
    auto ramp = [] (double start, double target, int length, int n) {
        return n < length ? start + (target - start) * (n + 1) / length : target;
    };

    SECTION ("a change ramps a, b, c, d linearly over 20 ms")
    {
        // Fully right: a = b = 0, c = d = 1
        setParameter (plugin, Parameters::Position::id, 1.0f);

        SECTION ("a and c")
        {
            const auto output = processOnesOn (plugin, left, rampLength + 10);
            for (int n = 0; n < output.getNumSamples(); ++n)
            {
                CAPTURE (n);
                CHECK_THAT (output.getSample (left, n), Catch::Matchers::WithinAbs (ramp (1.0, 0.0, rampLength, n), 1e-5));
                CHECK_THAT (output.getSample (right, n), Catch::Matchers::WithinAbs (ramp (0.0, 1.0, rampLength, n), 1e-5));
            }
        }

        SECTION ("b and d")
        {
            const auto output = processOnesOn (plugin, right, rampLength + 10);
            for (int n = 0; n < output.getNumSamples(); ++n)
            {
                CAPTURE (n);
                CHECK_THAT (output.getSample (left, n), Catch::Matchers::WithinAbs (ramp (1.0, 0.0, rampLength, n), 1e-5));
                CHECK_THAT (output.getSample (right, n), Catch::Matchers::WithinAbs (ramp (0.0, 1.0, rampLength, n), 1e-5));
            }
        }
    }

    SECTION ("a change during a ramp starts from the current gains")
    {
        setParameter (plugin, Parameters::Position::id, 1.0f);
        processOnesOn (plugin, left, rampLength / 2);

        // Halfway: a = 0.5, c = 0.5; ramp back to a = 1, c = 0
        setParameter (plugin, Parameters::Position::id, -1.0f);
        const auto output = processOnesOn (plugin, left, rampLength + 10);
        for (int n = 0; n < output.getNumSamples(); ++n)
        {
            CAPTURE (n);
            CHECK_THAT (output.getSample (left, n), Catch::Matchers::WithinAbs (ramp (0.5, 1.0, rampLength, n), 1e-5));
            CHECK_THAT (output.getSample (right, n), Catch::Matchers::WithinAbs (ramp (0.5, 0.0, rampLength, n), 1e-5));
        }
    }

    SECTION ("a sample-rate change recomputes the ramp length on prepare")
    {
        constexpr int rampLength96k = 1920; // 20 ms at 96 kHz
        plugin.prepareToPlay (96000.0, blockSize);

        setParameter (plugin, Parameters::Position::id, 1.0f);
        const auto output = processOnesOn (plugin, left, rampLength96k + 10);
        for (int n = 0; n < output.getNumSamples(); ++n)
        {
            CAPTURE (n);
            // Twice the steps of the 48 kHz ramp, so twice the accumulated float rounding
            CHECK_THAT (output.getSample (left, n), Catch::Matchers::WithinAbs (ramp (1.0, 0.0, rampLength96k, n), 1e-4));
        }
    }

    SECTION ("output does not depend on the block size")
    {
        // A deterministic noise signal on both inputs, through a parameter change
        constexpr int numSamples = 2 * rampLength;
        juce::AudioBuffer<float> input (2, numSamples);
        juce::Random random (42);
        for (int channel = 0; channel < 2; ++channel)
            for (int n = 0; n < numSamples; ++n)
                input.setSample (channel, n, random.nextFloat() * 2.0f - 1.0f);

        auto processInBlocks = [&] (int blockLength) {
            PluginProcessor p;
            setWidthAndPosition (p, 1.0f, -1.0f);
            p.prepareToPlay (sampleRate, blockLength);
            setWidthAndPosition (p, 0.3f, 0.4f);

            juce::AudioBuffer<float> output (input);
            juce::MidiBuffer midi;
            for (int start = 0; start < numSamples; start += blockLength)
            {
                juce::AudioBuffer<float> block (output.getArrayOfWritePointers(), 2, start, juce::jmin (blockLength, numSamples - start));
                p.processBlock (block, midi);
            }
            return output;
        };

        const auto reference = processInBlocks (blockSize);
        const auto blockLength = GENERATE (1, 7, 100);
        CAPTURE (blockLength);
        const auto output = processInBlocks (blockLength);

        for (int channel = 0; channel < 2; ++channel)
            for (int n = 0; n < numSamples; ++n)
                CHECK (juce::exactlyEqual (output.getSample (channel, n), reference.getSample (channel, n)));
    }

    SECTION ("reset sets the gains without a ramp")
    {
        setParameter (plugin, Parameters::Position::id, 1.0f);
        plugin.reset();

        const auto output = processOnesOn (plugin, left, 16);
        for (int n = 0; n < output.getNumSamples(); ++n)
        {
            CHECK_THAT (output.getSample (left, n), Catch::Matchers::WithinAbs (0.0, tolerance));
            CHECK_THAT (output.getSample (right, n), Catch::Matchers::WithinAbs (1.0, tolerance));
        }
    }

    SECTION ("loading a state sets the gains without a ramp")
    {
        setParameter (plugin, Parameters::Position::id, 1.0f);
        juce::MemoryBlock state;
        plugin.getStateInformation (state);

        setParameter (plugin, Parameters::Position::id, -1.0f);
        plugin.reset();

        plugin.setStateInformation (state.getData(), static_cast<int> (state.getSize()));
        const auto output = processOnesOn (plugin, left, 16);
        for (int n = 0; n < output.getNumSamples(); ++n)
        {
            CHECK_THAT (output.getSample (left, n), Catch::Matchers::WithinAbs (0.0, tolerance));
            CHECK_THAT (output.getSample (right, n), Catch::Matchers::WithinAbs (1.0, tolerance));
        }
    }
}

TEST_CASE ("Correlated inputs on one side reach +6 dB without clipping", "[dsp]")
{
    // w = 1, p = 1: c = d = 1, so x_L = x_R = 1 gives y_R = 2
    PluginProcessor plugin;
    setWidthAndPosition (plugin, 1.0f, 1.0f);
    plugin.prepareToPlay (sampleRate, blockSize);

    juce::AudioBuffer<float> buffer (2, blockSize);
    buffer.clear();
    buffer.setSample (left, 0, 1.0f);
    buffer.setSample (right, 0, 1.0f);

    juce::MidiBuffer midi;
    plugin.processBlock (buffer, midi);

    CHECK_THAT (buffer.getSample (left, 0), Catch::Matchers::WithinAbs (0.0, tolerance));
    CHECK_THAT (buffer.getSample (right, 0), Catch::Matchers::WithinAbs (2.0, tolerance));
}

TEST_CASE ("Bus layouts", "[instance]")
{
    PluginProcessor plugin;

    auto layout = [] (const juce::AudioChannelSet& in, const juce::AudioChannelSet& out) {
        juce::AudioProcessor::BusesLayout l;
        l.inputBuses.add (in);
        l.outputBuses.add (out);
        return l;
    };

    const auto mono = juce::AudioChannelSet::mono();
    const auto stereo = juce::AudioChannelSet::stereo();

    CHECK (plugin.checkBusesLayoutSupported (layout (stereo, stereo)));
    CHECK_FALSE (plugin.checkBusesLayoutSupported (layout (mono, mono)));
    CHECK_FALSE (plugin.checkBusesLayoutSupported (layout (mono, stereo)));
    CHECK_FALSE (plugin.checkBusesLayoutSupported (layout (stereo, mono)));
}
