// A base pedal implementation.

#pragma once

#include <JuceHeader.h>

#include <string>
#include <unordered_set>
#include <vector>

static juce::uint32 ID = 0;
static int MAX_CONNECTION_RANGE = 10;

// Forward declaring the Connector class
class Connector;

class Pedal : public juce::Component, public juce::AudioProcessor {
    public:
        Pedal();
        juce::AudioProcessorGraph::NodeID getUIDAsNodeID();

        int getPedalWidth();
        int getPedalHeight();

        /*
         * AudioProcessor Methods
         * Default implementations for all of these functions are provided, though some should be
         * redefined in classes that inherit the Pedal class. These are marked appropriately below.
         * Some functions are not used in this implementation, and return dummy values.
         */
        virtual const juce::String getName() const override;
        virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        virtual void releaseResources() override; // INHERITORS REDEFINE
        virtual void processBlock(juce::AudioBuffer<float> &buffer, juce::MidiBuffer &midiMessages) override; // INHERITORS REDEFINE
        virtual double getTailLengthSeconds() const override;
        virtual bool acceptsMidi() const override; // INHERITORS REDEFINE
        virtual bool producesMidi() const override; // INHERITORS REDEFINE
        virtual juce::AudioProcessorEditor* createEditor() override; // UNUSED
        virtual bool hasEditor() const override; // UNUSED
        virtual int getNumPrograms() override; // UNUSED
        virtual int getCurrentProgram() override; // UNUSED
        virtual void setCurrentProgram(int index) override; // UNUSED
        virtual const juce::String getProgramName(int index) override; // UNUSED
        virtual void changeProgramName(int index, const juce::String &newName) override; // UNUSED
        virtual void getStateInformation(juce::MemoryBlock &destData) override; // UNUSED
        virtual void setStateInformation(const void *data, int sizeInBytes) override; // UNUSED

        // Rendering logic
        void paint(juce::Graphics& g) override;
        void resized() override;

        // I/O
        int getNumInputChannels();
        int getNumOutputChannels();
        juce::Point<int> getGlobalPositionForInputChannel(int channel);
        juce::Point<int> getGlobalPositionForOutputChannel(int channel);
        void trackConnector(Connector* c);
        void untrackConnector(Connector* c);
        void updateAllConnectors();

        // Dragging logic
        juce::ComponentDragger drag;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        
    protected:
        // Name stuff
        std::string name = "DEFAULT";
        juce::Font nameFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MicrogrammaDBoldExte_otf, BinaryData::MicrogrammaDBoldExte_otfSize));
        float nameFontHeight = 36;

        // Pedal dimensions
        int width = 200;
        int height = 300;

        // I/O
        int numOutputChannels = 1;
        int numInputChannels = 1;

        // DSP stuff
        int sr;
        int blockSize;

    private:
        struct Port {
            juce::Point<int> position; // local position
            int channel;
        };

        std::vector<Port> inputPorts;
        std::vector<Port> outputPorts;
        void initializePorts();
        
        std::unordered_set<Connector*> connectors;

        // UID
        juce::uint32 uid;
};

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
        Connector(juce::AudioProcessorGraph* graph, Pedal* s, int channel);

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

        void attemptConnection(const juce::MouseEvent& e);

        juce::Path cablePath;
        juce::Point<int> cursorPosition;

        juce::AudioProcessorGraph* g;
};