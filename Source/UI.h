#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

namespace Theme
{
    inline const juce::Colour bg        { 0xff0e0e0e };
    inline const juce::Colour panel     { 0xff171717 };
    inline const juce::Colour panel2    { 0xff202020 };
    inline const juce::Colour accent    { 0xffff7a1a };
    inline const juce::Colour accentDim { 0xff5c2c0c };
    inline const juce::Colour text      { 0xffeeeeee };
    inline const juce::Colour textDim   { 0xff8a8a8a };
    inline const juce::Colour line      { 0xff2e2e2e };
}

inline juce::Font mkFont (float h, bool bold = false)
{
    return juce::Font (juce::FontOptions (h, bold ? juce::Font::bold : juce::Font::plain));
}

// ======================= LookAndFeel =======================
class SPLookAndFeel : public juce::LookAndFeel_V4
{
public:
    SPLookAndFeel()
    {
        setColour (juce::Slider::textBoxTextColourId, Theme::text);
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour (juce::PopupMenu::backgroundColourId, Theme::panel2);
        setColour (juce::PopupMenu::textColourId, Theme::text);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, Theme::accentDim);
        setColour (juce::PopupMenu::highlightedTextColourId, Theme::accent);
        setColour (juce::AlertWindow::backgroundColourId, Theme::panel2);
        setColour (juce::AlertWindow::textColourId, Theme::text);
        setColour (juce::TextEditor::backgroundColourId, Theme::bg);
        setColour (juce::TextEditor::textColourId, Theme::text);
    }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float startAngle, float endAngle, juce::Slider&) override
    {
        auto b = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h).reduced (3.f);
        const float d = juce::jmin (b.getWidth(), b.getHeight());
        const auto c = b.getCentre();
        const float radius = d * 0.5f;
        const float thick = juce::jmax (3.f, d * 0.04f);
        const float ringR = radius - thick;
        const float ang = startAngle + pos * (endAngle - startAngle);

        juce::Path track;
        track.addCentredArc (c.x, c.y, ringR, ringR, 0.f, startAngle, endAngle, true);
        g.setColour (Theme::accentDim);
        g.strokePath (track, juce::PathStrokeType (thick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        juce::Path val;
        val.addCentredArc (c.x, c.y, ringR, ringR, 0.f, startAngle, ang, true);
        g.setColour (Theme::accent);
        g.strokePath (val, juce::PathStrokeType (thick, juce::PathStrokeType::curved, juce::PathStrokeType::rounded));

        const float kr = ringR - thick * 2.2f;
        juce::ColourGradient grad (juce::Colour (0xff30342f), c.x - kr * 0.5f, c.y - kr * 0.7f,
                                   juce::Colour (0xff0b0d0b), c.x + kr * 0.6f, c.y + kr * 0.8f, true);
        g.setGradientFill (grad);
        g.fillEllipse (c.x - kr, c.y - kr, kr * 2.f, kr * 2.f);
        g.setColour (juce::Colour (0xff3d413c));
        g.drawEllipse (c.x - kr, c.y - kr, kr * 2.f, kr * 2.f, 1.2f);

        const float tr = kr * 0.72f;
        const float ir = juce::jmax (2.5f, thick * 0.9f);
        const float tx = c.x + tr * std::sin (ang), ty = c.y - tr * std::cos (ang);
        g.setColour (Theme::accent);
        g.fillEllipse (tx - ir, ty - ir, ir * 2.f, ir * 2.f);
    }

    void drawLinearSlider (juce::Graphics& g, int x, int y, int w, int h, float sliderPos, float minPos, float maxPos,
                           juce::Slider::SliderStyle style, juce::Slider& s) override
    {
        if (style == juce::Slider::LinearBar)
        {
            auto r = juce::Rectangle<float> ((float) x, (float) y, (float) w, (float) h);
            g.setColour (Theme::panel2);
            g.fillRoundedRectangle (r, 5.f);
            g.setColour (Theme::accentDim.withAlpha (0.7f));
            g.fillRoundedRectangle (r.withWidth (juce::jlimit (0.f, r.getWidth(), sliderPos - r.getX())), 5.f);
            g.setColour (Theme::line);
            g.drawRoundedRectangle (r.reduced (0.5f), 5.f, 1.f);
        }
        else
            LookAndFeel_V4::drawLinearSlider (g, x, y, w, h, sliderPos, minPos, maxPos, style, s);
    }

    juce::Font getSliderPopupFont (juce::Slider&) override { return mkFont (12.f); }

    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&, bool over, bool) override
    {
        auto r = b.getLocalBounds().toFloat().reduced (0.5f);
        const bool on = b.getToggleState();
        if (on)
        {
            g.setColour (Theme::accent.withAlpha (0.18f));
            g.fillRoundedRectangle (r, 5.f);
            g.setColour (Theme::accent);
            g.drawRoundedRectangle (r, 5.f, 1.4f);
        }
        else
        {
            g.setColour (over ? Theme::panel2.brighter (0.1f) : Theme::panel2);
            g.fillRoundedRectangle (r, 5.f);
            g.setColour (Theme::line);
            g.drawRoundedRectangle (r, 5.f, 1.f);
        }
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& b, bool, bool) override
    {
        g.setFont (mkFont (11.f, true));
        g.setColour (b.getToggleState() ? Theme::accent : Theme::textDim);
        g.drawText (b.getButtonText(), b.getLocalBounds(), juce::Justification::centred);
    }
};

// ======================= Boton con icono =======================
class IconButton : public juce::Button
{
public:
    using DrawFn = std::function<void (juce::Graphics&, juce::Rectangle<float>, bool on, bool over)>;
    IconButton (const juce::String& n, DrawFn f) : juce::Button (n), draw (std::move (f)) {}
    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        draw (g, getLocalBounds().toFloat(), getToggleState(), over || down);
    }
private:
    DrawFn draw;
};

// ======================= Grupo de botones ligado a un parametro Choice =======================
class ChoiceButtons : public juce::Component
{
public:
    ChoiceButtons (juce::RangedAudioParameter& p, const juce::StringArray& labels, int gap = 4)
        : gapPx (gap), attachment (p, [this] (float v) { setSelected ((int) std::lround (v)); })
    {
        for (int i = 0; i < labels.size(); ++i)
        {
            auto* b = buttons.add (new juce::TextButton (labels[i]));
            b->setClickingTogglesState (false);
            b->onClick = [this, i] { attachment.setValueAsCompleteGesture ((float) i); };
            addAndMakeVisible (b);
        }
        attachment.sendInitialUpdate();
    }

    int getSelected() const { return selected; }
    std::function<void (int)> onChange;

    void resized() override
    {
        const int n = buttons.size();
        if (n == 0) return;
        const int w = (getWidth() - gapPx * (n - 1)) / n;
        for (int i = 0; i < n; ++i)
            buttons[i]->setBounds (i * (w + gapPx), 0, w, getHeight());
    }

private:
    void setSelected (int i)
    {
        selected = i;
        for (int k = 0; k < buttons.size(); ++k)
            buttons[k]->setToggleState (k == i, juce::dontSendNotification);
        if (onChange) onChange (i);
    }

    int gapPx;
    int selected = 0;
    juce::OwnedArray<juce::TextButton> buttons;
    juce::ParameterAttachment attachment;
};

// ======================= Editor de curva =======================
// El eje X es el TIEMPO dentro del ciclo. La curva se dibuja desplazada segun el Offset,
// y la forma de onda del audio esta en el mismo eje, asi se ve exactamente donde "muerde" el efecto.
class CurveEditor : public juce::Component
{
public:
    explicit CurveEditor (KickCraftAudioProcessor& p) : proc (p) { model = proc.getCurve(); }

    std::function<void()> onEdited;

    void setModel (const CurveModel& m) { model = m; repaint(); }
    void setOffset (float o) { offset = juce::jlimit (0.f, 1.f, o); repaint(); }
    void setPlayheadVisible (bool v) { if (v != showPlayhead) { showPlayhead = v; repaint(); } }
    void setPlayhead (float p) { if (std::abs (p - playhead) > 1.0e-4f) { playhead = p; repaint(); } }

    void paint (juce::Graphics& g) override
    {
        auto full = getLocalBounds().toFloat();
        g.setColour (juce::Colour (0xff121212));
        g.fillRoundedRectangle (full, 6.f);

        auto a = area();

        // rejilla
        g.setColour (Theme::line.withAlpha (0.55f));
        for (int i = 0; i <= 16; ++i)
        {
            const float gx = a.getX() + a.getWidth() * (float) i / 16.f;
            g.drawVerticalLine ((int) gx, a.getY(), a.getBottom());
        }
        for (int i = 0; i <= 8; ++i)
        {
            const float gy = a.getY() + a.getHeight() * (float) i / 8.f;
            g.drawHorizontalLine ((int) gy, a.getX(), a.getRight());
        }

        // forma de onda del audio (entrada gris, salida naranja)
        {
            const int cols = KickCraftAudioProcessor::numWaveCols;
            const float cw = a.getWidth() / (float) cols;
            const float cy = a.getCentreY();
            for (int i = 0; i < cols; ++i)
            {
                const float hi = std::sqrt (proc.getWaveIn (i))  * a.getHeight() * 0.46f;
                const float ho = std::sqrt (proc.getWaveOut (i)) * a.getHeight() * 0.46f;
                if (hi < 0.3f && ho < 0.3f) continue;
                const float x = a.getX() + (float) i * cw;
                g.setColour (Theme::textDim.withAlpha (0.40f));
                g.fillRect (x, cy - hi, cw + 0.6f, hi * 2.f);
                g.setColour (Theme::accent.withAlpha (0.55f));
                g.fillRect (x, cy - ho, cw + 0.6f, ho * 2.f);
            }
        }

        // curva efectiva (ya desplazada por el offset)
        juce::Path curve;
        const int steps = juce::jmax (2, (int) (a.getWidth() / 2.f));
        for (int i = 0; i <= steps; ++i)
        {
            const float t = (float) i / (float) steps;
            auto p = toScreen (t, yAt (t));
            if (i == 0) curve.startNewSubPath (p); else curve.lineTo (p);
        }

        juce::Path fill (curve);
        fill.lineTo (a.getRight(), a.getBottom());
        fill.lineTo (a.getX(), a.getBottom());
        fill.closeSubPath();
        g.setGradientFill (juce::ColourGradient (Theme::accent.withAlpha (0.32f), 0.f, a.getY(),
                                                 Theme::accent.withAlpha (0.02f), 0.f, a.getBottom(), false));
        g.fillPath (fill);

        g.setColour (Theme::accent);
        g.strokePath (curve, juce::PathStrokeType (2.4f));

        // marca de INICIO de la curva cuando hay offset
        if (offset > 0.002f)
        {
            const float mx = a.getX() + offset * a.getWidth();
            g.setColour (Theme::text.withAlpha (0.55f));
            for (float yy = a.getY(); yy < a.getBottom(); yy += 8.f)
                g.fillRect (mx - 0.5f, yy, 1.f, 4.f);
            g.setFont (mkFont (9.f, true));
            const float tw = 38.f;
            const float tx = (mx + tw + 4.f > a.getRight()) ? mx - tw - 4.f : mx + 4.f;
            g.drawText ("START", juce::Rectangle<float> (tx, a.getY() + 2.f, tw, 12.f),
                        (mx + tw + 4.f > a.getRight()) ? juce::Justification::centredRight : juce::Justification::centredLeft);
        }

        // cursor de reproduccion (solo si hay audio)
        if (showPlayhead)
        {
            const auto ph = toScreen (playhead, yAt (playhead));
            g.setColour (juce::Colours::white.withAlpha (0.18f));
            g.drawVerticalLine ((int) ph.x, a.getY(), a.getBottom());
            g.setColour (juce::Colours::white);
            g.fillEllipse (ph.x - 3.5f, ph.y - 3.5f, 7.f, 7.f);
        }

        // puntos
        for (size_t i = 0; i < model.pts.size(); ++i)
        {
            const auto p = pointScreen ((int) i);
            const bool hot = ((int) i == hoverIndex || (int) i == dragIndex);
            if (hot) { g.setColour (Theme::accent.withAlpha (0.35f)); g.fillEllipse (p.x - 9.f, p.y - 9.f, 18.f, 18.f); }
            g.setColour (juce::Colours::white);
            g.fillEllipse (p.x - 4.5f, p.y - 4.5f, 9.f, 9.f);
            g.setColour (Theme::accent);
            g.drawEllipse (p.x - 4.5f, p.y - 4.5f, 9.f, 9.f, 1.2f);
        }

        g.setColour (Theme::line);
        g.drawRoundedRectangle (full.reduced (0.5f), 6.f, 1.f);
    }

    void mouseMove (const juce::MouseEvent& e) override
    {
        const int h = hitPoint (e.position);
        if (h != hoverIndex) { hoverIndex = h; repaint(); }
    }
    void mouseExit (const juce::MouseEvent&) override { hoverIndex = -1; repaint(); }

    void mouseDown (const juce::MouseEvent& e) override
    {
        dragIndex = hitPoint (e.position);

        if (e.mods.isPopupMenu() && dragIndex >= 0)
        {
            const int idx = dragIndex;
            dragIndex = -1;
            juce::PopupMenu m;
            m.addItem (1, "Segmento suave (S)", true, model.pts[(size_t) idx].smooth);
            m.addItem (2, "Resetear curvatura");
            m.addItem (3, "Borrar punto", idx > 0 && idx < (int) model.pts.size() - 1);
            juce::Component::SafePointer<CurveEditor> safe (this);
            m.showMenuAsync ({}, [safe, idx] (int r)
            {
                if (safe == nullptr || idx >= (int) safe->model.pts.size()) return;
                auto& pts = safe->model.pts;
                if (r == 1) pts[(size_t) idx].smooth = ! pts[(size_t) idx].smooth;
                else if (r == 2) pts[(size_t) idx].bend = 0.f;
                else if (r == 3) pts.erase (pts.begin() + idx);
                else return;
                safe->commit();
            });
        }
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (dragIndex < 0 || e.mods.isPopupMenu()) return;
        auto n = fromScreen (e.position);
        float nx = n.x, ny = n.y;

        if (e.mods.isCtrlDown()) { nx = std::round (nx * 16.f) / 16.f; ny = std::round (ny * 8.f) / 8.f; }

        auto& pts = model.pts;
        const int last = (int) pts.size() - 1;
        if (dragIndex == 0 || dragIndex == last)
            nx = pts[(size_t) dragIndex].x;
        else
            nx = juce::jlimit (pts[(size_t) dragIndex - 1].x + 0.004f, pts[(size_t) dragIndex + 1].x - 0.004f, nx);

        pts[(size_t) dragIndex].x = nx;
        pts[(size_t) dragIndex].y = juce::jlimit (0.f, 1.f, ny);
        commit();
    }

    void mouseUp (const juce::MouseEvent&) override { dragIndex = -1; repaint(); }

    void mouseDoubleClick (const juce::MouseEvent& e) override
    {
        const int h = hitPoint (e.position);
        auto& pts = model.pts;
        if (h >= 0)
        {
            if (h > 0 && h < (int) pts.size() - 1) { pts.erase (pts.begin() + h); commit(); }
            return;
        }
        auto n = fromScreen (e.position);
        CurvePoint np;
        np.x = juce::jlimit (0.01f, 0.99f, n.x);
        np.y = juce::jlimit (0.f, 1.f, n.y);
        pts.push_back (np);
        model.sanitize();
        commit();
    }

    void mouseWheelMove (const juce::MouseEvent& e, const juce::MouseWheelDetails& w) override
    {
        const float nx = fromScreen (e.position).x;
        auto& pts = model.pts;
        for (size_t i = 0; i + 1 < pts.size(); ++i)
            if (nx >= pts[i].x && nx <= pts[i + 1].x)
            {
                pts[i].bend = juce::jlimit (-1.f, 1.f, pts[i].bend + w.deltaY * 0.5f);
                commit();
                return;
            }
    }

private:
    juce::Rectangle<float> area() const { return getLocalBounds().toFloat().reduced (10.f); }

    // valor de la curva en el instante t (0..1) del ciclo, con offset aplicado
    float yAt (float t) const
    {
        float c = t - offset;
        c -= std::floor (c);
        return model.evaluate (c);
    }

    juce::Point<float> toScreen (float t, float y) const
    {
        auto a = area();
        return { a.getX() + t * a.getWidth(), a.getBottom() - y * a.getHeight() };
    }

    // posicion en pantalla de un punto de la curva (x de la curva + offset, con vuelta)
    juce::Point<float> pointScreen (int i) const
    {
        float t = model.pts[(size_t) i].x + offset;
        if (t > 1.0001f) t -= 1.f;
        return toScreen (t, model.pts[(size_t) i].y);
    }

    // pantalla -> coordenadas de la curva (quita el offset)
    juce::Point<float> fromScreen (juce::Point<float> p) const
    {
        auto a = area();
        const float t = juce::jlimit (0.f, 1.f, (p.x - a.getX()) / a.getWidth());
        float c = t - offset;
        if (c < 0.f) c += 1.f;
        return { juce::jlimit (0.f, 1.f, c),
                 juce::jlimit (0.f, 1.f, (a.getBottom() - p.y) / a.getHeight()) };
    }

    int hitPoint (juce::Point<float> p) const
    {
        int best = -1;
        float bestD = 10.f;
        for (int i = 0; i < (int) model.pts.size(); ++i)
        {
            const float d = pointScreen (i).getDistanceFrom (p);
            if (d < bestD) { bestD = d; best = i; }
        }
        return best;
    }

    void commit()
    {
        proc.setCurve (model);
        if (onEdited) onEdited();
        repaint();
    }

    KickCraftAudioProcessor& proc;
    CurveModel model;
    int dragIndex = -1, hoverIndex = -1;
    float playhead = 0.f, offset = 0.f;
    bool showPlayhead = false;
};

// ======================= Rejilla de presets de curva =======================
class PresetGrid : public juce::Component
{
public:
    PresetGrid() { for (int i = 0; i < CurveModel::numPresets; ++i) presets[(size_t) i] = CurveModel::preset (i); }

    std::function<void (int)> onSelect;
    void setSelected (int i) { if (i != selected) { selected = i; repaint(); } }

    void paint (juce::Graphics& g) override
    {
        for (int i = 0; i < CurveModel::numPresets; ++i)
        {
            auto r = cell (i);
            const bool on = (i == selected);
            g.setColour (on ? Theme::accent : Theme::panel2);
            g.fillRoundedRectangle (r, 5.f);
            g.setColour (on ? Theme::accent : Theme::line);
            g.drawRoundedRectangle (r.reduced (0.5f), 5.f, 1.f);

            auto ir = r.reduced (9.f, 9.f);
            juce::Path p;
            for (int s = 0; s <= 24; ++s)
            {
                const float nx = (float) s / 24.f;
                const float ny = presets[(size_t) i].evaluate (nx);
                const float px = ir.getX() + nx * ir.getWidth(), py = ir.getBottom() - ny * ir.getHeight();
                if (s == 0) p.startNewSubPath (px, py); else p.lineTo (px, py);
            }
            g.setColour (on ? Theme::bg : Theme::text);
            g.strokePath (p, juce::PathStrokeType (1.5f));
        }
    }

    void mouseDown (const juce::MouseEvent& e) override
    {
        for (int i = 0; i < CurveModel::numPresets; ++i)
            if (cell (i).contains (e.position))
            {
                selected = i;
                repaint();
                if (onSelect) onSelect (i);
                return;
            }
    }

private:
    juce::Rectangle<float> cell (int i) const
    {
        const float gap = 6.f;
        const float w = ((float) getWidth() - gap * 7.f) / 8.f;
        const float h = ((float) getHeight() - gap) / 2.f;
        const int col = i % 8, row = i / 8;
        return { (float) col * (w + gap), (float) row * (h + gap), w, h };
    }

    std::array<CurveModel, CurveModel::numPresets> presets;
    int selected = 8;
};

// ======================= Caja con el nombre del preset =======================
class NameBox : public juce::Component
{
public:
    std::function<void()> onClick;
    juce::String text = "Init";
    void setText (const juce::String& t) { text = t; repaint(); }
    void paint (juce::Graphics& g) override
    {
        g.setColour (Theme::text);
        g.setFont (mkFont (13.f, true));
        g.drawText (text, getLocalBounds(), juce::Justification::centred);
    }
    void mouseDown (const juce::MouseEvent&) override { if (onClick) onClick(); }
    juce::MouseCursor getMouseCursor() override { return juce::MouseCursor::PointingHandCursor; }
};
