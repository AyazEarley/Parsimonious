/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include <unordered_map>
#include <array>
//==============================================================================
/**
*/
class ParsimoniousAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    ParsimoniousAudioProcessor();
    ~ParsimoniousAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    std::vector<std::array<int, 4>> getChords ();
    std::vector<std::array<int, 3>> getTriads ();
    double getHostTempo();
    juce::File createMidiFile (std::vector<std::array<int, 4>> chords); 
    juce::File createMidiFileTriads (std::vector<std::array<int, 3>> chords); 

    juce::AudioProcessorValueTreeState apvts;

    int seededRandom (int lo, int hi);
    
private:
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ParsimoniousAudioProcessor)

    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    std::atomic<float>* barsParam = nullptr;
    std::atomic<float>* chordsPerBarParam = nullptr;
    std::atomic<float>* useTriadsParam = nullptr;

    std::atomic<float>* userSeedParam = nullptr;
    uint32_t currentSeed = 200; 

    static constexpr int MAJOR7 = 1;
    static constexpr int MINOR7 = 2;
    static constexpr int DOM7 = 3;
    static constexpr int HALF7 = 4;
    static constexpr int FULL7 = 5;

    static constexpr int MAJOR_TRIAD = 6;
    static constexpr int MINOR_TRIAD = 7;


    static constexpr std::array<std::array<int, 2>, 4> majOptions
    {{
        {{ 9, MINOR7}},
        {{ 4, MINOR7}},
        {{ 0, MINOR7}},
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

    std::unordered_map<int, std::array<int, 4>> chordIntervals {
        { MAJOR7, { 0, 4, 7, 11 } },
        { MINOR7, { 0, 3, 7, 10 } },
        { DOM7,   { 0, 4, 7, 10 } },
        { HALF7,  { 0, 3, 6, 10 } },
        { FULL7,  { 0, 3, 6, 9 } },
    };

    std::unordered_map<int, std::array<int, 3>> triadIntervals {
        { MAJOR_TRIAD, { 0, 4, 7} },
        { MINOR_TRIAD, { 0, 3, 7} },
    };

    static constexpr std::array<std::array<int, 2>, 4> majTriadOptions
    {{
        {{ 4, MINOR_TRIAD}},
        {{ 9, MINOR_TRIAD}},
        {{ 0, MINOR_TRIAD}},
    }};

    static constexpr std::array<std::array<int, 2>, 4> minTriadOptions
    {{
        {{ 3, MAJOR_TRIAD}},
        {{ 8, MAJOR_TRIAD}},
        {{ 0, MAJOR_TRIAD}},
    }};
    


};
