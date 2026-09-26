#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

// Purely decorative: a row of flat vertical bars, sized to fit whatever
// bounds it's given. Used to fill the header gap between the title and
// the checkbox column.
class HeaderTicks : public juce::Component
{
public:
    HeaderTicks()
    {
        setInterceptsMouseClicks (false, false);
    }

    void paint (juce::Graphics& g) override
    {
        static const float heights[] = { 0.55f, 0.85f, 0.4f, 0.75f, 0.3f, 0.65f };
        const int numTicks = (int) (sizeof (heights) / sizeof (heights[0]));
        const float tickWidth = 4.0f;
        const float gap = 3.0f;
        const float totalWidth = numTicks * tickWidth + (numTicks - 1) * gap;

        auto bounds = getLocalBounds().toFloat();
        float x = bounds.getCentreX() - totalWidth / 2.0f;

        for (int i = 0; i < numTicks; ++i)
        {
            float h = bounds.getHeight() * heights[i];
            juce::Rectangle<float> tick (x, bounds.getCentreY() - h / 2.0f, tickWidth, h);

            g.setColour (fillColour);
            g.fillRect (tick);
            g.setColour (lineColour);
            g.drawRect (tick, 1.0f);

            x += tickWidth + gap;
        }
    }

    juce::Colour fillColour { 0xffe8e5da };
    juce::Colour lineColour { 0xffc9c6ba };
};
