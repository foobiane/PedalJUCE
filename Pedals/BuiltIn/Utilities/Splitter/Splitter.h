#pragma once

#include <JuceHeader.h>

#include "../../../../Source/Pedal.h"

#include <cstring>

// TODO: Implement the rest of me!

class Splitter : public Pedal {
    public:
        Splitter(PedalJUCEAudioProcessorEditor* e) : Pedal(e) {  }
        virtual void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;
};

void Splitter::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    buffer.setSize(2, buffer.getNumSamples());
    buffer.copyFrom(1, 0, buffer, 0, 0, buffer.getNumSamples()); // copies samples from channel 0 to channel 1
}