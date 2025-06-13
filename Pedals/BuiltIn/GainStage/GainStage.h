#pragma once

#include "../Source/Pedal.h"

#include <iostream> // debug
#include <cmath>

class GainStage : public Pedal {
    public:
        GainStage(PedalJUCEAudioProcessorEditor* e);

        virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        virtual void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;

        virtual void paint(juce::Graphics& g) override;

    private:
        juce::Slider dbSlider;
        juce::dsp::Gain<float> gainControl;

        std::string name = "Gain Stage";
};

GainStage::GainStage(PedalJUCEAudioProcessorEditor* e) : Pedal(e, 100, 300, 30) {
    dbSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    dbSlider.setRange(juce::Range<double>(-24.0f, 24.0f), 0.01);
    dbSlider.setValue(0.0f);

    dbSlider.setBounds(10, 10 + 2 * nameFontHeight + 10, width - 10, height - 30 - 2 * nameFontHeight - pedalThickness);
    addAndMakeVisible(dbSlider);
}
void GainStage::prepareToPlay(double sampleRate, int maximumExpectedSamplesPerBlock) {
    prepareBasics(sampleRate, maximumExpectedSamplesPerBlock);

    juce::dsp::ProcessSpec spec{sampleRate, static_cast<juce::uint32>(maximumExpectedSamplesPerBlock), 2};
    gainControl.prepare(spec);
}

void GainStage::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    gainControl.setGainDecibels(dbSlider.getValue());

    float* p = buffer.getWritePointer(0);
    for (int i = 0; i < buffer.getNumSamples(); i++)
        p[i] = gainControl.processSample(p[i]);
}

void GainStage::paint(juce::Graphics& g) {
    g.setColour(juce::Colour(0.0f, 0.0f, 0.5f, 1.0f)); // sides
    g.fillRoundedRectangle(0, 0, width, height, 20);

    g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f)); // recessed top part
    g.fillRoundedRectangle(0, 0, width, height - pedalThickness, 20);

    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawFittedText(name, 0, 5, width, nameFontHeight * 3, juce::Justification::centred, 2);
}
