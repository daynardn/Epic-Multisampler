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

// CSP Create Task Procedure
const float * WaveTableManager::generate_wavetables(std::unordered_map<int, juce::AudioSampleBuffer> imported_wavetables, int index) {
    // If the index is not -1, change the  
    if (index != -1) {
        if (generated_wavetables.count(index) > 0) {
            return generated_wavetables[index].getReadPointer(0);
        }

        return nullptr;
    }

    int last_sample_index = -1;
    
    // Iterate on all possible notes
    for (int sample_index = 0; sample_index < 127; sample_index++) {
        // If we have no wavetable for the current note do not update the index 
        if (imported_wavetables.count(sample_index) != 0) {
            last_sample_index = sample_index;
        }
        
        // Check if we have a wavetable
        if (last_sample_index != -1) {
            // Get the number of samples in the last valid wavetable
            int sample_number = imported_wavetables[last_sample_index].getNumSamples();

            // Set our new generated wavetable to the contents of the inputted one 
            generated_wavetables[sample_index].setSize(1, sample_number);
            generated_wavetables[sample_index].copyFrom(0, 0, imported_wavetables[last_sample_index].getReadPointer(0), sample_number);
            
            // Pitch shift the sample by the number of semitone difference to make different notes

            // one semitone is 12th root of 2 faster so ~ 1.05946x faster per semitone
            // so a simple pitch shift could just sample everything faster
            double speedupFac = powf(1.05946, (sample_index - last_sample_index)); // 5% faster twice is not 10% faster

            float *writePtr = generated_wavetables[sample_index].getWritePointer(0);

            // All the samples in the inputted wavetable are iterated over
            for (auto i = 0; i < sample_number; ++i) {
                // We select the index of the array based on the sample index
                writePtr[i] = writePtr[(int) fmin((double) (i * speedupFac), (double)sample_number)];
                if ((double) (i * speedupFac) > (double)sample_number) {
                    writePtr[i] = 0;
                }
            }

            // remove clicks (0 crossings)
            generated_wavetables[sample_index].applyGainRamp(0, (int)fmin(10.0, (double)sample_number), 0.0, 1.0);
            generated_wavetables[sample_index].applyGainRamp((int)fmax(0.0, (double)sample_number - 10.0), (int)fmin(10.0, (double)sample_number), 1.0, 0.0);
        }

    }

    return nullptr;
}