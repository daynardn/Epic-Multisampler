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
    // loadNewSampleButton.setSize(100, 100 );
    noteInput.addItem (1, "A");
    noteInput.addItem (2, "Bb");
    noteInput.addItem (3, "B");
    noteInput.addItem (4, "C");
    noteInput.addItem (5, "Db");
    noteInput.addItem (6, "D");
    noteInput.addItem (7, "Eb");
    noteInput.addItem (8, "E");
    noteInput.addItem (9, "F");
    noteInput.addItem (10, "Gb");
    noteInput.addItem (11, "G");
    noteInput.addItem (12, "Ab");

    octaveInput.addItem (1, "0"); // 0 is no inpit
    octaveInput.addItem (2, "1");
    octaveInput.addItem (3, "2");
    octaveInput.addItem (4, "3");
    octaveInput.addItem (5, "4");
    octaveInput.addItem (6, "5");
    octaveInput.addItem (7, "6");
    octaveInput.addItem (8, "7");
    octaveInput.addItem (9, "8");
    octaveInput.addItem (10, "9");
 
    addAndMakeVisible (loadNewSampleButtonE4);
    addAndMakeVisible (sampleLabel);
    addAndMakeVisible (noteSetterButton);
    addAndMakeVisible (octaveSetterButton);

    noteSetterButton.onClick = [this] {
        noteInput.showMenuAsync(juce::PopupMenu::Options(),
                         [this] (int result)
                         {
                             if (result == 0)
                             {
                                 //no input, don't update
                             }
                             else {
                                this->selectedPitch = result;
                             }
                         });
    };


    octaveSetterButton.onClick = [this] {
        octaveInput.showMenuAsync(juce::PopupMenu::Options(),
                         [this] (int result)
                         {
                             if (result == 0)
                             {
                                 //no input, don't update
                             }
                             else {
                                this->selectedOctave = result; // 1 is C0 since 12*1 = 12 = C0
                             }
                         });
    };


    loadNewSampleButtonE4.onClick = [this, folderChooserFlags]
    {
        sampleSelector->launchAsync (folderChooserFlags, [this] (const juce::FileChooser& chooser)
        {
            juce::File sample (chooser.getResult());
    
            juce::AlertWindow::showMessageBoxAsync(
                            juce::AlertWindow::InfoIcon,
                            "",
                            "Ok"
                        );
            
            // juce::String note = sampleLabel.getText().substring(0,1);
            // // C0 is 12 so add #*12 for each  
            // int note = 12 + 12 * sampleLabel.getText().substring(0,1)
            // this->selectedPitch -4 since A -> Bb -> B -> C
            processorRef.addSample(sample, (this->selectedPitch - 4) +(this->selectedOctave*12));

            
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

    // const float *wavetable = processorRef.requestWavetable();

    // size_t samples = processorRef.requestWavetableLen();
    // float lineLength = waveTable.currentBounds.getWidth() / (float)(samples);
    // float height = waveTable.currentBounds.getHeight();
    
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
    loadNewSampleButtonE4.setBounds(bounds.removeFromRight(100).removeFromTop(100));
    noteSetterButton.setBounds(bounds.removeFromRight(100).removeFromTop(100));
    octaveSetterButton.setBounds(
        bounds.removeFromRight(100).removeFromRight(100)
        .removeFromTop(100).removeFromTop(100));

    sampleLabel.setBounds(bounds.removeFromRight(100).removeFromRight(100));
}