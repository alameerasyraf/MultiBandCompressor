/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

using namespace std;
using namespace juce;

//==============================================================================
MultiBandCompressorAudioProcessorEditor::MultiBandCompressorAudioProcessorEditor (MultiBandCompressorAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{

    buildElements();
    setSize(1400, 670);
    startTimer(100);
}

MultiBandCompressorAudioProcessorEditor::~MultiBandCompressorAudioProcessorEditor()
{
    buttonLowCompressorState = nullptr;
    buttonMidCompressorState = nullptr;
    buttonHighCompressorState = nullptr;
}

//==============================================================================
void MultiBandCompressorAudioProcessorEditor::paint (juce::Graphics& g)
{
    Image background = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt(background, 0, 0);

    //Draw the semi-transparent rectangle around components
    const Rectangle<float> area(10, 10, 1380, 650);
    g.setColour(Colours::ghostwhite);
    g.drawRoundedRectangle(area, 5.0f, 3.0f);

    //Draw background for rectangle
    g.setColour(Colours::black);
    g.setOpacity(0.75f);
    g.fillRoundedRectangle(area, 5.0f);

    // Draw text labels for each component
    // Title
    g.setColour(Colours::white);
    g.setFont(35.0f);
    g.drawFittedText("Multi-Band Compressor", getWidth() / 2 - 125, 10, 250, 75, Justification::centred, 1);

    // Others
    g.setFont(18.0f);

    // Low Cut Off and High Cut Off Frequencies
    g.drawText("Low Cutoff",  getWidth() / 2 - 390,    110, 200, 50, Justification::centred, false);
    g.drawText("Frequency",   getWidth() / 2 - 390,    130, 200, 50, Justification::centred, false);
    g.drawText("High Cutoff", getWidth() / 2 + 180,    110, 200, 50, Justification::centred, false);
    g.drawText("Frequency",   getWidth() / 2 + 180,    130, 200, 50, Justification::centred, false);

    // Low Cut Off and High Cut Off Frequencies
    g.drawText("Freq. Band", 3,    getHeight() / 2 - 110, 200, 50, Justification::centred, false);
    g.drawText("Threshold",  176,   getHeight() / 2 - 110, 200, 50, Justification::centred, false);
    g.drawText("Ratio",      354,   getHeight() / 2 - 110, 200, 50, Justification::centred, false);
    g.drawText("Attack",     535,   getHeight() / 2 - 110, 200, 50, Justification::centred, false);
    g.drawText("Release",    715,   getHeight() / 2 - 110, 200, 50, Justification::centred, false);
    g.drawText("Gain",       890,   getHeight() / 2 - 110, 200, 50, Justification::centred, false);

    // Knee Width and Overall Gain
    g.drawText("Knee Width",    getWidth() - 300, getHeight() / 2 + 140,     200, 50, Justification::centred, false);
    g.drawText("Overall Gain",  getWidth() - 300, getHeight() / 2 + 240,     200, 50, Justification::centred, false);

}

void MultiBandCompressorAudioProcessorEditor::resized()
{
    // Cutoff Frequency Sliders
    sliderLowCutoff.setBounds   (getWidth() / 2 - 250,  40, 220, 220);
    sliderHighCutoff.setBounds  (getWidth() / 2 + 20,   40, 220, 220);

    // Compressor State Buttons
    (*buttonLowCompressorState).setBounds   (50,    getHeight() / 2 - 35,  100, 60);
    (*buttonMidCompressorState).setBounds   (50,    getHeight() / 2 + 90,  100, 60);
    (*buttonHighCompressorState).setBounds  (50,    getHeight() / 2 + 215, 100, 60);

    // Threshold Sliders
    sliderLowThreshold.setBounds    (220, getHeight() / 2 - 65,     110, 110);
    sliderMidThreshold.setBounds    (220, getHeight() / 2 + 60,     110, 110);
    sliderHighThreshold.setBounds   (220, getHeight() / 2 + 185,    110, 110);

    // Low Band Knobs
    sliderLowRatio.setBounds        (400, getHeight() / 2 - 65, 110, 110);
    sliderLowAttack.setBounds       (580, getHeight() / 2 - 65, 110, 110);
    sliderLowRelease.setBounds      (760, getHeight() / 2 - 65, 110, 110);
    sliderLowGain.setBounds         (940, getHeight() / 2 - 65, 110, 110);

    // Mid Band Knobs
    sliderMidRatio.setBounds        (400, getHeight() / 2 + 60, 110, 110);
    sliderMidAttack.setBounds       (580, getHeight() / 2 + 60, 110, 110);
    sliderMidRelease.setBounds      (760, getHeight() / 2 + 60, 110, 110);
    sliderMidGain.setBounds         (940, getHeight() / 2 + 60, 110, 110);

    // High Band Knobs
    sliderHighRatio.setBounds       (400, getHeight() / 2 + 185, 110, 110);
    sliderHighAttack.setBounds      (580, getHeight() / 2 + 185, 110, 110);
    sliderHighRelease.setBounds     (760, getHeight() / 2 + 185, 110, 110);
    sliderHighGain.setBounds        (940, getHeight() / 2 + 185, 110, 110);

    // Knee Width and Overall Gain
    sliderKneeWidth.setBounds       (getWidth() - 295,  getHeight() / 2 - 40,   185, 185);
    sliderOverallGain.setBounds     (getWidth() - 350,  getHeight() / 2 + 200,   300, 50);
}

void MultiBandCompressorAudioProcessorEditor::sliderValueChanged(Slider* sliderMoved)
{}

void MultiBandCompressorAudioProcessorEditor::buttonClicked(Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == buttonLowCompressorState)
        audioProcessor.setLowCompressorState((*buttonLowCompressorState).getToggleState());
    else if (buttonThatWasClicked == buttonMidCompressorState)
        audioProcessor.setMidCompressorState((*buttonMidCompressorState).getToggleState());
    else if (buttonThatWasClicked == buttonHighCompressorState)
        audioProcessor.setHighCompressorState((*buttonHighCompressorState).getToggleState());
}

void MultiBandCompressorAudioProcessorEditor::timerCallback()
{
    sliderOverallGain.setValue  (audioProcessor.getOverallGain());
    sliderKneeWidth.setValue    (audioProcessor.getKneeWidth());

    sliderLowCutoff.setValue    (audioProcessor.getLowCutoff());
    sliderHighCutoff.setValue   (audioProcessor.getHighCutoff());

    sliderLowGain.setValue      (audioProcessor.getLowGain());
    sliderLowThreshold.setValue (audioProcessor.getLowThreshold());
    sliderLowRatio.setValue     (audioProcessor.getLowRatio());
    sliderLowAttack.setValue    (audioProcessor.getLowAttack());
    sliderLowRelease.setValue   (audioProcessor.getLowRelease());

    sliderMidGain.setValue      (audioProcessor.getMidGain());
    sliderMidThreshold.setValue (audioProcessor.getMidThreshold());
    sliderMidRatio.setValue     (audioProcessor.getMidRatio());
    sliderMidAttack.setValue    (audioProcessor.getMidAttack());
    sliderMidRelease.setValue   (audioProcessor.getMidRelease());

    sliderHighGain.setValue     (audioProcessor.getHighGain());
    sliderHighThreshold.setValue(audioProcessor.getHighThreshold());
    sliderHighRatio.setValue    (audioProcessor.getHighRatio());
    sliderHighAttack.setValue   (audioProcessor.getHighAttack());
    sliderHighRelease.setValue  (audioProcessor.getHighRelease());

    (*buttonLowCompressorState).setToggleState  (audioProcessor.getLowCompressorState(), dontSendNotification);
    (*buttonMidCompressorState).setToggleState  (audioProcessor.getMidCompressorState(), dontSendNotification);
    (*buttonHighCompressorState).setToggleState (audioProcessor.getHighCompressorState(), dontSendNotification);
    
}

void MultiBandCompressorAudioProcessorEditor::buildElements()
{
    // Knee Width and Overall Gain
    kneeWidthVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "kneeWidth", sliderKneeWidth);
    sliderKneeWidth.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderKneeWidth.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderKneeWidth.setRange(5.0f, 10.0f); addAndMakeVisible(&sliderKneeWidth);

    overallGainVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "overallGain", sliderOverallGain);
    sliderOverallGain.setSliderStyle(Slider::SliderStyle::LinearHorizontal);
    sliderOverallGain.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderOverallGain.setRange(0.0f, 4.0f); addAndMakeVisible(&sliderOverallGain);

    // Low Band Knobs
    lowThresholdVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowThresh", sliderLowThreshold);
    sliderLowThreshold.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderLowThreshold.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderLowThreshold.setRange(-80.0f, 0.0f); addAndMakeVisible(&sliderLowThreshold);

    lowRatioVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowRatio", sliderLowRatio);
    sliderLowRatio.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderLowRatio.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderLowRatio.setRange(1.0f, 10.0f); addAndMakeVisible(&sliderLowRatio);

    lowAttackVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowAttack", sliderLowAttack);
    sliderLowAttack.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderLowAttack.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderLowAttack.setRange(5.0f, 100.0f); addAndMakeVisible(&sliderLowAttack);

    lowReleaseVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowRelease", sliderLowRelease);
    sliderLowRelease.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderLowRelease.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderLowRelease.setRange(5.0f, 100.0f); addAndMakeVisible(&sliderLowRelease);

    lowGainVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowGain", sliderLowGain);
    sliderLowGain.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderLowGain.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderLowGain.setRange(0.0f, 4.0f); addAndMakeVisible(&sliderLowGain);

    // Mid Band Knobs
    midThresholdVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "midThresh", sliderMidThreshold);
    sliderMidThreshold.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderMidThreshold.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderMidThreshold.setRange(-80.0f, 0.0f); addAndMakeVisible(&sliderMidThreshold);

    midRatioVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "midRatio", sliderMidRatio);
    sliderMidRatio.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderMidRatio.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderMidRatio.setRange(1.0f, 10.0f); addAndMakeVisible(&sliderMidRatio);

    midAttackVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "midAttack", sliderMidAttack);
    sliderMidAttack.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderMidAttack.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderMidAttack.setRange(5.0f, 100.0f); addAndMakeVisible(&sliderMidAttack);

    midReleaseVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "midRelease", sliderMidRelease);
    sliderMidRelease.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderMidRelease.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderMidRelease.setRange(5.0f, 100.0f); addAndMakeVisible(&sliderMidRelease);

    midGainVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "midGain", sliderMidGain);
    sliderMidGain.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderMidGain.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderMidGain.setRange(0.0f, 4.0f); addAndMakeVisible(&sliderMidGain);

    // High Band Knobs
    highThresholdVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highThresh", sliderHighThreshold);
    sliderHighThreshold.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderHighThreshold.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderHighThreshold.setRange(-80.0f, 0.0f); addAndMakeVisible(&sliderHighThreshold);

    highRatioVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highRatio", sliderHighRatio);
    sliderHighRatio.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderHighRatio.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderHighRatio.setRange(1.0f, 10.0f); addAndMakeVisible(&sliderHighRatio);

    highAttackVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highAttack", sliderHighAttack);
    sliderHighAttack.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderHighAttack.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderHighAttack.setRange(5.0f, 100.0f); addAndMakeVisible(&sliderHighAttack);

    highReleaseVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highRelease", sliderHighRelease);
    sliderHighRelease.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderHighRelease.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderHighRelease.setRange(5.0f, 100.0f); addAndMakeVisible(&sliderHighRelease);

    highGainVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highGain", sliderHighGain);
    sliderHighGain.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderHighGain.setTextBoxStyle(Slider::TextBoxBelow, false, 70, 20);
    sliderHighGain.setRange(0.0f, 4.0f); addAndMakeVisible(&sliderHighGain);

    // Low Cutoff Frequency Slider
    lowCutOffVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "lowCutOff", sliderLowCutoff);
    sliderLowCutoff.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderLowCutoff.setTextBoxStyle(Slider::TextBoxRight, false, 70, 20);
    sliderLowCutoff.setRange(150.0f, 600.0f); sliderLowCutoff.setSkewFactor(2); addAndMakeVisible(&sliderLowCutoff);

    // High Cutoff Frequency Slider
    highCutOffVal = make_unique<AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.parameters, "highCutOff", sliderHighCutoff);
    sliderHighCutoff.setSliderStyle(Slider::SliderStyle::Rotary);
    sliderHighCutoff.setTextBoxStyle(Slider::TextBoxLeft, false, 70, 20);
    sliderHighCutoff.setRange(1000.0f, 4000.0f); sliderHighCutoff.setSkewFactor(2); addAndMakeVisible(&sliderHighCutoff);

    // Compressor State Buttons
    addAndMakeVisible(buttonLowCompressorState = new TextButton("Low Compressor"));
    (*buttonLowCompressorState).setButtonText(TRANS("Low"));
    (*buttonLowCompressorState).setColour(TextButton::buttonOnColourId, Colours::lightblue);
    (*buttonLowCompressorState).setColour(TextButton::textColourOnId, Colours::black);
    (*buttonLowCompressorState).setColour(TextButton::textColourOffId, Colours::white);
    (*buttonLowCompressorState).addListener(this);

    addAndMakeVisible(buttonMidCompressorState = new TextButton("Mid Compressor"));
    (*buttonMidCompressorState).setButtonText(TRANS("Mid"));
    (*buttonMidCompressorState).setColour(TextButton::buttonOnColourId, Colours::lightblue);
    (*buttonMidCompressorState).setColour(TextButton::textColourOnId, Colours::black);
    (*buttonMidCompressorState).setColour(TextButton::textColourOffId, Colours::white);
    (*buttonMidCompressorState).addListener(this);

    addAndMakeVisible(buttonHighCompressorState = new TextButton("High Compressor"));
    (*buttonHighCompressorState).setButtonText(TRANS("High"));
    (*buttonHighCompressorState).setColour(TextButton::buttonOnColourId, Colours::lightblue);
    (*buttonHighCompressorState).setColour(TextButton::textColourOnId, Colours::black);
    (*buttonHighCompressorState).setColour(TextButton::textColourOffId, Colours::white);
    (*buttonHighCompressorState).addListener(this);
    
    (*buttonLowCompressorState).setClickingTogglesState(true);
    (*buttonMidCompressorState).setClickingTogglesState(true);
    (*buttonHighCompressorState).setClickingTogglesState(true);
}