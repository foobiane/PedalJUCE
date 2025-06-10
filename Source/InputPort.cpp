#include "Pedal.h"

InputPort::InputPort() {
    // Nothing for now.
}

InputPort::~InputPort() {
    // Nothing for now.
}

void InputPort::setIncomingConnector(Connector* c) {
    incomingConnector = c;
}

Connector* InputPort::getIncomingConnector() {
    return incomingConnector;
}

void InputPort::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::red);
    
    juce::Rectangle<int> bounds = getBounds();
    int width = bounds.getWidth(), height = bounds.getHeight();

    g.fillEllipse(0, 0, width, height);
}

void InputPort::resized() {
    /* Nothing for now. Our bounds should never change. */
}