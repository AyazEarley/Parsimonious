#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>
#include <array>
#include <map>

class ChordWebComponent : public juce::Component
{

private:

    static constexpr int MAJOR7 = 1;
    static constexpr int MINOR7 = 2;
    static constexpr int DOM7 = 3;
    static constexpr int HALF7 = 4;
    static constexpr int FULL7 = 5;

    static constexpr std::array<std::array<int, 2>, 3> full7Chords = {{
        { 0, FULL7 },
        { 1, FULL7 },
        { 2, FULL7 }
    }};

    static constexpr std::array<std::array<int, 2>, 12> major7Chords = [] {
        std::array<std::array<int, 2>, 12> arr{};
        for (int i = 0; i < 12; ++i)
            arr[i] = { i, MAJOR7 };
        return arr;
    }();

    static constexpr std::array<std::array<int, 2>, 12> minor7Chords = [] {
        std::array<std::array<int, 2>, 12> arr{};
        for (int i = 0; i < 12; ++i)
            arr[i] = { i, MINOR7 };
        return arr;
    }();

    static constexpr std::array<std::array<int, 2>, 12> dom7Chords = [] {
        std::array<std::array<int, 2>, 12> arr{};
        for (int i = 0; i < 12; ++i)
            arr[i] = { i, DOM7 };
        return arr;
    }();

    static constexpr std::array<std::array<int, 2>, 12> half7Chords = [] {
        std::array<std::array<int, 2>, 12> arr{};
        for (int i = 0; i < 12; ++i)
            arr[i] = { i, HALF7 };
        return arr;
    }();

    static constexpr double rotationStep = juce::MathConstants<double>::twoPi / 12;
    static constexpr double radius = 5.0;
    static constexpr double radiusStep = radius / 9;

    static std::array<double, 2> getXY(double angle, double r){
        return {r * std::sin(angle), r * std::cos(angle)};
    }

    std::map<std::array<int, 2>, std::array<double, 2>> positionMap = {
        //full diminished
        {full7Chords[0], getXY(0, radiusStep * 9)},
        {full7Chords[1], getXY(rotationStep * 4, radiusStep * 9)},
        {full7Chords[2], getXY(rotationStep * 8, radiusStep * 9)},

        //major7
        {major7Chords[5], getXY(0, radiusStep * 1)},
        {major7Chords[8], getXY(0, radiusStep * 2)},
        {major7Chords[11], getXY(0, radiusStep * 3)},
        {major7Chords[2], getXY(0, radiusStep * 4)},

        {major7Chords[9], getXY(rotationStep * 4, radiusStep * 1)},
        {major7Chords[0], getXY(rotationStep * 4, radiusStep * 2)},
        {major7Chords[3], getXY(rotationStep * 4, radiusStep * 3)},
        {major7Chords[6], getXY(rotationStep * 4, radiusStep * 4)},

        {major7Chords[1], getXY(rotationStep * 8, radiusStep * 1)},
        {major7Chords[4], getXY(rotationStep * 8, radiusStep * 2)},
        {major7Chords[7], getXY(rotationStep * 8, radiusStep * 3)},
        {major7Chords[10], getXY(rotationStep * 8, radiusStep * 4)},

        //minor7
        {minor7Chords[6], getXY(rotationStep * 2, radiusStep * 5)},
        {minor7Chords[9], getXY(rotationStep * 2, radiusStep * 6)},
        {minor7Chords[0], getXY(rotationStep * 2, radiusStep * 7)},
        {minor7Chords[3], getXY(rotationStep * 2, radiusStep * 8)},

        {minor7Chords[10], getXY(rotationStep * 6, radiusStep * 5)},
        {minor7Chords[1], getXY(rotationStep * 6, radiusStep * 6)},
        {minor7Chords[4], getXY(rotationStep * 6, radiusStep * 7)},
        {minor7Chords[7], getXY(rotationStep * 6, radiusStep * 8)},

        {minor7Chords[2], getXY(rotationStep * 10, radiusStep * 5)},
        {minor7Chords[5], getXY(rotationStep * 10, radiusStep * 6)},
        {minor7Chords[8], getXY(rotationStep * 10, radiusStep * 7)},
        {minor7Chords[11], getXY(rotationStep * 10, radiusStep * 8)},

        //half diminished
        {half7Chords[6], getXY(rotationStep * 1, radiusStep * 5)},
        {half7Chords[9], getXY(rotationStep * 1, radiusStep * 6)},
        {half7Chords[0], getXY(rotationStep * 1, radiusStep * 7)},
        {half7Chords[3], getXY(rotationStep * 1, radiusStep * 8)},

        {half7Chords[10], getXY(rotationStep * 5, radiusStep * 5)},
        {half7Chords[1], getXY(rotationStep * 5, radiusStep * 6)},
        {half7Chords[4], getXY(rotationStep * 5, radiusStep * 7)},
        {half7Chords[7], getXY(rotationStep * 5, radiusStep * 8)},

        {half7Chords[2], getXY(rotationStep * 9, radiusStep * 5)},
        {half7Chords[5], getXY(rotationStep * 9, radiusStep * 6)},
        {half7Chords[8], getXY(rotationStep * 9, radiusStep * 7)},
        {half7Chords[11], getXY(rotationStep * 9, radiusStep * 8)},

        //dom7
        {dom7Chords[9], getXY(rotationStep * 3, radiusStep * 5)},
        {dom7Chords[0], getXY(rotationStep * 3, radiusStep * 6)},
        {dom7Chords[3], getXY(rotationStep * 3, radiusStep * 7)},
        {dom7Chords[6], getXY(rotationStep * 3, radiusStep * 8)},

        {dom7Chords[1], getXY(rotationStep * 7, radiusStep * 5)},
        {dom7Chords[4], getXY(rotationStep * 7, radiusStep * 6)},
        {dom7Chords[7], getXY(rotationStep * 7, radiusStep * 7)},
        {dom7Chords[10], getXY(rotationStep * 7, radiusStep * 8)},

        {dom7Chords[5], getXY(rotationStep * 11, radiusStep * 5)},
        {dom7Chords[8], getXY(rotationStep * 11, radiusStep * 6)},
        {dom7Chords[11], getXY(rotationStep * 11, radiusStep * 7)},
        {dom7Chords[2], getXY(rotationStep * 11, radiusStep * 8)},
    };
    

public:
    ChordWebComponent()
    {
        setOpaque (false);
        setInterceptsMouseClicks (false, false); // doesn't steal clicks (for now)
    }

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.drawRect (bounds.reduced (0.5f), 1.0f);

        const auto centre = bounds.getCentre();

        const float margin     = 20.0f;
        const float pixelRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f - margin;
        const float scale = pixelRadius / (float) radius;
        const float diameter = 5.0f; //juce::jmax (6.0f, pixelRadius * 0.005f);
        
        bool useTriads = false; //for now we're only working on the nontriad option
        if (! useTriads)
        {
            g.setColour (juce::Colours::black);

            for (const auto& [chord, pos] : positionMap)
            {
                const float x = centre.x + (float) pos[0] * scale;
                const float y = centre.y - (float) pos[1] * scale; // flip y so angles go counter-clockwise

                g.fillEllipse (x - diameter * 0.5f,
                            y - diameter * 0.5f,
                            diameter, diameter);
            }
        }
    }
};