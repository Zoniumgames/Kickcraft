#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include <atomic>
#include "CurveModel.h"

class KickCraftAudioProcessor : public juce::AudioProcessor
{
public:
    static constexpr int numWaveCols = 256;

    KickCraftAudioProcessor();
    ~KickCraftAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported (const BusesLayout&) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }

    const juce::String getName() const override { return "KickCraft"; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}

    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    // ---- Curva (hilo de mensajes) ----
    CurveModel getCurve() const { return curve; }
    void setCurve (const CurveModel& m, bool writeToState = true);
    int getCurveVersion() const { return curveVersion.load(); }

    // ---- Presets ----
    juce::StringArray getPresetNames() const;
    int getNumFactoryPresets() const { return CurveModel::numPresets; }
    void loadPreset (int index);
    void savePreset (const juce::String& name);
    juce::String getCurrentPresetName() const;
    bool isFavourite() const;
    void setFavourite (bool);
    static juce::File getPresetDirectory();

    // ---- Datos para la interfaz ----
    float getWaveIn (int i) const  { return waveIn[(size_t) i].load(); }
    float getWaveOut (int i) const { return waveOut[(size_t) i].load(); }

    juce::AudioProcessorValueTreeState apvts;
    std::atomic<float> uiPhase { 0.f };

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createLayout();
    void syncCurveFromState();

    CurveModel curve;
    std::atomic<int> curveVersion { 0 };

    std::array<float, CurveModel::lutSize + 1> lutShared {}, lutLocal {};
    juce::SpinLock lutLock;
    std::atomic<bool> lutDirty { true };

    std::array<std::atomic<float>, numWaveCols> waveIn, waveOut;

    std::atomic<float> *pMix, *pRate, *pOffset, *pBypass;

    double sr = 44100.0;
    double freePhase = 0.0;
    float smL = 1.f, smR = 1.f;
    int lastCol = 0;
    float accIn = 0.f, accOut = 0.f;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (KickCraftAudioProcessor)
};
