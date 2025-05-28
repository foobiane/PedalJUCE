#include "Pedal.h"

Pedal::Pedal() {
    nameFont.setHeight(nameFontHeight);
    setSize(width, height);
    uid = ID++;
    initializePorts();
}

void Pedal::initializePorts() {
    for (int i = 0; i < numInputChannels; i++) {
        juce::Point<int> pos = {width, (int) ((i + 1) * (height / (numInputChannels + 1)))};
        inputPorts.push_back({pos, i});
    }
    for (int i = 0; i < numOutputChannels; i++) {
        juce::Point<int> pos = {0, (int) ((i + 1) * (height / (numOutputChannels + 1)))};
        outputPorts.push_back({pos, i});
    }
}

juce::AudioProcessorGraph::NodeID Pedal::getUIDAsNodeID() {
    return juce::AudioProcessorGraph::NodeID(uid);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// AudioProcessor Methods
///////////////////////////////////////////////////////////////////////////////////////////////////

const juce::String Pedal::getName() const { return name; }
void Pedal::prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) { sr = sampleRate; blockSize = maximumExpectedSamplesPerBlock; }
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

void Pedal::paint(juce::Graphics& g) {
    // TODO: Enable color selection
    // TODO: Fonts, text
    // TODO: Cool details (shine, etc.)

    g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 0.5f, 1.0f)); // sides
    g.fillRoundedRectangle(0, 0, width, height, 20);

    g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 0.75f, 1.0f)); // recessed top part
    g.fillRoundedRectangle(0, 0, width, height - 25, 20);

    // g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 1.0f, 1.0f)); // top pedal component
    // g.fillRoundedRectangle(0, (int) (0.5 * (height - 25)) - nameFontHeight, width, (int) (0.5 * (height - 25)) + nameFontHeight, 20);

    // g.setColour(juce::Colour((219.0f / 360.0f), 0.59f, 1.0f, 1.0f)); // top pedal component
    // g.fillRoundedRectangle(0, (int) (0.5 * (height - 25)) - nameFontHeight, width, (int) (0.5 * (height - 25)) + nameFontHeight, 20);
    
    // g.setColour(juce::Colour(0.0f, 0.0f, 0.2f, 1.0f)); // flat foot switch
    // g.fillRoundedRectangle(10, (int) (0.5 * (height - 25)), width - 20, (int) (0.5 * (height - 25)) - 10, 10);

    g.setColour(juce::Colours::black);
    g.setFont(nameFont);
    g.drawText(name, 0, (int) (0.5 * (height - 25)) - nameFontHeight, width, nameFontHeight, juce::Justification::centred, true);
}

void Pedal::resized() {
    // Nothing for now
}

int Pedal::getNumInputChannels() {
    return numInputChannels;
}

int Pedal::getNumOutputChannels() {
    return numOutputChannels;
}

juce::Point<int> Pedal::getGlobalPositionForInputChannel(int channel) {
    if (channel < numInputChannels)
        return inputPorts[channel].position + getPosition();

    return juce::Point<int>(0, 0);
}

juce::Point<int> Pedal::getGlobalPositionForOutputChannel(int channel) {
    if (channel < numOutputChannels)
        return outputPorts[channel].position + getPosition();

    return juce::Point<int>(0, 0);
}

/**
 * trackConnector():
 *
 * Marks a connected component for tracking. The pedal tracks its connectors *only* for the case
 * where the pedal is dragged, which should cause all of the connectors to repaint.
 */
void Pedal::trackConnector(Connector* c) {
    if (!connectors.contains(c))
        connectors.insert(c);
}

void Pedal::untrackConnector(Connector* c) {
    connectors.erase(c);
}

void Pedal::updateAllConnectors() {
    for (Connector* c : connectors) {
        if (c->isConnected())
            c->adjustBounds();
        else
            c->resetBounds();

        c->repaint();
    }
}

void Pedal::mouseDown(const juce::MouseEvent& e) {
    drag.startDraggingComponent(this, e);
}

void Pedal::mouseDrag(const juce::MouseEvent& e) {
    drag.dragComponent(this, e, nullptr);
    updateAllConnectors(); // this might be slow for a lot of connectors
}
