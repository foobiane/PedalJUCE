#include "../../Source/Pedal.h"

class InputBox : public Pedal {
    public:
        InputBox();
        InputBox(unsigned numChannels);
        ~InputBox();

        void paint(juce::Graphics& g) override;

    private:
        unsigned numChannels;
};