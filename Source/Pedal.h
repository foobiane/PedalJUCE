// A base pedal implementation.

#pragma once

#include <JuceHeader.h>

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

static uint32_t ID = 0;

class Pedal : public juce::Component, public juce::AudioProcessor {
    public:
        Pedal();
        uint32_t getUID();

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
        void connectToInputPort(Connector* c);

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

        std::vector<Port*> inputPorts;
        std::vector<Port*> outputPorts;

        // UID
        uint32_t uid;
};

