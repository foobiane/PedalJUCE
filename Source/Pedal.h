#include <string>
#include <vector>

#include <JuceHeader.h>

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

        int x, y;

        // We declare a virtual function dsp, which must be implemented by any derived pedal class.
        // This function modifies an audio buffer of samples and returns a reference to the buffer.
        virtual juce::AudioBuffer<float>& dsp(juce::AudioBuffer<float>& samples);

        void render(juce::Graphics& g);

    protected:
        std::string name = "DEFAULT";

        // TODO: Tweak these
        juce::Font nameFont("Arial", 12.0f, juce::Font::plain);
        juce::Font paramFont("Arial", 12.0f, juce::Font::plain);

        // We maintain a vector of our pedal parameters, which can be readily rendered and modified
        // at any time.
        std::vector<Parameter> parameters;

        int width = 100, height = 100;
};

Pedal::Pedal() {
    // Nothing for now
}

Pedal::~Pedal() {
    // Nothing for now
}

void Pedal::render(juce::Graphics& g) {
    g.fillRect(x, y, width, height);

    g.setFont(nameFont);
    g.drawFittedText(name, x, y, width, 0.125 * height, juce::Justification::centered, 1);

    g.setFont(paramFont);

    // Renders each of the parameters as knobs or sliders on the pedal body
    for (int i = 0; i < parameters.size(); i++) {
        Parameter& p = parameters[i];

        // TODO: Render parameters
    }
}

