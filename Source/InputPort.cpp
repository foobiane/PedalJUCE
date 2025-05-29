#include "Pedal.h"

InputPort::InputPort() {
    /* Nothing for now. */
}

void InputPort::setIncomingConnector(Connector* c) {
    incomingConnector = c;
}

Connector* InputPort::getIncomingConnector() {
    return incomingConnector;
}

void InputPort::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::red);
    g.fillEllipse(getBounds().toFloat());
}

void InputPort::resized() {
    /* Nothing for now. Our bounds should never change. */
}