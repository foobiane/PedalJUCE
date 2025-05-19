#pragma once

#include <JuceHeader.h>

#include <string>
#include <vector>

enum class ParameterType { FootSwitch, Button, Barrel, HorizontalSlider, VerticalSlider };

struct Parameter {
    std::string name;
    float min, max;
    float currValue;
    ParameterType type;
};

class Pedal {
    public:
        Pedal();
        ~Pedal();

        // We declare a virtual function dsp, which must be implemented by any derived pedal class.
        // This function modifies an audio buffer of samples and returns a reference to the buffer.
        // By default, this function just returns the unmodified buffer.
        virtual juce::AudioBuffer<float>& dsp(juce::AudioBuffer<float>& samples) {
            return samples;
        }

    protected:
        std::string name = "DEFAULT";

        // We maintain a vector of our pedal parameters, which can be readily rendered and modified
        // at any time.
        std::vector<Parameter> parameters;
};

Pedal::Pedal() {
    // Nothing for now
}

Pedal::~Pedal() {
    // Nothing for now
}
