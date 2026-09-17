/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class ParsimoniousAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                          private juce::Button::Listener
{
public:
    ParsimoniousAudioProcessorEditor (ParsimoniousAudioProcessor&);
    ~ParsimoniousAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void buttonClicked (juce::Button* button) override;

    ParsimoniousAudioProcessor& audioProcessor;

    juce::TextButton generateButton;
    
    juce::Slider numBarsKnob;
    juce::Slider chordsPerBarKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> numBarsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chordsPerBarAttachment;

    juce::Label dragArea;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParsimoniousAudioProcessorEditor)
};
