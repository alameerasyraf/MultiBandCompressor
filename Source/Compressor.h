/*
  ==============================================================================

    This file contains the basic functions for a Compressor.

  ==============================================================================
*/

#ifndef Compressor_h
#define Compressor_h
#include <JuceHeader.h>

using namespace std;
using namespace juce;

class Compressor
{
public:
    Compressor() {}
    ~Compressor() {}
    
    void prepareToPlay (double samplerate, int samplesPerBlock, int numInputChannels);
    void processBlock(AudioSampleBuffer &buffer);
    void setParameters(float ratio, float threshold, float attack, float release, float makeUpGain, float kneeWidth);

private:
    // Parameters
    float cRatio;
    float cThreshold;
    float cAttack;
    float cRelease;
    float cMakeUpGain;
    float cKneeWidth;
    float cSampleRate;

    // Compressor ON-OFF state
    int compressorState = 1;

    // Gain and Levels
    float inputGain;
    float inputLevel;
    float outputGain;
    float outputLevel;

    float previousOutputLevel;
    
    float controlVoltage;
};

#endif /* Compressor_h */
