/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Connector.h"
#include "Pedal.h"
#include "PluginProcessor.h"

#include <unordered_set>

//==============================================================================
/**
*/
class PedalJUCEAudioProcessorEditor  : public juce::AudioProcessorEditor
{
public:
    PedalJUCEAudioProcessorEditor (PedalJUCEAudioProcessor&);
    ~PedalJUCEAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

    void dragConnector(const juce::MouseEvent& e);
    void beginConnectorDrag(std::pair<Pedal*, int> start, std::pair<Pedal*, int> end, const juce::MouseEvent& e);
    void endConnectorDrag(const juce::MouseEvent& e);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PedalJUCEAudioProcessor& audioProcessor;

    std::unordered_set<Connector*> connectors;
    Connector* draggingConnector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalJUCEAudioProcessorEditor)
};
