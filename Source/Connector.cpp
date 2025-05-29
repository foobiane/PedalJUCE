#include "Pedal.h"

Connector::Connector(juce::AudioProcessorGraph* graph, juce::AudioProcessorGraph::NodeID id, int channel, juce::Point<int> startPt) {
    g = graph;

    start.nodeID = id; start.channelIndex = channel;;
    startPoint = startPt;

    resetBounds();
}

void Connector::updateStartPoint(juce::Point<int> newStart) {
    startPoint = newStart;
    adjustBounds();
}

void Connector::updateEndPoint(juce::Point<int> newEnd) {
    endPoint = newEnd;
    adjustBounds();
}

juce::Point<int> Connector::getStartPoint() {
    return startPoint;
}

Pedal* Connector::getStartPedal() {
    return static_cast<Pedal*>(g->getNodeForId(start.nodeID)->getProcessor());
}

Pedal* Connector::getEndPedal() {
    return static_cast<Pedal*>(g->getNodeForId(end.nodeID)->getProcessor());
}

/**
 * resetBounds():
 *
 * Sets the boundaries of the component to a square encircling the starting port.
 */
void Connector::resetBounds() {
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
    juce::Point<int> startPointAdjusted = juce::Point<int>(
        endPoint.x < startPoint.x ? startPoint.x + MAX_CONNECTION_RANGE : startPoint.x - MAX_CONNECTION_RANGE,
        endPoint.y < startPoint.y ? startPoint.y + MAX_CONNECTION_RANGE : startPoint.y - MAX_CONNECTION_RANGE
    ); // adjusting start point to include start ellipse

    juce::Rectangle<int> newBounds = juce::Rectangle<int>(startPointAdjusted, endPoint).expanded(4.0f);

    if (getBounds() != newBounds)
        setBounds(newBounds);
    else
        resized();

    repaint();
}

/**
 * isConnected():
 *
 * Returns whether or not the connector is connected to an endpoint.
 */
bool Connector::isConnected() {
    return connected; 
}

/**
 * disconnect():
 *
 * Disconnects the Connector by removing the relevant connection from the AudioProcessorGraph,
 * untracking it from the end pedal, and setting the end to a nullptr.
 */
void Connector::disconnect() {
    if (connected) {
        g->removeConnection(juce::AudioProcessorGraph::Connection(start, end));
        getEndPedal()->inputPorts[end.channelIndex]->setIncomingConnector(nullptr);
        connected = false;
    }
}

// Inherited from juce::Component.
void Connector::mouseDown(const juce::MouseEvent& e) {
    if (!connected) {
        dragging = true;
        endPoint = e.getPosition() + getPosition();

        adjustBounds();
    }
    
    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::mouseDrag(const juce::MouseEvent& e) {
    if (!connected) {
        endPoint = e.getPosition() + getPosition();

        adjustBounds();
    }

    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::mouseUp(const juce::MouseEvent& e) {
    if (!connected) {
        attemptConnection(e);

        if (!connected) 
            resetBounds();
        else {
            endPoint = getEndPedal()->getPositionOfInputPort(end.channelIndex);
            adjustBounds();
        }

        dragging = false;
    }   

    // TODO: Add logic for deleting connections
}

// Inherited from juce::Component.
void Connector::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::yellow);

    juce::Point<int> startPointAdjusted = startPoint - getPosition();
    juce::Rectangle<float> area(startPointAdjusted.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE).toFloat(), startPointAdjusted.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE).toFloat());
    g.fillEllipse(area);

    if (isConnected() || dragging)
        g.fillPath(cablePath);
}

// Inherited from juce::Component.
void Connector::resized() {
    // Note that we do all of this in resized() since this function is called during a boundary
    // change. Boundary changes only occur when we're dragging. 

    juce::Point<int> startPointAdjusted = startPoint - getPosition();
    juce::Point<int> endPointAdjusted = endPoint - getPosition();

    cablePath.clear();
    cablePath.startNewSubPath(startPointAdjusted.toFloat());
    cablePath.cubicTo(
        startPointAdjusted.x, startPointAdjusted.y + (endPointAdjusted.y - startPointAdjusted.y) * 0.33f,
        endPointAdjusted.x, startPointAdjusted.y + (endPointAdjusted.y - startPointAdjusted.y) * 0.66f,
        endPointAdjusted.x, endPointAdjusted.y
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
    for (juce::AudioProcessorGraph::Node* pedalNode : g->getNodes()) {
        Pedal* ped = static_cast<Pedal*>(pedalNode->getProcessor());

        for (int channel = 0; channel < ped->getNumInputChannels(); channel++) {
            juce::AudioProcessorGraph::Connection potentialConnection(start, {pedalNode->nodeID, channel});

            if (ped->getPositionOfInputPort(channel).getDistanceFrom(endPoint) <= MAX_CONNECTION_RANGE && g->canConnect(potentialConnection)) {
                end = {pedalNode->nodeID, channel};
                ped->inputPorts[channel]->setIncomingConnector(this);
                g->addConnection(potentialConnection);

                return;
            }
        }
    }
}
