/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Constants.h"
#include "IOBoxes.h"
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

private:
    PedalJUCEAudioProcessor& audioProcessor;
    
    void addIOBoxesToEditor();
    Pedal* addPedalToEditor(std::unique_ptr<Pedal> ped);
    void removePedalFromEditor(Pedal* ped);

    int editorWidth = DEFAULT_EDITOR_WIDTH, editorHeight = DEFAULT_EDITOR_HEIGHT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalJUCEAudioProcessorEditor)
};
