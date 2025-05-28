#include "Connector.h"

Connector::Connector(juce::AudioProcessorGraph& g, Pedal* s, int channel) : g(g) {
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
    juce::Point<int> startPoint = start.pedal->getGlobalPositionForInputChannel(start.channel);
    juce::Rectangle<int> area(startPoint.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE), startPoint.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE));

    setBounds(area);
}

/**
 * adjustBounds():
 *
 * Sets the boundaries of the component to span the starting port to the most recent cursor point.
 */
void Connector::adjustBounds() {
    juce::Point<int> startPoint = start.pedal->getGlobalPositionForInputChannel(start.channel);
    juce::Rectangle<int> area(startPoint, cursorPosition.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE));

    setBounds(area);
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
 * Disconnects the connector by removing the relevant connection from the AudioProcessorGraph,
 * untracking it from the end pedal, and setting the end to a nullptr.
 */
void Connector::disconnect() {
    if (isConnected()) {
        juce::AudioProcessorGraph::Connection c = {{g.getNodeForId(start.pedal->uid), start.channel}, {g.getNodeForId(end.pedal->uid), end.channel}};
        g.removeConnection(c);

        end.pedal->untrackConnector(this);
        end = {nullptr, 0};
    }
}

// Inherited from juce::Component.
void Connector::mouseDown(const juce::MouseEvent& e) {
    if (!isConnected()) {
        dragging = true;
        cursorPosition = e.getPosition();
        adjustBounds();
        
        repaint();
    }
    
    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::mouseDrag(const juce::MouseEvent& e) {
    if (!isConnected()) {
        cursorPosition = e.getPosition();
        adjustBounds();

        repaint();
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
            cursorPosition = end.pedal->getGlobalPositionForInputChannel(end.channel);
            adjustBounds();
        }

        dragging = false;

        repaint();
    }   

    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::yellow);

    if (isConnected() || dragging)
        g.fillPath(cablePath);

    else {
        juce::Point<int> startPoint = start.pedal->getGlobalPositionForOutputChannel(start.channel) - getPosition();
        juce::Rectangle<int> area(startPoint.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE), startPoint.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE));
        g.fillEllipse(area);
    }
}

// Inherited from juce::Component.
void Connector::resized() {
    // Note that we do all of this in resized() since this function is called during a boundary
    // change. Boundary changes only occur when we're dragging.

    juce::Point<int> startPoint = start.pedal->getGlobalPositionForOutputChannel(start.channel) - getPosition();
    
    cablePath.clear();
    cablePath.startNewSubPath(startPoint);
    cablePath.cubicTo(
        startPoint.x, startPoint.y + (cursorPosition.y - startPoint.y) * 0.33f,
        cursorPosition.x, cursorPosition.y + (cursorPosition.y - startPoint.y) * 0.66f,
        cursorPosition.x, cursorPosition.y
    );

    PathStrokeType wideStroke (8.0f);
    wideStroke.createStrokedPath(cablePath, cablePath);
>
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
            juce::AudioProcessorGraph::Connection potentialConnection = {{startNode, start.channel}, {pedalNode, channel}};

            if (ped->getGlobalPositionForInputChannel(channel).getDistanceFrom(getGlobalPositionOf(e.getPosition())) <= MAX_CONNECTION_RANGE && g.canConnect(potentialConnection)) {
                end = {ped, channel};
                ped->trackConnector(this); 
                g.addConnection(potentialConnection);

                return;
            }
        }
    }
}

/**
 * getGlobalPositionOf():
 * 
 * Returns the "global" position of a point represented within the component, i.e., the position
 * of the point relative to the component containing the connector (in this case, the editor
 * window).
 */
juce::Point<int> Connector::getGlobalPositionOf(const Point<int>& p) {
    return p + getPosition();
}