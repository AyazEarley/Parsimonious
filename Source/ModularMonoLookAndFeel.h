#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Flat "modular mono" skin: cream background, dark hairlines, no bevels/gradients.
// Uses Corbel where available (Windows); falls back to the system default elsewhere.
class ModularMonoLookAndFeel : public juce::LookAndFeel_V4
{
public:
    ModularMonoLookAndFeel()
    {
        setColour (juce::ResizableWindow::backgroundColourId, background);
        setColour (juce::Label::textColourId, ink);
        setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        setColour (juce::TextButton::textColourOffId, ink);
        setColour (juce::ComboBox::outlineColourId, line);
        setColour (juce::TextEditor::backgroundColourId, background);
        setColour (juce::TextEditor::outlineColourId, line);
        setColour (juce::TextEditor::textColourId, ink);
        setColour (juce::ToggleButton::textColourId, ink);
        setColour (juce::ToggleButton::tickColourId, ink);
        setColour (juce::ToggleButton::tickDisabledColourId, line);
    }

    juce::Font getLabelFont (juce::Label& label) override
    {
        // Labels that explicitly set their own font keep it;
        // everything else (e.g. dragArea) uses the default style.
        if (label.getProperties().getWithDefault ("customFont", false))
            return label.getFont();

        return juce::Font (juce::FontOptions ("Corbel", 15.0f, juce::Font::bold));
    }

    juce::Font getTextButtonFont (juce::TextButton&, int height) override
    {
        return juce::Font (juce::FontOptions ("Corbel", (float) height * 0.5f, juce::Font::plain));
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& button, const juce::Colour&,
                                bool isMouseOverButton, bool isButtonDown) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        const bool filled = button.getProperties().getWithDefault ("filled", false);

        if (filled)
        {
            auto fillBounds = bounds.reduced (1.0f);

            if (isButtonDown)
            {
                g.setColour (line);
                g.fillRoundedRectangle (fillBounds, 2.0f);
            }

            g.setColour (juce::Colours::black);
            g.drawRoundedRectangle (fillBounds, 2.0f, 1.5f);
        }
        else
        {
            g.setColour (isButtonDown ? line : background);
            g.fillRoundedRectangle (bounds, 2.0f);
            g.setColour (isMouseOverButton ? ink : ink.withAlpha (0.8f));
            g.drawRoundedRectangle (bounds.reduced (0.5f), 2.0f, 1.0f);
        }
    }

    juce::Colour getTextButtonTextColour (juce::TextButton& button, bool)
    {
        const bool filled = button.getProperties().getWithDefault ("filled", false);
        return filled ? background : ink;
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPosProportional,
                            float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float> ((float) x, (float) y, (float) width, (float) height).reduced (3.0f);
        const float ringThickness = juce::jmax (2.5f, bounds.getWidth() * 0.08f);
        auto radius = juce::jmin (bounds.getWidth(), bounds.getHeight()) / 2.0f - ringThickness / 2.0f;
        auto centre = bounds.getCentre();

        // Background ring (full circle, shows the unfilled range).
        g.setColour (knobFill);
        g.drawEllipse (centre.x - radius, centre.y - radius, radius * 2.0f, radius * 2.0f, ringThickness);

        // Value arc (filled portion, from rotaryStartAngle to the current position).
        auto angle = rotaryStartAngle + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);
        juce::Path valueArc;
        valueArc.addCentredArc (centre.x, centre.y, radius, radius, 0.0f,
                                 rotaryStartAngle, angle, true);
        g.setColour (ink);
        g.strokePath (valueArc, juce::PathStrokeType (ringThickness,
                                                        juce::PathStrokeType::curved,
                                                        juce::PathStrokeType::rounded));

        // Centered numeric readout, e.g. "4".
        g.setFont (juce::Font (juce::FontOptions ("Corbel", radius * 0.55f, juce::Font::bold)));
        g.drawText (juce::String ((int) std::round (slider.getValue())),
                    bounds.toNearestInt(), juce::Justification::centred);
    }

    void drawToggleButton (juce::Graphics& g, juce::ToggleButton& button, bool, bool) override
    {
        auto bounds = button.getLocalBounds().toFloat();
        auto boxSize = 14.0f;
        juce::Rectangle<float> box (bounds.getX(), bounds.getCentreY() - boxSize / 2.0f, boxSize, boxSize);

        g.setColour (background);
        g.fillRect (box);
        g.setColour (ink);
        g.drawRect (box, 1.0f);

        if (button.getToggleState())
            g.fillRect (box.reduced (3.0f));

        g.setFont (juce::Font (juce::FontOptions ("Corbel", 11.0f, juce::Font::plain)));
        g.drawFittedText (button.getButtonText(),
                           bounds.getX() > box.getRight()
                               ? bounds.toNearestInt()
                               : juce::Rectangle<int> ((int) box.getRight() + 6, 0, (int) bounds.getWidth(), (int) bounds.getHeight()),
                           juce::Justification::centredLeft, 1);
    }

    const juce::Colour background { 0xfff4f2ec };
    const juce::Colour ink        { 0xff1c1c1a };
    const juce::Colour line       { 0xffc9c6ba };
    const juce::Colour knobFill   { 0xffe8e5da };
    const juce::Colour accent { 0xffffffff };
};
