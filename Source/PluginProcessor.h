/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Compressor.h"

using namespace std;
using namespace juce;

//==============================================================================
/**
*/
class MultiBandCompressorAudioProcessor  : public juce::AudioProcessor
{
public:
    //==============================================================================
    MultiBandCompressorAudioProcessor();
    ~MultiBandCompressorAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    //==============================================================================
    // Compressor States
    void setLowCompressorState(int lowCompressorState)      { pLowCompressorState = lowCompressorState; }
    void setMidCompressorState(int midCompressorState)      { pMidCompressorState = midCompressorState; }
    void setHighCompressorState(int highCompressorState)    { pHighCompressorState = highCompressorState; }

    //==============================================================================
    // Getter Functions for each parameter

    float getOverallGain()
    {
        auto pOverallGain = parameters.getRawParameterValue("overallGain")->load();
        return pOverallGain;
    }
    float getKneeWidth()
    {
        auto kneeWidth = parameters.getRawParameterValue("kneeWidth")->load();
        return kneeWidth;
    }

    // Cutoff Parameters
    float getLowCutoff()
    {
        auto pLowPassCutoff = parameters.getRawParameterValue("lowCutOff")->load();
        return pLowPassCutoff;
    }
    float getHighCutoff()
    {
        auto pHighPassCutoff = parameters.getRawParameterValue("highCutOff")->load();
        return pHighPassCutoff;
    }

    // Low Compressor Parameters
    float getLowGain()
    {
        auto pLowGain = parameters.getRawParameterValue("lowGain")->load();
        return pLowGain;
    }
    float getLowThreshold()
    {
        auto pLowThreshold = parameters.getRawParameterValue("lowThresh")->load();
        return pLowThreshold;
    }
    float getLowRatio()
    {
        auto pLowRatio = parameters.getRawParameterValue("lowRatio")->load();
        return pLowRatio;
    }
    float getLowAttack()
    {
        auto pLowAttack = parameters.getRawParameterValue("lowAttack")->load();
        return pLowAttack;
    }
    float getLowRelease()
    {
        auto pLowRelease = parameters.getRawParameterValue("lowRelease")->load();
        return pLowRelease;
    }

    // Mid Compressor Parameters
    float getMidGain()
    {
        auto pMidGain = parameters.getRawParameterValue("midGain")->load();
        return pMidGain;
    }
    float getMidThreshold()
    {
        auto pMidThreshold = parameters.getRawParameterValue("midThresh")->load();
        return pMidThreshold;
    }
    float getMidRatio()
    {
        auto pMidRatio = parameters.getRawParameterValue("midRatio")->load();
        return pMidRatio;
    }
    float getMidAttack()
    {
        auto pMidAttack = parameters.getRawParameterValue("midAttack")->load();
        return pMidAttack;
    }
    float getMidRelease()
    {
        auto pMidRelease = parameters.getRawParameterValue("midRelease")->load();
        return pMidRelease;
    }

    // High Compressor Parameters
    float getHighGain()
    {
        auto pHighGain = parameters.getRawParameterValue("highGain")->load();
        return pHighGain;
    }
    float getHighThreshold()
    {
        auto pHighThreshold = parameters.getRawParameterValue("highThresh")->load();
        return pHighThreshold;
    }
    float getHighRatio()
    {
        auto pHighRatio = parameters.getRawParameterValue("highRatio")->load();
        return pHighRatio;
    }
    float getHighAttack()
    {
        auto pHighAttack = parameters.getRawParameterValue("highAttack")->load();
        return pHighAttack;
    }
    float getHighRelease()
    {
        auto pHighRelease = parameters.getRawParameterValue("highRelease")->load();
        return pHighRelease;
    }

    // Compressor States
    float getLowCompressorState()                           { return pLowCompressorState; }
    float getMidCompressorState()                           { return pMidCompressorState; }
    float getHighCompressorState()                          { return pHighCompressorState; }

    AudioProcessorValueTreeState    parameters;

private:
    
    //============================FILTER DEFINITIONS============================================//
    // Low Frequency Band Left and Right
    IIRFilter   lowBandFilterL1,        lowBandFilterL2,        lowBandFilterR1,        lowBandFilterR2;

    // Mid Frequency Band Left and Right
    IIRFilter   lowMidBandFilterL1,     lowMidBandFilterL2,     lowMidBandFilterR1,     lowMidBandFilterR2;
    IIRFilter   highMidBandFilterL1,    highMidBandFilterL2,    highMidBandFilterR1,    highMidBandFilterR2;

    // High Frequency Band Left and Right
    IIRFilter   highBandFilterL1,       highBandFilterL2,       highBandFilterR1,       highBandFilterR2;

    // Coefficient values
    IIRCoefficients coefficients;

    // Compressors
    Compressor   lowCompressor;
    Compressor   midCompressor;
    Compressor   highCompressor;

    // Parameters
    int                         numChannels;
    float                       pOverallGain;
    float                       kneeWidth;

    // Cutoff Frequencies
    float                       pLowPassCutoff;
    float                       pHighPassCutoff;

    // Compressor Parameters
    float           pLowGain,   pLowThreshold,  pLowRatio,  pLowAttack,     pLowRelease;
    float           pMidGain,   pMidThreshold,  pMidRatio,  pMidAttack,     pMidRelease;
    float           pHighGain,  pHighThreshold, pHighRatio, pHighAttack,    pHighRelease;

    // Compressor States
    int             pLowCompressorState;
    int             pMidCompressorState;
    int             pHighCompressorState;

    //=====================FUNCTIONS===============================================================//
    AudioProcessorValueTreeState::ParameterLayout createParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiBandCompressorAudioProcessor)
};
