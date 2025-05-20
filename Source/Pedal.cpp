#include "Pedal.h"

juce::AudioBuffer<float>& Pedal::dsp(juce::AudioBuffer<float>& samples) {
    return samples;
}

Pedal::Pedal() {
    nameFont.setHeight(nameFontHeight);
    setSize(width, height);
}

Pedal::~Pedal() {
    // Nothing for now
}

unsigned Pedal::getPedalWidth() {
    return width;
}

unsigned Pedal::getPedalHeight() {
    return height;
}

void Pedal::paint(juce::Graphics& g) {
    // TODO: Enable color selection
    // TODO: Fonts, text
    // TODO: Cool details (shine, etc.)
    g.fillAll(juce::Colours::lightblue);
    
    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(name, 0, 10, width, 25, juce::Justification::centred, true);
}

void Pedal::resized() {
    // Nothing for now
}

void Pedal::mouseDown(const juce::MouseEvent& e) {
    drag.startDraggingComponent(this, e);
}

void Pedal::mouseDrag(const juce::MouseEvent& e) {
    drag.dragComponent(this, e, nullptr);
}