#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

constexpr double sampleRate = 48000.0;
constexpr int blockSize = 512;

static void setGainDb (PluginProcessor& plugin, float gainDb)
{
    auto* param = plugin.parameters.getParameter ("gain");
    REQUIRE (param != nullptr);
    param->setValueNotifyingHost (param->convertTo0to1 (gainDb));
}

// Expected values from spec.md: y[n] = g · x[n] with g = 10^(L/20), so an impulse comes out
// as g at sample 0 and 0 elsewhere, on every channel (relative tolerance 1e-6)
static void checkImpulse (PluginProcessor& plugin, int numChannels, float gainDb)
{
    setGainDb (plugin, gainDb);
    // Settled parameter: prepare resets the smoother to the current g
    plugin.prepareToPlay (sampleRate, blockSize);

    juce::AudioBuffer<float> buffer (numChannels, blockSize);
    buffer.clear();
    for (int channel = 0; channel < numChannels; ++channel)
        buffer.setSample (channel, 0, 1.0f);

    juce::MidiBuffer midi;
    plugin.processBlock (buffer, midi);

    const auto expected = std::pow (10.0, gainDb / 20.0);
    for (int channel = 0; channel < numChannels; ++channel)
    {
        CHECK_THAT (buffer.getSample (channel, 0), Catch::Matchers::WithinRel (expected, 1e-6));
        for (int i = 1; i < blockSize; ++i)
            CHECK (buffer.getSample (channel, i) == 0.0f);
    }

    plugin.releaseResources();
}

TEST_CASE ("Impulse response", "[dsp]")
{
    const auto gainDb = GENERATE (-60.0f, 0.0f, 12.0f);
    CAPTURE (gainDb);

    PluginProcessor plugin;

    SECTION ("stereo")
    {
        checkImpulse (plugin, 2, gainDb);
    }

    SECTION ("mono")
    {
        juce::AudioProcessor::BusesLayout mono;
        mono.inputBuses.add (juce::AudioChannelSet::mono());
        mono.outputBuses.add (juce::AudioChannelSet::mono());
        REQUIRE (plugin.setBusesLayout (mono));

        checkImpulse (plugin, 1, gainDb);
    }
}

// Processes a block of ones, so the output is g[n]
static juce::AudioBuffer<float> processOnes (PluginProcessor& plugin, int numSamples)
{
    juce::AudioBuffer<float> buffer (2, numSamples);
    for (int channel = 0; channel < 2; ++channel)
        juce::FloatVectorOperations::fill (buffer.getWritePointer (channel), 1.0f, numSamples);

    juce::MidiBuffer midi;
    plugin.processBlock (buffer, midi);
    return buffer;
}

TEST_CASE ("Gain smoothing", "[dsp]")
{
    PluginProcessor plugin;
    setGainDb (plugin, 0.0f);
    plugin.prepareToPlay (sampleRate, blockSize);

    SECTION ("a change ramps g linearly over 20 ms")
    {
        constexpr int rampLength = 960; // 20 ms at 48 kHz
        setGainDb (plugin, -60.0f);
        const auto output = processOnes (plugin, rampLength + 10);

        const double target = 0.001;
        for (int n = 0; n < output.getNumSamples(); ++n)
        {
            const auto expected = n < rampLength ? 1.0 + (target - 1.0) * (n + 1) / rampLength : target;
            CAPTURE (n);
            CHECK_THAT (output.getSample (0, n), Catch::Matchers::WithinAbs (expected, 1e-5));
            CHECK (juce::exactlyEqual (output.getSample (1, n), output.getSample (0, n)));
        }
    }

    SECTION ("loading a state sets g without a ramp")
    {
        setGainDb (plugin, 12.0f);
        juce::MemoryBlock state;
        plugin.getStateInformation (state);

        setGainDb (plugin, 0.0f);
        plugin.reset();

        plugin.setStateInformation (state.getData(), static_cast<int> (state.getSize()));
        const auto output = processOnes (plugin, 16);

        const auto expected = std::pow (10.0, 12.0 / 20.0);
        for (int n = 0; n < output.getNumSamples(); ++n)
            CHECK_THAT (output.getSample (0, n), Catch::Matchers::WithinRel (expected, 1e-6));
    }
}
