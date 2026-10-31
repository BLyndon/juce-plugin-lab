#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
              .withInput ("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "Parameters", createParameterLayout())
{
    gainDb = parameters.getRawParameterValue ("gain");
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    return {
        std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { "gain", 1 },
            "Gain",
            juce::NormalisableRange<float> (-60.0f, 12.0f),
            0.0f,
            juce::AudioParameterFloatAttributes().withLabel ("dB"))
    };
}

PluginProcessor::~PluginProcessor()
{
}

//==============================================================================
const juce::String PluginProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PluginProcessor::acceptsMidi() const
{
    return false;
}

bool PluginProcessor::producesMidi() const
{
    return false;
}

bool PluginProcessor::isMidiEffect() const
{
    return false;
}

double PluginProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PluginProcessor::getNumPrograms()
{
    // Some hosts don't cope well with 0 programs, so report at least 1
    return 1;
}

int PluginProcessor::getCurrentProgram()
{
    return 0;
}

void PluginProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String PluginProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    // Steinberg's VST3 validator fails plugins whose single default program has no name
    return "Default";
}

void PluginProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
float PluginProcessor::getTargetGain() const
{
    // g = 10^(L/20)
    return std::pow (10.0f, gainDb->load() / 20.0f);
}

void PluginProcessor::snapToTargetGain()
{
    targetGain = getTargetGain();
    currentGain = targetGain;
    gainStep = 0.0f;
    rampSamplesRemaining = 0;
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    // Ramp time 20 ms
    rampLengthSamples = juce::jmax (1, juce::roundToInt (0.02 * sampleRate));
    snapRequested = false;
    snapToTargetGain();
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::reset()
{
    snapToTargetGain();
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Mono or stereo, input layout must match output layout
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    return layouts.getMainOutputChannelSet() == layouts.getMainInputChannelSet();
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // Output channels without input data may contain garbage
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    if (snapRequested.exchange (false))
    {
        snapToTargetGain();
    }
    else
    {
        // On every change, start a new linear ramp from the current g to the new target
        const auto newTarget = getTargetGain();
        if (!juce::exactlyEqual (newTarget, targetGain))
        {
            targetGain = newTarget;
            rampSamplesRemaining = rampLengthSamples;
            gainStep = (targetGain - currentGain) / static_cast<float> (rampLengthSamples);
        }
    }

    // y[n] = g[n] * x[n], the same g[n] for every channel
    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (rampSamplesRemaining > 0)
        {
            --rampSamplesRemaining;
            // The last ramp step lands exactly on the target
            currentGain = rampSamplesRemaining == 0 ? targetGain : currentGain + gainStep;
        }

        for (int channel = 0; channel < totalNumInputChannels; ++channel)
            buffer.getWritePointer (channel)[sample] *= currentGain;
    }
}

//==============================================================================
bool PluginProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor()
{
    return new PluginEditor (*this);
}

//==============================================================================
void PluginProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, destData);
}

void PluginProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));
    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
    {
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
        // A loaded state takes effect without a ramp
        snapRequested = true;
    }
}

//==============================================================================
// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PluginProcessor();
}
