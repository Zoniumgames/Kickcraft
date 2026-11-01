#pragma once
#include <juce_core/juce_core.h>
#include <algorithm>
#include <array>
#include <cmath>
#include <vector>

// Un punto de la curva. 'smooth' = segmento en S (tangente horizontal), 'bend' = curvatura extra del segmento (-1..1)
struct CurvePoint
{
    float x = 0.f, y = 0.f;
    bool smooth = true;
    float bend = 0.f;
};

class CurveModel
{
public:
    static constexpr int lutSize = 2048;
    static constexpr int numPresets = 16;

    CurveModel() { pts = { { 0.f, 0.f, true, 0.f }, { 1.f, 1.f, true, 0.f } }; }

    std::vector<CurvePoint> pts;

    static float shape (float u, bool smooth, float bend)
    {
        float s = smooth ? u * u * (3.f - 2.f * u) : u;
        if (bend > 0.f)      s = 1.f - std::pow (1.f - s, 1.f + 3.f * bend);
        else if (bend < 0.f) s = std::pow (s, 1.f + 3.f * (-bend));
        return s;
    }

    float evaluate (float x) const
    {
        if (pts.empty()) return 1.f;
        if (x <= pts.front().x) return pts.front().y;
        if (x >= pts.back().x)  return pts.back().y;

        for (size_t i = 0; i + 1 < pts.size(); ++i)
        {
            const auto& a = pts[i];
            const auto& b = pts[i + 1];
            if (x <= b.x)
            {
                const float w = b.x - a.x;
                const float u = w > 1.0e-6f ? (x - a.x) / w : 1.f;
                return a.y + (b.y - a.y) * shape (u, a.smooth, a.bend);
            }
        }
        return pts.back().y;
    }

    void buildLut (std::array<float, lutSize + 1>& out) const
    {
        for (int i = 0; i <= lutSize; ++i)
            out[(size_t) i] = juce::jlimit (0.f, 1.f, evaluate ((float) i / (float) lutSize));
    }

    void sanitize()
    {
        if (pts.size() < 2) { *this = CurveModel(); return; }
        for (auto& p : pts)
        {
            p.x = juce::jlimit (0.f, 1.f, p.x);
            p.y = juce::jlimit (0.f, 1.f, p.y);
            p.bend = juce::jlimit (-1.f, 1.f, p.bend);
        }
        std::stable_sort (pts.begin(), pts.end(), [] (const CurvePoint& a, const CurvePoint& b) { return a.x < b.x; });
        pts.front().x = 0.f;
        pts.back().x = 1.f;
    }

    juce::String toString() const
    {
        juce::StringArray parts;
        for (auto& p : pts)
            parts.add (juce::String (p.x, 5) + "," + juce::String (p.y, 5) + "," + (p.smooth ? "1" : "0") + "," + juce::String (p.bend, 4));
        return parts.joinIntoString (";");
    }

    static CurveModel fromString (const juce::String& s)
    {
        CurveModel m;
        m.pts.clear();
        for (auto& part : juce::StringArray::fromTokens (s, ";", ""))
        {
            auto f = juce::StringArray::fromTokens (part, ",", "");
            if (f.size() < 4) continue;
            m.pts.push_back ({ f[0].getFloatValue(), f[1].getFloatValue(), f[2].getIntValue() != 0, f[3].getFloatValue() });
        }
        m.sanitize();
        return m;
    }

    static juce::String presetName (int i)
    {
        static const char* n[numPresets] = { "Fast Linear", "Slow Linear", "Trapezoid", "Hard Gate", "Late Ramp", "V Dip",
                                             "Soft Dip", "Late Hit", "Classic S", "Exponential", "Logarithmic", "Double Pump",
                                             "Bump", "Half Duck", "Smooth Hold", "Sawtooth" };
        return n[juce::jlimit (0, numPresets - 1, i)];
    }

    static CurveModel preset (int i)
    {
        auto P = [] (float x, float y, bool s = true, float b = 0.f) { return CurvePoint { x, y, s, b }; };
        CurveModel m;
        switch (i)
        {
            case 0:  m.pts = { P (0, 0, false), P (.30f, 1, false), P (1, 1) }; break;
            case 1:  m.pts = { P (0, 0, false), P (.65f, 1, false), P (1, 1) }; break;
            case 2:  m.pts = { P (0, 0, false), P (.15f, 0, false), P (.45f, 1, false), P (1, 1) }; break;
            case 3:  m.pts = { P (0, 0, false), P (.49f, 0, false), P (.50f, 1, false), P (1, 1) }; break;
            case 4:  m.pts = { P (0, 0, false), P (.40f, 0, false), P (.80f, 1, false), P (1, 1) }; break;
            case 5:  m.pts = { P (0, 1, false), P (.20f, 0, false), P (.70f, 1, false), P (1, 1) }; break;
            case 6:  m.pts = { P (0, .6f, false), P (.15f, 0, false), P (.60f, .8f, false), P (1, 1) }; break;
            case 7:  m.pts = { P (0, 0, false), P (.90f, 0, false), P (.95f, 1, false), P (1, 1) }; break;
            case 8:  m.pts = { P (0, 0), P (.50f, .55f), P (.93f, 1), P (1, 1) }; break;
            case 9:  m.pts = { P (0, 0, false, -.6f), P (1, 1) }; break;
            case 10: m.pts = { P (0, 0, false, .7f), P (1, 1) }; break;
            case 11: m.pts = { P (0, 0, false), P (.20f, 1, false), P (.32f, .35f, false), P (.55f, 1, false), P (1, 1) }; break;
            case 12: m.pts = { P (0, .25f), P (.35f, 0), P (.70f, 1), P (1, 1) }; break;
            case 13: m.pts = { P (0, .5f, false), P (.30f, .5f, false), P (.70f, 1, false), P (1, 1) }; break;
            case 14: m.pts = { P (0, 0), P (.40f, .05f), P (.80f, .95f), P (1, 1) }; break;
            default: m.pts = { P (0, 0, false), P (.25f, .5f, false), P (.50f, .25f, false), P (.80f, 1, false), P (1, 1) }; break;
        }
        m.sanitize();
        return m;
    }
};
