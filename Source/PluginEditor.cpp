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

//==============================================================================
PedalJUCEAudioProcessorEditor::PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    std::unique_ptr<Pedal> ped = std::make_unique<Pedal>(&audioProcessor.connectionMap);

    // Because we can only have one pointer at a time with unique pointers, we have to do this
    // annoying step of getting the UID first before it gets moved.
    juce::AudioProcessorGraph::NodeID uid = ped->getNodeID();

    audioProcessor.connectionMap.addNode(std::move(ped), uid);

    for (juce::AudioProcessorGraph::Node* pedalNode : audioProcessor.connectionMap.getNodes()) {
        Pedal* pdl = static_cast<Pedal*>(pedalNode->getProcessor());
        
        for (int i = 0; i < pdl->getNumOutputChannels(); i++)
            addAndMakeVisible(pdl->connectors[i]);
        
        for (int i = 0; i < pdl->getNumInputChannels(); i++)
            addAndMakeVisible(pdl->inputPorts[i]);

        addAndMakeVisible(pdl);
    }

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
        ped->setBounds(100, 100, ped->getPedalWidth(), ped->getPedalHeight()); 
    }
}