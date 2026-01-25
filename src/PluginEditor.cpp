#include "PluginProcessor.h"
#include "PluginEditor.h"

// Based on JUCE template project

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
}

//==============================================================================
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("HelloW orld", getLocalBounds(), juce::Justification::centred, 1);

    const float *wavetable = processorRef.requestWavetable();

    size_t samples = processorRef.requestWavetableLen();
    int lineLength = getLocalBounds().getWidth() / (samples);
    int height = getLocalBounds().getHeight();
    
    if (samples > 1) {
        for (size_t i = 0; i < samples - 2; i++) {
            g.setColour(juce::Colour::fromRGB(4, 3, 255));
            g.drawLine(lineLength * i, 
                ((wavetable[i] + 1.0) / 2.0) * height, 
                lineLength * (i+1), 
                ((wavetable[i+1] + 1.0) / 2.0) * height
            );
        }
    }
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}