#pragma once

#include "PluginProcessor.h"

//==============================================================================
// Custom label that knows how to start an external file drag
class MidiDragLabel : public juce::Label
{
public:
    std::function<juce::File()> onDragRequested; // returns the midi file to drag

    void mouseDown (const juce::MouseEvent& e) override
    {
        // let normal label behaviour happen too, if you want click-to-select etc.
        juce::Label::mouseDown (e);
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (dragging || onDragRequested == nullptr)
            return;

        // small threshold so a click doesn't register as a drag
        if (e.getDistanceFromDragStart() < 5)
            return;

        juce::File midiFile = onDragRequested();

        if (! midiFile.existsAsFile())
            return;

        dragging = true;

        auto* container = juce::DragAndDropContainer::findParentDragContainerFor (this);
        if (container != nullptr)
        {
            container->performExternalDragDropOfFiles (
                juce::StringArray (midiFile.getFullPathName()),
                false, // don't allow moving the file, just copy/drop
                this,
                nullptr);
        }

        dragging = false;
    }

private:
    bool dragging = false;
};

//==============================================================================
class ParsimoniousAudioProcessorEditor  : public juce::AudioProcessorEditor,
                                           public juce::Button::Listener,
                                           public juce::DragAndDropContainer
{
public:
    ParsimoniousAudioProcessorEditor (ParsimoniousAudioProcessor&);
    ~ParsimoniousAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;
    void buttonClicked (juce::Button* button) override;

private:
    ParsimoniousAudioProcessor& audioProcessor;

    juce::TextButton generateButton;
    juce::Label title;
    MidiDragLabel dragArea;

    juce::Slider numBarsKnob;
    juce::Slider chordsPerBarKnob;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> numBarsAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chordsPerBarAttachment;

    juce::File lastGeneratedFile;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParsimoniousAudioProcessorEditor)
};