/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BasicDelayAudioProcessor::BasicDelayAudioProcessor()
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
}

BasicDelayAudioProcessor::~BasicDelayAudioProcessor()
{
}

//==============================================================================
const juce::String BasicDelayAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool BasicDelayAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool BasicDelayAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool BasicDelayAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double BasicDelayAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int BasicDelayAudioProcessor::getNumPrograms()
{
    return 1;
}

int BasicDelayAudioProcessor::getCurrentProgram()
{
    return 0;
}

void BasicDelayAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String BasicDelayAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void BasicDelayAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

//==============================================================================
void BasicDelayAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (samplesPerBlock);

    delayBuffer.setSize (getTotalNumOutputChannels(),
                         static_cast<int> (sampleRate * 2.0));

    delayBuffer.clear();

    delaySamples = juce::roundToInt (sampleRate * 0.5);
    writePosition = 0;
}

void BasicDelayAudioProcessor::releaseResources()
{
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BasicDelayAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void BasicDelayAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    static bool hasLoggedChannelCounts = false;

    if (! hasLoggedChannelCounts)
    {
        DBG ("Inputs: " << getTotalNumInputChannels()
                       << " Outputs: "
                       << getTotalNumOutputChannels());
        hasLoggedChannelCounts = true;
    }

    juce::ScopedNoDenormals noDenormals;
    juce::ignoreUnused (midiMessages);

    const auto totalInputChannels = getTotalNumInputChannels();
    const auto totalOutputChannels = getTotalNumOutputChannels();
    const auto passThroughNumSamples = buffer.getNumSamples();

    float inputPeak = 0.0f;

    for (auto channel = 0; channel < totalInputChannels; ++channel)
    {
        const auto* channelData = buffer.getReadPointer (channel);

        for (auto sample = 0; sample < passThroughNumSamples; ++sample)
            inputPeak = juce::jmax (inputPeak, std::abs (channelData[sample]));
    }

    static int inputPeakDebugCounter = 0;

    if (++inputPeakDebugCounter % 30 == 0)
        DBG ("Input peak: " << inputPeak);

    for (auto channel = totalInputChannels; channel < totalOutputChannels; ++channel)
        buffer.clear (channel, 0, passThroughNumSamples);
}

//==============================================================================
bool BasicDelayAudioProcessor::hasEditor() const
{
    return true;
}

juce::AudioProcessorEditor* BasicDelayAudioProcessor::createEditor()
{
    return new BasicDelayAudioProcessorEditor (*this);
}

//==============================================================================
void BasicDelayAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    juce::ignoreUnused (destData);
}

void BasicDelayAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    juce::ignoreUnused (data, sizeInBytes);
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new BasicDelayAudioProcessor();
}

/*
Temporary padding to force this Xcode overwrite to be longer than any stale editor buffer.
This comment is harmless and can be removed with the diagnostics later.
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
................................................................................
*/
