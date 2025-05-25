#include "../../Source/Pedal.h"

class OutputBox : public Pedal {
    public:
        OutputBox();
        OutputBox(unsigned numChannels);
        ~OutputBox();

        void paint(juce::Graphics& g) override;

    private:
        unsigned numChannels;
}