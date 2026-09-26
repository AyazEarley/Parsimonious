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
    useTriadsParam = apvts.getRawParameterValue ("useTriads");
    userSeedParam = apvts.getRawParameterValue ("userSeed");
    forceLoopParam = apvts.getRawParameterValue ("forceLoop");

    cleanupOldMidiFiles();
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

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "useTriads", 1 },
        "useTriads",
        false));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "userSeed", 1 },
        "userSeed",
        1, 999, 7));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "forceLoop", 1 },
        "forceLoop",
        false));

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
    buffer.clear();
}

void ParsimoniousAudioProcessor::cleanupOldMidiFiles (int maxAgeInSeconds)
{
    juce::File tempDir = juce::File::getSpecialLocation (juce::File::tempDirectory);

    juce::Array<juce::File> files;
    tempDir.findChildFiles (files, juce::File::findFiles, false, "GeneratedChords_*.mid");

    juce::int64 now = juce::Time::currentTimeMillis();

    for (auto& f : files)
    {
        juce::int64 age = now - f.getLastModificationTime().toMilliseconds();
        if (age > (juce::int64) maxAgeInSeconds * 1000)
            f.deleteFile();
    }
}

int ParsimoniousAudioProcessor::seededRandom(int lo, int hi) {
    currentSeed = currentSeed * 1664525u + 1013904223u;
    uint32_t range = (uint32_t)(hi - lo + 1);
    return lo + (int)((currentSeed >> 16) % range);
}

int makeState (int quality, int root)
{
    return (quality - 1) * 12 + root;
}

int stateQuality (int state) { return state / 12 + 1; }
int stateRoot    (int state) { return state % 12; }

int ParsimoniousAudioProcessor::makeStateTriad (int quality, int root) const
{
    int localQuality = (quality == MAJOR_TRIAD) ? 0 : 1;
    return localQuality * 12 + root;
}

int ParsimoniousAudioProcessor::stateQualityTriad (int state) const
{
    return (state / 12 == 0) ? MAJOR_TRIAD : MINOR_TRIAD;
}

int ParsimoniousAudioProcessor::stateRootTriad (int state) const
{
    return state % 12;
}

std::vector<int> ParsimoniousAudioProcessor::neighbors (int state)
{
    int quality = stateQuality (state);
    int root    = stateRoot (state);

    std::vector<int> result;

    if (quality == MAJOR7)
        for (auto& opt : majOptions)
            result.push_back (makeState (opt[1], (root + opt[0]) % 12));
    else if (quality == MINOR7)
        for (auto& opt : minOptions)
            result.push_back (makeState (opt[1], (root + opt[0]) % 12));
    else if (quality == DOM7)
        for (auto& opt : domOptions)
            result.push_back (makeState (opt[1], (root + opt[0]) % 12));
    else if (quality == HALF7)
        for (auto& opt : halfOptions)
            result.push_back (makeState (opt[1], (root + opt[0]) % 12));
    else 
        for (auto& opt : fullOptions)
            result.push_back (makeState (opt[1], (root + opt[0]) % 12));

    return result;
}

std::vector<std::vector<bool>> ParsimoniousAudioProcessor::buildCanReach (int home, int maxSteps)
{
    std::vector<std::vector<bool>> canReach (maxSteps + 1, std::vector<bool> (60, false));

    canReach[0][home] = true;   // base case: can_reach[0] = {home}

    for (int r = 1; r <= maxSteps; ++r)
    {
        for (int node = 0; node < 60; ++node)
        {
            for (int n : neighbors (node))
            {
                if (canReach[r - 1][n])
                {
                    canReach[r][node] = true;
                    break;
                }
            }
        }
    }

    return canReach;
}

std::vector<int> ParsimoniousAudioProcessor::generateLoop (int start, int nSteps, const std::vector<std::vector<bool>>& canReach)
{
    std::vector<int> sequence { start };
    int current = start;


    for (int i = 1; i <= nSteps; ++i)
    {
        int remainingAfter = nSteps - i;

        std::vector<int> options;
        for (int next : neighbors (current))
            if (canReach[remainingAfter][next])
                options.push_back (next);

        current = options[seededRandom(0, options.size() - 1)];
        sequence.push_back (current);
    }

    return sequence;
}

std::vector<int> ParsimoniousAudioProcessor::neighborsTriad (int state)
{
    int quality = stateQualityTriad (state);
    int root    = stateRootTriad (state);

    std::vector<int> result;

    if (quality == MAJOR_TRIAD)
        for (auto& opt : majTriadOptions)
            result.push_back (makeStateTriad (opt[1], (root + opt[0]) % 12));
    else
        for (auto& opt : minTriadOptions)
            result.push_back (makeStateTriad (opt[1], (root + opt[0]) % 12));

    return result;
}

std::vector<std::vector<bool>> ParsimoniousAudioProcessor::buildCanReachTriads (int home, int maxSteps)
{
    const int stateSpace = 24; // 2 qualities x 12 roots

    std::vector<std::vector<bool>> canReach (maxSteps + 1, std::vector<bool> (stateSpace, false));

    canReach[0][home] = true;

    for (int r = 1; r <= maxSteps; ++r)
    {
        for (int node = 0; node < stateSpace; ++node)
        {
            for (int n : neighborsTriad (node))
            {
                if (canReach[r - 1][n])
                {
                    canReach[r][node] = true;
                    break;
                }
            }
        }
    }

    return canReach;
}

std::vector<int> ParsimoniousAudioProcessor::generateLoopTriads (int start, int nSteps, const std::vector<std::vector<bool>>& canReach)
{
    std::vector<int> sequence { start };
    int current = start;

    for (int i = 1; i <= nSteps; ++i)
    {
        int remainingAfter = nSteps - i;

        std::vector<int> options;
        for (int next : neighborsTriad (current))
            if (canReach[remainingAfter][next])
                options.push_back (next);

        current = options[seededRandom (0, options.size() - 1)];
        sequence.push_back (current);
    }

    return sequence;
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

    int userSeedTemp = static_cast<int> (userSeedParam->load());
    currentSeed = userSeedTemp;

    std::vector<int> choices = {MAJOR7, MINOR7, DOM7, HALF7, FULL7};

    int randomIndex = seededRandom(0, choices.size() - 1);

    int quality = choices[randomIndex];

    int root = seededRandom(0,11);
    
    if(static_cast<bool> (forceLoopParam->load())){
        int home = makeState(quality, root);
        std::vector<std::vector<bool>> canReach = buildCanReach(home, numChords);
        std::vector<int> sequence = generateLoop (home, numChords, canReach);
        sequence.pop_back();

        std::vector<std::array<int, 4>> retChords;
        for(int i = 0; i < sequence.size(); i++){
            int state = sequence[i];
            int quality = stateQuality(state);
            int root = stateRoot(state);

            std::array<int, 4> pitches;
            for(int j = 0; j < 4; j++){
                int pitch = chordIntervals.at(quality)[j];
                pitch = (pitch + root) % 12;
                pitches[j] = pitch;
            }
            retChords.push_back(pitches);
        }
        return retChords;
    }

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
            index = seededRandom(0, 3);
            newRoot = (lastRoot + majOptions[index][0]) % 12;
            newQuality = majOptions[index][1];
        }
        else if (lastQuality == MINOR7){
            index = index = seededRandom(0, 3);
            newRoot = (lastRoot + minOptions[index][0]) % 12;
            newQuality = minOptions[index][1];
        }
        else if (lastQuality == DOM7){
            index = index = seededRandom(0, 3);
            newRoot = (lastRoot + domOptions[index][0]) % 12;
            newQuality = domOptions[index][1];
        }
        else if (lastQuality == HALF7){
            index = index = seededRandom(0, 2);
            newRoot = (lastRoot + halfOptions[index][0]) % 12;
            newQuality = halfOptions[index][1];
        }
        else{
            std::uniform_int_distribution<int> distrib(0, 7);
            index = index = seededRandom(0, 7);
            newRoot = (lastRoot + fullOptions[index][0]) % 12;
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


std::vector<std::array<int, 3>> ParsimoniousAudioProcessor::getTriads (){
    int bars = static_cast<int> (barsParam->load());
    int chordsPerBar = static_cast<int> (chordsPerBarParam->load());

    int numChords = bars * chordsPerBar;

    bool forceLoop = static_cast<bool> (forceLoopParam->load());

    if (forceLoop && numChords % 2 != 0)
        return {};

    int userSeedTemp = static_cast<int> (userSeedParam->load());
    currentSeed = userSeedTemp;

    std::vector<int> choices = {MAJOR_TRIAD, MINOR_TRIAD};

    int randomIndex = seededRandom(0, choices.size() - 1);

    int quality = choices[randomIndex];

    int root = seededRandom(0,11);

    if (forceLoop)
    {
        int home = makeStateTriad (quality, root);
        std::vector<std::vector<bool>> canReach = buildCanReachTriads (home, numChords);
        std::vector<int> sequence = generateLoopTriads (home, numChords, canReach);
        sequence.pop_back();

        std::vector<std::array<int, 3>> retChords;
        for (int i = 0; i < (int) sequence.size(); i++)
        {
            int state = sequence[i];
            int q = stateQualityTriad (state);
            int r = stateRootTriad (state);

            std::array<int, 3> intervals = triadIntervals.at (q);
            for (int j = 0; j < 3; j++)
                intervals[j] = (intervals[j] + r) % 12;

            std::sort (intervals.begin(), intervals.end());
            retChords.push_back (intervals);
        }
        return retChords;
    }

    std::array<int, 2> initialChord = {root, quality};
    std::vector<std::array<int, 2>> chordSequence;
    chordSequence.push_back(initialChord);
    
    for(int i = 0; i < numChords - 1; i++){
        int lastQuality = chordSequence.back()[1];
        int lastRoot = chordSequence.back()[0];
        
        int newQuality = MAJOR_TRIAD; //Placeholder
        int newRoot = 0;

        int index = 0;
        if (lastQuality == MAJOR_TRIAD){
            index = seededRandom(0,2);
            newRoot = (lastRoot + majTriadOptions[index][0]) % 12;
            newQuality = majTriadOptions[index][1];
        }
        else{
            index = seededRandom(0,2);
            newRoot = (lastRoot + minTriadOptions[index][0]) % 12;
            newQuality = minTriadOptions[index][1];
        }
        chordSequence.push_back({newRoot, newQuality});
    }

    std::vector<std::array<int, 3>> retChords;
    for(int i = 0; i <numChords; i++){
        std::array<int, 3> intervals = triadIntervals.at(chordSequence[i][1]);
        for(int j = 0; j < 3; j++){
            intervals[j] = (intervals[j] + chordSequence[i][0]) % 12;
        }
        std::sort(intervals.begin(), intervals.end());
        retChords.push_back(intervals);
    }
    return retChords;
}

juce::File ParsimoniousAudioProcessor::createMidiFileTriads (std::vector<std::array<int, 3>> chords)
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
        for (int j = 0; j < 3; j++)
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