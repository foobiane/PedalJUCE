/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "Connector.h"
#include "Pedal.h"

#include <memory>

//==============================================================================
PedalJUCEAudioProcessorEditor::PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    std::unique_ptr<Pedal> p = std::make_unique<Pedal>();
    audioProcessor.connectionMap.addNode(p, p.get()->getUID());

    for (juce::AudioProcessorGraph::Node* pedalNode : audioProcessor.connectionMap.getNodes()) {
        Pedal* ped = static_cast<Pedal*>(pedalNode->getProcessor())
        
        for (int i = 0; i < ped->getNumOutputChannels(); i++) {
            Connector* c = new Connector(audioProcessor.connectionMap, ped, i);
            ped->trackConnector(c);
            addAndMakeVisible(c);
        }

        addAndMakeVisible(ped);
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

    for (juce::AudioProcessorGraph::Node* pedalNode: audioProcessor.connectionMap.getNodes()) {
        Pedal* ped = static_cast<Pedal*>(pedalNode->getProcessor());
        ped->setBounds(0, 0, ped->getPedalWidth(), ped->getPedalHeight()); 
    }
}