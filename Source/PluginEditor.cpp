/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Pedal.h"
// #include "Connector.h"

#include <memory>
#include <type_traits>

template <typename T>
Pedal* PedalJUCEAudioProcessorEditor::addPedalToEditor() {
    static_assert(std::is_base_of<Pedal, T>::value, "T must inherit from Pedal");

    // Adding to the graph must be handled outside of the Pedal class due to the use of unique_ptr.
    // (Why the fuck do we do this to ourselves?)
    std::unique_ptr<Pedal> ped = std::make_unique<T>(&audioProcessor.connectionMap);
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

template <typename T>
void PedalJUCEAudioProcessorEditor::removePedalFromEditor(Pedal* p) {
    static_assert(std::is_base_of<Pedal, T>::value, "T must inherit from Pedal");

    for (int i = 0; i < p->getNumOutputChannels(); i++)
        removeChildComponent(p->connectors[i]);
    
    for (int i = 0; i < p->getNumInputChannels(); i++)
        removeChildComponent(p->inputPorts[i]);

    removeChildComponent(p);

    audioProcessor.connectionMap.removeNode(p->getNodeID());
    delete p;
}   

//==============================================================================
PedalJUCEAudioProcessorEditor::PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    addPedalToEditor<Pedal>();
    addPedalToEditor<Pedal>();

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1200, 900);
}

PedalJUCEAudioProcessorEditor::~PedalJUCEAudioProcessorEditor()
{
    // TODO: Clean up AudioProcessorGraph
}

//==============================================================================
void PedalJUCEAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void PedalJUCEAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..

    for (juce::AudioProcessorGraph::Node* pedalNode: audioProcessor.connectionMap.getNodes()) {
        Pedal* ped = static_cast<Pedal*>(pedalNode->getProcessor());
        ped->setBounds(0, 0, ped->getPedalWidth(), ped->getPedalHeight()); 
    }
}