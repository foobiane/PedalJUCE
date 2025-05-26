/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Pedal.h"

//==============================================================================
PedalJUCEAudioProcessorEditor::PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    Pedal* test = new Pedal();
    audioProcessor.connectionMap.addVertex(test);

    for (Pedal* ped : audioProcessor.connectionMap.vertices())
        addAndMakeVisible(ped);

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1200, 900);
}

PedalJUCEAudioProcessorEditor::~PedalJUCEAudioProcessorEditor()
{
}

//==============================================================================
void PedalJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    // g.setColour (juce::Colours::white);
    // g.setFont (juce::FontOptions (15.0f));
    // g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void PedalJUCEAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    for (Pedal* ped : audioProcessor.connectionMap.vertices())
        ped->setBounds(0, 0, ped->getPedalWidth(), ped->getPedalHeight()); // 
}

void PedalJUCEAudioProcessorEditor::beginConnectorDrag(std::pair<Pedal*, int> start, std::pair<Pedal*, int> end, const juce::MouseEvent& e) {
    Connector* c = (Connector*) e.originalComponent;
    connectors.erase(c);
    draggingConnector = c;

    if (draggingConnector == nullptr)
        draggingConnector = new Connector(this);

    draggingConnector.setOutput(start.first, start.second)
    draggingConnector.setInput(end.first, end.second);

    addAndMakeVisible(draggingConnector);

    dragConnector(e);
}

void PedalJUCEAudioProcessorEditor::dragConnector(const juce::MouseEvent& e) {
    juce::MouseEvent* e2 = e.getEventRelativeTo(this);

    if (draggingConnector != nullptr) {
        Point<float> pos = e2.position();
        
    }
}

std::pair<Pedal*, int> PedalJUCEAudioProcessorEditor::getPortAt(Point<float> pos) {
    for (Pedal* p : audioProcessor.connectionMap.vertices()) {
        for ()
    }
}

void PedalJUCEAudioProcessorEditor::endConnectorDrag(const juce::MouseEvent& e) {

}