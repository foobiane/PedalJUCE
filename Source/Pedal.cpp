#include "Pedal.h"

Pedal::Pedal() {
    nameFont.setHeight(nameFontHeight);
    setSize(width, height);
    uid = ID++;
}

uint32_t Pedal::getUID() {
    return uid;
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

    return Point<int>(0, 0);
}

juce::Point<int> Pedal::getGlobalPositionForOutputChannel(int channel) {
    if (channel < numOutputChannels)
        return outputPorts[channel].position + getPosition();

    return Point<int>(0, 0);
}

void Pedal::mouseDown(const juce::MouseEvent& e) {
    drag.startDraggingComponent(this, e);
}

void Pedal::mouseDrag(const juce::MouseEvent& e) {
    drag.dragComponent(this, e, nullptr);
}
