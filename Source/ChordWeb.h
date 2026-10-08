#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include <cmath>
#include <array>
#include <map>
#include <vector>
#include <utility>

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
        {full7Chords[0], getXY(0, radiusStep * 8)},
        {full7Chords[1], getXY(rotationStep * 4, radiusStep * 8)},
        {full7Chords[2], getXY(rotationStep * 8, radiusStep * 8)},

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

    //==============================================================================
    // Lines
    //==============================================================================
    using ChordKey = std::array<int, 2>;              // { root, quality }
    using EdgeKey  = std::pair<ChordKey, ChordKey>;   // always stored sorted

    struct LineInfo
    {
        bool highlighted = false;
    };

    std::map<EdgeKey, LineInfo> lines;

    // Option tables: { semitone offset from the source chord's root, quality of target }
    static constexpr std::array<std::array<int, 2>, 4> majOptions
    {{
        {{ 9, MINOR7}},
        {{ 4, MINOR7}},
        {{ 0, DOM7}},
        {{ 1, HALF7}}
    }};

    static constexpr std::array<std::array<int, 2>, 4> minOptions
    {{
        {{ 0, DOM7}},
        {{ 3, MAJOR7}},
        {{ 9, HALF7}},
        {{ 0, HALF7}}
    }};

    static constexpr std::array<std::array<int, 2>, 4> domOptions
    {{
        {{ 0, MINOR7}},
        {{ 9, MINOR7}},
        {{ 4, HALF7}},
        {{ 1, FULL7}}
    }};

    static constexpr std::array<std::array<int, 2>, 3> halfOptions
    {{
        {{ 0, FULL7}},
        {{ 0, MINOR7}},
        {{ 3, MINOR7}}
    }};

    static constexpr std::array<std::array<int, 2>, 8> fullOptions
    {{
        {{ 0, HALF7}},
        {{ 3, HALF7}},
        {{ 6, HALF7}},
        {{ 9, HALF7}},

        {{ 2, DOM7}},
        {{ 11, DOM7}},
        {{ 8, DOM7}},
        {{ 5, DOM7}},
    }};

    // Wrap roots into the valid range (full diminished chords only exist for roots 0-2)
    static ChordKey normalise (ChordKey c)
    {
        if (c[1] == FULL7)
            c[0] = ((c[0] % 3) + 3) % 3;
        else
            c[0] = ((c[0] % 12) + 12) % 12;
        return c;
    }

    // Lines are undirected, so sort the endpoints to get one canonical key
    static EdgeKey makeEdgeKey (ChordKey a, ChordKey b)
    {
        a = normalise (a);
        b = normalise (b);
        return (a < b) ? EdgeKey { a, b } : EdgeKey { b, a };
    }

    template <typename Options>
    void addEdges (const ChordKey& from, const Options& options)
    {
        for (const auto& opt : options)
        {
            ChordKey to = normalise (ChordKey { from[0] + opt[0], opt[1] });

            if (positionMap.count (to) > 0)   // skip anything that isn't on the map
                lines.emplace (makeEdgeKey (from, to), LineInfo{});   // duplicates ignored
        }
    }

    void buildLines()
    {
        lines.clear();

        for (const auto& entry : positionMap)
        {
            const ChordKey& chord = entry.first;

            switch (chord[1])
            {
                case MAJOR7: addEdges (chord, majOptions);  break;
                case MINOR7: addEdges (chord, minOptions);  break;
                case DOM7:   addEdges (chord, domOptions);  break;
                case HALF7:  addEdges (chord, halfOptions); break;
                case FULL7:  addEdges (chord, fullOptions); break;
                default: break;
            }
        }
    }

public:
    ChordWebComponent()
    {
        setOpaque (false);
        setInterceptsMouseClicks (false, false); // doesn't steal clicks (for now)
        buildLines();
    }

    void clearHighlights()
    {
        for (auto& entry : lines)
            entry.second.highlighted = false;

        repaint();
    }

    // Highlight (or un-highlight) a single connection between two chords
    void highlightLine (ChordKey a, ChordKey b, bool on = true)
    {
        auto it = lines.find (makeEdgeKey (a, b));

        if (it != lines.end())
            it->second.highlighted = on;

        repaint();
    }

    // Highlight a pathway: consecutive chords in the list are joined.
    // Clears any previous highlights first.
    void highlightPath (const std::vector<ChordKey>& path)
    {
        for (auto& entry : lines)
            entry.second.highlighted = false;

        for (size_t i = 0; i + 1 < path.size(); ++i)
        {
            auto it = lines.find (makeEdgeKey (path[i], path[i + 1]));

            if (it != lines.end())
                it->second.highlighted = true;
        }

        repaint();
    }

    int getNumLines() const { return (int) lines.size(); }

    static constexpr int Major7 = MAJOR7;
    static constexpr int Minor7 = MINOR7;
    static constexpr int Dom7   = DOM7;
    static constexpr int Half7  = HALF7;
    static constexpr int Full7  = FULL7;

    void paint (juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();

        g.setColour (juce::Colours::black.withAlpha (0.35f));
        g.drawRect (bounds.reduced (0.5f), 1.0f);

        const auto centre = bounds.getCentre();

        const float margin = 10.0f;
        const float pixelRadius = juce::jmin (bounds.getWidth(), bounds.getHeight()) * 0.5f - margin;
        const float scale = pixelRadius / (float) radius;
        const float diameter = 3.5f; //juce::jmax (6.0f, pixelRadius * 0.005f);

        bool useTriads = false; //for now we're only working on the nontriad option
        if (! useTriads)
        {
            auto toPixel = [&] (const ChordKey& c) -> juce::Point<float>
            {
                const auto& p = positionMap.at (c);
                return { centre.x + (float) p[0] * scale,
                         centre.y - (float) p[1] * scale };
            };

            for (int pass = 0; pass < 2; ++pass)
            {
                for (const auto& entry : lines)
                {
                    const auto& key  = entry.first;
                    const auto& info = entry.second;

                    if (info.highlighted != (pass == 1))
                        continue;

                    g.setColour (info.highlighted ? juce::Colours::black.withAlpha (0.8f)
                                                  : juce::Colours::black.withAlpha (0.2f));

                    g.drawLine (juce::Line<float> (toPixel (key.first), toPixel (key.second)),
                                info.highlighted ? 1.0f : 1.0f); //Change thickness of lines
                }
            }

            g.setColour (juce::Colours::black);

            for (const auto& entry : positionMap)
            {
                const auto p = toPixel (entry.first);

                g.fillEllipse (p.x - diameter * 0.5f,
                               p.y - diameter * 0.5f,
                               diameter, diameter);
            }
        }
    }
};