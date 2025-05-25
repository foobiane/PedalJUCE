// A base pedal implementation.

#pragma once

#include <JuceHeader.h>

#include "GuiConstants.h"

#include <string>
#include <unordered_map>
#include <vector>

class Connector : public juce::Component {
    public:
        Connector();
        ~Connector();

        void setInput(Pedal* p, int channel);
        void setOutput(Pedal* p, int channel);

        void dragStart(juce::Point<float> pos);
        void dragEnd(juce::Point<float> pos);
        void mouseDown(const MouseEvent& e) override;
        void mouseDrag(const MouseEvent& e) override;

        void paint(juce::Graphics& g) override;

    private:
        // Connectors go from one channel of a pedal's output to one channel of a pedal's input
        std::pair<Pedal*, int> outputPedalAndChannel;
        std::pair<Pedal*, int> inputPedalAndChannel;
}

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

        std::unordered_map<int, juce::Point<float>> inputChannelCoords;
        std::unordered_map<int, juce::Point<float>> outputChannelCoords;

        void mapIOChannelsToCoords();
};
