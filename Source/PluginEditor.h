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

    void paint(Graphics&) override;
    void resized() override;
    void sliderValueChanged(Slider* sliderMoved) override;
    void buttonClicked(Button* buttonClicked) override;
    void timerCallback() override;
    void buildElements();

    // Crossover Cutoff Values
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowCutOffVal;            // Attachment for Low Cutoff Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highCutOffVal;           // Attachment for High Cutoff Value

    // Low Band Frequency Values
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowThresholdVal;         // Attachment for Low Threshold Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowRatioVal;             // Attachment for Low Ratio Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowAttackVal;            // Attachment for Low Attack Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowReleaseVal;           // Attachment for Low Release Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> lowGainVal;              // Attachment for Low Gain Value

    // Mid Band Frequency Values
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midThresholdVal;         // Attachment for Mid Threshold Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midRatioVal;             // Attachment for Mid Ratio Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midAttackVal;            // Attachment for Mid Attack Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midReleaseVal;           // Attachment for Mid Release Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> midGainVal;              // Attachment for Mid Gain Value

    // High Band Frequency Values
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highThresholdVal;        // Attachment for High Threshold Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highRatioVal;            // Attachment for High Ratio Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highAttackVal;           // Attachment for High Attack Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highReleaseVal;          // Attachment for High Release Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> highGainVal;             // Attachment for High Gain Value

    // Knee Width and Overall Gain
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> kneeWidthVal;            // Attachment for Knee Width Value
    unique_ptr<AudioProcessorValueTreeState::SliderAttachment> overallGainVal;          // Attachment for Overall Gain Value

private:
    // Audio Processor Object
    MultiBandCompressorAudioProcessor& audioProcessor;

    // Crossover Cutoff Sliders
    Slider sliderLowCutoff;
    Slider sliderHighCutoff;

    // Low Compressor Sliders
    Slider sliderLowThreshold;
    Slider sliderLowRatio;
    Slider sliderLowAttack;
    Slider sliderLowRelease;
    Slider sliderLowGain;

    // Mid Compressor Sliders
    Slider sliderMidThreshold;
    Slider sliderMidRatio;
    Slider sliderMidAttack;
    Slider sliderMidRelease;
    Slider sliderMidGain;

    // High Compressor Sliders
    Slider sliderHighThreshold;
    Slider sliderHighRatio;
    Slider sliderHighAttack;
    Slider sliderHighRelease;
    Slider sliderHighGain;

    // Knee Width and Overall Gain
    Slider sliderKneeWidth;
    Slider sliderOverallGain;

    // Buttons to Switch the Compressor states to ON/OFF
    ScopedPointer<TextButton> buttonLowCompressorState;
    ScopedPointer<TextButton> buttonMidCompressorState;
    ScopedPointer<TextButton> buttonHighCompressorState;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MultiBandCompressorAudioProcessorEditor)
};
