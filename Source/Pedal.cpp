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

    g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 0.5f, 1.0f)); // sides
    g.fillRoundedRectangle(0, 0, width, height, 20);

    g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 0.75f, 1.0f)); // recessed top part
    g.fillRoundedRectangle(0, 0, width, height - 25, 20);

    // g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 1.0f, 1.0f)); // top pedal component
    // g.fillRoundedRectangle(0, (int) (0.5 * (height - 25)) - nameFontHeight, width, (int) (0.5 * (height - 25)) + nameFontHeight, 20);

    // g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 1.0f, 1.0f)); // top pedal component
    // g.fillRoundedRectangle(0, (int) (0.5 * (height - 25)) - nameFontHeight, width, (int) (0.5 * (height - 25)) + nameFontHeight, 20);
    
    // g.setColour(juce::Colour(0.0f, 0.0f, 0.2f, 1.0f)); // flat foot switch
    // g.fillRoundedRectangle(10, (int) (0.5 * (height - 25)), width - 20, (int) (0.5 * (height - 25)) - 10, 10);

    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(name, 0, (int) (0.5 * (height - 25)) - nameFontHeight, width, nameFontHeight, juce::Justification::centred, true);
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