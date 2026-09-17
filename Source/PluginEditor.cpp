/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
ParsimoniousAudioProcessorEditor::ParsimoniousAudioProcessorEditor (ParsimoniousAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addAndMakeVisible(generateButton);
    generateButton.setButtonText("generate");
    generateButton.addListener(this);


    addAndMakeVisible(dragArea);

    dragArea.setText ("No file yet", juce::dontSendNotification);
    dragArea.setJustificationType (juce::Justification::centred);
    dragArea.setColour (juce::Label::backgroundColourId, juce::Colours::darkgrey);
    dragArea.setColour (juce::Label::outlineColourId, juce::Colours::black);


    setSize (400, 300);
}

ParsimoniousAudioProcessorEditor::~ParsimoniousAudioProcessorEditor()
{
}

//==============================================================================
void ParsimoniousAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (juce::FontOptions (15.0f));
    g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void ParsimoniousAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10);

    generateButton.setBounds (area.removeFromTop (40));
    area.removeFromTop (10);
    dragArea.setBounds (area);
}

void ParsimoniousAudioProcessorEditor::buttonClicked(juce::Button* button){
    if(button== &generateButton){
        dragArea.setText("Clicked", juce::dontSendNotification);
    }
}