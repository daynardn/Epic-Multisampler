#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include "WaveTable.h"

// Based on JUCE template project
// This project is appropriately licenced as GNU as per JUCE usage guidelines

//==============================================================================
class AudioPluginAudioProcessor final : public juce::AudioProcessor
{
public:
    //==============================================================================
    
    // from the JUCE template

    AudioPluginAudioProcessor();
    ~AudioPluginAudioProcessor() override;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    // Student Developed
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    using AudioProcessor::processBlock;


    // from the JUCE template
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

    // Student Developed Functions
    const float *requestWavetable(int index);
    int requestWavetableLen(int index);
    void addSample(juce::File sample, int index); 
   
private:
    //==============================================================================
    // from the JUCE template
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessor)

    // pitch to message
    // Student Developed
    std::unordered_map<int, juce::MidiMessage> playing_messages;
    double current_sample_rate;
    std::unordered_map<int, double> phases;
    std::unordered_map<int, juce::AudioSampleBuffer> note_wavetables;
    double sample_rate;
    WaveTableManager *wavetableHandler;
};