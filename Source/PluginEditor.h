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
    void applySkin (int index);
    void showThemeMenu();
    void showPresetMenu();
    void stepPreset (int delta);
    void askSavePreset();
    void setupRotary (juce::Slider&, bool textBelow);

    KickCraftAudioProcessor& proc;
    SkinLAF laf;
    const Skin* skin = &allSkins()[0];
    int skinIndex = 0;

    LogoButton logo;
    juce::Slider mixSlider, offsetSlider;
    ChoiceButtons rateButtons;
    CurveEditor curveEditor;
    PresetGrid presetGrid;
    NameBox nameBox;
    IconButton prevBtn, nextBtn, favBtn, saveBtn, resetBtn, bypassBtn;

    std::unique_ptr<SliderAtt> mixAtt, offsetAtt;
    std::unique_ptr<ButtonAtt> bypassAtt;

    int lastCurveVersion = -1;
    bool lastActive = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KickCraftAudioProcessorEditor)
};
