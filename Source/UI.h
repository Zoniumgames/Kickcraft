#pragma once
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "Skin.h"

inline juce::Font mkFont (const Skin& s, float h, bool bold = false)
{
    return juce::Font (juce::FontOptions (s.font == FontKind::Mono ? juce::Font::getDefaultMonospacedFontName()
                                                                   : juce::Font::getDefaultSansSerifFontName(),
                                          h, bold ? juce::Font::bold : juce::Font::plain));
}

// =====================================================================================
//  Dibujo por tema
// =====================================================================================
namespace SkinDraw
{
    using PST = juce::PathStrokeType;

    inline juce::Point<float> polar (juce::Point<float> c, float r, float a) { return { c.x + r * std::sin (a), c.y - r * std::cos (a) }; }

    // ------------------------------ PERILLAS ------------------------------
    inline void knob (juce::Graphics& g, juce::Rectangle<float> bounds, float pos, float a0, float a1, const Skin& s)
    {
        auto b = bounds.reduced (3.f);
        const float d = juce::jmin (b.getWidth(), b.getHeight());
        const auto c = b.getCentre();
        const float R = d * 0.5f;
        const float ang = a0 + pos * (a1 - a0);

        auto arc = [&] (float r, float from, float to, float thick, juce::Colour col, PST::EndCapStyle cap = PST::rounded)
        {
            if (to - from < 0.003f) return;
            juce::Path p;
            p.addCentredArc (c.x, c.y, r, r, 0.f, from, to, true);
            g.setColour (col);
            g.strokePath (p, PST (thick, PST::curved, cap));
        };
        auto disc = [&] (float r, juce::Colour top, juce::Colour bot)
        {
            g.setGradientFill (juce::ColourGradient (top, c.x, c.y - r, bot, c.x, c.y + r, false));
            g.fillEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f);
        };
        auto line = [&] (float r0, float r1, float a, float w, juce::Colour col)
        {
            juce::Path p;
            p.startNewSubPath (polar (c, r0, a));
            p.lineTo (polar (c, r1, a));
            g.setColour (col);
            g.strokePath (p, PST (w, PST::curved, PST::rounded));
        };
        auto outline = [&] (float r, juce::Colour col, float w)
        {
            g.setColour (col);
            g.drawEllipse (c.x - r, c.y - r, r * 2.f, r * 2.f, w);
        };

        switch (s.knob)
        {
            case KnobStyle::Ring:
            {
                const float thick = juce::jmax (3.f, d * 0.04f), ringR = R - thick;
                arc (ringR, a0, a1, thick, s.accentDim);
                arc (ringR, a0, ang, thick, s.accent);
                const float kr = ringR - thick * 2.2f;
                g.setGradientFill (juce::ColourGradient (s.knobHi, c.x - kr * 0.5f, c.y - kr * 0.7f,
                                                         s.knobLo, c.x + kr * 0.6f, c.y + kr * 0.8f, true));
                g.fillEllipse (c.x - kr, c.y - kr, kr * 2.f, kr * 2.f);
                outline (kr, s.line.brighter (0.3f), 1.2f);
                const auto tip = polar (c, kr * 0.72f, ang);
                const float ir = juce::jmax (2.5f, thick * 0.9f);
                g.setColour (s.accent);
                g.fillEllipse (tip.x - ir, tip.y - ir, ir * 2.f, ir * 2.f);
                break;
            }
            case KnobStyle::Flat3D:
            {
                const float thick = juce::jmax (3.f, d * 0.05f), rr = R - thick * 0.5f;
                arc (rr, a0, a1, thick, s.line.darker (0.3f), PST::butt);
                arc (rr, a0, ang, thick, s.accent, PST::butt);
                const float br = rr - thick * 1.6f;
                disc (br, s.knobHi, s.knobLo);
                outline (br, juce::Colours::black.withAlpha (0.55f), 1.5f);
                arc (br - 2.f, -1.0f, 1.0f, 1.5f, juce::Colours::white.withAlpha (0.28f));
                line (br * 0.15f, br * 0.92f, ang, thick * 0.75f, s.accent);
                break;
            }
            case KnobStyle::Arc:
            {
                const float thick = juce::jmax (4.f, d * 0.1f), rr = R - thick * 0.5f - 1.f;
                arc (rr, a0, a1, thick, s.line, PST::butt);
                arc (rr, a0, ang, thick, s.accent, PST::butt);
                const float ir = rr - thick * 1.3f;
                g.setColour (s.panel2);
                g.fillEllipse (c.x - ir, c.y - ir, ir * 2.f, ir * 2.f);
                outline (ir, s.line, 1.f);
                line (ir * 0.35f, ir * 0.95f, ang, 2.2f, s.text);
                break;
            }
            case KnobStyle::Classic:
            {
                for (int i = 0; i <= 10; ++i)
                {
                    const float t = (float) i / 10.f;
                    line (R * 0.80f, R * 0.96f, a0 + (a1 - a0) * t, 1.4f, t <= pos + 0.001f ? s.accent : s.textDim);
                }
                const float br = R * 0.64f;
                disc (br, s.knobHi, s.knobLo);
                outline (br, s.line.brighter (0.4f), 1.3f);
                line (br * 0.1f, br * 0.95f, ang, 2.4f, s.text);
                break;
            }
            case KnobStyle::Metal:
            {
                arc (R - 1.5f, a0, a1, 2.6f, s.line, PST::butt);
                arc (R - 1.5f, a0, ang, 2.6f, s.accent, PST::butt);
                const float br = R * 0.76f;
                g.setGradientFill (juce::ColourGradient (s.knobHi, c.x - br * 0.35f, c.y - br * 0.4f,
                                                         s.knobLo, c.x + br * 0.6f, c.y + br * 0.7f, true));
                g.fillEllipse (c.x - br, c.y - br, br * 2.f, br * 2.f);
                for (int k = 1; k <= 3; ++k)
                    outline (br * (1.f - 0.18f * (float) k), juce::Colours::black.withAlpha (0.10f), 1.f);
                outline (br, juce::Colours::black.withAlpha (0.65f), 1.6f);
                line (br * 0.25f, br * 0.92f, ang, 3.f, juce::Colour (0xff1c2126));
                break;
            }
            case KnobStyle::Brushed:
            {
                const float thick = 3.6f, rr = R - thick;
                arc (rr, a0, a1, thick, s.line);
                arc (rr, a0, ang, thick, s.accent);
                const float br = R * 0.70f;
                disc (br, s.knobHi, s.knobLo);
                for (int i = 0; i < 6; ++i)
                    outline (br * (0.30f + 0.12f * (float) i), juce::Colours::white.withAlpha (0.045f), 1.f);
                g.setGradientFill (juce::ColourGradient (juce::Colours::white.withAlpha (0.20f), c.x, c.y - br,
                                                         juce::Colours::transparentBlack, c.x, c.y, false));
                g.fillEllipse (c.x - br, c.y - br, br * 2.f, br * 2.f);
                outline (br, juce::Colours::black.withAlpha (0.6f), 1.4f);
                const auto dp = polar (c, br * 0.74f, ang);
                g.setColour (juce::Colours::white);
                g.fillEllipse (dp.x - 3.2f, dp.y - 3.2f, 6.4f, 6.4f);
                break;
            }
            case KnobStyle::Glass:
            {
                const float thick = 3.5f, rr = R - thick * 1.5f;
                arc (rr, a0, a1, thick, s.line);
                arc (rr, a0, ang, thick * 3.2f, s.accent.withAlpha (0.22f));
                arc (rr, a0, ang, thick, s.accent);
                const float br = R * 0.68f;
                g.setGradientFill (juce::ColourGradient (s.knobHi, c.x - br * 0.3f, c.y - br * 0.4f,
                                                         s.knobLo, c.x + br * 0.7f, c.y + br * 0.8f, true));
                g.fillEllipse (c.x - br, c.y - br, br * 2.f, br * 2.f);
                outline (br, s.accent.withAlpha (0.55f), 1.3f);
                g.setColour (juce::Colours::white.withAlpha (0.16f));
                g.fillEllipse (c.x - br * 0.62f, c.y - br * 0.9f, br * 1.24f, br * 0.7f);
                const auto dp = polar (c, br * 0.76f, ang);
                g.setColour (s.accent.withAlpha (0.35f));
                g.fillEllipse (dp.x - 6.f, dp.y - 6.f, 12.f, 12.f);
                g.setColour (juce::Colours::white);
                g.fillEllipse (dp.x - 3.f, dp.y - 3.f, 6.f, 6.f);
                break;
            }
            case KnobStyle::Hardware:
            {
                for (int i = 0; i <= 10; ++i)
                {
                    const float t = (float) i / 10.f;
                    const auto p = polar (c, R * 0.90f, a0 + (a1 - a0) * t);
                    g.setColour (t <= pos + 0.001f ? s.accent : s.textDim.withAlpha (0.6f));
                    g.fillEllipse (p.x - 2.f, p.y - 2.f, 4.f, 4.f);
                }
                const float sr = R * 0.74f;
                disc (sr, juce::Colour (0xff2e2f31), juce::Colour (0xff0c0d0e));
                outline (sr, juce::Colours::black, 1.5f);
                const float br = R * 0.60f;
                disc (br, s.knobHi, s.knobLo);
                for (int i = 0; i < 30; ++i)
                    line (br * 0.86f, br * 0.99f, juce::MathConstants<float>::twoPi * (float) i / 30.f, 1.f, juce::Colours::white.withAlpha (0.12f));
                outline (br, juce::Colours::black, 1.4f);
                line (br * 0.18f, br * 0.88f, ang, 3.2f, s.text);
                break;
            }
        }
    }

    // ------------------------------ BOTONES ------------------------------
    inline juce::Colour buttonTextColour (bool on, const Skin& s)
    {
        switch (s.btn)
        {
            case BtnStyle::Rounded:   return on ? s.accent : s.textDim;
            case BtnStyle::Bevel:     return on ? s.onAccent : s.text;
            case BtnStyle::Flat:      return on ? s.onAccent : s.text;
            case BtnStyle::Outline:   return on ? s.accent : s.textDim;
            case BtnStyle::Segment:   return on ? s.onAccent : s.textDim;
            case BtnStyle::PillMetal: return on ? s.onAccent : s.text;
            case BtnStyle::PillGlow:  return on ? s.text : s.textDim;
            case BtnStyle::Rubber:    return on ? s.text : s.textDim;
        }
        return s.text;
    }

    inline void button (juce::Graphics& g, juce::Rectangle<float> r, bool on, bool over, bool down, const Skin& s)
    {
        r = r.reduced (0.5f);
        auto vgrad = [&] (juce::Colour top, juce::Colour bot)
        {
            g.setGradientFill (juce::ColourGradient (top, 0.f, r.getY(), bot, 0.f, r.getBottom(), false));
        };
        const float lift = over ? 0.08f : 0.f;

        switch (s.btn)
        {
            case BtnStyle::Rounded:
                g.setColour (on ? s.accent.withAlpha (0.18f) : (over ? s.panel2.brighter (0.1f) : s.panel2));
                g.fillRoundedRectangle (r, 5.f);
                g.setColour (on ? s.accent : s.line);
                g.drawRoundedRectangle (r, 5.f, on ? 1.4f : 1.f);
                break;

            case BtnStyle::Bevel:
            {
                const float rad = s.radius;
                if (on) vgrad (s.accent.brighter (0.2f + lift), s.accent.darker (0.25f));
                else    vgrad (s.panel.brighter (0.16f + lift), s.panel.darker (0.18f));
                if (down) vgrad (s.panel.darker (0.3f), s.panel.brighter (0.05f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (juce::Colours::black.withAlpha (0.55f));
                g.drawRoundedRectangle (r, rad, 1.f);
                g.setColour (juce::Colours::white.withAlpha (on ? 0.35f : 0.14f));
                g.drawHorizontalLine ((int) r.getY() + 1, r.getX() + rad, r.getRight() - rad);
                break;
            }
            case BtnStyle::Flat:
                g.setColour (on ? s.accent : (over ? s.panel2.brighter (0.12f) : s.panel2));
                g.fillRect (r);
                break;

            case BtnStyle::Outline:
                g.setColour (on ? s.accentDim : (over ? s.panel2.brighter (0.1f) : s.panel2));
                g.fillRoundedRectangle (r, 2.f);
                g.setColour (on ? s.accent : (over ? s.textDim : s.line));
                g.drawRoundedRectangle (r, 2.f, 1.2f);
                break;

            case BtnStyle::Segment:
                if (on) vgrad (s.accent.brighter (0.12f + lift), s.accent.darker (0.2f));
                else    vgrad (s.panel.brighter (0.06f + lift), s.panel2);
                g.fillRect (r);
                g.setColour (on ? s.accent.darker (0.5f) : s.line);
                g.drawRect (r, 1.f);
                break;

            case BtnStyle::PillMetal:
            {
                const float rad = juce::jmin (r.getHeight() * 0.5f, 15.f);
                if (on) vgrad (s.accent.brighter (0.3f + lift), s.accent.darker (0.2f));
                else    vgrad (s.panel.brighter (0.22f + lift), s.panel.darker (0.12f));
                if (down) vgrad (s.panel.darker (0.2f), s.panel.brighter (0.1f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (juce::Colours::black.withAlpha (0.55f));
                g.drawRoundedRectangle (r, rad, 1.f);
                g.setColour (juce::Colours::white.withAlpha (on ? 0.45f : 0.2f));
                g.drawHorizontalLine ((int) r.getY() + 1, r.getX() + rad * 0.7f, r.getRight() - rad * 0.7f);
                break;
            }
            case BtnStyle::PillGlow:
            {
                const float rad = juce::jmin (r.getHeight() * 0.5f, 15.f);
                if (on)
                {
                    g.setColour (s.accent.withAlpha (0.20f));
                    g.drawRoundedRectangle (r.expanded (1.f), rad + 1.f, 4.f);
                    g.setColour (s.accent.withAlpha (0.20f));
                }
                else g.setColour (s.panel2.withAlpha (over ? 0.95f : 0.65f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (on ? s.accent : s.line);
                g.drawRoundedRectangle (r, rad, on ? 1.5f : 1.f);
                break;
            }
            case BtnStyle::Rubber:
            {
                vgrad (juce::Colour (0xff4d4e50).brighter (lift), juce::Colour (0xff222324));
                if (down) vgrad (juce::Colour (0xff222324), juce::Colour (0xff3a3b3d));
                g.fillRoundedRectangle (r, 2.5f);
                g.setColour (juce::Colours::black);
                g.drawRoundedRectangle (r, 2.5f, 1.2f);
                g.setColour (juce::Colours::white.withAlpha (0.10f));
                g.drawHorizontalLine ((int) r.getY() + 1, r.getX() + 3.f, r.getRight() - 3.f);
                juce::Rectangle<float> led (r.getCentreX() - 4.f, r.getY() + 4.f, 8.f, 3.f);
                if (on)
                {
                    g.setColour (s.accent.withAlpha (0.30f));
                    g.fillRoundedRectangle (led.expanded (3.f, 2.f), 3.f);
                }
                g.setColour (on ? s.accent : s.accentDim);
                g.fillRoundedRectangle (led, 1.5f);
                break;
            }
        }
    }

    // ------------------------------ PANELES ------------------------------
    inline void screw (juce::Graphics& g, float x, float y, float rot)
    {
        g.setGradientFill (juce::ColourGradient (juce::Colour (0xffd5d5d5), x - 3.f, y - 3.f, juce::Colour (0xff555555), x + 4.f, y + 4.f, true));
        g.fillEllipse (x - 5.f, y - 5.f, 10.f, 10.f);
        g.setColour (juce::Colours::black.withAlpha (0.6f));
        g.drawEllipse (x - 5.f, y - 5.f, 10.f, 10.f, 1.f);
        juce::Path slot;
        slot.addRectangle (-3.6f, -0.8f, 7.2f, 1.6f);
        g.setColour (juce::Colour (0xff2a2a2a));
        g.fillPath (slot, juce::AffineTransform::rotation (rot).translated (x, y));
    }

    inline void panel (juce::Graphics& g, juce::Rectangle<float> r, const Skin& s)
    {
        auto vgrad = [&] (juce::Colour top, juce::Colour bot)
        {
            g.setGradientFill (juce::ColourGradient (top, 0.f, r.getY(), bot, 0.f, r.getBottom(), false));
        };
        const float rad = s.radius;

        switch (s.panelStyle)
        {
            case PanelStyle::Rounded:
                g.setColour (s.panel);
                g.fillRoundedRectangle (r, rad);
                g.setColour (s.line);
                g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.f);
                break;

            case PanelStyle::Bevel:
                vgrad (s.panel.brighter (0.07f), s.panel.darker (0.12f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (juce::Colours::black.withAlpha (0.55f));
                g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.2f);
                g.setColour (juce::Colours::white.withAlpha (0.10f));
                g.drawRoundedRectangle (r.reduced (1.8f), rad, 1.f);
                break;

            case PanelStyle::Flat:
                g.setColour (s.panel);
                g.fillRect (r);
                g.setColour (s.line);
                g.drawRect (r, 1.f);
                break;

            case PanelStyle::Utility:
                g.setColour (s.panel);
                g.fillRoundedRectangle (r, rad);
                g.setColour (s.line.brighter (0.2f));
                g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.f);
                break;

            case PanelStyle::Metal:
                vgrad (s.panel.brighter (0.12f), s.panel.darker (0.14f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (juce::Colours::black.withAlpha (0.55f));
                g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.f);
                g.setColour (juce::Colours::white.withAlpha (0.09f));
                g.drawRoundedRectangle (r.reduced (1.6f), juce::jmax (0.f, rad - 1.f), 1.f);
                break;

            case PanelStyle::Glass:
                vgrad (s.panel.withAlpha (0.95f), s.panel2.withAlpha (0.9f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (s.accent.withAlpha (0.30f));
                g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.3f);
                g.setColour (juce::Colours::white.withAlpha (0.05f));
                g.drawRoundedRectangle (r.reduced (2.f), juce::jmax (0.f, rad - 2.f), 1.f);
                break;

            case PanelStyle::Rack:
            {
                vgrad (s.panel.brighter (0.10f), s.panel.darker (0.10f));
                g.fillRoundedRectangle (r, rad);
                g.setColour (juce::Colours::white.withAlpha (0.018f));
                for (float y = r.getY() + 3.f; y < r.getBottom() - 3.f; y += 2.f)
                    g.drawHorizontalLine ((int) y, r.getX() + 2.f, r.getRight() - 2.f);
                g.setColour (juce::Colours::black.withAlpha (0.8f));
                g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.5f);
                screw (g, r.getX() + 11.f, r.getY() + 11.f, 0.5f);
                screw (g, r.getRight() - 11.f, r.getY() + 11.f, 2.1f);
                screw (g, r.getX() + 11.f, r.getBottom() - 11.f, 1.3f);
                screw (g, r.getRight() - 11.f, r.getBottom() - 11.f, 0.2f);
                break;
            }
        }
    }

    // campo hundido (barra de presets)
    inline void field (juce::Graphics& g, juce::Rectangle<float> r, const Skin& s)
    {
        const float rad = juce::jmin (s.radius, 10.f);
        const bool lcd = (s.fill == FillStyle::Hatch);
        g.setColour (lcd ? s.plot : s.panel2);
        g.fillRoundedRectangle (r, rad);
        g.setColour (juce::Colours::black.withAlpha (s.panelStyle == PanelStyle::Flat ? 0.f : 0.35f));
        g.drawRoundedRectangle (r.reduced (1.f), rad, 1.f);
        g.setColour (s.line);
        g.drawRoundedRectangle (r.reduced (0.5f), rad, 1.f);
    }
}

// =====================================================================================
//  LookAndFeel por tema
// =====================================================================================
class SkinLAF : public juce::LookAndFeel_V4
{
public:
    SkinLAF() { applyColours(); }

    void setSkin (const Skin& s) { skin = &s; applyColours(); }
    const Skin& getSkin() const { return *skin; }

    void drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float pos,
                           float a0, float a1, juce::Slider&) override
    {
        SkinDraw::knob (g, { (float) x, (float) y, (float) w, (float) h }, pos, a0, a1, *skin);
    }

    void drawButtonBackground (juce::Graphics& g, juce::Button& b, const juce::Colour&, bool over, bool down) override
    {
        SkinDraw::button (g, b.getLocalBounds().toFloat(), b.getToggleState(), over, down, *skin);
    }

    void drawButtonText (juce::Graphics& g, juce::TextButton& b, bool, bool) override
    {
        g.setFont (mkFont (*skin, 11.f, true));
        g.setColour (SkinDraw::buttonTextColour (b.getToggleState(), *skin));
        auto r = b.getLocalBounds();
        if (skin->btn == BtnStyle::Rubber) r = r.withTrimmedTop (6);
        g.drawText (b.getButtonText(), r, juce::Justification::centred);
    }

    juce::Font getLabelFont (juce::Label&) override { return mkFont (*skin, 12.f); }
    juce::Font getPopupMenuFont() override { return mkFont (*skin, 13.f); }
    juce::Font getTextButtonFont (juce::TextButton&, int) override { return mkFont (*skin, 11.f, true); }

private:
    void applyColours()
    {
        const auto& s = *skin;
        setColour (juce::Slider::textBoxTextColourId, s.text);
        setColour (juce::Slider::textBoxOutlineColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxBackgroundColourId, juce::Colours::transparentBlack);
        setColour (juce::Slider::textBoxHighlightColourId, s.accentDim);
        setColour (juce::Label::textColourId, s.text);
        setColour (juce::Label::textWhenEditingColourId, s.text);
        setColour (juce::PopupMenu::backgroundColourId, s.panel2);
        setColour (juce::PopupMenu::textColourId, s.text);
        setColour (juce::PopupMenu::headerTextColourId, s.textDim);
        setColour (juce::PopupMenu::highlightedBackgroundColourId, s.accent);
        setColour (juce::PopupMenu::highlightedTextColourId, s.onAccent);
        setColour (juce::AlertWindow::backgroundColourId, s.panel2);
        setColour (juce::AlertWindow::textColourId, s.text);
        setColour (juce::AlertWindow::outlineColourId, s.line);
        setColour (juce::TextEditor::backgroundColourId, s.plot);
        setColour (juce::TextEditor::textColourId, s.plotText);
        setColour (juce::TextEditor::highlightColourId, s.accentDim);
        setColour (juce::TextEditor::highlightedTextColourId, s.plotText);
        setColour (juce::TextEditor::outlineColourId, s.line);
        setColour (juce::TextEditor::focusedOutlineColourId, s.accent);
        setColour (juce::CaretComponent::caretColourId, s.accent);
        setColour (juce::TextButton::buttonColourId, s.panel);
        setColour (juce::TextButton::textColourOffId, s.text);
    }

    const Skin* skin = &allSkins()[0];
};

// =====================================================================================
//  Componentes
// =====================================================================================
class IconButton : public juce::Button
{
public:
    using DrawFn = std::function<void (juce::Graphics&, juce::Rectangle<float>, bool on, bool over, const Skin&)>;
    IconButton (const juce::String& n, DrawFn f) : juce::Button (n), draw (std::move (f)) {}
    void setSkin (const Skin& s) { skin = &s; repaint(); }
    void paintButton (juce::Graphics& g, bool over, bool down) override
    {
        draw (g, getLocalBounds().toFloat(), getToggleState(), over || down, *skin);
    }
private:
    DrawFn draw;
    const Skin* skin = &allSkins()[0];
};

// Logo de texto: al hacer clic se abre el menu de temas
class LogoButton : public juce::Component
{
public:
    std::function<void()> onClick;

    void setSkin (const Skin& s) { skin = &s; repaint(); }

    void paint (juce::Graphics& g) override
    {
        const auto& s = *skin;
        juce::AttributedString as;
        as.setJustification (juce::Justification::centredLeft);
        as.append ("KICK", mkFont (s, 26.f, false), s.text);
        as.append ("CRAFT", mkFont (s, 26.f, true), s.accent);
        as.draw (g, getLocalBounds().toFloat().withTrimmedRight (18.f));

        // flechita que indica que es un menu
        juce::Path tri;
        const float x = (float) getWidth() - 14.f, y = (float) getHeight() * 0.5f - 2.f;
        tri.addTriangle (x, y, x + 9.f, y, x + 4.5f, y + 5.f);
        g.setColour (hover ? s.accent : s.textDim);
        g.fillPath (tri);
    }

    void mouseEnter (const juce::MouseEvent&) override { hover = true; repaint(); }
    void mouseExit (const juce::MouseEvent&) override { hover = false; repaint(); }
    void mouseUp (const juce::MouseEvent& e) override { if (e.mouseWasClicked() && onClick) onClick(); }
    juce::MouseCursor getMouseCursor() override { return juce::MouseCursor::PointingHandCursor; }

private:
    const Skin* skin = &allSkins()[0];
    bool hover = false;
};

// Grupo de botones ligado a un parametro Choice
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
    void setGap (int g) { gapPx = g; resized(); }
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

// =====================================================================================
//  Editor de curva (eje X = tiempo del ciclo; la curva se muestra desplazada por el Offset)
// =====================================================================================
class CurveEditor : public juce::Component
{
public:
    explicit CurveEditor (KickCraftAudioProcessor& p) : proc (p) { model = proc.getCurve(); }

    std::function<void()> onEdited;

    void setSkin (const Skin& s) { skin = &s; repaint(); }
    void setModel (const CurveModel& m) { model = m; repaint(); }
    void setOffset (float o) { offset = juce::jlimit (0.f, 1.f, o); repaint(); }
    void setPlayheadVisible (bool v) { if (v != showPlayhead) { showPlayhead = v; repaint(); } }
    void setPlayhead (float p) { if (std::abs (p - playhead) > 1.0e-4f) { playhead = p; repaint(); } }

    void paint (juce::Graphics& g) override
    {
        const auto& s = *skin;
        auto full = getLocalBounds().toFloat();
        const float prad = juce::jmin (s.radius, 8.f);
        g.setColour (s.plot);
        g.fillRoundedRectangle (full, prad);

        auto a = area();

        // ---- rejilla ----
        switch (s.grid)
        {
            case GridStyle::Lines:
                g.setColour (s.gridCol);
                for (int i = 0; i <= 16; ++i)
                    g.drawVerticalLine ((int) (a.getX() + a.getWidth() * (float) i / 16.f), a.getY(), a.getBottom());
                for (int i = 0; i <= 8; ++i)
                    g.drawHorizontalLine ((int) (a.getY() + a.getHeight() * (float) i / 8.f), a.getX(), a.getRight());
                break;
            case GridStyle::Fine:
                for (int i = 0; i <= 32; ++i)
                {
                    g.setColour (s.gridCol.withAlpha (i % 4 == 0 ? 1.f : 0.45f));
                    g.drawVerticalLine ((int) (a.getX() + a.getWidth() * (float) i / 32.f), a.getY(), a.getBottom());
                }
                for (int i = 0; i <= 16; ++i)
                {
                    g.setColour (s.gridCol.withAlpha (i % 4 == 0 ? 1.f : 0.45f));
                    g.drawHorizontalLine ((int) (a.getY() + a.getHeight() * (float) i / 16.f), a.getX(), a.getRight());
                }
                break;
            case GridStyle::Dots:
                g.setColour (s.gridCol.brighter (0.5f));
                for (int ix = 0; ix <= 16; ++ix)
                    for (int iy = 0; iy <= 8; ++iy)
                        g.fillRect (a.getX() + a.getWidth() * (float) ix / 16.f - 1.f,
                                    a.getY() + a.getHeight() * (float) iy / 8.f - 1.f, 2.f, 2.f);
                break;
            case GridStyle::None:
                break;
        }

        // ---- forma de onda del audio ----
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
                g.setColour (s.waveIn.withAlpha (0.45f));
                g.fillRect (x, cy - hi, cw + 0.6f, hi * 2.f);
                g.setColour (s.waveOut.withAlpha (0.55f));
                g.fillRect (x, cy - ho, cw + 0.6f, ho * 2.f);
            }
        }

        // ---- curva efectiva (con offset) ----
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

        switch (s.fill)
        {
            case FillStyle::Gradient:
                g.setGradientFill (juce::ColourGradient (s.accent.withAlpha (0.32f), 0.f, a.getY(),
                                                         s.accent.withAlpha (0.02f), 0.f, a.getBottom(), false));
                g.fillPath (fill);
                break;
            case FillStyle::Solid:
                g.setColour (s.accent.withAlpha (0.28f));
                g.fillPath (fill);
                break;
            case FillStyle::Hatch:
                g.saveState();
                g.reduceClipRegion (fill);
                g.setColour (s.accent.withAlpha (0.35f));
                for (float xx = a.getX() - a.getHeight(); xx < a.getRight(); xx += 6.f)
                    g.drawLine (xx, a.getBottom(), xx + a.getHeight(), a.getY(), 1.f);
                g.restoreState();
                break;
            case FillStyle::None:
                break;
        }

        if (s.glow)
        {
            g.setColour (s.accent.withAlpha (0.18f));
            g.strokePath (curve, juce::PathStrokeType (s.curveWidth * 3.2f));
        }
        g.setColour (s.accent);
        g.strokePath (curve, juce::PathStrokeType (s.curveWidth));

        // ---- marca START cuando hay offset ----
        if (offset > 0.002f)
        {
            const float mx = a.getX() + offset * a.getWidth();
            g.setColour (s.plotText.withAlpha (0.55f));
            for (float yy = a.getY(); yy < a.getBottom(); yy += 8.f)
                g.fillRect (mx - 0.5f, yy, 1.f, 4.f);
            g.setFont (mkFont (s, 9.f, true));
            const float tw = 40.f;
            const bool flip = (mx + tw + 4.f > a.getRight());
            g.drawText ("START", juce::Rectangle<float> (flip ? mx - tw - 4.f : mx + 4.f, a.getY() + 2.f, tw, 12.f),
                        flip ? juce::Justification::centredRight : juce::Justification::centredLeft);
        }

        // ---- cursor ----
        if (showPlayhead)
        {
            const auto ph = toScreen (playhead, yAt (playhead));
            g.setColour (s.plotText.withAlpha (0.20f));
            g.drawVerticalLine ((int) ph.x, a.getY(), a.getBottom());
            g.setColour (s.plotText);
            g.fillEllipse (ph.x - 3.5f, ph.y - 3.5f, 7.f, 7.f);
        }

        // ---- puntos ----
        for (size_t i = 0; i < model.pts.size(); ++i)
        {
            const auto p = pointScreen ((int) i);
            const bool hot = ((int) i == hoverIndex || (int) i == dragIndex);
            if (hot || s.glow)
            {
                g.setColour (s.accent.withAlpha (hot ? 0.38f : 0.20f));
                g.fillEllipse (p.x - 9.f, p.y - 9.f, 18.f, 18.f);
            }
            juce::Path shape;
            switch (s.point)
            {
                case PointStyle::Circle:  shape.addEllipse (p.x - 4.5f, p.y - 4.5f, 9.f, 9.f); break;
                case PointStyle::Square:  shape.addRectangle (p.x - 4.5f, p.y - 4.5f, 9.f, 9.f); break;
                case PointStyle::Diamond: shape.addQuadrilateral (p.x, p.y - 6.f, p.x + 6.f, p.y, p.x, p.y + 6.f, p.x - 6.f, p.y); break;
            }
            g.setColour (s.pointFill);
            g.fillPath (shape);
            g.setColour (s.accent);
            g.strokePath (shape, juce::PathStrokeType (1.3f));
        }

        g.setColour (s.line);
        g.drawRoundedRectangle (full.reduced (0.5f), prad, 1.f);
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

    juce::Point<float> pointScreen (int i) const
    {
        float t = model.pts[(size_t) i].x + offset;
        if (t > 1.0001f) t -= 1.f;
        return toScreen (t, model.pts[(size_t) i].y);
    }

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
    const Skin* skin = &allSkins()[0];
    CurveModel model;
    int dragIndex = -1, hoverIndex = -1;
    float playhead = 0.f, offset = 0.f;
    bool showPlayhead = false;
};

// =====================================================================================
//  Rejilla de presets de curva (los botones toman la forma del tema)
// =====================================================================================
class PresetGrid : public juce::Component
{
public:
    PresetGrid() { for (int i = 0; i < CurveModel::numPresets; ++i) presets[(size_t) i] = CurveModel::preset (i); }

    std::function<void (int)> onSelect;
    void setSkin (const Skin& s) { skin = &s; repaint(); }
    void setSelected (int i) { if (i != selected) { selected = i; repaint(); } }

    void paint (juce::Graphics& g) override
    {
        const auto& s = *skin;
        for (int i = 0; i < CurveModel::numPresets; ++i)
        {
            auto r = cell (i);
            const bool on = (i == selected);
            SkinDraw::button (g, r, on, i == hover, false, s);

            auto ir = r.reduced (9.f, 9.f);
            if (s.btn == BtnStyle::Rubber) ir = ir.withTrimmedTop (3.f);
            juce::Path p;
            for (int k = 0; k <= 24; ++k)
            {
                const float nx = (float) k / 24.f;
                const float ny = presets[(size_t) i].evaluate (nx);
                const float px = ir.getX() + nx * ir.getWidth(), py = ir.getBottom() - ny * ir.getHeight();
                if (k == 0) p.startNewSubPath (px, py); else p.lineTo (px, py);
            }
            g.setColour (on ? SkinDraw::buttonTextColour (true, s) : s.text.withAlpha (0.85f));
            g.strokePath (p, juce::PathStrokeType (1.5f));
        }
    }

    void mouseMove (const juce::MouseEvent& e) override
    {
        int h = -1;
        for (int i = 0; i < CurveModel::numPresets; ++i) if (cell (i).contains (e.position)) h = i;
        if (h != hover) { hover = h; repaint(); }
    }
    void mouseExit (const juce::MouseEvent&) override { hover = -1; repaint(); }

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
        const float gap = (float) juce::jmax (skin->btnGap, 3);
        const float w = ((float) getWidth() - gap * 7.f) / 8.f;
        const float h = ((float) getHeight() - gap) / 2.f;
        const int col = i % 8, row = i / 8;
        return { (float) col * (w + gap), (float) row * (h + gap), w, h };
    }

    const Skin* skin = &allSkins()[0];
    std::array<CurveModel, CurveModel::numPresets> presets;
    int selected = 8, hover = -1;
};

// Nombre del preset (clic = menu)
class NameBox : public juce::Component
{
public:
    std::function<void()> onClick;
    juce::String text = "Init";
    void setSkin (const Skin& s) { skin = &s; repaint(); }
    void setText (const juce::String& t) { text = t; repaint(); }
    void paint (juce::Graphics& g) override
    {
        g.setColour (skin->fill == FillStyle::Hatch ? skin->accent : skin->text);
        g.setFont (mkFont (*skin, 13.f, true));
        g.drawText (text, getLocalBounds(), juce::Justification::centred);
    }
    void mouseDown (const juce::MouseEvent&) override { if (onClick) onClick(); }
    juce::MouseCursor getMouseCursor() override { return juce::MouseCursor::PointingHandCursor; }
private:
    const Skin* skin = &allSkins()[0];
};
