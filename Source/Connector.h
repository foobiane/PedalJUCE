#pragma once

#include <JuceHeader.h>

#include "Pedal.h"
#include "PluginEditor.h"
#include "PluginProcessor.h"

class Connector : public juce::Component {
    public:
        explicit Connector(PedalJUCEAudioProcessorEditor& editor, Graph<Pedal*>& connectionMap);

        void setInput(Pedal* p, int channel);
        void setOutput(Pedal* p, int channel);

        std::pair<std::pair<Pedal*, int>, std::pair<Pedal*, int>> getConnection();

        void dragStart(juce::Point<float> pos);
        void dragEnd(juce::Point<float> pos);

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;

        std::pair<juce::Point<float>, juce::Point<float>> getConnectionCoords();
        void update();
        void resizeToFit();

        bool hitTest(int x, int y) override;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        std::pair<float, float> distancesFromEnds(juce::Point<float> pos);

        // Connectors go from one channel of a pedal's output to one channel of a pedal's input
        std::pair<Pedal*, int> outputPedalAndChannel;
        std::pair<Pedal*, int> inputPedalAndChannel;

        Point<float> lastInputPos;
        Point<float> lastOutputPos;

        juce::Path linePath;
        juce::Path hitPath;

        // We maintain a reference to the editor for connector drag logic
        PedalJUCEAudioProcessorEditor& editor;

        // We also maintain a reference to the pedal graph for modeling our connections
        Graph<Pedal*>& connectionMap;

        bool dragging;
};