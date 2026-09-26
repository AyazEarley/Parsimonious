    /*
    ==============================================================================

        This file contains the basic framework code for a JUCE plugin editor.

    ==============================================================================
    */

    #include "PluginProcessor.h"
    #include "PluginEditor.h"
    #include "HeaderTicks.h"

    //==============================================================================
    ParsimoniousAudioProcessorEditor::ParsimoniousAudioProcessorEditor (ParsimoniousAudioProcessor& p)
        : AudioProcessorEditor (&p), audioProcessor (p)
    {
        setLookAndFeel (&modularMonoLookAndFeel);

        addAndMakeVisible(generateButton);
        generateButton.setButtonText("generate");
        generateButton.getProperties().set ("filled", true);
        generateButton.addListener(this);

        addAndMakeVisible(title);
        title.setText("Parsimonious", juce::dontSendNotification);
        title.setFont (juce::Font (juce::FontOptions ("Corbel", 20.0f, juce::Font::bold)));
        title.getProperties().set ("customFont", true);
        title.setColour (juce::Label::textColourId, modularMonoLookAndFeel.ink);
        title.setJustificationType (juce::Justification::centredLeft);

        //addAndMakeVisible(headerTicks);

        addAndMakeVisible(dragArea);
        dragArea.setText ("drag me into your track", juce::dontSendNotification);
        dragArea.setJustificationType (juce::Justification::centred);
        dragArea.setColour (juce::Label::backgroundColourId, modularMonoLookAndFeel.background);
        dragArea.setColour (juce::Label::outlineColourId, modularMonoLookAndFeel.line);
        dragArea.setColour (juce::Label::textColourId, modularMonoLookAndFeel.ink);

        numBarsKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        numBarsKnob.setRange(1.0, 32.0, 1.0);
        numBarsKnob.setValue(4.0);
        numBarsKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(numBarsKnob);
        numBarsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, "bars", numBarsKnob);

        addAndMakeVisible(numBarsLabel);
        numBarsLabel.setText ("bars", juce::dontSendNotification);
        numBarsLabel.setJustificationType (juce::Justification::centred);
        numBarsLabel.setFont (juce::Font (juce::FontOptions ("Corbel", 10.0f, juce::Font::plain)));
        numBarsLabel.setColour (juce::Label::textColourId, modularMonoLookAndFeel.ink);

        chordsPerBarKnob.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        chordsPerBarKnob.setRange(1.0, 8.0, 1.0);
        chordsPerBarKnob.setValue(4.0);
        chordsPerBarKnob.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
        addAndMakeVisible(chordsPerBarKnob);
        chordsPerBarAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
            audioProcessor.apvts, "chordsPerBar", chordsPerBarKnob);

        addAndMakeVisible(chordsPerBarLabel);
        chordsPerBarLabel.setText ("chords/bar", juce::dontSendNotification);
        chordsPerBarLabel.setJustificationType (juce::Justification::centred);
        chordsPerBarLabel.setFont (juce::Font (juce::FontOptions ("Corbel", 10.0f, juce::Font::plain)));
        chordsPerBarLabel.setColour (juce::Label::textColourId, modularMonoLookAndFeel.ink);
        chordsPerBarLabel.setBounds (chordsPerBarKnob.getBounds().getCentreX() - 44,
                              chordsPerBarKnob.getBottom() + 2, 88, 14);

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

        triadsCheck.setButtonText("use triads");
        addAndMakeVisible(triadsCheck);
        useTriadsAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            audioProcessor.apvts, "useTriads", triadsCheck);

        loopCheck.setButtonText("force loop");
        addAndMakeVisible(loopCheck);
        forceLoopAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
            audioProcessor.apvts, "forceLoop", loopCheck);

        // Tighter window: sized to what's actually on screen, no reserved dead space.
        setSize (420, 210);
    }

    ParsimoniousAudioProcessorEditor::~ParsimoniousAudioProcessorEditor()
    {
        setLookAndFeel (nullptr);
    }

    //==============================================================================
    void ParsimoniousAudioProcessorEditor::paint (juce::Graphics& g)
    {
        g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
    }

    void ParsimoniousAudioProcessorEditor::resized()
    {
        auto area = getLocalBounds().reduced (14);

        auto header = area.removeFromTop (34);
        auto checkboxArea = header.removeFromRight (80);
        juce::FlexBox checkboxColumn;
        checkboxColumn.flexDirection = juce::FlexBox::Direction::column;
        checkboxColumn.justifyContent = juce::FlexBox::JustifyContent::spaceBetween;
        checkboxColumn.items.add (juce::FlexItem (triadsCheck).withHeight (16.0f).withWidth (80.0f));
        checkboxColumn.items.add (juce::FlexItem (loopCheck).withHeight (16.0f).withWidth (80.0f));
        checkboxColumn.performLayout (checkboxArea.toFloat());

        title.setBounds (header.removeFromLeft (200));
        header.removeFromLeft (12);
        header.removeFromRight (12);
        headerTicks.setBounds (header.withSizeKeepingCentre (header.getWidth(), 20));

        area.removeFromTop (10);

        const int gap = 8; // shared spacing used everywhere below
        const int knobSize = 56;

        auto knobRow = area.removeFromTop (knobSize);

        juce::FlexBox controlsBox;
        controlsBox.flexDirection = juce::FlexBox::Direction::row;
        controlsBox.alignItems = juce::FlexBox::AlignItems::flexStart;
        controlsBox.items.add (juce::FlexItem (numBarsKnob).withWidth ((float) knobSize).withHeight ((float) knobSize));
        controlsBox.items.add (juce::FlexItem().withWidth (14.0f));
        controlsBox.items.add (juce::FlexItem (chordsPerBarKnob).withWidth ((float) knobSize).withHeight ((float) knobSize));
        controlsBox.items.add (juce::FlexItem().withWidth (18.0f));
        controlsBox.items.add (juce::FlexItem (generateButton).withMinWidth (100.0f).withHeight (32.0f).withFlex (1.0f));
        controlsBox.performLayout (knobRow.toFloat());

        numBarsLabel.setBounds (numBarsKnob.getX(), numBarsKnob.getBottom() + 2, knobSize, 14);
        const int chordsLabelWidth = 88;
        chordsPerBarLabel.setBounds (chordsPerBarKnob.getBounds().getCentreX() - chordsLabelWidth / 2,
                                    chordsPerBarKnob.getBottom() + 2, chordsLabelWidth, 14);

        auto seedRow = juce::Rectangle<int> (generateButton.getX(), generateButton.getBottom() + gap,
                                            area.getRight() - generateButton.getX(), 30);
        juce::FlexBox seedBox;
        seedBox.flexDirection = juce::FlexBox::Direction::row;
        seedBox.items.add (juce::FlexItem (intBox).withWidth (44.0f).withHeight (28.0f));
        seedBox.items.add (juce::FlexItem().withWidth (8.0f));
        seedBox.items.add (juce::FlexItem (randomButton).withFlex (1.0f).withHeight (28.0f));
        seedBox.performLayout (seedRow.toFloat());

        // Drag target fills everything left, with the same gap above it
        // as the one between the generate button and the seed row.
        auto dragTop = seedRow.getBottom() + gap;
        dragArea.setBounds (area.getX(), dragTop, area.getWidth(), area.getBottom() - dragTop);
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
                if(triads.empty()){
                    dragArea.setText ("cannot generate triad loops of odd length!", juce::dontSendNotification);
                    return;
                }
                else{
                    lastGeneratedFile = audioProcessor.createMidiFileTriads(triads);
                }
            }

            if (lastGeneratedFile.existsAsFile())
                dragArea.setText ("drag me into your track", juce::dontSendNotification);
            else
                dragArea.setText ("generation failed", juce::dontSendNotification);
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
