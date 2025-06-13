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
void PedalJUCEAudioProcessorEditor::addPedalToEditor(std::unique_ptr<Pedal> ped) {
    juce::AudioProcessorGraph::NodeID uid = ped->getNodeID();
    audioProcessor.connectionMap.addNode(std::move(ped), uid);

    // Getting our pedal back from the graph
    Pedal* p = static_cast<Pedal*>(audioProcessor.connectionMap.getNodeForId(uid)->getProcessor());
    
    addAndMakeVisible(p);

    for (int i = 0; i < p->getNumOutputChannels(); i++)
        addAndMakeVisible(p->connectors[i]);
    
    for (int i = 0; i < p->getNumInputChannels(); i++)
        addAndMakeVisible(p->inputPorts[i]);

    pedals.push_back(p);
}

// Removes a pedal from the editor interface.
void PedalJUCEAudioProcessorEditor::removePedalFromEditor(Pedal* p) {
    for (int i = 0; i < p->getNumOutputChannels(); i++)
        removeChildComponent(p->connectors[i]);
    
    for (int i = 0; i < p->getNumInputChannels(); i++) {
        if (p->inputPorts[i]->getIncomingConnector() != nullptr)
            p->inputPorts[i]->getIncomingConnector()->disconnect();
        
        removeChildComponent(p->inputPorts[i]);
    }

    removeChildComponent(p);
    pedals.erase(std::find(pedals.begin(), pedals.end(), p)); // TODO: Switch the data structure to a set so this search is less expensive
    audioProcessor.connectionMap.removeNode(p->getNodeID());
}

// Adds IO boxes to the editor interface.
void PedalJUCEAudioProcessorEditor::addIOBoxesToEditor() {
    std::unique_ptr<InputBox> ipb = std::make_unique<InputBox>(&audioProcessor.connectionMap, 0.1 * editorWidth, 0.95 * editorHeight);
    std::unique_ptr<OutputBox> opb = std::make_unique<OutputBox>(&audioProcessor.connectionMap, 0.1 * editorWidth, 0.95 * editorHeight);

    juce::AudioProcessorGraph::Node::Ptr ipb_node = audioProcessor.connectionMap.addNode(std::move(ipb), INPUT_BOX_NODE_ID); 
    juce::AudioProcessorGraph::Node::Ptr opb_node = audioProcessor.connectionMap.addNode(std::move(opb), OUTPUT_BOX_NODE_ID);

    InputBox* ipb_ptr = dynamic_cast<InputBox*>(ipb_node->getProcessor());
    addAndMakeVisible(ipb_ptr);
    
    for (int i = 0; i < ipb_ptr->getNumChannels(); i++)
        addAndMakeVisible(ipb_ptr->ports[i]);
    
    OutputBox* opb_ptr = dynamic_cast<OutputBox*>(opb_node->getProcessor());
    addAndMakeVisible(opb_ptr);

    for (int i = 0; i < opb_ptr->getNumChannels(); i++)
        addAndMakeVisible(opb_ptr->ports[i]);

    outputBox = opb_ptr;
    inputBox = ipb_ptr;
}

void PedalJUCEAudioProcessorEditor::removeIOBoxesFromEditor(InputBox* ipb, OutputBox* opb) {
    for (int i = 0; i < ipb->getNumChannels(); i++)
        removeChildComponent(ipb->ports[i]);

    removeChildComponent(ipb);

    for (int i = 0; i < opb->getNumChannels(); i++)
        removeChildComponent(opb->ports[i]);

    removeChildComponent(opb);

    audioProcessor.connectionMap.removeNode(INPUT_BOX_NODE_ID);
    audioProcessor.connectionMap.removeNode(OUTPUT_BOX_NODE_ID);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Editor Subcomponent Functionality
///////////////////////////////////////////////////////////////////////////////////////////////////

void PedalJUCEAudioProcessorEditor::initializePedalSelect() {
    addAndMakeVisible(pedalSelect);

    for (int i = 0; i < PEDAL_TYPES.size(); i++)
        pedalSelect.addItem(PEDAL_TYPES[i], i+1);

    pedalSelect.setSelectedItemIndex(1);
}

void PedalJUCEAudioProcessorEditor::initializeAddPedalButton() {
    addAndMakeVisible(addPedal);

    addPedal.onClick = [this](){
        addPedalToEditor(getPedalFromName(pedalSelect.getText().toStdString(), this));
    };
}

///////////////////////////////////////////////////////////////////////////////////////////////////
// Editor Main Functionality
///////////////////////////////////////////////////////////////////////////////////////////////////

PedalJUCEAudioProcessorEditor::PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addIOBoxesToEditor();
    // addPedalToEditor(getPedalFromName("GainStage", &audioProcessor.connectionMap, editorWidth, editorHeight));

    initializePedalSelect();
    initializeAddPedalButton();

    deviceManager.initialiseWithDefaultDevices(2, 2); 
    deviceManager.addAudioCallback(&player);
    player.setProcessor(&audioProcessor);

    setSize(editorWidth, editorHeight);
}

PedalJUCEAudioProcessorEditor::~PedalJUCEAudioProcessorEditor() {
    deviceManager.removeAudioCallback(&player);
    
    for (Pedal* p : pedals)
        removePedalFromEditor(p);
    
    removeIOBoxesFromEditor(inputBox, outputBox);

    // audioProcessor.connectionMap.clear();
}

void PedalJUCEAudioProcessorEditor::paint (juce::Graphics& g) {
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
}

void PedalJUCEAudioProcessorEditor::resized() {
    addPedal.setBounds((editorWidth - 135) / 2.0f, editorHeight - 35, 25, 25);
    pedalSelect.setBounds((editorWidth - 135) / 2.0f + 35, editorHeight - 35, 100, 25);

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