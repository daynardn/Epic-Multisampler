#include "WaveTable.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"


// Based on JUCE template project
// This project is appropriately licenced as GNU as per JUCE usage guidelines

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
// This function is from the JUCE template
const juce::String AudioPluginAudioProcessor::getName() const
{
    // This should be set but as a failsafe
    #ifndef JucePlugin_Name
        #define JucePlugin_Name "Juce Plugin"
    #endif
    
    return JucePlugin_Name;
}

// This function is from the JUCE template
bool AudioPluginAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

// This function is from the JUCE template
bool AudioPluginAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

// This function is from the JUCE template
bool AudioPluginAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

// This function is from the JUCE template
double AudioPluginAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

// This function is from the JUCE template
int AudioPluginAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

// This function is from the JUCE template
int AudioPluginAudioProcessor::getCurrentProgram()
{
    return 0;
}

// This function is from the JUCE template
void AudioPluginAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

// This function is from the JUCE template
const juce::String AudioPluginAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

// This function is from the JUCE template
void AudioPluginAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
// This function is from the JUCE template
void AudioPluginAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    juce::ignoreUnused (sampleRate, samplesPerBlock);
    current_sample_rate = sampleRate;
}

// This function is from the JUCE template
void AudioPluginAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

// This function is from the JUCE template
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

// Student Created
void AudioPluginAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer,
                                              juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused (midiMessages);

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
        // Call to CSP Create Task Procedure
        const float* wavetable = wavetableHandler->generate_wavetables(note_wavetables, message.first);
        if (wavetable) {
            waveTablePtrs[message.first] = wavetable;
        }
    }

    size_t active_notes = playing_messages.size();

    for (auto message : playing_messages) {
        if (!wavetableHandler->containsIndex(message.first)) {
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
            spline *= message.second.getVelocity() / 127.0;

            // This code is the output of the program, placing samples into the buffer based 
            // on the inputted wavetables, `waveTablePtrs`
            // waveTablePtrs is the modified buffer with pitch shifting as per the create task function
            // `generate_wavetables`

            channelLeftData[i] +=
                spline;
            channelRightData[i] +=
                spline;

            phases[message.first] += 1 * (sample_rate / current_sample_rate);
            if (wavetableHandler->containsIndex(message.first)) { // redundant
                juce::AudioSampleBuffer wavetable = wavetableHandler->generated_wavetables[message.first];
                if (phases[message.first] > wavetable.getNumSamples()) {
                    phases[message.first] -= wavetable.getNumSamples();
                }
            }
        }
    }
}

// Student created
const float *AudioPluginAudioProcessor::requestWavetable(int index) {
    return note_wavetables[index].getReadPointer(0);
}
// Student created
int AudioPluginAudioProcessor::requestWavetableLen(int index) {
    return note_wavetables[index].getNumSamples();
}

//==============================================================================
// This function is from the juce template
bool AudioPluginAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

// This function is from the juce template
juce::AudioProcessorEditor* AudioPluginAudioProcessor::createEditor()
{
    return new AudioPluginAudioProcessorEditor (*this);
}

// Student Created
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

    // Call to CSP Create Task Procedure
    wavetableHandler->generate_wavetables(note_wavetables, -1);
}

//==============================================================================
// This function is from the JUCE template
void AudioPluginAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::ignoreUnused (destData);
}

// This function is from the JUCE template
void AudioPluginAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
// This creates new instances of the plugin..
// This function is from the JUCE template
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPluginAudioProcessor();
}