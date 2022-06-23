/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

using namespace std;
using namespace juce;

//==============================================================================
/**
*/
class MultiBandCompressorAudioProcessorEditor  : public juce::AudioProcessorEditor, public Timer, public Slider::Listener, public Button::Listener
{
public:
    MultiBandCompressorAudioProcessorEditor (MultiBandCompressorAudioProcessor&);
    ~MultiBandCompressorAudioProcessorEditor() override;

    //==============================================================================
    void timerCallback() override;

    void paint(Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* sliderMoved) override;
    void buttonClicked(Button* buttonClicked) override;

private:
    // Audio Processor Object
    MultiBandCompressorAudioProcessor& audioProcessor;

    //================================================
    //Appended by Amogh Matt
    // Low Compressor Sliders
    ScopedPointer<Slider> sliderLowThreshold;
    ScopedPointer<Slider> sliderLowRatio;
    ScopedPointer<Slider> sliderLowAttack;
    ScopedPointer<Slider> sliderLowRelease;
    ScopedPointer<Slider> sliderLowGain;
    // Mid Compressor Sliders
    ScopedPointer<Slider> sliderMidThreshold;
    ScopedPointer<Slider> sliderMidRatio;
    ScopedPointer<Slider> sliderMidAttack;
    ScopedPointer<Slider> sliderMidRelease;
    ScopedPointer<Slider> sliderMidGain;
    // High Compressor Sliders
    ScopedPointer<Slider> sliderHighThreshold;
    ScopedPointer<Slider> sliderHighRatio;
    ScopedPointer<Slider> sliderHighAttack;
    ScopedPointer<Slider> sliderHighRelease;
    ScopedPointer<Slider> sliderHighGain;

    ScopedPointer<Slider> sliderKneeWidth;
    ScopedPointer<Slider> sliderOverallGain;

    // Labels
    ScopedPointer<Label> ratioLabel;
    ScopedPointer<Label> attackLabel;
    ScopedPointer<Label> releaseLabel;
    ScopedPointer<Label> gainLabel;
    ScopedPointer<Label> kneeWidthLabel;
    ScopedPointer<Label> overallGainLabel;
    ScopedPointer<Label> thresholdLabel;
    ScopedPointer<Label> cutoffLabel;

    // Buttons to Switch the Compressor states to ON/OFF
    ScopedPointer<TextButton> buttonLowCompressorState;
    ScopedPointer<TextButton> buttonMidCompressorState;
    ScopedPointer<TextButton> buttonHighCompressorState;

    // Crossover Cutoff Sliders
    ScopedPointer<Slider> sliderLowCutoff;
    ScopedPointer<Slider> sliderHighCutoff;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiBandCompressorAudioProcessorEditor)
};
