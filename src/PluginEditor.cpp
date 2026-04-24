#include "PluginProcessor.h"
#include "PluginEditor.h"

// Based on JUCE template project
// The JUCE licencing agreement can be found here https://juce.com/juce-legal/
// The JUCE 8 licence can be found here https://juce.com/legal/juce-8-licence/

//Student Developed Procedure is contained within WaveTable.cpp (generate_wavetables)

// Input is in PluginEditor.cpp and is defined as the sampleSelector, called from the LoadNewSampleButtonE4 callback.

// Output is generated in the PluginProcessor.cpp file, the processBlock function as audio


// This project is appropriately licenced as GNU as per JUCE usage guidelines
// This program is an extension of the JUCE framework,
// All the sampling code is my own, however the overall structure is templated, 
// namely the AudioPluginAudioProcessor definitions 

//==============================================================================
AudioPluginAudioProcessorEditor::AudioPluginAudioProcessorEditor (AudioPluginAudioProcessor& p)
    : AudioProcessorEditor (&p), processorRef (p)
{
    juce::ignoreUnused (processorRef);
    
    // This is the size of the plugin window
    setSize (400, 300);

    sampleSelector = std::make_unique<juce::FileChooser> ("Please select the file you want to load...",
                                               juce::File::getSpecialLocation (juce::File::userHomeDirectory),
                                               "*.wav");
 
    auto folderChooserFlags = juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles;

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

    octaveInput.addItem (1, "0"); // 0 is no input
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

    // The user chooses a sample audio file when this button is pressed via the async sample selector window. 
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
            
            // C0 is 12 so add #*12 for each  
            // this->selectedPitch -4 since A -> Bb -> B -> C
            processorRef.addSample(sample, (this->selectedPitch - 4) + (this->selectedOctave * 12));

            
        });
    };
}

AudioPluginAudioProcessorEditor::~AudioPluginAudioProcessorEditor()
{
    
}

//==============================================================================
// This function is defined by juce, but the code is student developed
void AudioPluginAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    g.setColour (juce::Colours::white);
    g.setFont (15.0f);
    g.drawFittedText (note_text, getLocalBounds(), juce::Justification::centred, 1);

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

    delete(uiBox);
}

// Student developed
void AudioPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    loadNewSampleButtonE4.setBounds(bounds.removeFromRight(100).removeFromTop(100));
    noteSetterButton.setBounds(bounds.removeFromRight(100).removeFromTop(100));
    octaveSetterButton.setBounds(
        bounds.removeFromRight(100).removeFromRight(100)
        .removeFromTop(100).removeFromTop(100));

    sampleLabel.setBounds(bounds.removeFromRight(100).removeFromRight(100));
}
