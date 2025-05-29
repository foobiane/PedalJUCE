#include "Pedal.h"

Connector::Connector(juce::AudioProcessorGraph* graph, Pedal* s, int channel) {
    g = graph;
    start = {s, channel};
    end = {nullptr, 0};

    resetBounds();
}

/**
 * resetBounds():
 *
 * Sets the boundaries of the component to a square encircling the starting port.
 */
void Connector::resetBounds() {
    juce::Point<int> startPoint = start.pedal->getGlobalPositionForOutputChannel(start.channel);
    juce::Rectangle<int> area(startPoint.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE), startPoint.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE));

    setBounds(area);
    repaint();
}

/**
 * adjustBounds():
 *
 * Sets the boundaries of the component to span the starting port to the most recent cursor point.
 */
void Connector::adjustBounds() {
    juce::Point<int> startPoint = start.pedal->getGlobalPositionForOutputChannel(start.channel);
    juce::Point<int> startPointAdjusted = juce::Point<int>(
        cursorPosition.x < startPoint.x ? startPoint.x + MAX_CONNECTION_RANGE : startPoint.x - MAX_CONNECTION_RANGE,
        cursorPosition.y < startPoint.y ? startPoint.y + MAX_CONNECTION_RANGE : startPoint.y - MAX_CONNECTION_RANGE
    ); // adjusting start point to include start ellipse

    juce::Rectangle<int> newBounds = juce::Rectangle<int>(startPointAdjusted, cursorPosition).expanded(4.0f);

    if (getBounds() != newBounds)
        setBounds(newBounds);
    else
        resized();

    repaint();
}

/**
 * isConnected():
 *
 * Returns whether or not the connector is connected to an endpoint. This abstracts away the
 * need to check the endpoint manually into a convenient call.
 */
bool Connector::isConnected() {
    return end.pedal != nullptr;    
}

/**
 * disconnect():
 *
 * Disconnects the Connector by removing the relevant connection from the AudioProcessorGraph,
 * untracking it from the end pedal, and setting the end to a nullptr.
 */
void Connector::disconnect() {
    if (isConnected()) {
        juce::AudioProcessorGraph::Connection c({start.pedal->getUIDAsNodeID(), start.channel}, {end.pedal->getUIDAsNodeID(), end.channel});
        g->removeConnection(c);

        end.pedal->untrackConnector(this);
        end = {nullptr, 0};
    }
}

// Inherited from juce::Component.
void Connector::mouseDown(const juce::MouseEvent& e) {
    if (!isConnected()) {
        dragging = true;
        cursorPosition = e.getPosition() + getPosition();

        adjustBounds();
    }
    
    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::mouseDrag(const juce::MouseEvent& e) {
    if (!isConnected()) {
        cursorPosition = e.getPosition() + getPosition();

        adjustBounds();
    }

    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::mouseUp(const juce::MouseEvent& e) {
    if (!isConnected()) {
        attemptConnection(e);

        if (!isConnected()) 
            resetBounds();
        else {
            cursorPosition = end.pedal->getGlobalPositionForInputChannel(end.channel); // TODO: Fix me
            adjustBounds();
        }

        dragging = false;
    }   

    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::yellow);

    juce::Point<int> startPoint = start.pedal->getGlobalPositionForOutputChannel(start.channel) - getPosition();
    juce::Rectangle<float> area(startPoint.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE).toFloat(), startPoint.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE).toFloat());
    g.fillEllipse(area);

    if (isConnected() || dragging)
        g.fillPath(cablePath);
}

// Inherited from juce::Component.
void Connector::resized() {
    // Note that we do all of this in resized() since this function is called during a boundary
    // change. Boundary changes only occur when we're dragging. 

    juce::Point<int> startPoint = start.pedal->getGlobalPositionForOutputChannel(start.channel) - getPosition();
    juce::Point<int> endPoint = cursorPosition - getPosition();

    cablePath.clear();
    cablePath.startNewSubPath(startPoint.toFloat());
    cablePath.cubicTo(
        startPoint.x, startPoint.y + (endPoint.y - startPoint.y) * 0.33f,
        endPoint.x, startPoint.y + (endPoint.y - startPoint.y) * 0.66f,
        endPoint.x, endPoint.y
    );

    juce::PathStrokeType wideStroke (8.0f);
    wideStroke.createStrokedPath(cablePath, cablePath);
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
    juce::AudioProcessorGraph::NodeID startNodeID = start.pedal->getUIDAsNodeID();

    for (juce::AudioProcessorGraph::Node* pedalNode : g->getNodes()) {
        Pedal* ped = static_cast<Pedal*>(pedalNode->getProcessor());

        for (int channel = 0; channel < ped->getNumInputChannels(); channel++) {
            juce::AudioProcessorGraph::Connection potentialConnection = {{startNodeID, start.channel}, {pedalNode->nodeID, channel}};

            if (ped->getGlobalPositionForInputChannel(channel).getDistanceFrom(cursorPosition) <= MAX_CONNECTION_RANGE && g->canConnect(potentialConnection)) {
                end = {ped, channel};
                ped->trackConnector(this); 
                g->addConnection(potentialConnection);

                return;
            }
        }
    }
}
