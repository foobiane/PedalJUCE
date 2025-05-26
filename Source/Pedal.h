// A base pedal implementation.

#pragma once

#include <JuceHeader.h>

#include "GuiConstants.h"

#include <string>
#include <unordered_map>
#include <vector>

class Pedal : public juce::Component {
    public:
        Pedal();
        ~Pedal();

        unsigned getPedalWidth();
        unsigned getPedalHeight();
        
        // We declare a virtual function dsp, which must be implemented by any derived pedal class.
        // This function modifies an audio buffer of samples and returns a reference to the buffer.
        // By default, this function just returns the unmodified buffer.
        virtual juce::AudioBuffer<float>& dsp(juce::AudioBuffer<float>& samples);

        // Rendering logic
        void paint(juce::Graphics& g) override;
        void resized() override;

        // Dragging logic
        juce::ComponentDragger drag;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;

        // Interfacing with IO ports
        juce::Point<float> getInputPortCoord(unsigned channel);
        juce::Point<float> getOutputPortCoord(unsigned channel);
        
    protected:
        // Name stuff
        std::string name = "DEFAULT";
        juce::Font nameFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MicrogrammaDBoldExte_otf, BinaryData::MicrogrammaDBoldExte_otfSize));
        float nameFontHeight = 36;

        // Pedal dimensions
        unsigned width = 200;
        unsigned height = 300;

        // I/O ports
        unsigned numInputChannels = 1;
        unsigned numOutputChannels = 1;

        // Mapping of I/O channels to where they would be drawn on the pedals
        std::unordered_map<int, juce::Point<float>> inputChannelCoords;
        std::unordered_map<int, juce::Point<float>> outputChannelCoords;

        void mapIOChannelsToGlobalCoords();
};
