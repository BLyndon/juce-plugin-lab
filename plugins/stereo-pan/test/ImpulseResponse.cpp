#include <PluginProcessor.h>
#include <catch2/catch_test_macros.hpp>

// Expected values from spec.md: y[n] = x[n], so an impulse comes out unchanged (bit-exact)
static void checkImpulseIsUnchanged (PluginProcessor& plugin, int numChannels)
{
    constexpr double sampleRate = 48000.0;
    constexpr int blockSize = 512;

    plugin.prepareToPlay (sampleRate, blockSize);

    juce::AudioBuffer<float> buffer (numChannels, blockSize);
    buffer.clear();
    for (int channel = 0; channel < numChannels; ++channel)
        buffer.setSample (channel, 0, 1.0f);

    juce::MidiBuffer midi;
    plugin.processBlock (buffer, midi);

    for (int channel = 0; channel < numChannels; ++channel)
    {
        CHECK (buffer.getSample (channel, 0) == 1.0f);
        for (int i = 1; i < blockSize; ++i)
            CHECK (buffer.getSample (channel, i) == 0.0f);
    }

    plugin.releaseResources();
}

TEST_CASE ("Impulse response", "[dsp]")
{
    PluginProcessor plugin;

    SECTION ("stereo")
    {
        checkImpulseIsUnchanged (plugin, 2);
    }

    SECTION ("mono")
    {
        juce::AudioProcessor::BusesLayout mono;
        mono.inputBuses.add (juce::AudioChannelSet::mono());
        mono.outputBuses.add (juce::AudioChannelSet::mono());
        REQUIRE (plugin.setBusesLayout (mono));

        checkImpulseIsUnchanged (plugin, 1);
    }
}
