#pragma once

#include <JuceHeader.h>

/**
 * This class provides useful DSP wrappers or extensions for PedalJUCE that are not themselves
 * included in JUCE.
 */
class DSPExtras {
    public:
        static void resample(juce::AudioBuffer<float>& buffer, double currentSr, double newSr);
        static float quantize(float value, float step);
};

/**
 * Resamples all channels of a given audio buffer to a new sample rate.
 */
void DSPExtras::resample(juce::AudioBuffer<float>& buffer, double currentSr, double newSr) {
    if (currentSr == newSr)
        return;
    
    double ratio = newSr / currentSr;

    // Storing the results of the resample in temp
    juce::AudioBuffer<float> temp;
    temp.clear();
    temp.setSize(buffer.getNumChannels(), static_cast<int>((static_cast<double>(buffer.getNumSamples()) / ratio)));
    
    juce::Interpolators::Lagrange interp;

    auto ins = buffer.getArrayOfReadPointers();
    auto outs = temp.getArrayOfWritePointers();

    for (int i = 0; i < buffer.getNumChannels(); i++) {
        interp.reset();
        interp.process(ratio, ins[i], outs[i], temp.getNumSamples());
    }

    buffer = temp; // copies over the results back to the original buffer
}

/**
 * Quantizes a floating point value to the nearest multiple of a given step size.
 */
float DSPExtras::quantize(float value, float step) {
    return step * static_cast<int>(value / step + 0.5);
}