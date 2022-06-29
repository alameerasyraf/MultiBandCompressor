/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace std;
using namespace juce;

//==============================================================================
MultiBandCompressorAudioProcessor::MultiBandCompressorAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       ), parameters(*this, nullptr, "Parameter", createParameters())
#endif
{
    // Default Compressor States
    pLowCompressorState = 1;
    pMidCompressorState = 1;
    pHighCompressorState = 1;
}

MultiBandCompressorAudioProcessor::~MultiBandCompressorAudioProcessor()
{
}

//==============================================================================
const juce::String MultiBandCompressorAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool MultiBandCompressorAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool MultiBandCompressorAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool MultiBandCompressorAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double MultiBandCompressorAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int MultiBandCompressorAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int MultiBandCompressorAudioProcessor::getCurrentProgram()
{
    return 0;
}

void MultiBandCompressorAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String MultiBandCompressorAudioProcessor::getProgramName (int index)
{
    return {};
}

void MultiBandCompressorAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void MultiBandCompressorAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    numChannels = getTotalNumInputChannels();

    // Calculate Filter Coefficients
    //==============================
    // First Left Channel Filters
    lowBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // First Right Channel Filters
    lowBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // Second Left Channel Filters
    lowBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // Second Right Channel Filters
    lowBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // Prepare the Compressors
    lowCompressor.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumInputChannels());
    midCompressor.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumInputChannels());
    highCompressor.prepareToPlay(sampleRate, samplesPerBlock, getTotalNumInputChannels());

    // Initialise the Compressor Parameters
    lowCompressor.setParameters(getLowRatio(), getLowThreshold(), getLowAttack(), getLowRelease(), getLowGain(), getKneeWidth());
    midCompressor.setParameters(getMidRatio(), getMidThreshold(), getMidAttack(), getMidRelease(), getMidGain(), getKneeWidth());
    highCompressor.setParameters(getHighRatio(), getHighThreshold(), getHighAttack(), getHighRelease(), getHighGain(), getKneeWidth());
}

void MultiBandCompressorAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool MultiBandCompressorAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
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

void MultiBandCompressorAudioProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    //=========================VARIABLES====================================================================//
    ScopedNoDenormals noDenormals;
    const int totalNumInputChannels = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();
    const int numSamples = buffer.getNumSamples();

    // Initialise Buffer for Each Band
    AudioSampleBuffer lowOutput;
    AudioSampleBuffer midOutput;
    AudioSampleBuffer highOutput;

    // Set each buffer to an input
    lowOutput.makeCopyOf(buffer);
    midOutput.makeCopyOf(buffer);
    highOutput.makeCopyOf(buffer);

    float sampleRate = getSampleRate();

    // In case we have more outputs than inputs, this code clears any output channels that didn't contain input data
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i) { buffer.clear(i, 0, buffer.getNumSamples()); }

    //===========================DSP PROCESSING STARTS HERE====================================================//

    // Recalculate the coefficients in case the cutoffs are altered
    // First Left Channel Filters
    lowBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterL1.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterL1.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // First Right Channel Filters
    lowBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterR1.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterR1.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // Second Left Channel Filters
    lowBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterL2.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterL2.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // Second Right Channel Filters
    lowBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, getLowCutoff()));
    lowMidBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, getLowCutoff()));
    highMidBandFilterR2.setCoefficients(coefficients.makeLowPass(sampleRate, getHighCutoff()));
    highBandFilterR2.setCoefficients(coefficients.makeHighPass(sampleRate, getHighCutoff()));

    // Apply Filter onto the buffer
    //==============================
    // First Low Band Filtering Stage
    lowBandFilterL1.processSamples(lowOutput.getWritePointer(0), numSamples);
    lowBandFilterR1.processSamples(lowOutput.getWritePointer(1), numSamples);
    // Second Low Band Filtering Stage
    lowBandFilterL2.processSamples(lowOutput.getWritePointer(0), numSamples);
    lowBandFilterR2.processSamples(lowOutput.getWritePointer(1), numSamples);

    // First Low - Mid Band Filtering Stage
    lowMidBandFilterL1.processSamples(midOutput.getWritePointer(0), numSamples);
    lowMidBandFilterR1.processSamples(midOutput.getWritePointer(1), numSamples);
    // Second Low - Mid Band Filtering Stage
    lowMidBandFilterL2.processSamples(midOutput.getWritePointer(0), numSamples);
    lowMidBandFilterR2.processSamples(midOutput.getWritePointer(1), numSamples);

    //First High - Mid Band Filtering Stage
    highMidBandFilterL1.processSamples(midOutput.getWritePointer(0), numSamples);
    highMidBandFilterR1.processSamples(midOutput.getWritePointer(1), numSamples);
    // Second High - Mid Band Filtering Stage
    highMidBandFilterL2.processSamples(midOutput.getWritePointer(0), numSamples);
    highMidBandFilterR2.processSamples(midOutput.getWritePointer(1), numSamples);

    // First High Band Filtering Stage
    highBandFilterL1.processSamples(highOutput.getWritePointer(0), numSamples);
    highBandFilterR1.processSamples(highOutput.getWritePointer(1), numSamples);
    // Second High Band Filtering Stage
    highBandFilterL2.processSamples(highOutput.getWritePointer(0), numSamples);
    highBandFilterR2.processSamples(highOutput.getWritePointer(1), numSamples);

    // Set the Compressor Parameters
    lowCompressor.setParameters(getLowRatio(), getLowThreshold(), getLowAttack(), getLowRelease(), getLowGain(), getKneeWidth());

    midCompressor.setParameters(getMidRatio(), getMidThreshold(), getMidAttack(), getMidRelease(), getMidGain(), getKneeWidth());
    highCompressor.setParameters(getHighRatio(), getHighThreshold(), getHighAttack(), getHighRelease(), getHighGain(), getKneeWidth());

    // Compress Each Band
    if (getLowCompressorState())
        lowCompressor.processBlock(lowOutput);
    if (getMidCompressorState())
        midCompressor.processBlock(midOutput);
    if (getHighCompressorState())
        highCompressor.processBlock(highOutput);

    // Sum Each Band
    buffer.clear();
    for (int channel = 0; channel < totalNumInputChannels; channel++)
    {
        buffer.addFrom(channel, 0, lowOutput, channel, 0, numSamples, 1.0 / 3.0);
        buffer.addFrom(channel, 0, midOutput, channel, 0, numSamples, 1.0 / 3.0);
        buffer.addFrom(channel, 0, highOutput, channel, 0, numSamples, 1.0 / 3.0);
    }

    // Apply the Overall Gain
    buffer.applyGain(getOverallGain());
}

AudioProcessorValueTreeState::ParameterLayout MultiBandCompressorAudioProcessor::createParameters()
{
    // Parameter Vector
    vector<unique_ptr<RangedAudioParameter>> parameterVector;

    // Low Cutoff and High Cutoff
    parameterVector.push_back(make_unique<AudioParameterFloat>("lowCutOff",     "Low CutOff Value",     150.0f, 600.0f, 450.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("highCutOff",    "High CutOff Value",    1000.0f,4000.0f, 2500.0f));

    // Low Frequency Band Parameters
    parameterVector.push_back(make_unique<AudioParameterFloat>("lowThresh",     "Low Band Threshold",   -80.0f, 0.0f,   0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("lowRatio",      "Low Band Ratio",       1.0f,   10.0f,  1.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("lowAttack",     "Low Band Attack",      5.0f,   100.0f, 5.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("lowRelease",    "Low Band Release",     5.0f,   100.0f, 5.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("lowGain",       "Low Band Gain",        0.0f,   4.0f,   1.0f));

    // Mid Frequency Band Parameters
    parameterVector.push_back(make_unique<AudioParameterFloat>("midThresh",     "Mid Band Threshold",   -80.0f, 0.0f,   0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("midRatio",      "Mid Band Ratio",       1.0f,   10.0f,  0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("midAttack",     "Mid Band Attack",      5.0f,   100.0f, 0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("midRelease",    "Mid Band Release",     5.0f,   100.0f, 0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("midGain",       "Mid Band Gain",        0.0f,   4.0f,   1.0f));

    // High Frequency Band Parameters
    parameterVector.push_back(make_unique<AudioParameterFloat>("highThresh",    "High Band Threshold",  -80.0f, 0.0f,   0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("highRatio",     "High Band Ratio",      1.0f,   10.0f,  0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("highAttack",    "High Band Attack",     5.0f,   100.0f, 0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("highRelease",   "High Band Release",    5.0f,   100.0f, 0.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("highGain",      "High Band Gain",       0.0f,   4.0f,   1.0f));

    //Knee Width and Overall Gain
    parameterVector.push_back(make_unique<AudioParameterFloat>("kneeWidth",     "Knee Width",           5.0f, 100.0f,   5.0f));
    parameterVector.push_back(make_unique<AudioParameterFloat>("overallGain",   "Overall Gain",         0.0f, 4.0f,     1.0f));

    return { parameterVector.begin(), parameterVector.end() };
}

//==============================================================================
bool MultiBandCompressorAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* MultiBandCompressorAudioProcessor::createEditor()
{
    return new MultiBandCompressorAudioProcessorEditor (*this);
}

//==============================================================================
void MultiBandCompressorAudioProcessor::getStateInformation(MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void MultiBandCompressorAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MultiBandCompressorAudioProcessor();
}