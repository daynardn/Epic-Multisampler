// Handles importing and generating wavetables, as well as generating inbetwen wavetables
#include "WaveTable.h"

WaveTableManager::WaveTableManager()
{
}

WaveTableManager::~WaveTableManager()
{
}

bool WaveTableManager::containsIndex(int index) {
    return generated_wavetables.count(index) != 0;
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
            int sample_number = imported_wavetables[last_sample_index].getNumSamples();
            generated_wavetables[sample_index].setSize(1, sample_number);
            generated_wavetables[sample_index].copyFrom(0, 0, imported_wavetables[last_sample_index].getReadPointer(0), sample_number);
            // one semitone is 12th root of 2 faster so ~ 1.05946x faster per semitone
            // so a simple pitch shift could just sample everything faster

            double speedupFac = powf(1.05946, (sample_index - last_sample_index)); // 5% faster twice is not 10% faster

            float *writePtr = generated_wavetables[sample_index].getWritePointer(0);

            for (auto i = 0; i < sample_number; ++i) {
                writePtr[i] = writePtr[(int) fmin((double) (i * speedupFac), (double)sample_number)];
            }

            // remove clicks (0 crossings)
            generated_wavetables[sample_index].applyGainRamp(0, (int)fmin(10.0, (double)sample_number), 0.0, 1.0);
            generated_wavetables[sample_index].applyGainRamp((int)fmax(0.0, (double)sample_number - 10.0), (int)fmin(10.0, (double)sample_number), 0.0, 1.0);
        }

    }

    return nullptr;
}