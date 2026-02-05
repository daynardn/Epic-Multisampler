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

    sampleSelector = std::make_unique<juce::FileChooser> ("Please select the file you want to load...",
                                               juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                                               "*.wav");
 
    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

    // loadNewSampleButton.setTopRightPosition(100, 100);
    // loadNewSampleButton.setSize(100, 100);
 
    addAndMakeVisible (loadNewSampleButton);

    loadNewSampleButton.onClick = [this, folderChooserFlags]
    {
        sampleSelector->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
        {
            juce::File sample (chooser.getResult());
    
            juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::InfoIcon,
                            "Selected File!",
                            "Ok"
                        );

            processorRef.addSample(sample);

            
        });
    };
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

    double wavetableWidth = getLocalBounds().getWidth() * .8;

    juce::FlexBox *uiBox = new juce::FlexBox(juce::FlexBox::JustifyContent::center);
    juce::FlexItem pianoSampler = juce::FlexItem(getLocalBounds().getWidth() * .2, 300);
    juce::FlexItem waveTable = juce::FlexItem(wavetableWidth, 100);

    uiBox->items.add(pianoSampler);
    uiBox->items.add(waveTable);
    uiBox->performLayout(
        getLocalBounds()
    );

    pianoSampler = uiBox->items[0];
    waveTable = uiBox->items[1];

    g.setColour(juce::Colours::antiquewhite);
    g.fillRect(pianoSampler.currentBounds);

    const float *wavetable = processorRef.requestWavetable();

    size_t samples = processorRef.requestWavetableLen();
    float lineLength = waveTable.currentBounds.getWidth() / (float)(samples);
    float height = waveTable.currentBounds.getHeight();
    
    // if (samples > 1) {
    //     juce::Path path = juce::Path();
    //     for (size_t i = 0; i < samples - 2; i++) {
    //         g.setColour(juce::Colour::fromRGB(4, 3, 255));
    //         path.addLineSegment(
    //             juce::Line<float>(
    //                 lineLength * i, 
    //                 (((float)wavetable[i] + 1.0) / 2.0) * height, 
    //                 lineLength * (i+1), 
    //                 (((float)wavetable[i+1] + 1.0) / 2.0) * height
    //             ), 1);
    //     }
    //     path.scaleToFit(
    //         waveTable.currentBounds.getPosition().getX(), 
    //         waveTable.currentBounds.getPosition().getY(), 
    //         waveTable.currentBounds.getWidth(),
    //         waveTable.currentBounds.getHeight(), 
    //     false);

    //     g.strokePath(path, juce::PathStrokeType(1));
    // }


    delete(uiBox);
}

void AudioPluginAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    auto bounds = getLocalBounds();
    loadNewSampleButton.setBounds(bounds.removeFromRight(100));
}