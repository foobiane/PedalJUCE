#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "IOBoxes.h"
#include "Pedal.h"
#include "PedalIncludes.h"

#include <memory>
#include <type_traits>

///////////////////////////////////////////////////////////////////////////////////////////////////
// Pedal Manipulation
///////////////////////////////////////////////////////////////////////////////////////////////////

// Adds the pedal referenced by a unique pointer to the editor interface.
Pedal* PedalJUCEAudioProcessorEditor::addPedalToEditor(std::unique_ptr<Pedal> ped) {
    juce::AudioProcessorGraph::NodeID uid = ped->getNodeID();
    audioProcessor.connectionMap.addNode(std::move(ped), uid);

    // Getting our pedal back from the graph
    Pedal* p = static_cast<Pedal*>(audioProcessor.connectionMap.getNodeForId(uid)->getProcessor());
    
    addAndMakeVisible(p);

    for (int i = 0; i < p->getNumOutputChannels(); i++)
        addAndMakeVisible(p->connectors[i]);
    
    for (int i = 0; i < p->getNumInputChannels(); i++)
        addAndMakeVisible(p->inputPorts[i]);

    return p;
}

// Removes a pedal from the editor interface.
void PedalJUCEAudioProcessorEditor::removePedalFromEditor(Pedal* p) {
    for (int i = 0; i < p->getNumOutputChannels(); i++)
        removeChildComponent(p->connectors[i]);
    
    for (int i = 0; i < p->getNumInputChannels(); i++)
        removeChildComponent(p->inputPorts[i]);

    removeChildComponent(p);

    delete p;
}   

// Adds IO boxes to the editor interface.
void PedalJUCEAudioProcessorEditor::addIOBoxesToEditor() {
    std::unique_ptr<InputBox> ipb = std::make_unique<InputBox>(&audioProcessor.connectionMap, 0.1 * editorWidth, 0.95 * editorHeight);
    std::unique_ptr<OutputBox> opb = std::make_unique<OutputBox>(&audioProcessor.connectionMap, 0.1 * editorWidth, 0.95 * editorHeight);

    audioProcessor.connectionMap.addNode(std::move(ipb), INPUT_BOX_NODE_ID); // TODO: This returns a Node, which we can use to shorten later searches
    audioProcessor.connectionMap.addNode(std::move(opb), OUTPUT_BOX_NODE_ID);

    InputBox* ipb_ptr = dynamic_cast<InputBox*>(audioProcessor.connectionMap.getNodeForId(INPUT_BOX_NODE_ID)->getProcessor());
    addAndMakeVisible(ipb_ptr);
    
    for (int i = 0; i < ipb_ptr->getNumChannels(); i++)
        addAndMakeVisible(ipb_ptr->ports[i]);
    
    OutputBox* opb_ptr = dynamic_cast<OutputBox*>(audioProcessor.connectionMap.getNodeForId(OUTPUT_BOX_NODE_ID)->getProcessor());
    addAndMakeVisible(opb_ptr);

    for (int i = 0; i < opb_ptr->getNumChannels(); i++)
        addAndMakeVisible(opb_ptr->ports[i]);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Editor Functionality
///////////////////////////////////////////////////////////////////////////////////////////////////

PedalJUCEAudioProcessorEditor::PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addIOBoxesToEditor();
    addPedalToEditor(getPedalFromName("GainStage", &audioProcessor.connectionMap, editorWidth, editorHeight));

    setSize(editorWidth, editorHeight);
}

PedalJUCEAudioProcessorEditor::~PedalJUCEAudioProcessorEditor() {
    audioProcessor.connectionMap.clear();
}

void PedalJUCEAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PedalJUCEAudioProcessorEditor::resized() {
    for (juce::AudioProcessorGraph::Node* n: audioProcessor.connectionMap.getNodes()) {
        if (n->nodeID == INPUT_BOX_NODE_ID) {
            InputBox* ipb = dynamic_cast<InputBox*>(n->getProcessor());
            juce::Point<float> start(0.875 * editorWidth, 0.025 * editorHeight);

            ipb->setBounds(start.x, start.y, ipb->getWidth(), ipb->getHeight());
        }
        else if (n->nodeID == OUTPUT_BOX_NODE_ID) {
            OutputBox* opb = dynamic_cast<OutputBox*>(n->getProcessor());
            juce::Point<float> start(0.025 * editorWidth, 0.025 * editorHeight);

            opb->setBounds(start.x, start.y, opb->getWidth(), opb->getHeight());
        }
        else {
            Pedal* ped = static_cast<Pedal*>(n->getProcessor());
            ped->setBounds(0, 0, ped->getPedalWidth(), ped->getPedalHeight()); 
        }
    }
}