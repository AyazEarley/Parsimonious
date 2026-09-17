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

    addAndMakeVisible(title);
    title.setText("Parsimonious", juce::dontSendNotification);

    title.setFont (juce::Font (32.0f, juce::Font::bold));
    title.setColour (juce::Label::textColourId, juce::Colours::white);
    title.setJustificationType (juce::Justification::centred);

    addAndMakeVisible(dragArea);

    dragArea.setText ("No file yet", juce::dontSendNotification);
    dragArea.setJustificationType (juce::Justification::centred);
    dragArea.setColour (juce::Label::backgroundColourId, juce::Colours::darkgrey);
    dragArea.setColour (juce::Label::outlineColourId, juce::Colours::black);

    numBarsKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    numBarsKnob.setRange(1.0, 32.0, 1.0);
    numBarsKnob.setValue(4.0);
    numBarsKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(numBarsKnob);
    numBarsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    audioProcessor.apvts, "bars", numBarsKnob);

    chordsPerBarKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    chordsPerBarKnob.setRange(1.0, 8.0, 1.0);
    chordsPerBarKnob.setValue(4.0);
    chordsPerBarKnob.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(chordsPerBarKnob);
    chordsPerBarAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
    audioProcessor.apvts, "chordsPerBar", chordsPerBarKnob);

    setSize (400, 200);
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
    g.drawFittedText ("", getLocalBounds(), juce::Justification::centred, 1);
}

void ParsimoniousAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (10);

    title.setBounds(5,5, 200, 30);

    const int knobWidth = 100;
    const int buttonWidth = 80;

    numBarsKnob.setBounds (area.removeFromLeft (knobWidth));
    area.removeFromLeft (10);

    chordsPerBarKnob.setBounds (area.removeFromLeft (knobWidth));
    area.removeFromLeft (10);

    generateButton.setBounds (area.removeFromLeft (buttonWidth).withSizeKeepingCentre (buttonWidth, 40));
    area.removeFromLeft (10);

    dragArea.setBounds (area);
}

void ParsimoniousAudioProcessorEditor::buttonClicked(juce::Button* button){
    if(button== &generateButton){
        int numBar = static_cast<int>(numBarsKnob.getValue());
        dragArea.setText(juce::String(numBar), juce::dontSendNotification);
    }
}