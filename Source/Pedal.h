// A base pedal implementation.

#pragma once

#include <JuceHeader.h>

#include "Constants.h"

#include <string>
#include <unordered_set>
#include <vector>

static juce::uint32 ID = 2;

// Forward declaring the Pedal class for use in Connector
class Pedal;

// Forward declaring our editor class for use in Pedal
class PedalJUCEAudioProcessorEditor;

class Connector : public juce::Component {
    public:
        Connector(juce::AudioProcessorGraph* graph, juce::AudioProcessorGraph::NodeID id, int channel, juce::Point<int> startPt);
        ~Connector();

        bool isConnected();
        void disconnect();

        void updateStartPoint(juce::Point<int> newStart);
        void updateEndPoint(juce::Point<int> newEnd);

        void resetBounds();
        void adjustBounds();

        juce::Point<int> getStartPoint();

        Pedal* getStartPedal();
        Pedal* getEndPedal();

        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        void mouseUp(const juce::MouseEvent& e) override;

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        juce::AudioProcessorGraph::NodeAndChannel start;
        juce::AudioProcessorGraph::NodeAndChannel end;
        
        // juce::Point<int> intermediatePoint1;
        // juce::Point<int> intermediatePoint2; // Used for adjusting the connector position, maybe implement me

        bool connected = false;
        bool dragging = false;
        bool showingControls = false;

        void attemptConnection();

        juce::Path cablePath;
        float pathThickness = 8.0f;

        juce::Point<int> startPoint; // relative to the global parent class, NOT this class's coordinates
        juce::Point<int> endPoint;

        juce::AudioProcessorGraph* g;
};

class InputPort : public juce::Component {
    public:
        InputPort();
        ~InputPort();

        void setIncomingConnector(Connector* c);
        Connector* getIncomingConnector();

        void paint(juce::Graphics& g) override;
        void resized() override;

    private:
        Connector* incomingConnector;
};

class Pedal : public juce::Component, public juce::AudioProcessor {
    public:
        Pedal(PedalJUCEAudioProcessorEditor* e, int w = 200, int h = 300, int nfh = 36);
        ~Pedal();

        virtual int getPedalWidth() final;
        virtual int getPedalHeight() final;
        virtual juce::AudioProcessorGraph::NodeID getNodeID() final;

        /*
         * AudioProcessor Methods
         * Default implementations for all of these functions are provided, though some should be
         * redefined in classes that inherit the Pedal class. These are marked appropriately below.
         * Some functions are not used in this implementation, and return dummy values.
         */
        virtual const juce::String getName() const override;
        virtual void prepareToPlay (double sampleRate, int maximumExpectedSamplesPerBlock) override;
        virtual void releaseResources() override; // INHERITORS REDEFINE (IF NECESSARY)
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
        virtual int getNumInputChannels() final;
        virtual int getNumOutputChannels() final;

        std::vector<Connector*> connectors; // send output
        std::vector<InputPort*> inputPorts; // receive input

        virtual juce::Point<int> getPositionOfInputPort(int channel) final;
        virtual juce::Point<int> getPositionOfOutputPort(int channel) final;
    
        // Dragging logic
        juce::ComponentDragger drag;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDrag(const juce::MouseEvent& e) override;
        
    protected:
        // Name stuff
        std::string name = "Default";
        juce::Font nameFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MicrogrammaDBoldExte_otf, BinaryData::MicrogrammaDBoldExte_otfSize));
        float nameFontHeight = 36;

        // Pedal dimensions
        int width = 200;
        int height = 300;
        int pedalThickness = 25;

        // I/O
        int numOutputChannels = 1;
        int numInputChannels = 1;
        virtual void updatePorts() final;

        // DSP stuff
        int sr = DEFAULT_SAMPLE_RATE;
        int blockSize = DEFAULT_BLOCK_SIZE;
        virtual void prepareBasics(double sampleRate, int maximumExpectedSamplesPerBlock) final;

        // Pedal removing functionality
        bool showingTrash = false;
        class TrashIcon : public juce::Component {
            public:
                TrashIcon(PedalJUCEAudioProcessorEditor* e, Pedal* p) : editor(e), pedal(p) { }

                void paint(juce::Graphics& g) override;
                void mouseDown(const juce::MouseEvent& e) override;

            private:
                PedalJUCEAudioProcessorEditor* editor;
                Pedal* pedal;
        };
        TrashIcon trashIcon;

    private:
        void initializePorts();

        // UID
        juce::AudioProcessorGraph::NodeID uid;

        PedalJUCEAudioProcessorEditor* editor;
        juce::AudioProcessorGraph* g;
};