/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "Constants.h"
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

    void addIOBoxesToEditor();

    template <typename T>
    Pedal* addPedalToEditor();
    
    template <typename T>
    void removePedalFromEditor(Pedal* ped);

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    PedalJUCEAudioProcessor& audioProcessor;
    
    int editorWidth = DEFAULT_EDITOR_WIDTH, editorHeight = DEFAULT_EDITOR_HEIGHT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalJUCEAudioProcessorEditor)
};
