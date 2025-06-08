/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PedalJUCEAudioProcessor::PedalJUCEAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    connectionMap.setPlayConfigDetails(
        getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        DEFAULT_SAMPLE_RATE,
        DEFAULT_BLOCK_SIZE
    );
}

PedalJUCEAudioProcessor::~PedalJUCEAudioProcessor()
{
}

//==============================================================================
const juce::String PedalJUCEAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PedalJUCEAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PedalJUCEAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PedalJUCEAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PedalJUCEAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PedalJUCEAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PedalJUCEAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PedalJUCEAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String PedalJUCEAudioProcessor::getProgramName (int index)
{
    return {};
}

void PedalJUCEAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void PedalJUCEAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    connectionMap.setPlayConfigDetails(
        getMainBusNumInputChannels(),
        getMainBusNumOutputChannels(),
        sampleRate,
        samplesPerBlock
    );

    connectionMap.prepareToPlay(sampleRate, samplesPerBlock);
}

void PedalJUCEAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PedalJUCEAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void PedalJUCEAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    for (int i = getTotalNumInputChannels(); i < getTotalNumOutputChannels(); i++)
        buffer.clear(i, 0, buffer.getNumSamples());
    
    connectionMap.processBlock(buffer, midiMessages);
}

//==============================================================================
bool PedalJUCEAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* PedalJUCEAudioProcessor::createEditor()
{
    return new PedalJUCEAudioProcessorEditor (*this);
}

//==============================================================================
void PedalJUCEAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void PedalJUCEAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PedalJUCEAudioProcessor();
}
