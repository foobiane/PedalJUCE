#include "IOBoxes.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
// InputBox
///////////////////////////////////////////////////////////////////////////////////////////////////

InputBox::InputBox(juce::AudioProcessorGraph* graph, int w, int h) : juce::AudioProcessorGraph::AudioGraphIOProcessor(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioInputNode) {
    g = graph;
    width = w;
    height = h;

    labelFont.setHeight(labelFontHeight);

    numChannels = g->getTotalNumInputChannels(); // TODO: Make this detect the number of audio channels automatically

    // TODO: We are running into a problem where the ports are being initialized before the bounds have properly been set.
    // This causes issues where the connector will sense clicks outside of what should be its normal range, creating a "sticky"
    // UI for the first couple clicks. We need to find a way to initialize ports *after* the bounds have been set. This would
    // involve setting the bounds somewhere around here.
    initializePorts();
}

InputBox::~InputBox() {
    for (Connector* c : ports)
        delete c;
}

void InputBox::initializePorts() {
    for (int i = 0; i < numChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(0, (int)((i + 1) * (height / (numChannels + 1)))) + getPosition();
        ports.push_back(new Connector(g, INPUT_BOX_NODE_ID, i, pos));
    }
}

void InputBox::updatePorts() {
    for (int i = 0; i < numChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(0, (int)((i + 1) * (height / (numChannels + 1)))) + getPosition();
        ports[i]->updateStartPoint(pos);
    }
}

void InputBox::paint(juce::Graphics& g) {
    // TODO: Add labels
    juce::Rectangle<int> area = getBounds();

    g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f));
    g.fillRoundedRectangle(0, 0, area.getWidth(), area.getHeight(), 20);

    area.setSize(0.75 * width, height);

    g.setColour(juce::Colour(0.0f, 0.0f, 0.70f, 1.0f));
    g.fillRoundedRectangle(0.25 * width, 0, area.getWidth(), area.getHeight(), 20);

    g.setColour(juce::Colour(0.0f, 0.0f, 0.25f, 1.0f));
    g.setFont(labelFont);

    for (int i = 0; i < numChannels; i++) {
        juce::Point<int> connectorPoint = ports[i]->getStartPoint();
        g.drawFittedText("Input " + std::to_string(i), 0.25 * width, connectorPoint.y - 2 * MAX_CONNECTION_RANGE - 0.5 * labelFontHeight, 0.75 * width, labelFontHeight, juce::Justification::centred, 1);
    }
}

void InputBox::resized() {
    updatePorts();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// OutputBox
///////////////////////////////////////////////////////////////////////////////////////////////////

OutputBox::OutputBox(juce::AudioProcessorGraph* graph, int w, int h) : juce::AudioProcessorGraph::AudioGraphIOProcessor(juce::AudioProcessorGraph::AudioGraphIOProcessor::audioOutputNode) {
    g = graph;
    width = w;
    height = h;

    labelFont.setHeight(labelFontHeight);

    numChannels = g->getTotalNumOutputChannels();

    initializePorts();
}

OutputBox::~OutputBox() {
    for (InputPort* ip : ports)
        delete ip;
}

void OutputBox::initializePorts() {
    for (int i = 0; i < numChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(width, (int)((i + 1) * (height / (numChannels + 1)))) + getPosition();
        juce::Rectangle<int> area(pos.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE), pos.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE));

        InputPort* port = new InputPort();
        port->setBounds(area);

        ports.push_back(port);
    }
}

void OutputBox::updatePorts() {
    for (int i = 0; i < numChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(width, (int)((i + 1) * (height / (numChannels + 1)))) + getPosition();
        juce::Rectangle<int> oldBounds = ports[i]->getBounds();

        ports[i]->setBounds(pos.x - MAX_CONNECTION_RANGE, pos.y - MAX_CONNECTION_RANGE, oldBounds.getWidth(), oldBounds.getHeight());

        // If our input port is connected to another connector, update that connector too
        Connector* incoming = ports[i]->getIncomingConnector();
        if (incoming != nullptr)
            incoming->updateEndPoint(pos);
    }
}

void OutputBox::paint(juce::Graphics& g) {
    // TODO: Add labels
    juce::Rectangle<int> area = getBounds();

    g.setColour(juce::Colour(0.0f, 0.0f, 0.75f, 1.0f));
    g.fillRoundedRectangle(0, 0, area.getWidth(), area.getHeight(), 20);

    area.setSize(0.75 * width, height);

    g.setColour(juce::Colour(0.0f, 0.0f, 0.70f, 1.0f));
    g.fillRoundedRectangle(0, 0, area.getWidth(), area.getHeight(), 20);

    g.setColour(juce::Colour(0.0f, 0.0f, 0.25f, 1.0f));
    g.setFont(labelFont);

    for (int i = 0; i < numChannels; i++) {
        juce::Point<int> connectorPoint = ports[i]->getPosition().translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE);
        g.drawFittedText("Output " + std::to_string(i), 0, connectorPoint.y - 2 * MAX_CONNECTION_RANGE - 0.5 * labelFontHeight, 0.75 * width, labelFontHeight, juce::Justification::centred, 1);
    }
}

void OutputBox::resized() {
    updatePorts();
}