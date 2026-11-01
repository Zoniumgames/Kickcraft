#pragma once
#include "PluginProcessor.h"
#include "UI.h"

class KickCraftAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit KickCraftAudioProcessorEditor (KickCraftAudioProcessor&);
    ~KickCraftAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    using SliderAtt = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAtt = juce::AudioProcessorValueTreeState::ButtonAttachment;

    void timerCallback() override;
    void showPresetMenu();
    void stepPreset (int delta);
    void askSavePreset();
    void setupRotary (juce::Slider&, bool textBelow);

    KickCraftAudioProcessor& proc;
    SPLookAndFeel laf;

    juce::Slider mixSlider, offsetSlider;
    ChoiceButtons rateButtons;
    CurveEditor curveEditor;
    PresetGrid presetGrid;
    NameBox nameBox;
    juce::ArrowButton presetPrev { "prevP", 0.5f, Theme::textDim };
    juce::ArrowButton presetNext { "nextP", 0.0f, Theme::textDim };
    IconButton favBtn, saveBtn, resetBtn, bypassBtn;

    std::unique_ptr<SliderAtt> mixAtt, offsetAtt;
    std::unique_ptr<ButtonAtt> bypassAtt;

    int lastCurveVersion = -1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KickCraftAudioProcessorEditor)
};
