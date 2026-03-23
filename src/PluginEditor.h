#pragma once

#include "PluginProcessor.h"

// Based on JUCE template project

//==============================================================================
class AudioPluginAudioProcessorEditor final : public juce::AudioProcessorEditor
{
public:
    explicit AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor&);
    ~AudioPluginAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    AudioPluginAudioProcessor& processorRef;
    std::unique_ptr<juce::FileChooser> sampleSelector;
    juce::TextButton loadNewSampleButtonE4 { "Load New Sample E4" };
    juce::PopupMenu noteInput = {};
    juce::PopupMenu octaveInput = {};
    int selectedPitch = 0; // A=1 Bb=2 etc etc
    int selectedOctave = 0; // C0 C1 etc etc
    juce::TextButton noteSetterButton { "Change Pitch" };
    juce::TextButton octaveSetterButton { "Change Octave" };
    juce::TextEditor sampleLabel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AudioPluginAudioProcessorEditor)
};