#include "Connector.h"

Connector::Connector(juce::AudioProcessorGraph& g, Pedal* s, int channel) : g(g) {
    start = {start, channel};
}

// Inherited from juce::Component.
void Connector::mouseDown(const juce::MouseEvent& e) {

}

// Inherited from juce::Component.
void Connector::mouseDrag(const juce::MouseEvent& e) {

}

// Inherited from juce::Component.
void Connector::mouseUp(const juce::MouseEvent& e) {

}

// Inherited from juce::Component.
void Connector::paint(juce::Graphics& g) {
    cablePath.clear();
}

/**
 * attemptConnection():
 * 
 * Checks if the current mouse event is within range of a Pedal's input port.
 * 
 * If so, this function makes a connection by updating the Connector, the Pedal, and the
 * AudioProcessorGraph.
 * 
 * Otherwise, nothing happens.
 */
void Connector::attemptConnection(const juce::MouseEvent& e) {
    juce::AudioProcessorGraph::Node* startNode = g.getNodeForId(start.pedal->uid);

    for (juce::AudioProcessorGraph::Node* pedalNode : g) {
        Pedal* ped = static_cast<Pedal*>(pedalNode.getProcessor());

        for (int channel = 0; channel < ped->getNumInputChannels(); channel++) {
            Connection potentialConnection = {{startNode, start.channel}, {pedalNode, channel}};

            if (ped->getGlobalPositionForInputChannel(channel).getDistanceFrom(getPosition() + e.getPosition()) <= MAX_CONNECTION_RANGE && g.canConnect(potentialConnection)) {
                end = {ped, channel};
                ped->connectToInputPort(this);
                g.addConnection(potentialConnection);

                return;
            }
        }
    }
}