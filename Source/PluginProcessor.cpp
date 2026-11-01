#include "PluginProcessor.h"
#include "PluginEditor.h"

namespace
{
    // Longitud de un ciclo en negras: 1/1 = 1 vez por compas (4 negras) ... 1/16 = 16 veces por compas
    const double kBeats[5] = { 4.0, 2.0, 1.0, 0.5, 0.25 };

    inline double frac (double x) { return x - std::floor (x); }
}

juce::AudioProcessorValueTreeState::ParameterLayout KickCraftAudioProcessor::createLayout()
{
    using namespace juce;
    std::vector<std::unique_ptr<RangedAudioParameter>> p;

    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "mix", 1 }, "Mix",
        NormalisableRange<float> (0.f, 100.f, 0.1f), 100.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int) { return String ((int) std::lround (v)) + " %"; })
            .withValueFromStringFunction ([] (const String& t) { return t.getFloatValue(); })));

    p.push_back (std::make_unique<AudioParameterChoice> (ParameterID { "rate", 1 }, "Rate",
                                                         StringArray { "1/1", "1/2", "1/4", "1/8", "1/16" }, 2));

    p.push_back (std::make_unique<AudioParameterFloat> (ParameterID { "offset", 1 }, "Offset",
        NormalisableRange<float> (0.f, 100.f, 0.1f), 0.f,
        AudioParameterFloatAttributes()
            .withStringFromValueFunction ([] (float v, int) { return String ((int) std::lround (v)) + " %"; })
            .withValueFromStringFunction ([] (const String& t) { return t.getFloatValue(); })));

    p.push_back (std::make_unique<AudioParameterBool> (ParameterID { "bypass", 1 }, "Bypass", false));

    return { p.begin(), p.end() };
}

KickCraftAudioProcessor::KickCraftAudioProcessor()
    : AudioProcessor (BusesProperties()
                          .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                          .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "STATE", createLayout())
{
    pMix    = apvts.getRawParameterValue ("mix");
    pRate   = apvts.getRawParameterValue ("rate");
    pOffset = apvts.getRawParameterValue ("offset");
    pBypass = apvts.getRawParameterValue ("bypass");

    for (auto& a : waveIn)  a.store (0.f);
    for (auto& a : waveOut) a.store (0.f);

    setCurve (CurveModel::preset (8));
    apvts.state.setProperty ("presetName", CurveModel::presetName (8), nullptr);
}

bool KickCraftAudioProcessor::isBusesLayoutSupported (const BusesLayout& l) const
{
    const auto& out = l.getMainOutputChannelSet();
    if (out != juce::AudioChannelSet::stereo() && out != juce::AudioChannelSet::mono()) return false;
    return out == l.getMainInputChannelSet();
}

void KickCraftAudioProcessor::prepareToPlay (double sampleRate, int)
{
    sr = sampleRate;
    freePhase = 0.0;
    smL = smR = 1.f;
    lastCol = 0;
    accIn = accOut = 0.f;
}

void KickCraftAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    const int numSamples = buffer.getNumSamples();
    const int nCh = juce::jmin (2, buffer.getNumChannels());
    if (nCh == 0) return;

    if (lutDirty.load())
    {
        juce::SpinLock::ScopedTryLockType l (lutLock);
        if (l.isLocked()) { lutLocal = lutShared; lutDirty = false; }
    }

    // Info del host: siempre sincronizado al tempo / compas
    double bpm = 120.0, ppq = 0.0;
    bool playing = false, havePpq = false;
    if (auto* ph = getPlayHead())
        if (auto pos = ph->getPosition())
        {
            if (auto b = pos->getBpm()) bpm = *b;
            playing = pos->getIsPlaying();
            if (auto q = pos->getPpqPosition()) { ppq = *q; havePpq = true; }
        }
    if (bpm < 1.0) bpm = 120.0;

    const bool bypass   = pBypass->load() > 0.5f;
    const float mix     = pMix->load() * 0.01f;
    const int rateIdx   = juce::jlimit (0, 4, (int) pRate->load());
    const double offset = (double) pOffset->load() * 0.01;
    const double beats  = kBeats[rateIdx];
    const double ppqPerSample = bpm / 60.0 / sr;
    const double cyclesPerSample = ppqPerSample / beats;
    const float coef = 1.f - std::exp (-1.f / (0.001f * (float) sr));   // ~1 ms anti-click

    auto lookup = [this] (double p)
    {
        const double f = juce::jlimit (0.0, 1.0, p) * CurveModel::lutSize;
        const int i = (int) f;
        if (i >= CurveModel::lutSize) return lutLocal[(size_t) CurveModel::lutSize];
        const float fr = (float) (f - i);
        return lutLocal[(size_t) i] + (lutLocal[(size_t) i + 1] - lutLocal[(size_t) i]) * fr;
    };

    float* ch0 = buffer.getWritePointer (0);
    float* ch1 = nCh > 1 ? buffer.getWritePointer (1) : nullptr;
    double pLast = 0.0;

    for (int i = 0; i < numSamples; ++i)
    {
        double p;
        if (playing && havePpq)
            p = frac ((ppq + (double) i * ppqPerSample) / beats);
        else
        {
            p = freePhase;
            freePhase = frac (freePhase + cyclesPerSample);
        }
        // Offset = mover la posicion del efecto dentro del ciclo
        p = frac (p - offset + 1.0);
        pLast = p;

        const float inL = ch0[i];
        const float inR = ch1 != nullptr ? ch1[i] : inL;

        float outL = inL, outR = inR;
        if (! bypass)
        {
            const float y = lookup (p);
            const float g = 1.f - mix * (1.f - y);
            smL += (g - smL) * coef;
            smR = smL;
            outL = inL * smL;
            outR = inR * smR;
            ch0[i] = outL;
            if (ch1 != nullptr) ch1[i] = outR;
        }

        // Captura de forma de onda para la interfaz (plegada sobre un ciclo)
        const int col = juce::jlimit (0, numWaveCols - 1, (int) (p * numWaveCols));
        if (col != lastCol)
        {
            auto upd = [] (std::atomic<float>& a, float v)
            {
                const float old = a.load();
                a.store (v > old ? v : old * 0.9f + v * 0.1f);
            };
            upd (waveIn[(size_t) lastCol], accIn);
            upd (waveOut[(size_t) lastCol], accOut);
            accIn = accOut = 0.f;
            lastCol = col;
        }
        accIn  = juce::jmax (accIn,  juce::jmax (std::abs (inL),  std::abs (inR)));
        accOut = juce::jmax (accOut, juce::jmax (std::abs (outL), std::abs (outR)));
    }

    uiPhase.store ((float) pLast);
}

// ---------------- Curva ----------------
void KickCraftAudioProcessor::setCurve (const CurveModel& m, bool writeToState)
{
    curve = m;
    curve.sanitize();

    std::array<float, CurveModel::lutSize + 1> tmp {};
    curve.buildLut (tmp);
    {
        juce::SpinLock::ScopedLockType l (lutLock);
        lutShared = tmp;
    }
    lutDirty = true;
    ++curveVersion;

    if (writeToState)
        apvts.state.setProperty ("curve", curve.toString(), nullptr);
}

void KickCraftAudioProcessor::syncCurveFromState()
{
    auto s = apvts.state.getProperty ("curve").toString();
    setCurve (s.isEmpty() ? CurveModel::preset (8) : CurveModel::fromString (s), false);
}

// ---------------- Presets ----------------
juce::File KickCraftAudioProcessor::getPresetDirectory()
{
    return juce::File::getSpecialLocation (juce::File::userDocumentsDirectory).getChildFile ("KickCraft").getChildFile ("Presets");
}

juce::StringArray KickCraftAudioProcessor::getPresetNames() const
{
    juce::StringArray names;
    for (int i = 0; i < CurveModel::numPresets; ++i)
        names.add (CurveModel::presetName (i));

    auto files = getPresetDirectory().findChildFiles (juce::File::findFiles, false, "*.xml");
    files.sort();
    for (auto& f : files) names.add (f.getFileNameWithoutExtension());
    return names;
}

void KickCraftAudioProcessor::loadPreset (int index)
{
    if (index < 0) return;

    if (index < CurveModel::numPresets)
    {
        setCurve (CurveModel::preset (index));
        apvts.state.setProperty ("presetName", CurveModel::presetName (index), nullptr);
        return;
    }

    auto files = getPresetDirectory().findChildFiles (juce::File::findFiles, false, "*.xml");
    files.sort();
    const int fi = index - CurveModel::numPresets;
    if (fi >= files.size()) return;

    if (auto xml = juce::XmlDocument::parse (files[fi]))
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            apvts.state.setProperty ("presetName", files[fi].getFileNameWithoutExtension(), nullptr);
            syncCurveFromState();
        }
}

void KickCraftAudioProcessor::savePreset (const juce::String& name)
{
    auto clean = juce::File::createLegalFileName (name.trim());
    if (clean.isEmpty()) return;

    auto dir = getPresetDirectory();
    dir.createDirectory();
    apvts.state.setProperty ("presetName", clean, nullptr);
    apvts.state.setProperty ("curve", curve.toString(), nullptr);

    if (auto xml = apvts.copyState().createXml())
        dir.getChildFile (clean + ".xml").replaceWithText (xml->toString());
}

juce::String KickCraftAudioProcessor::getCurrentPresetName() const
{
    return apvts.state.getProperty ("presetName", "Init").toString();
}

bool KickCraftAudioProcessor::isFavourite() const { return (bool) apvts.state.getProperty ("fav", false); }
void KickCraftAudioProcessor::setFavourite (bool b) { apvts.state.setProperty ("fav", b, nullptr); }

// ---------------- Estado ----------------
void KickCraftAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    apvts.state.setProperty ("curve", curve.toString(), nullptr);
    if (auto xml = apvts.copyState().createXml())
        copyXmlToBinary (*xml, dest);
}

void KickCraftAudioProcessor::setStateInformation (const void* data, int size)
{
    if (auto xml = getXmlFromBinary (data, size))
        if (xml->hasTagName (apvts.state.getType()))
        {
            apvts.replaceState (juce::ValueTree::fromXml (*xml));
            syncCurveFromState();
        }
}

juce::AudioProcessorEditor* KickCraftAudioProcessor::createEditor() { return new KickCraftAudioProcessorEditor (*this); }

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new KickCraftAudioProcessor(); }
