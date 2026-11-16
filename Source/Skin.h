#pragma once
#include <juce_graphics/juce_graphics.h>
#include <array>

// ---- Estilos visuales (cada tema elige uno de cada) ----
enum class KnobStyle  { Ring, Flat3D, Arc, Classic, Metal, Brushed, Glass, Hardware };
enum class BtnStyle   { Rounded, Bevel, Flat, Outline, Segment, PillMetal, PillGlow, Rubber };
enum class PanelStyle { Rounded, Bevel, Flat, Utility, Metal, Glass, Rack };
enum class GridStyle  { Lines, Dots, Fine, None };
enum class FillStyle  { Gradient, Solid, Hatch, None };
enum class PointStyle { Circle, Square, Diamond };
enum class FontKind   { Sans, Mono };

struct Skin
{
    const char* name = "";
    juce::Colour bgTop, bgBottom, panel, panel2, plot, plotText, gridCol, accent, accentDim, onAccent,
                 text, textDim, line, waveIn, waveOut, pointFill, knobHi, knobLo;
    KnobStyle  knob       = KnobStyle::Ring;
    BtnStyle   btn        = BtnStyle::Rounded;
    PanelStyle panelStyle = PanelStyle::Rounded;
    GridStyle  grid       = GridStyle::Lines;
    FillStyle  fill       = FillStyle::Gradient;
    PointStyle point      = PointStyle::Circle;
    FontKind   font       = FontKind::Sans;
    float radius = 8.f;       // redondeo de paneles
    float curveWidth = 2.4f;
    bool  glow = false;       // brillo neon
    int   btnGap = 4;         // separacion entre botones
};

namespace Skins
{
    using C = juce::Colour;

    // 0 - Default: minimalista naranja / negro
    inline Skin makeDefault()
    {
        Skin s; s.name = "Default";
        s.bgTop = s.bgBottom = C (0xff0e0e0e);
        s.panel = C (0xff171717); s.panel2 = C (0xff202020); s.plot = C (0xff121212);
        s.plotText = C (0xffeeeeee); s.gridCol = C (0xff2e2e2e);
        s.accent = C (0xffff7a1a); s.accentDim = C (0xff5c2c0c); s.onAccent = C (0xff0e0e0e);
        s.text = C (0xffeeeeee); s.textDim = C (0xff8a8a8a); s.line = C (0xff2e2e2e);
        s.waveIn = C (0xff8a8a8a); s.waveOut = C (0xffff7a1a); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xff30342f); s.knobLo = C (0xff0b0d0b);
        s.knob = KnobStyle::Ring; s.btn = BtnStyle::Rounded; s.panelStyle = PanelStyle::Rounded;
        s.grid = GridStyle::Lines; s.fill = FillStyle::Gradient; s.point = PointStyle::Circle; s.font = FontKind::Sans;
        s.radius = 10.f; s.curveWidth = 2.4f; s.glow = false; s.btnGap = 4;
        return s;
    }

    // 1 - Mango Studio: gris azulado, perillas 3D, botones biselados, naranja mango
    inline Skin makeMango()
    {
        Skin s; s.name = "Mango Studio";
        s.bgTop = C (0xff3a444d); s.bgBottom = C (0xff2d353c);
        s.panel = C (0xff414c56); s.panel2 = C (0xff2f3840); s.plot = C (0xff262d33);
        s.plotText = C (0xffe6ebf0); s.gridCol = C (0xff39434c);
        s.accent = C (0xffff8c1a); s.accentDim = C (0xff6b4312); s.onAccent = C (0xff1d1d1d);
        s.text = C (0xffe6ebf0); s.textDim = C (0xff9aa7b2); s.line = C (0xff56626d);
        s.waveIn = C (0xff7d8b97); s.waveOut = C (0xffffb347); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xff8d9aa6); s.knobLo = C (0xff3b444c);
        s.knob = KnobStyle::Flat3D; s.btn = BtnStyle::Bevel; s.panelStyle = PanelStyle::Bevel;
        s.grid = GridStyle::Fine; s.fill = FillStyle::Gradient; s.point = PointStyle::Circle; s.font = FontKind::Sans;
        s.radius = 4.f; s.curveWidth = 2.2f; s.glow = false; s.btnGap = 3;
        return s;
    }

    // 2 - Alberto: gris claro plano, todo cuadrado, amarillo ambar
    inline Skin makeAlberto()
    {
        Skin s; s.name = "Alberto";
        s.bgTop = s.bgBottom = C (0xffb4b4b4);
        s.panel = C (0xffc9c9c9); s.panel2 = C (0xffa9a9a9); s.plot = C (0xff2b2b2b);
        s.plotText = C (0xffdddddd); s.gridCol = C (0xff3f3f3f);
        s.accent = C (0xffffb400); s.accentDim = C (0xff8a6a14); s.onAccent = C (0xff151515);
        s.text = C (0xff151515); s.textDim = C (0xff565656); s.line = C (0xff8d8d8d);
        s.waveIn = C (0xff8f8f8f); s.waveOut = C (0xffffb400); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xffd8d8d8); s.knobLo = C (0xffbdbdbd);
        s.knob = KnobStyle::Arc; s.btn = BtnStyle::Flat; s.panelStyle = PanelStyle::Flat;
        s.grid = GridStyle::Lines; s.fill = FillStyle::Solid; s.point = PointStyle::Square; s.font = FontKind::Sans;
        s.radius = 0.f; s.curveWidth = 2.0f; s.glow = false; s.btnGap = 2;
        return s;
    }

    // 3 - Devils: gris carbon utilitario, rojo, tipografia monoespaciada
    inline Skin makeDevils()
    {
        Skin s; s.name = "Devils";
        s.bgTop = s.bgBottom = C (0xff2b2b2b);
        s.panel = C (0xff333333); s.panel2 = C (0xff262626); s.plot = C (0xff1a1a1a);
        s.plotText = C (0xffd8d8d8); s.gridCol = C (0xff2f2f2f);
        s.accent = C (0xffe53935); s.accentDim = C (0xff5a1b19); s.onAccent = C (0xffffffff);
        s.text = C (0xffd8d8d8); s.textDim = C (0xff8c8c8c); s.line = C (0xff4a4a4a);
        s.waveIn = C (0xff6a6a6a); s.waveOut = C (0xffe53935); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xff4a4a4a); s.knobLo = C (0xff222222);
        s.knob = KnobStyle::Classic; s.btn = BtnStyle::Outline; s.panelStyle = PanelStyle::Utility;
        s.grid = GridStyle::Fine; s.fill = FillStyle::None; s.point = PointStyle::Square; s.font = FontKind::Mono;
        s.radius = 2.f; s.curveWidth = 1.6f; s.glow = false; s.btnGap = 2;
        return s;
    }

    // 4 - Bro Tools: grafito frio, azul, botones segmentados, perillas plateadas
    inline Skin makeBroTools()
    {
        Skin s; s.name = "Bro Tools";
        s.bgTop = C (0xff23272b); s.bgBottom = C (0xff1a1d20);
        s.panel = C (0xff2d3237); s.panel2 = C (0xff22262a); s.plot = C (0xff15181b);
        s.plotText = C (0xffdfe6ee); s.gridCol = C (0xff262c32);
        s.accent = C (0xff2f8cff); s.accentDim = C (0xff173f72); s.onAccent = C (0xffffffff);
        s.text = C (0xffdfe6ee); s.textDim = C (0xff8793a0); s.line = C (0xff3b434b);
        s.waveIn = C (0xff5d6b7a); s.waveOut = C (0xff62adff); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xffe4e8ec); s.knobLo = C (0xff6f7780);
        s.knob = KnobStyle::Metal; s.btn = BtnStyle::Segment; s.panelStyle = PanelStyle::Metal;
        s.grid = GridStyle::Lines; s.fill = FillStyle::Gradient; s.point = PointStyle::Square; s.font = FontKind::Sans;
        s.radius = 3.f; s.curveWidth = 2.0f; s.glow = false; s.btnGap = 1;
        return s;
    }

    // 5 - Logico: aluminio grafito cepillado, botones pastilla, azul cielo
    inline Skin makeLogico()
    {
        Skin s; s.name = "Logico";
        s.bgTop = C (0xff3b3d41); s.bgBottom = C (0xff2a2c2f);
        s.panel = C (0xff484b50); s.panel2 = C (0xff2f3236); s.plot = C (0xff1e2124);
        s.plotText = C (0xffeceff3); s.gridCol = C (0xff30343a);
        s.accent = C (0xff35aaff); s.accentDim = C (0xff1d4d73); s.onAccent = C (0xffffffff);
        s.text = C (0xffeceff3); s.textDim = C (0xff9a9fa7); s.line = C (0xff5a5e64);
        s.waveIn = C (0xff7b8088); s.waveOut = C (0xff5cc0ff); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xff6b6f76); s.knobLo = C (0xff2b2d31);
        s.knob = KnobStyle::Brushed; s.btn = BtnStyle::PillMetal; s.panelStyle = PanelStyle::Metal;
        s.grid = GridStyle::Dots; s.fill = FillStyle::Gradient; s.point = PointStyle::Circle; s.font = FontKind::Sans;
        s.radius = 12.f; s.curveWidth = 2.4f; s.glow = false; s.btnGap = 5;
        return s;
    }

    // 6 - Lunatic: violeta profundo, cristal, neon
    inline Skin makeLunatic()
    {
        Skin s; s.name = "Lunatic";
        s.bgTop = C (0xff1b1530); s.bgBottom = C (0xff0d0a18);
        s.panel = C (0xff241c3d); s.panel2 = C (0xff1a1430); s.plot = C (0xff120e22);
        s.plotText = C (0xfff0eaff); s.gridCol = C (0xff261d45);
        s.accent = C (0xffa66bff); s.accentDim = C (0xff4b2d86); s.onAccent = C (0xffffffff);
        s.text = C (0xfff0eaff); s.textDim = C (0xff9a8fc0); s.line = C (0xff3b2f66);
        s.waveIn = C (0xff6f62a3); s.waveOut = C (0xffff5fd2); s.pointFill = C (0xffffffff);
        s.knobHi = C (0xff5b3fa8); s.knobLo = C (0xff1d1238);
        s.knob = KnobStyle::Glass; s.btn = BtnStyle::PillGlow; s.panelStyle = PanelStyle::Glass;
        s.grid = GridStyle::Dots; s.fill = FillStyle::Gradient; s.point = PointStyle::Circle; s.font = FontKind::Sans;
        s.radius = 16.f; s.curveWidth = 2.6f; s.glow = true; s.btnGap = 6;
        return s;
    }

    // 7 - Razor: rack de hardware, tornillos, pantalla LCD ambar
    inline Skin makeRazor()
    {
        Skin s; s.name = "Razor";
        s.bgTop = C (0xff1c1c1c); s.bgBottom = C (0xff141414);
        s.panel = C (0xff3a3b3d); s.panel2 = C (0xff2a2b2c); s.plot = C (0xff161200);
        s.plotText = C (0xffffb000); s.gridCol = C (0xff2e2400);
        s.accent = C (0xffffb000); s.accentDim = C (0xff4d3500); s.onAccent = C (0xff1a1200);
        s.text = C (0xffe8e6df); s.textDim = C (0xff9b978a); s.line = C (0xff55565a);
        s.waveIn = C (0xff7a5c00); s.waveOut = C (0xffffb000); s.pointFill = C (0xffffd36b);
        s.knobHi = C (0xff55575a); s.knobLo = C (0xff151617);
        s.knob = KnobStyle::Hardware; s.btn = BtnStyle::Rubber; s.panelStyle = PanelStyle::Rack;
        s.grid = GridStyle::Fine; s.fill = FillStyle::Hatch; s.point = PointStyle::Diamond; s.font = FontKind::Mono;
        s.radius = 3.f; s.curveWidth = 1.8f; s.glow = false; s.btnGap = 6;
        return s;
    }
}

inline const std::array<Skin, 8>& allSkins()
{
    static const std::array<Skin, 8> skins {{ Skins::makeDefault(), Skins::makeMango(), Skins::makeAlberto(), Skins::makeDevils(),
                                              Skins::makeBroTools(), Skins::makeLogico(), Skins::makeLunatic(), Skins::makeRazor() }};
    return skins;
}
