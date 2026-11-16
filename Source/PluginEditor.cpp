#include "PluginEditor.h"

namespace
{
    constexpr float kPi = juce::MathConstants<float>::pi;
    const int kHitsPerBar[5] = { 1, 2, 4, 8, 16 };

    // ---------------- iconos (toman los colores del tema) ----------------
    void iconHeart (juce::Graphics& g, juce::Rectangle<float> r, bool on, bool over, const Skin& s)
    {
        auto c = r.getCentre();
        const float w = 8.f, h = 8.f;
        juce::Path hp;
        hp.startNewSubPath (c.x, c.y + h * 0.9f);
        hp.cubicTo (c.x - w * 1.6f, c.y - h * 0.2f, c.x - w * 0.9f, c.y - h * 1.5f, c.x, c.y - h * 0.5f);
        hp.cubicTo (c.x + w * 0.9f, c.y - h * 1.5f, c.x + w * 1.6f, c.y - h * 0.2f, c.x, c.y + h * 0.9f);
        hp.closeSubPath();
        g.setColour (on ? s.accent : (over ? s.text : s.textDim));
        if (on) g.fillPath (hp); else g.strokePath (hp, juce::PathStrokeType (1.4f));
    }

    void iconSave (juce::Graphics& g, juce::Rectangle<float> r, bool, bool over, const Skin& s)
    {
        auto b = r.reduced (5.f);
        g.setColour (over ? s.accent : s.textDim);
        g.drawRoundedRectangle (b, 2.f, 1.4f);
        g.drawRect (b.getX() + 3.f, b.getY(), b.getWidth() - 6.f, b.getHeight() * 0.4f, 1.2f);
        g.fillRect (b.getX() + 4.f, b.getBottom() - b.getHeight() * 0.38f, b.getWidth() - 8.f, 2.f);
    }

    void iconReset (juce::Graphics& g, juce::Rectangle<float> r, bool, bool over, const Skin& s)
    {
        auto c = r.getCentre();
        juce::Path a;
        a.addCentredArc (c.x, c.y, 6.f, 6.f, 0.f, 0.6f, 2.f * kPi - 0.3f, true);
        g.setColour (over ? s.accent : s.textDim);
        g.strokePath (a, juce::PathStrokeType (1.5f));
        juce::Path tri;
        tri.addTriangle (c.x + 3.f, c.y - 8.f, c.x + 3.f, c.y - 2.f, c.x + 8.f, c.y - 5.f);
        g.fillPath (tri);
    }

    void iconBypass (juce::Graphics& g, juce::Rectangle<float> r, bool on, bool over, const Skin& s)
    {
        auto c = r.getCentre();
        g.setColour (on ? s.text : (over ? s.accent : s.textDim));
        g.drawEllipse (c.x - 8.f, c.y - 8.f, 16.f, 16.f, 2.f);
        g.fillRect (c.x - 1.f, c.y - 11.f, 2.f, 9.f);
        if (on) { g.setColour (s.accent); g.fillEllipse (c.x - 2.5f, c.y - 2.5f, 5.f, 5.f); }
    }

    template <bool Left>
    void iconArrow (juce::Graphics& g, juce::Rectangle<float> r, bool, bool over, const Skin& s)
    {
        auto c = r.getCentre();
        juce::Path t;
        if (Left) t.addTriangle (c.x + 3.f, c.y - 5.f, c.x + 3.f, c.y + 5.f, c.x - 4.f, c.y);
        else      t.addTriangle (c.x - 3.f, c.y - 5.f, c.x - 3.f, c.y + 5.f, c.x + 4.f, c.y);
        g.setColour (over ? s.accent : s.textDim);
        g.fillPath (t);
    }
}

KickCraftAudioProcessorEditor::KickCraftAudioProcessorEditor (KickCraftAudioProcessor& p)
    : AudioProcessorEditor (&p), proc (p),
      rateButtons (*p.apvts.getParameter ("rate"), { "1/1", "1/2", "1/4", "1/8", "1/16" }, 4),
      curveEditor (p),
      prevBtn ("prev", iconArrow<true>),
      nextBtn ("next", iconArrow<false>),
      favBtn ("fav", iconHeart),
      saveBtn ("save", iconSave),
      resetBtn ("reset", iconReset),
      bypassBtn ("bypass", iconBypass)
{
    setLookAndFeel (&laf);
    setSize (800, 540);

    addAndMakeVisible (logo);
    addAndMakeVisible (rateButtons);
    addAndMakeVisible (curveEditor);
    addAndMakeVisible (presetGrid);
    addAndMakeVisible (nameBox);
    for (auto* b : { &prevBtn, &nextBtn, &favBtn, &saveBtn, &resetBtn, &bypassBtn }) addAndMakeVisible (b);

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

    logo.onClick     = [this] { showThemeMenu(); };
    nameBox.onClick  = [this] { showPresetMenu(); };
    prevBtn.onClick  = [this] { stepPreset (-1); };
    nextBtn.onClick  = [this] { stepPreset (1); };
    favBtn.onClick   = [this] { proc.setFavourite (! proc.isFavourite()); };
    saveBtn.onClick  = [this] { askSavePreset(); };
    resetBtn.onClick = [this]
    {
        proc.loadPreset (8);
        lastCurveVersion = -1;
        presetGrid.setSelected (8);
    };

    nameBox.setText (proc.getCurrentPresetName());
    favBtn.setToggleState (proc.isFavourite(), juce::dontSendNotification);
    applySkin (proc.getTheme());
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

// Cambia TODO el aspecto: colores, perillas, botones, paneles, rejilla, puntos y tipografia
void KickCraftAudioProcessorEditor::applySkin (int index)
{
    skinIndex = juce::jlimit (0, (int) allSkins().size() - 1, index);
    skin = &allSkins()[(size_t) skinIndex];

    laf.setSkin (*skin);
    logo.setSkin (*skin);
    curveEditor.setSkin (*skin);
    presetGrid.setSkin (*skin);
    nameBox.setSkin (*skin);
    for (auto* b : { &prevBtn, &nextBtn, &favBtn, &saveBtn, &resetBtn, &bypassBtn }) b->setSkin (*skin);
    rateButtons.setGap (skin->btnGap);

    sendLookAndFeelChange();
    repaint();
}

void KickCraftAudioProcessorEditor::showThemeMenu()
{
    juce::PopupMenu m;
    m.addSectionHeader ("THEME");
    for (int i = 0; i < (int) allSkins().size(); ++i)
        m.addItem (i + 1, allSkins()[(size_t) i].name, true, i == skinIndex);

    juce::Component::SafePointer<KickCraftAudioProcessorEditor> safe (this);
    m.showMenuAsync (juce::PopupMenu::Options().withTargetComponent (&logo), [safe] (int r)
    {
        if (safe != nullptr && r > 0)
        {
            safe->proc.setTheme (r - 1);
            safe->applySkin (r - 1);
        }
    });
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
        curveEditor.repaint();
    }

    if (proc.getCurveVersion() != lastCurveVersion)
    {
        lastCurveVersion = proc.getCurveVersion();
        curveEditor.setModel (proc.getCurve());
    }

    if (proc.getTheme() != skinIndex)   // el DAW restauro otra sesion
        applySkin (proc.getTheme());

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
    const auto& s = *skin;

    // fondo
    g.setGradientFill (juce::ColourGradient (s.bgTop, 0.f, 0.f, s.bgBottom, 0.f, (float) getHeight(), false));
    g.fillAll();
    if (s.glow)
    {
        g.setGradientFill (juce::ColourGradient (s.accent.withAlpha (0.13f), 520.f, 30.f,
                                                 juce::Colours::transparentBlack, 520.f, 480.f, true));
        g.fillAll();
    }

    // panel izquierdo
    SkinDraw::panel (g, { 10.f, 10.f, 250.f, (float) getHeight() - 20.f }, s);
    g.setColour (s.line);
    g.drawHorizontalLine (62, 24.f, 246.f);

    // MIX
    g.setFont (mkFont (s, 12.f, true));
    g.setColour (s.textDim);
    g.drawText ("MIX", 30, 292, 90, 22, juce::Justification::centredRight);
    g.setColour (s.text);
    g.drawText (juce::String ((int) std::lround (mixSlider.getValue())) + "%", 126, 292, 90, 22, juce::Justification::centredLeft);

    // OFFSET
    g.setColour (s.textDim);
    g.setFont (mkFont (s, 10.f, true));
    g.drawText ("OFFSET", 80, 332, 100, 14, juce::Justification::centred);

    // RATE
    g.drawText ("RATE", 20, 440, 226, 14, juce::Justification::centred);
    const int sel = juce::jlimit (0, 4, rateButtons.getSelected());
    const int hits = kHitsPerBar[sel];
    g.setColour (s.text);
    g.setFont (mkFont (s, 11.f, false));
    g.drawText (juce::String (hits) + (hits == 1 ? " golpe por compas" : " golpes por compas"),
                20, 500, 226, 18, juce::Justification::centred);

    // barra de presets
    SkinDraw::field (g, { 270.f, 16.f, 350.f, 44.f }, s);
}

void KickCraftAudioProcessorEditor::resized()
{
    logo.setBounds (22, 14, 226, 44);

    mixSlider.setBounds (35, 80, 190, 190);
    offsetSlider.setBounds (90, 350, 80, 84);
    rateButtons.setBounds (20, 462, 226, 30);

    favBtn.setBounds    (276, 28, 24, 22);
    nameBox.setBounds   (304, 22, 200, 32);
    prevBtn.setBounds   (504, 26, 18, 24);
    nextBtn.setBounds   (524, 26, 18, 24);
    saveBtn.setBounds   (586, 25, 28, 26);

    resetBtn.setBounds  (742, 26, 24, 26);
    bypassBtn.setBounds (768, 26, 24, 26);

    curveEditor.setBounds (270, 72, 520, 330);
    presetGrid.setBounds  (270, 414, 520, 106);
}
