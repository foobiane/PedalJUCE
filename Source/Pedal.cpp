#include "Pedal.h"

Pedal::Pedal(juce::AudioProcessorGraph* g) {
    this->g = g;
    uid = juce::AudioProcessorGraph::NodeID(ID++);

    nameFont.setHeight(nameFontHeight);

    setPlayConfigDetails(numInputChannels, numOutputChannels, DEFAULT_SAMPLE_RATE, DEFAULT_BLOCK_SIZE);
    initializePorts();
    
    setSize(width, height);
}

Pedal::~Pedal() {
    for (Connector* c : connectors) {
        c->disconnect();
        delete c; // TODO: Handle connections too
    }

    for (InputPort* ip : inputPorts)
        delete ip;
}

void Pedal::initializePorts() {
    for (int i = 0; i < numOutputChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(0, (int)((i + 1) * (height / (numOutputChannels + 1)))) + getPosition();
        Connector* c = new Connector(g, uid, i, pos);
        connectors.push_back(c);
    }

    for (int i = 0; i < numInputChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(width, (int)((i + 1) * (height / (numInputChannels + 1)))) + getPosition();
        juce::Rectangle<int> area(pos.translated(-1 * MAX_CONNECTION_RANGE, -1 * MAX_CONNECTION_RANGE), pos.translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE));

        InputPort* port = new InputPort();
        port->setBounds(area);

        inputPorts.push_back(port);
    }
}

void Pedal::updatePorts() {
    // All outgoing connectors
    for (int i = 0; i < numOutputChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(0, (int)((i + 1) * (height / (numOutputChannels + 1)))) + getPosition();
        connectors[i]->updateStartPoint(pos);
    }

    // All input ports and incoming connectors
    for (int i = 0; i < numInputChannels; i++) {
        juce::Point<int> pos = juce::Point<int>(width, (int)((i + 1) * (height / (numInputChannels + 1)))) + getPosition();
        juce::Rectangle<int> oldBounds = inputPorts[i]->getBounds();

        inputPorts[i]->setBounds(pos.x - MAX_CONNECTION_RANGE, pos.y - MAX_CONNECTION_RANGE, oldBounds.getWidth(), oldBounds.getHeight()); // TODO: Will this repaint properly??

        // If our input port is connected to another connector, update that connector too
        Connector* incoming = inputPorts[i]->getIncomingConnector();
        if (incoming != nullptr)
            incoming->updateEndPoint(pos);
    }
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// AudioProcessor Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

const juce::String Pedal::getName() const { return name; }

void Pedal::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) { 
    sr = sampleRate; 
    blockSize = maximumExpectedSamplesPerBlock; 

    setPlayConfigDetails(numInputChannels, numOutputChannels, sr, blockSize);
}

void Pedal::releaseResources() { /* Nothing for now. */ } 
void Pedal::processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) { /* Nothing for now. */ } 
double Pedal::getTailLengthSeconds() const { return 0.0; }
bool Pedal::acceptsMidi() const { return false; } 
bool Pedal::producesMidi() const { return false; } 
juce::AudioProcessorEditor* Pedal::createEditor() { return nullptr; } 
bool Pedal::hasEditor() const { return false; } 
int Pedal::getNumPrograms() { return 0; } 
int Pedal::getCurrentProgram() { return 0; } 
void Pedal::setCurrentProgram(int index) { /* Nothing for now. */ } 
const juce::String Pedal::getProgramName(int index) { return ""; } 
void Pedal::changeProgramName(int index, const juce::String &newName) { /* Nothing for now. */ } 
void Pedal::getStateInformation(juce::MemoryBlock &destData) { /* Nothing for now. */ } 
void Pedal::setStateInformation(const void *data, int sizeInBytes) { /* Nothing for now. */ } 

///////////////////////////////////////////////////////////////////////////////////////////////////

int Pedal::getPedalWidth() {
    return width;
}

int Pedal::getPedalHeight() {
    return height;
}

juce::AudioProcessorGraph::NodeID Pedal::getNodeID() {
    return uid;
}

void Pedal::paint(juce::Graphics& g) {
    g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 0.5f, 1.0f)); // sides
    g.fillRoundedRectangle(0, 0, width, height, 20);

    g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 0.75f, 1.0f)); // recessed top part
    g.fillRoundedRectangle(0, 0, width, height - 25, 20);

    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(name, 0, (int) (0.5 * (height - 25)) - nameFontHeight, width, nameFontHeight, juce::Justification::centred, true);
}

void Pedal::resized() {
    updatePorts();
}

int Pedal::getNumInputChannels() {
    return numInputChannels;
}

int Pedal::getNumOutputChannels() {
    return numOutputChannels;
}

juce::Point<int> Pedal::getPositionOfInputPort(int channel) {
    return inputPorts[channel]->getPosition().translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE);
}

juce::Point<int> Pedal::getPositionOfOutputPort(int channel) {
    return connectors[channel]->getStartPoint();
}

void Pedal::mouseDown(const juce::MouseEvent& e) {
    drag.startDraggingComponent(this, e);
}

void Pedal::mouseDrag(const juce::MouseEvent& e) {
    drag.dragComponent(this, e, nullptr);
    updatePorts();
}
