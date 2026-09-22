/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include <random>

//==============================================================================
ParsimoniousAudioProcessor::ParsimoniousAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ),
       apvts (*this, nullptr, "Parameters", createParameterLayout())
#endif
{
    barsParam = apvts.getRawParameterValue ("bars");
    chordsPerBarParam = apvts.getRawParameterValue ("chordsPerBar");


    
}

juce::AudioProcessorValueTreeState::ParameterLayout ParsimoniousAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "bars", 1 },
        "bars",
        1, 32, 1));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "chordsPerBar", 1 },
        "chordsPerBar",
        1, 8, 2));

    return { params.begin(), params.end() };
}

ParsimoniousAudioProcessor::~ParsimoniousAudioProcessor()
{
}

//==============================================================================
const juce::String ParsimoniousAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool ParsimoniousAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool ParsimoniousAudioProcessor::producesMidi() const
{
   return true;
}

bool ParsimoniousAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double ParsimoniousAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int ParsimoniousAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int ParsimoniousAudioProcessor::getCurrentProgram()
{
    return 0;
}

void ParsimoniousAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String ParsimoniousAudioProcessor::getProgramName (int index)
{
    return {};
}

void ParsimoniousAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void ParsimoniousAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void ParsimoniousAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool ParsimoniousAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void ParsimoniousAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
}

//==============================================================================
bool ParsimoniousAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* ParsimoniousAudioProcessor::createEditor()
{
    return new ParsimoniousAudioProcessorEditor (*this);
}

//==============================================================================
void ParsimoniousAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void ParsimoniousAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ParsimoniousAudioProcessor();
}

double ParsimoniousAudioProcessor::getHostTempo()
{
    if (auto* playHead = getPlayHead())
    {
        if (auto position = playHead->getPosition())
        {
            if (auto bpm = position->getBpm())
                return *bpm;
        }
    }

    return 120.0;
}


std::vector<std::array<int, 4>> ParsimoniousAudioProcessor::getChords (){
    int bars = static_cast<int> (barsParam->load());
    int chordsPerBar = static_cast<int> (chordsPerBarParam->load());

    int numChords = bars * chordsPerBar;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<int> choices = {MAJOR7, MINOR7, DOM7, HALF7, FULL7};
    std::uniform_int_distribution<std::size_t> dist(0, choices.size() - 1);
    std::size_t randomIndex = dist(gen);

    int quality = choices[randomIndex];


    std::uniform_int_distribution<int> distrib(0, 11);
    int root = distrib(gen);

    std::array<int, 2> initialChord = {root, quality};
    std::vector<std::array<int, 2>> chordSequence;
    chordSequence.push_back(initialChord);
    
    for(int i = 0; i < numChords - 1; i++){
        int lastQuality = chordSequence.back()[1];
        int lastRoot = chordSequence.back()[0];
        
        int newQuality = MAJOR7; //Placeholder
        int newRoot = 0;

        int index = 0; //pre declared for random number generation
        if (lastQuality == MAJOR7){
            std::uniform_int_distribution<int> distrib(0, 3);
            index = distrib(gen);
            newRoot = majOptions[index][0];
            newQuality = majOptions[index][1];
        }
        else if (lastQuality == MINOR7){
            std::uniform_int_distribution<int> distrib(0, 3);
            index = distrib(gen);
            newRoot = minOptions[index][0];
            newQuality = minOptions[index][1];
        }
        else if (lastQuality == DOM7){
            std::uniform_int_distribution<int> distrib(0, 3);
            index = distrib(gen);
            newRoot = domOptions[index][0];
            newQuality = domOptions[index][1];
        }
        else if (lastQuality == HALF7){
            std::uniform_int_distribution<int> distrib(0, 2);
            index = distrib(gen);
            newRoot = halfOptions[index][0];
            newQuality = halfOptions[index][1];
        }
        else{
            std::uniform_int_distribution<int> distrib(0, 7);
            index = distrib(gen);
            newRoot = fullOptions[index][0];
            newQuality = fullOptions[index][1];
        }
        chordSequence.push_back({newRoot, newQuality});
    }

    std::vector<std::array<int, 4>> retChords;
    for(int i = 0; i <numChords; i++){
        std::array<int, 4> intervals = chordIntervals.at(chordSequence[i][1]);
        for(int j = 0; j < 4; j++){
            intervals[j] = (intervals[j] + chordSequence[i][0]) % 12;
        }
        std::sort(intervals.begin(), intervals.end());
        retChords.push_back(intervals);
    }
    return retChords;
}

juce::File ParsimoniousAudioProcessor::createMidiFile (std::vector<std::array<int, 4>> chords)
{
    juce::MidiMessageSequence sequence;
    int chordsPerBar = static_cast<int> (chordsPerBarParam->load());

    const int ticksPerQuarterNote = 960;
    const int beatsPerBar = 4;

    jassert (chordsPerBar > 0);

    double ticksPerBar = ticksPerQuarterNote * beatsPerBar;
    double chordLength = ticksPerBar / chordsPerBar;

    double currentTime = 0.0;

    for (int i = 0; i < (int) chords.size(); i++)
    {
        for (int j = 0; j < 4; j++)
        {
            sequence.addEvent (juce::MidiMessage::noteOn  (1, chords[i][j] + 60, (juce::uint8) 100), currentTime);
            sequence.addEvent (juce::MidiMessage::noteOff (1, chords[i][j] + 60), currentTime + chordLength);
        }

        currentTime += chordLength;
    }

    sequence.updateMatchedPairs();
    sequence.sort();

    juce::MidiFile midiFile;
    midiFile.setTicksPerQuarterNote (ticksPerQuarterNote);
    midiFile.addTrack (sequence);

    juce::File tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory);
    juce::File midiOut = tempDir.getChildFile ("GeneratedChords_" + juce::String (juce::Time::currentTimeMillis()))
                                 .withFileExtension (".mid");

    if (auto stream = std::make_unique<juce::FileOutputStream> (midiOut))
    {
        stream->setPosition (0);
        stream->truncate();
        midiFile.writeTo (*stream);
    }

    return midiOut;
}