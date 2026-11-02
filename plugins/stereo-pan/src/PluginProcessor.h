#pragma once

#include "Parameters.h"
#include <array>
#include <juce_audio_processors/juce_audio_processors.h>

class PluginProcessor : public juce::AudioProcessor
{
public:
    PluginProcessor();
    ~PluginProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    void reset() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState parameters;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    // The four gains of the 2×2 mix, in the order a, b, c, d
    using Gains = std::array<float, 4>;

    // Target a, b, c, d from the current Width and Position values
    Gains getTargetGains() const;
    // Jumps a, b, c, d to their targets without a ramp
    void snapToTargetGains();

    std::atomic<float>* width = nullptr;
    std::atomic<float>* position = nullptr;

    // One linear smoother per gain, all four sharing one ramp counter
    Gains currentGains { 1.0f, 0.0f, 0.0f, 1.0f };
    Gains targetGains { 1.0f, 0.0f, 0.0f, 1.0f };
    Gains gainSteps {};
    int rampSamplesRemaining = 0;
    int rampLengthSamples = 1;

    // Set by setStateInformation, consumed by the audio thread
    std::atomic<bool> snapRequested { false };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginProcessor)
};
