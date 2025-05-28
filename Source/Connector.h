#pragma once

#include <JuceHeader.h>

#include "Pedal.h"

static int MAX_CONNECTION_RANGE = 10;

// This class is still being designed.
// Here's what my abstract ideas are for a connector:
// 1. Connectors sit on top of our pedals and are *not* subcomponents of them (but they may be tracked by them).
// 2. Connectors are created when a user drags from the output port of a pedal to anywhere on the screen.
// 3. Connectors constantly repaint themselves while the user is dragging.
// 4. Connectors "connect" to the input ports of other pedals, and have a sensitivity of some amount of pixels.
// 5. Once connected, connectors will modify the AudioProcessorGraph to reflect the connection between ports.
// 6. Connectors are supposed to look like curled up cords.
// 7. Connectors will move when the pedals they're connected to move.

class Connector : public juce::Component {
    public:
        Connector(juce::AudioProcessorGraph& g, Pedal* s, int channel);

        bool isConnected();
        void disconnect();

        void resetBounds();
        void adjustBounds();

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        struct PedalAndChannel {
            Pedal* pedal;
            int channel;
        };

        PedalAndChannel start;
        PedalAndChannel end;
        
        bool dragging = false;

        void attemptConnection();
        juce::Point<int> getGlobalPositionOf(const Point<int>& p);

        juce::Path cablePath;

        juce::AudioProcessorGraph& g;
}