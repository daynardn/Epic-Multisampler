#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_formats/juce_audio_formats.h>

// Handles importing and generating wavetables, as well as generating inbetwen wavetables

class WaveTableManager
{
public:
    WaveTableManager();
    ~WaveTableManager();

    const float * generate_wavetables(std::unordered_map<int, juce::AudioSampleBuffer> imported_wavetables, int index);

    std::unordered_map<int, juce::AudioSampleBuffer> generated_wavetables;
private:
};