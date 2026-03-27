#include "WaveTable.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

// Based on JUCE template project

//==============================================================================
AudioPluginAudioProcessor::AudioPluginAudioProcessor()
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
{
    wavetableHandler = new WaveTableManager();
}

AudioPluginAudioProcessor::~AudioPluginAudioProcessor()
{
}

//==============================================================================
const juce::String AudioPluginAudioProcessor::getName() const
{
    // This should be set but as a failsafe
    #ifndef JucePlugin_Name
        #define JucePlugin_Name "Juce Plugin"
    #endif
    
    return JucePlugin_Name;
}

bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    current_sample_rate = sampleRate;
}

void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

bool AudioPluginAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // clears empty output channels (no input)
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    juce::AudioPlayHead * playhead = getPlayHead();

    juce::AudioPlayHead::PositionInfo time_info = playhead->getPosition().orFallback(juce::AudioPlayHead::PositionInfo());
    
    double angle_delta = (440.0 / current_sample_rate) * 2.0 * juce::MathConstants<double>::pi;

    for (auto data : midiMessages) {
        juce::MidiMessage message = data.getMessage();
        
        if (message.isNoteOn()) {
            playing_messages[message.getNoteNumber()] = message;
            phases[message.getNoteNumber()] = 0.0;
            // waveTable.clear();
        }else if (message.isNoteOff()) {
            playing_messages.erase(message.getNoteNumber());
        }
    }

    auto* channelLeftData = buffer.getWritePointer (0);
    auto* channelRightData = buffer.getWritePointer (1);
    std::unordered_map<int, const float *> waveTablePtrs;
    for (auto message : playing_messages) {
        waveTablePtrs[message.first] = wavetableHandler->generate_wavetables(note_wavetables, message.first);
    }

    size_t active_notes = playing_messages.size();

    for (auto message : playing_messages) {
        if (note_wavetables.count(message.first) == 0) {
            continue;
        }
        for (auto i = 0; i < buffer.getNumSamples(); ++i) {
            // trapezoidal estimation
            double spline = 
                waveTablePtrs[message.first][int(ceil(phases[message.first]))] - 
                waveTablePtrs[message.first][int(floor(phases[message.first]))];

            double intpart;
            spline *= modf(phases[message.first], &intpart);
            spline += waveTablePtrs[message.first][int(floor(phases[message.first]))];

            channelLeftData[i] +=
                spline;
            channelRightData[i] +=
                spline;

            phases[message.first] += 1 * (sample_rate / current_sample_rate);
            if (phases[message.first] > note_wavetables[message.first].getNumSamples()) {
                phases[message.first] -= note_wavetables[message.first].getNumSamples();
            }
        }
    }
}

const float *AudioPluginAudioProcessor::requestWavetable(int index) {
    return note_wavetables[index].getReadPointer(0);
}
int AudioPluginAudioProcessor::requestWavetableLen(int index) {
    return note_wavetables[index].getNumSamples();
}

//==============================================================================
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

void AudioPluginAudioProcessor::addSample(juce::File sample, int index) {
    juce::AudioFormatManager formatManager;
    formatManager.registerBasicFormats();

    auto* reader = formatManager.createReaderFor(sample);
    juce::AudioBuffer<float> audioBuffer;
    audioBuffer.setSize(reader->numChannels, reader->lengthInSamples);
    reader->read(&audioBuffer, 0, reader->lengthInSamples, 0, true, true);
    sample_rate = reader->sampleRate;
    delete reader;

    note_wavetables[index] = audioBuffer;

    wavetableHandler->generate_wavetables(note_wavetables, -1);
}

//==============================================================================
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}