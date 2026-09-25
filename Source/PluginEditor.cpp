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

    setSize (520, 260);

    dragArea.setText ("No file yet", juce::dontSendNotification);
    dragArea.setJustificationType (juce::Justification::centred);
    dragArea.setColour (juce::Label::backgroundColourId, juce::Colours::darkgrey);
    dragArea.setColour (juce::Label::outlineColourId, juce::Colours::black);

    dragArea.onDragRequested = [this]() -> juce::File
    {
        return lastGeneratedFile;
    };

    addAndMakeVisible(intBox);
    intBox.setInputRestrictions(3, "0123456789");

    intBox.setText("0");
    intBox.setJustification(juce::Justification::centred);

    intBox.onReturnKey = [this] { validateAndClampInput(); };
    intBox.onFocusLost = [this] { validateAndClampInput(); };

    addAndMakeVisible(randomButton);
    randomButton.setButtonText("random seed");
    randomButton.addListener(this);

    triadsCheck.setButtonText("Use Triads");
    addAndMakeVisible(triadsCheck);

    useTriadsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        audioProcessor.apvts, "useTriads", triadsCheck);
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

    // Title strip across the top
    title.setBounds (area.removeFromTop (40));
    area.removeFromTop (10);

    // Reserve a strip at the bottom for the drag area first,
    // so nothing else can encroach on it
    dragArea.setBounds (area.removeFromBottom (50));
    area.removeFromBottom (10);

    // Everything else lives in the remaining middle row
    const int knobWidth   = 100;
    const int buttonWidth = 80;
    const int intBoxWidth = 50;

    numBarsKnob.setBounds (area.removeFromLeft (knobWidth));
    area.removeFromLeft (10);

    chordsPerBarKnob.setBounds (area.removeFromLeft (knobWidth));
    area.removeFromLeft (10);

    generateButton.setBounds (area.removeFromLeft (buttonWidth).withSizeKeepingCentre (buttonWidth, 40));
    area.removeFromLeft (10);

    intBox.setBounds (area.removeFromLeft (intBoxWidth).withSizeKeepingCentre (intBoxWidth, 25));
    area.removeFromLeft (10);

    randomButton.setBounds (area.removeFromLeft (buttonWidth).withSizeKeepingCentre (buttonWidth, 40));

    triadsCheck.setBounds(10, 10, 200, 24);
}

void ParsimoniousAudioProcessorEditor::buttonClicked(juce::Button* button){
    if(button == &generateButton){
        int numBar = static_cast<int>(numBarsKnob.getValue());
        
        std::vector<std::array<int, 4>> chords;
        std::vector<std::array<int, 3>> triads;
        bool useTriads = audioProcessor.apvts.getRawParameterValue ("useTriads")->load() > 0.5f;
        if(!useTriads){
            chords = audioProcessor.getChords();
            lastGeneratedFile = audioProcessor.createMidiFile(chords);
        }
        else{
            triads = audioProcessor.getTriads();
            lastGeneratedFile = audioProcessor.createMidiFileTriads(triads);
        }
        

        if (lastGeneratedFile.existsAsFile())
            dragArea.setText ("Drag me into your track!", juce::dontSendNotification);
        else
            dragArea.setText ("Generation failed", juce::dontSendNotification);
    }
    else if(button == &randomButton){
        int seed = randomSeedGenerator.nextInt(1000); // 0-999
        intBox.setText(juce::String(seed), juce::dontSendNotification);
        validateAndClampInput(); // pushes the new value into the "userSeed" parameter
    }
}

void ParsimoniousAudioProcessorEditor::validateAndClampInput()
{
    auto text = intBox.getText();
    int value = text.getIntValue();

    value = juce::jlimit(1, 999, value);

    intBox.setText(juce::String(value), juce::dontSendNotification);

    if (auto* param = audioProcessor.apvts.getParameter("userSeed"))
    {
        auto range = audioProcessor.apvts.getParameterRange("userSeed");
        float normalized = range.convertTo0to1(static_cast<float>(value));

        param->setValueNotifyingHost(normalized);
    }
}