#include "Pedal.h"
#include "PluginEditor.h"

Pedal::Pedal(PedalJUCEAudioProcessorEditor* e, int w, int h, int nfh) : 
    trashIcon(e, this) 
{
    g = &e->audioProcessor.connectionMap;
    editor = e;

    width = w;
    height = h;
    nameFontHeight = nfh;

    uid = juce::AudioProcessorGraph::NodeID(ID++);

    nameFont.setHeight(nameFontHeight);

    setPlayConfigDetails(
        numInputChannels, 
        numOutputChannels, 
        DEFAULT_SAMPLE_RATE, 
        DEFAULT_BLOCK_SIZE
    );

    initializePorts();

    setSize(width, height);
}

Pedal::~Pedal() {
    for (Connector* c : connectors)
        delete c; // the connector destructor handles the rest

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

void Pedal::prepareBasics(double sampleRate, int maximumExpectedSamplesPerBlock) {
    sr = sampleRate; 
    blockSize = maximumExpectedSamplesPerBlock; 

    setPlayConfigDetails(
        numInputChannels, 
        numOutputChannels, 
        sr, 
        blockSize
    );
}

void Pedal::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) { 
    prepareBasics(sampleRate, maximumExpectedSamplesPerBlock);
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
    g.fillRoundedRectangle(0, 0, width, height - pedalThickness, 20);

    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(name, 0, (int) (0.5 * (height - pedalThickness)) - nameFontHeight, width, nameFontHeight, juce::Justification::centred, true);
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
    if (!showingTrash) {
        trashIcon.setBounds(width - 25, 0, 25, 25);
        addAndMakeVisible(&trashIcon);
        showingTrash = true;
    }
    else {
        removeChildComponent(&trashIcon);
        showingTrash = false;
    }

    drag.startDraggingComponent(this, e);
}

void Pedal::mouseDrag(const juce::MouseEvent& e) {
    drag.dragComponent(this, e, nullptr);
    updatePorts();
}

Pedal::TrashIcon::TrashIcon(PedalJUCEAudioProcessorEditor* e, Pedal* p) : 
    icon(juce::ImageFileFormat::loadFrom(BinaryData::trashsolid_png, BinaryData::trashsolid_pngSize)),
    editor(e), 
    pedal(p)
{

}

void Pedal::TrashIcon::paint(juce::Graphics& g) {
    int width = getLocalBounds().getWidth();
    int height = getLocalBounds().getHeight();
    g.drawImageWithin(icon, 0, 0, width, height, juce::RectanglePlacement::stretchToFit);
}

void Pedal::TrashIcon::mouseDown(const juce::MouseEvent& e) {
    editor->removePedalFromEditor(pedal);
}