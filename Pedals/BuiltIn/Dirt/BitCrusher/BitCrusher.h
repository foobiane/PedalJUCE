#pragma once

#include <JuceHeader.h>

#include "../../../../Source/Pedal.h"
#include "../../../../Source/DSPExtras.h"

#include <cstdint>

// TODO: Implement the rest of me!

class BitCrusher : public Pedal {
    public:
        BitCrusher(PedalJUCEAudioProcessorEditor* e) : Pedal(e) {  }
        virtual void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;

    private:
        double newSr;
        int resolution;
};

void BitCrusher::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    if (newSr != sr) // only resampling if necessary
        DSPExtras::resample(buffer, sr, newSr);

    float* p = buffer.getWritePointer(0);
    int n = buffer.getNumSamples();

    if (resolution != sizeof(float)) { // only quantizing if necessary
        for (int i = 0; i < n; i++)
            p[i] = DSPExtras::quantize(p[i], 1.0f / (resolution / 2));
    }
}