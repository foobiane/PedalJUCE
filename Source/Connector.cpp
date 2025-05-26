#include "Connector.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// CONNECTOR IMPLEMENTATION  
// Note that this was heavily influenced by the implementation used in JUCE's audio plugin host.
///////////////////////////////////////////////////////////////////////////////////////////////////

Connector::Connector(PedalJUCEAudioProcessorEditor& editor, Graph<Pedal*>& connectionMap) : editor(editor), connectionMap(connectionMap) { }

void Connector::setInput(Pedal* p, int channel) {
    inputPedalAndChannel = {p, channel};
}

void Connector::setOutput(Pedal* p, int channel) {
    outputPedalAndChannel = {p, channel};
}

std::pair<std::pair<Pedal*, int>, std::pair<Pedal*, int>> getConnection() {
    return {outputPedalAndChannel, inputPedalAndChannel};
}

void Connector::dragStart(juce::Point<float> pos) {
    lastOutputPos = pos;
    resizeToFit();
}

void Connector::dragEnd(juce::Point<float> pos) {
    lastInputPos = pos;
    resizeToFit();
}

void Connector::mouseDown(const juce::MouseEvent& e) {
    dragging = false;
}

void Connector::mouseDrag(const juce::MouseEvent& e) {
    if (dragging)
        editor.dragConnector(e);

    else if (e.mouseWasDraggedSinceMouseDown()) {
        dragging = true;

        // TODO: Add channel support to our graph
        connectionMap.removeEdge(outputPedalAndChannel.first, inputPedalAndChannel.first);

        std::pair<float, float> dists = distancesFromEnds(getPosition().toFloat() + e.position);
        bool isCloserToSource = (dists.first < dists.second);
        std::pair<Pedal*, int> dummyConnection = {nullptr, 0};

        editor.beginConnectorDrag(isCloserToSource ? dummyConnection : outputPedalAndChannel,
                                  isCloserToSource ? inputPedalAndChannel : dummyConnection,
                                  e);
    }
}

void Connector::mouseUp(const juce::MouseEvent& e) {
    if (dragging) editor.endConnectorDrag(e);
}

std::pair<juce::Point<float>, juce::Point<float>> Connector::getConnectionCoords() {
    // Obtaining the global coordinates of the ports involved in the connection.
    std::pair<juce::Point<float>, juce::Point<float>> result = {lastOutputPos, lastInputPos};

    if (outputPedalAndChannel.first)
        result.first = outputPedalAndChannel.first->getOutputPortCoord(outputPedalAndChannel.second);
    if (inputPedalAndChannel.first)
        result.second = inputPedalAndChannel.first->getInputPortCoord(inputPedalAndChannel.second);

    return result;
}

void Connector::update() {
    std::pair<juce::Point<float>, juce::Point<float>> pts = getConnectionCoords();

    // Causes a re-render if the connection coordinates were altered
    if (pts.first != lastOutputPos && pts.second != lastInputPos)
        resizeToFit();
}

void Connector::resizeToFit() {
    std::pair<juce::Point<float>, juce::Point<float>> bounds = getConnectionCoords();

    // Potential bug: Using global coords?
    juce::Rectangle<int> newBounds = Rectangle<float>(bounds.first, bounds.second).expanded(4.0f).getSmallestIntegerContainer();

    if (newBounds != getBounds())
        setBounds(newBounds);
    else
        resized();

    repaint();
}

bool Connector::hitTest(int x, int y) {
    juce::Point<float> pos = juce::Point<int>(x, y).toFloat();

    if (hitPath.contains(pos)) {
        std::pair<float, float> dists = distancesFromEnds(pos);

        return dists.first < 7.0f && dists.second < 7.0f;
    }

    return false;
}

void Connector::resized() {
    std::pair<juce::Point<float>, juce::Point<float>> ends = getConnectionCoords();
    lastOutputPos = ends.first;
    lastInputPos = ends.second;

    ends.first -= getPosition.toFloat();
    ends.second -= getPosition.toFloat();

    linePath.clear();
    linePath.startNewSubpath(ends.first);
    linePath.cubicTo(
        ends.first.x, ends.first.y + (ends.second.y - ends.first.y) * 0.33f,
        ends.second.x, ends.second.y + (ends.second.y - ends.first.y) * 0.66f,
        ends.second.x, ends.second.y
    );
    
    juce::PathStrokeType(8.0f).createStrokedPath(hitPath, linePath);
    juce::PathStrokeType(2.5f).createStrokedPath(linePath, linePath);
}

void Connector::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::red);
    g.fillPath(linePath);
}

std::pair<float, float> Connector::distancesFromEnds(juce::Point<float> pos) {
    std::pair<juce::Point<float>, juce::Point<float>> ends = getConnectionCoords();

    return {ends.first.getDistanceFrom(pos), ends.second.getDistanceFrom(pos)};
}