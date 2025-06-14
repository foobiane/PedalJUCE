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

    void addIOBoxesToEditor();
    void removeIOBoxesFromEditor(InputBox* ipb, OutputBox* opb);
    void addPedalToEditor(std::unique_ptr<Pedal> ped);
    void removePedalFromEditor(Pedal* ped);

    PedalJUCEAudioProcessor& audioProcessor;

private:
    void initializePedalSelect();
    void initializeAddPedalButton();

    juce::ComboBox pedalSelect;

    class AddPedalButton : public juce::TextButton {
      public:
        void paintButton(juce::Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
          g.setColour(juce::Colours::lightblue);
          g.fillEllipse(getLocalBounds().toFloat());

          g.setColour(juce::Colours::white);
          g.setFont(juce::Font(juce::FontOptions("Arial", getLocalBounds().getHeight(), 0)));
          g.drawFittedText("+", getLocalBounds(), juce::Justification::centred, 1);
        }
    };

    AddPedalButton addPedal;

    std::vector<Pedal*> pedals;
    InputBox* inputBox;
    OutputBox* outputBox;
        
    juce::AudioDeviceManager deviceManager;
    juce::AudioProcessorPlayer player;

    int editorWidth = DEFAULT_EDITOR_WIDTH, editorHeight = DEFAULT_EDITOR_HEIGHT;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PedalJUCEAudioProcessorEditor)
};
