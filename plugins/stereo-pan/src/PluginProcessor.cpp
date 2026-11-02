#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PluginProcessor::PluginProcessor()
    : AudioProcessor (BusesProperties()
              .withInput ("Input", juce::AudioChannelSet::stereo(), true)
              .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "Parameters", createParameterLayout())
{
    width = parameters.getRawParameterValue (Parameters::Width::id);
    position = parameters.getRawParameterValue (Parameters::Position::id);
    snapToTargetGains();
}

juce::AudioProcessorValueTreeState::ParameterLayout PluginProcessor::createParameterLayout()
{
    // Width w in 0 … 1, shown as 0 … 100 %
    auto widthToText = [] (float value, int) {
        return juce::String (juce::roundToInt (value * 100.0f)) + " %";
    };
    auto textToWidth = [] (const juce::String& text) {
        return text.getFloatValue() / 100.0f;
    };

    // Position p in −1 … +1, shown as 100 % L … C … 100 % R
    auto positionToText = [] (float value, int) {
        const auto percent = juce::roundToInt (value * 100.0f);
        if (percent == 0)
            return juce::String ("C");
        return juce::String (std::abs (percent)) + (percent < 0 ? " % L" : " % R");
    };
    auto textToPosition = [] (const juce::String& text) {
        const auto magnitude = std::abs (text.getFloatValue()) / 100.0f;
        return text.containsIgnoreCase ("L") ? -magnitude : magnitude;
    };

    return {
        std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { Parameters::Width::id, Parameters::Width::version },
            Parameters::Width::name,
            juce::NormalisableRange<float> (Parameters::Width::min, Parameters::Width::max),
            Parameters::Width::defaultValue,
            juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction (widthToText)
                .withValueFromStringFunction (textToWidth)),
        std::make_unique<juce::AudioParameterFloat> (
            juce::ParameterID { Parameters::Position::id, Parameters::Position::version },
            Parameters::Position::name,
            juce::NormalisableRange<float> (Parameters::Position::min, Parameters::Position::max),
            Parameters::Position::defaultValue,
            juce::AudioParameterFloatAttributes()
                .withStringFromValueFunction (positionToText)
                .withValueFromStringFunction (textToPosition))
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
PluginProcessor::Gains PluginProcessor::getTargetGains() const
{
    const auto w = width->load();
    const auto p = position->load();

    // Limit the width so both input positions stay in −1 … +1 and the image centre stays at p
    const auto wEff = juce::jmin (w, 1.0f - std::abs (p));
    const auto pL = p - wEff;
    const auto pR = p + wEff;

    // Constant-power law per input: θ = π/4 · (p + 1)
    const auto thetaL = juce::MathConstants<float>::pi / 4.0f * (pL + 1.0f);
    const auto thetaR = juce::MathConstants<float>::pi / 4.0f * (pR + 1.0f);

    return { std::cos (thetaL), std::cos (thetaR), std::sin (thetaL), std::sin (thetaR) };
}

void PluginProcessor::snapToTargetGains()
{
    targetGains = getTargetGains();
    currentGains = targetGains;
    gainSteps = {};
    rampSamplesRemaining = 0;
}

void PluginProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    rampLengthSamples = juce::jmax (1, juce::roundToInt (Parameters::rampTimeSeconds * sampleRate));
    snapRequested = false;
    snapToTargetGains();
}

void PluginProcessor::releaseResources()
{
}

void PluginProcessor::reset()
{
    snapToTargetGains();
}

bool PluginProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    // Stereo in, stereo out only
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
           && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void PluginProcessor::processBlock (juce::AudioBuffer<float>& buffer,
    juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;

    if (snapRequested.exchange (false))
    {
        snapToTargetGains();
    }
    else
    {
        // On every change, start a new linear ramp from the current gains to the new targets
        const auto newTargets = getTargetGains();
        if (newTargets != targetGains)
        {
            targetGains = newTargets;
            rampSamplesRemaining = rampLengthSamples;
            for (size_t i = 0; i < gainSteps.size(); ++i)
                gainSteps[i] = (targetGains[i] - currentGains[i]) / static_cast<float> (rampLengthSamples);
        }
    }

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getWritePointer (1);

    for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
    {
        if (rampSamplesRemaining > 0)
        {
            --rampSamplesRemaining;
            // The last ramp step lands exactly on the targets
            for (size_t i = 0; i < currentGains.size(); ++i)
                currentGains[i] = rampSamplesRemaining == 0 ? targetGains[i] : currentGains[i] + gainSteps[i];
        }

        const auto [a, b, c, d] = currentGains;

        // Read both inputs before writing: the buffer is processed in place
        const auto xL = left[sample];
        const auto xR = right[sample];

        // y_L = a·x_L + b·x_R, y_R = c·x_L + d·x_R
        left[sample] = a * xL + b * xR;
        right[sample] = c * xL + d * xR;
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
