// A base pedal implementation.

#pragma once

#include <JuceHeader.h>

#include <string>
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
        
    protected:
        // Name stuff
        std::string name = "DEFAULT";
        juce::Font nameFont = juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::MicrogrammaDBoldExte_otf, BinaryData::MicrogrammaDBoldExte_otfSize));
        float nameFontHeight = 36;

        // Pedal dimensions
        unsigned width = 200;
        unsigned height = 300;
};
