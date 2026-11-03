#include "PluginEditor.h"

namespace
{
    constexpr float kPi = juce::MathConstants<float>::pi;
    const int kHitsPerBar[5] = { 1, 2, 4, 8, 16 };
}

KickCraftAudioProcessorEditor::KickCraftAudioProcessorEditor (KickCraftAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p),
      rateButtons (*p.apvts.getParameter ("rate"), { "1/1", "1/2", "1/4", "1/8", "1/16" }, 4),
      curveEditor (p),
      favBtn ("fav", [] (juce::Graphics& g, juce::Rectangle<float> r, bool on, bool over)
      {
          auto c = r.getCentre();
          const float w = 8.f, h = 8.f;
          juce::Path hp;
          hp.startNewSubPath (c.x, c.y + h * 0.9f);
          hp.cubicTo (c.x - w * 1.6f, c.y - h * 0.2f, c.x - w * 0.9f, c.y - h * 1.5f, c.x, c.y - h * 0.5f);
          hp.cubicTo (c.x + w * 0.9f, c.y - h * 1.5f, c.x + w * 1.6f, c.y - h * 0.2f, c.x, c.y + h * 0.9f);
          hp.closeSubPath();
          g.setColour (on ? Theme::accent : (over ? Theme::text : Theme::textDim));
          if (on) g.fillPath (hp); else g.strokePath (hp, juce::PathStrokeType (1.4f));
      }),
      saveBtn ("save", [] (juce::Graphics& g, juce::Rectangle<float> r, bool, bool over)
      {
          auto b = r.reduced (5.f);
          g.setColour (over ? Theme::accent : Theme::textDim);
          g.drawRoundedRectangle (b, 2.f, 1.4f);
          g.drawRect (b.getX() + 3.f, b.getY(), b.getWidth() - 6.f, b.getHeight() * 0.4f, 1.2f);
          g.fillRect (b.getX() + 4.f, b.getBottom() - b.getHeight() * 0.38f, b.getWidth() - 8.f, 2.f);
      }),
      resetBtn ("reset", [] (juce::Graphics& g, juce::Rectangle<float> r, bool, bool over)
      {
          auto c = r.getCentre();
          juce::Path a;
          a.addCentredArc (c.x, c.y, 6.f, 6.f, 0.f, 0.6f, 2.f * kPi - 0.3f, true);
          g.setColour (over ? Theme::accent : Theme::textDim);
          g.strokePath (a, juce::PathStrokeType (1.5f));
          juce::Path tri;
          tri.addTriangle (c.x + 3.f, c.y - 8.f, c.x + 3.f, c.y - 2.f, c.x + 8.f, c.y - 5.f);
          g.fillPath (tri);
      }),
      bypassBtn ("bypass", [] (juce::Graphics& g, juce::Rectangle<float> r, bool on, bool over)
      {
          auto c = r.getCentre();
          g.setColour (on ? Theme::text : (over ? Theme::accent : Theme::textDim));
          g.drawEllipse (c.x - 8.f, c.y - 8.f, 16.f, 16.f, 2.f);
          g.fillRect (c.x - 1.f, c.y - 11.f, 2.f, 9.f);
          if (on) { g.setColour (Theme::accent); g.fillEllipse (c.x - 2.5f, c.y - 2.5f, 5.f, 5.f); }
      })
{
    setLookAndFeel (&laf);
    setSize (800, 540);

    addAndMakeVisible (rateButtons);
    addAndMakeVisible (curveEditor);
    addAndMakeVisible (presetGrid);
    addAndMakeVisible (nameBox);
    addAndMakeVisible (presetPrev);
    addAndMakeVisible (presetNext);
    for (auto* b : { &favBtn, &saveBtn, &resetBtn, &bypassBtn }) addAndMakeVisible (b);

    setupRotary (mixSlider, false);
    setupRotary (offsetSlider, true);

    mixAtt    = std::make_unique<SliderAtt> (proc.apvts, "mix", mixSlider);
    offsetAtt = std::make_unique<SliderAtt> (proc.apvts, "offset", offsetSlider);
    bypassBtn.setClickingTogglesState (true);
    bypassAtt = std::make_unique<ButtonAtt> (proc.apvts, "bypass", bypassBtn);

    mixSlider.onValueChange = [this] { repaint (30, 292, 200, 28); };
    offsetSlider.onValueChange = [this] { curveEditor.setOffset ((float) offsetSlider.getValue() * 0.01f); };
    curveEditor.setOffset ((float) offsetSlider.getValue() * 0.01f);
    rateButtons.onChange = [this] (int) { repaint (20, 496, 226, 24); };

    curveEditor.onEdited = [this]
    {
        lastCurveVersion = proc.getCurveVersion();
        presetGrid.setSelected (-1);
    };
    presetGrid.onSelect = [this] (int i) { proc.loadPreset (i); lastCurveVersion = -1; };

    nameBox.onClick    = [this] { showPresetMenu(); };
    presetPrev.onClick = [this] { stepPreset (-1); };
    presetNext.onClick = [this] { stepPreset (1); };
    favBtn.onClick     = [this] { proc.setFavourite (! proc.isFavourite()); };
    saveBtn.onClick    = [this] { askSavePreset(); };
    resetBtn.onClick   = [this]
    {
        proc.setCurve (CurveModel::preset (8));
        proc.loadPreset (8);
        lastCurveVersion = -1;
        presetGrid.setSelected (8);
    };

    startTimerHz (4);   // en reposo casi no consume; sube a 30 Hz solo cuando hay audio
}

KickCraftAudioProcessorEditor::~KickCraftAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void KickCraftAudioProcessorEditor::setupRotary (juce::Slider& s, bool textBelow)
{
    s.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    s.setRotaryParameters (kPi * 1.2f, kPi * 2.8f, true);
    s.setTextBoxStyle (textBelow ? juce::Slider::TextBoxBelow : juce::Slider::NoTextBox, false, 74, 18);
    s.setTextBoxIsEditable (true);
    addAndMakeVisible (s);
}

void KickCraftAudioProcessorEditor::timerCallback()
{
    // Sin audio: refresco lento y sin redibujar. Con audio: 30 fps.
    const bool active = proc.isSignalActive();
    if (active != lastActive)
    {
        lastActive = active;
        startTimerHz (active ? 30 : 4);
        curveEditor.setPlayheadVisible (active);
        curveEditor.repaint();
    }
    if (active)
    {
        curveEditor.setPlayhead (proc.uiPhase.load());
        curveEditor.repaint();   // refresca la forma de onda
    }

    if (proc.getCurveVersion() != lastCurveVersion)
    {
        lastCurveVersion = proc.getCurveVersion();
        curveEditor.setModel (proc.getCurve());
    }

    const auto nm = proc.getCurrentPresetName();
    if (nm != nameBox.text) nameBox.setText (nm);
    favBtn.setToggleState (proc.isFavourite(), juce::dontSendNotification);
}

void KickCraftAudioProcessorEditor::showPresetMenu()
{
    auto names = proc.getPresetNames();
    juce::PopupMenu m;
    m.addSectionHeader ("Factory");
    for (int i = 0; i < names.size(); ++i)
    {
        if (i == proc.getNumFactoryPresets())
        {
            m.addSeparator();
            m.addSectionHeader ("User");
        }
        m.addItem (i + 1, names[i], true, names[i] == proc.getCurrentPresetName());
    }
    juce::Component::SafePointer<KickCraftAudioProcessorEditor> safe (this);
    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&nameBox), [safe] (int r)
    {
        if (safe != nullptr && r > 0)
        {
            safe->proc.loadPreset (r - 1);
            safe->presetGrid.setSelected (r - 1 < safe->proc.getNumFactoryPresets() ? r - 1 : -1);
        }
    });
}

void KickCraftAudioProcessorEditor::stepPreset (int delta)
{
    auto names = proc.getPresetNames();
    if (names.isEmpty()) return;
    int cur = names.indexOf (proc.getCurrentPresetName());
    cur = (cur < 0) ? 0 : (cur + delta + names.size()) % names.size();
    proc.loadPreset (cur);
    presetGrid.setSelected (cur < proc.getNumFactoryPresets() ? cur : -1);
}

void KickCraftAudioProcessorEditor::askSavePreset()
{
    auto* aw = new juce::AlertWindow ("Guardar preset", "Nombre del preset:", juce::MessageBoxIconType::NoIcon, this);
    aw->addTextEditor ("name", proc.getCurrentPresetName());
    aw->addButton ("Guardar", 1, juce::KeyPress (juce::KeyPress::returnKey));
    aw->addButton ("Cancelar", 0, juce::KeyPress (juce::KeyPress::escapeKey));

    juce::Component::SafePointer<KickCraftAudioProcessorEditor> safe (this);
    aw->enterModalState (true, juce::ModalCallbackFunction::create ([safe, aw] (int r)
    {
        if (r == 1 && safe != nullptr)
            safe->proc.savePreset (aw->getTextEditorContents ("name"));
    }), true);
}

void KickCraftAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (Theme::bg);

    auto left = juce::Rectangle<float> (10.f, 10.f, 250.f, (float) getHeight() - 20.f);
    g.setColour (Theme::panel);
    g.fillRoundedRectangle (left, 10.f);
    g.setColour (Theme::line);
    g.drawRoundedRectangle (left.reduced (0.5f), 10.f, 1.f);

    // logo
    g.setColour (Theme::text);
    g.setFont (mkFont (26.f, false));
    g.drawText ("KICK", 26, 18, 80, 36, juce::Justification::centredLeft);
    g.setColour (Theme::accent);
    g.setFont (mkFont (26.f, true));
    g.drawText ("CRAFT", 86, 18, 140, 36, juce::Justification::centredLeft);
    g.setColour (Theme::line);
    g.drawHorizontalLine (62, 24.f, 246.f);

    // MIX
    g.setFont (mkFont (12.f, true));
    g.setColour (Theme::textDim);
    g.drawText ("MIX", 30, 292, 90, 22, juce::Justification::centredRight);
    g.setColour (Theme::text);
    g.drawText (juce::String ((int) std::lround (mixSlider.getValue())) + "%", 126, 292, 90, 22, juce::Justification::centredLeft);

    // OFFSET
    g.setColour (Theme::textDim);
    g.setFont (mkFont (10.f, true));
    g.drawText ("OFFSET", 80, 332, 100, 14, juce::Justification::centred);

    // RATE
    g.drawText ("RATE", 20, 440, 226, 14, juce::Justification::centred);
    const int sel = juce::jlimit (0, 4, rateButtons.getSelected());
    g.setColour (Theme::text);
    g.setFont (mkFont (11.f, false));
    const int hits = kHitsPerBar[sel];
    g.drawText (juce::String (hits) + (hits == 1 ? " golpe por compas" : " golpes por compas"),
                20, 500, 226, 18, juce::Justification::centred);

    // barra de presets
    g.setColour (Theme::panel);
    g.fillRoundedRectangle (270.f, 16.f, 350.f, 44.f, 8.f);
    g.setColour (Theme::line);
    g.drawRoundedRectangle (270.5f, 16.5f, 349.f, 43.f, 8.f, 1.f);
}

void KickCraftAudioProcessorEditor::resized()
{
    mixSlider.setBounds (35, 80, 190, 190);
    offsetSlider.setBounds (90, 350, 80, 84);

    rateButtons.setBounds (20, 462, 226, 30);

    favBtn.setBounds     (276, 28, 24, 22);
    nameBox.setBounds    (304, 22, 200, 32);
    presetPrev.setBounds (508, 32, 12, 14);
    presetNext.setBounds (524, 32, 12, 14);
    saveBtn.setBounds    (586, 25, 28, 26);

    resetBtn.setBounds  (742, 26, 24, 26);
    bypassBtn.setBounds (768, 26, 24, 26);

    curveEditor.setBounds (270, 72, 520, 330);
    presetGrid.setBounds  (270, 414, 520, 106);
}
