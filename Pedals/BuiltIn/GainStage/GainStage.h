#pragma once

#include "../Source/Pedal.h"

#include <cmath>

class GainStage : public Pedal {
    public:
        GainStage(juce::AudioProcessorGraph* g, int editorW, int editorH);

        virtual void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override;
        virtual bool acceptsMidi() const override { return false; }
        virtual bool producesMidi() const override { return false; }

        virtual void paint(juce::Graphics& g) override;

    private:
        juce::Slider dbSlider;

        int numOutputChannels = 1;
        int numInputChannels = 1;

        std::string name = "Gain Stage";
        int width = 100;
        int height = 300;
};

GainStage::GainStage(juce::AudioProcessorGraph* g, int editorW, int editorH) : Pedal(g, editorW, editorH) {
    dbSlider = juce::Slider();
    dbSlider.setStyle(juce::Slider::SliderStyle::LinearVertical);
    dbSlider.setRange(juce::Range<double>(-24.0f, 24.0f), 1);
    dbSlider.setValue(0.0f);

    dbSlider.setBounds(width * 0.5 - 10, nameFontHeight, 20, height - nameFontHeight);
    addAndMakeVisible(dbSlider);
}

void GainStage::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) {
    float fac = std::pow(10, dbSlider.getValue() / 20.0f);

    float* p = buffer.getWritePointer();
    for (int i = 0; i < buffer.getNumSamples(); i++)
        p[i] *= fac;
}

void GainStage::paint(juce::Graphics& g) {
    g.setColour(juce::Colour(0.0f, 0.0f, 0.5f, 1.0f)); // sides
    g.fillRoundedRectangle(0, 0, width, height, 20);

    g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f)); // recessed top part
    g.fillRoundedRectangle(0, 0, width, height - pedalThickness, 20);

    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(name, 0, (int) (0.5 * (height - pedalThickness)) - nameFontHeight, width, nameFontHeight, juce::Justification::centred, true);
}