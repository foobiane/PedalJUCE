#include "IOBoxes.h"
#include "Pedal.h"

#include <iostream> // debug

Connector::Connector(juce::AudioProcessorGraph* graph, juce::AudioProcessorGraph::NodeID id, int channel, juce::Point<int> startPt) {
    g = graph;

    start = {id, channel};
    startPoint = startPt;

    resetBounds();
    juce::Rectangle<int> bounds = getBounds(); // debugging
    printf("startPoint: (%d, %d)\n", startPoint.x, startPoint.y);
    printf("getBounds(): (%d, %d, %d, %d)\n", bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight());
}

Connector::~Connector() {
    // Nothing for now.
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
    juce::Rectangle<int> newBounds = juce::Rectangle<int>(startPoint, endPoint).expanded(MAX_CONNECTION_RANGE);

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
        g->disconnectNode(start.nodeID);

        if (end.nodeID == OUTPUT_BOX_NODE_ID)
            static_cast<OutputBox*>(g->getNodeForId(end.nodeID)->getProcessor())->ports[end.channelIndex]->setIncomingConnector(nullptr);
        else
            getEndPedal()->inputPorts[end.channelIndex]->setIncomingConnector(nullptr);

        connected = false;

        resetBounds();
    }
}

// Inherited from juce::Component.
void Connector::mouseDown(const juce::MouseEvent& e) {
    if (!connected) {
        dragging = true;
        endPoint = e.getPosition() + getPosition();

        adjustBounds();
    }
    // For connected components
    else {
        // If the user clicked on the x
        if (showingControls && cablePath.contains(e.getPosition().toFloat())) {
            juce::Point<float> m = cablePath.getPointAlongPath(cablePath.getLength() / 4).translated(-0.5 * pathThickness, -0.5 * pathThickness);

            if ((e.getPosition() + getPosition()).toFloat().getDistanceFrom(m + getPosition().toFloat()) <= MAX_CONNECTION_RANGE)
                disconnect();
        }

        showingControls = !showingControls; // toggle
        repaint();
    }
}

// Inherited from juce::Component.
void Connector::mouseDrag(const juce::MouseEvent& e) {
    if (!connected) {
        endPoint = e.getPosition() + getPosition();

        adjustBounds();
    }

    // TODO: Add logic for dragging connections
}

// Inherited from juce::Component.
void Connector::mouseUp(const juce::MouseEvent& e) {
    if (!connected) {
        endPoint = e.getPosition() + getPosition();
        attemptConnection();

        if (!connected) 
            resetBounds();
        else {
            // Good code would make this polymorphic so we don't have to check, but IDC for now.
            if (end.nodeID == OUTPUT_BOX_NODE_ID)
                endPoint = static_cast<OutputBox*>(g->getNodeForId(end.nodeID)->getProcessor())->getPositionOfPort(end.channelIndex);
            else
                endPoint = getEndPedal()->getPositionOfInputPort(end.channelIndex);
            adjustBounds();
        }

        dragging = false;
    }
}

// Inherited from juce::Component.
void Connector::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::yellow);

    juce::Point<int> startPointAdjusted = startPoint - getPosition();
    juce::Rectangle<float> area(startPointAdjusted.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE).toFloat(), startPointAdjusted.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE).toFloat());
    g.fillEllipse(area);

    if (isConnected() || dragging)
        g.fillPath(cablePath);

    if (showingControls) {
        // Draws an X at the center of the cable path for removing connections
        std::cout << "Displaying controls" << std::endl;

        juce::Point<float> m = cablePath.getPointAlongPath(cablePath.getLength() / 4).translated(-0.5 * pathThickness, -0.5 * pathThickness);

        g.setColour(juce::Colours::red);
        g.drawLine(
            m.x - MAX_CONNECTION_RANGE, m.y - MAX_CONNECTION_RANGE,
            m.x + MAX_CONNECTION_RANGE, m.y + MAX_CONNECTION_RANGE,
            6.0f
        );

        g.drawLine(
            m.x - MAX_CONNECTION_RANGE, m.y + MAX_CONNECTION_RANGE,
            m.x + MAX_CONNECTION_RANGE, m.y - MAX_CONNECTION_RANGE,
            6.0f
        );
    }
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

    juce::PathStrokeType wideStroke(pathThickness);
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
void Connector::attemptConnection() {
    OutputBox* opb = dynamic_cast<OutputBox*>(g->getNodeForId(OUTPUT_BOX_NODE_ID)->getProcessor());

    // Edge Case: Handling a connection to the output box
    for (int channel = 0; channel < opb->getNumChannels(); channel++) {
        juce::AudioProcessorGraph::Connection potentialConnection(start, {OUTPUT_BOX_NODE_ID, channel});

        if (opb->getPositionOfPort(channel).getDistanceFrom(endPoint) <= MAX_CONNECTION_RANGE && g->canConnect(potentialConnection)) {
            end = {OUTPUT_BOX_NODE_ID, channel};
            opb->ports[channel]->setIncomingConnector(this);
            g->addConnection(potentialConnection);
            connected = true; 

            printf("Connection successfully made between start: {%d, %d} and end: {%d, %d}\n", start.nodeID, start.channelIndex, end.nodeID, end.channelIndex); // debug

            return;
        }
    }

    // Handling all other pedals in the graph
    for (juce::AudioProcessorGraph::Node* pedalNode : g->getNodes()) {
        // Skip IO boxes, since we will have already checked them at this point
        if (pedalNode->nodeID == INPUT_BOX_NODE_ID || pedalNode->nodeID == OUTPUT_BOX_NODE_ID)
            continue;

        Pedal* ped = static_cast<Pedal*>(pedalNode->getProcessor());

        for (int channel = 0; channel < ped->getNumInputChannels(); channel++) {
            juce::AudioProcessorGraph::Connection potentialConnection(start, {pedalNode->nodeID, channel});

            if (ped->getPositionOfInputPort(channel).getDistanceFrom(endPoint) <= MAX_CONNECTION_RANGE && !g->isConnected(potentialConnection) && g->canConnect(potentialConnection)) {                
                end = {pedalNode->nodeID, channel}; // marking the end connection
                ped->inputPorts[channel]->setIncomingConnector(this); // storing the connection in the port
                g->addConnection(potentialConnection); // marking the connection in the AudioProcessorGraph
                connected = true; // maintaining a boolean to know when to not read end

                printf("Connection successfully made between start: {%d, %d} and end: {%d, %d}\n", start.nodeID, start.channelIndex, end.nodeID, end.channelIndex); // debug

                return;
            }
        }
    }
}
