// Handles importing and generating wavetables, as well as generating inbetwen wavetables
#include "WaveTable.h"

WaveTableManager::WaveTableManager()
{
}

WaveTableManager::~WaveTableManager()
{
}

const float * WaveTableManager::generate_wavetables(std::unordered_map<int, juce::AudioSampleBuffer> imported_wavetables, int index) {
    if (index != -1) {
        if (generated_wavetables.count(index) > 0) {
            return generated_wavetables[index].getReadPointer(0);
        }

        return nullptr;
    }

    int last_sample_index = -1;
    
    for (int sample_index = 0; sample_index < 127; sample_index++) {
        // This is wasteful but fine since there's only 127 possibilities
        if (imported_wavetables.count(sample_index) != 0) {
            last_sample_index = sample_index;
        }
        
        if (last_sample_index != -1) {
            generated_wavetables[sample_index] = imported_wavetables[last_sample_index];
        }

    }

    return nullptr;
}