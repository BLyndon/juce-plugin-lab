#include "PluginEditor.h"

PluginEditor::PluginEditor (PluginProcessor& p)
    : AudioProcessorEditor (&p),
      processorRef (p),
      widthAttachment (processorRef.parameters, Parameters::Width::id, widthSlider),
      positionAttachment (processorRef.parameters, Parameters::Position::id, positionSlider)
{
    for (auto* label : { &widthLabel, &positionLabel })
        label->setJustificationType (juce::Justification::centred);

    widthLabel.attachToComponent (&widthSlider, false);
    positionLabel.attachToComponent (&positionSlider, false);

    addAndMakeVisible (widthSlider);
    addAndMakeVisible (positionSlider);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

PluginEditor::~PluginEditor()
{
}

void PluginEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PluginEditor::resized()
{
    auto area = getLocalBounds().reduced (40);
    widthSlider.setBounds (area.removeFromLeft (area.getWidth() / 2));
    positionSlider.setBounds (area);
}
