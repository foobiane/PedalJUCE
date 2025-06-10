#pragma once

#include <JuceHeader.h>

#include "Pedal.h"

const juce::AudioProcessorGraph::NodeID INPUT_BOX_NODE_ID = juce::AudioProcessorGraph::NodeID(0);
const juce::AudioProcessorGraph::NodeID OUTPUT_BOX_NODE_ID = juce::AudioProcessorGraph::NodeID(1);

class InputBox : public juce::AudioProcessorGraph::AudioGraphIOProcessor, public juce::Component {
    public:
        InputBox(juce::AudioProcessorGraph* graph, int w, int h);
        ~InputBox();

        int getNumChannels() { return numChannels; }
        void setWidth(int newWidth) { width = newWidth; }
        int getWidth() { return width; }
        void setHeight(int newHeight) { height = newHeight; }
        int getHeight() { return height; }
        
        void paint(juce::Graphics& g) override;
        void resized() override;

        std::vector<Connector*> ports;

    private:
        int width, height;
        int numChannels;

        void initializePorts();
        void updatePorts();

        juce::AudioProcessorGraph* g;
};

class OutputBox : public juce::AudioProcessorGraph::AudioGraphIOProcessor, public juce::Component {
    public:
        OutputBox(juce::AudioProcessorGraph* graph, int w, int h);
        ~OutputBox();
        
        int getNumChannels() { return numChannels; }
        void setWidth(int newWidth) { width = newWidth; }
        int getWidth() { return width; }
        void setHeight(int newHeight) { height = newHeight; }
        int getHeight() { return height; }

        juce::Point<int> getPositionOfPort(int channel) { return ports[channel]->getPosition().translated(MAX_CONNECTION_RANGE, MAX_CONNECTION_RANGE); }

        void paint(juce::Graphics& g) override;
        void resized() override;

        std::vector<InputPort*> ports;

    private:
        int width, height;
        int numChannels;
        
        void initializePorts();
        void updatePorts();

        juce::AudioProcessorGraph* g;
};