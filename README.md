<img width="795" height="545" alt="image" src="https://github.com/user-attachments/assets/e5d54516-e390-4298-8f6e-c3d5ece17e88" />

# KickCraft – Sidechain / Volume Shaper VST3 (Windows)

Pon KickCraft en el sonido que quieres "bombear" (bajo, pad, synth...). No necesita señal de bombo:
el efecto se sincroniza solo con el tempo y el compas del DAW.

## Controles
| Control | Funcion |
|---|---|
| **MIX** | Intensidad del efecto, 0-100 % |
| **RATE** | 1/1, 1/2, 1/4, 1/8, 1/16 = cuantos golpes por compas (1, 2, 4, 8, 16) |
| **OFFSET** | Mueve la posicion del efecto dentro del ciclo (0-100 %) |
| **Rejilla de 16 formas** | Curvas de fabrica listas para usar |
| **Barra de presets** | Flechas para navegar, nombre = menu, disquete = guardar preset de usuario, corazon = favorito |
| **Circulo / flecha circular** | Bypass / restablecer curva |

## Curva y forma de onda
- La curva naranja es el volumen durante un ciclo (abajo = silencio, arriba = volumen pleno).
- La onda gris es el audio de entrada y la naranja el audio ya procesado, plegados sobre un ciclo. Se llenan al reproducir.
- Edicion opcional: arrastra puntos (Ctrl = snap), doble clic = crear/borrar punto, clic derecho = menu del punto, rueda sobre un segmento = curvatura.

## Compilar en Windows
Opcion A (sin instalar nada): sube la carpeta a un repo de GitHub; el workflow `.github/workflows/build.yml` compila y deja `KickCraft.vst3` en *Actions > Artifacts* (descargalo, descomprime y copia la carpeta KickCraft.vst3).

Opcion B (local): Visual Studio 2022 ("Desktop development with C++") + CMake:
```
cmake -B build -G "Visual Studio 17 2022" -A x64
cmake --build build --config Release
```
Resultado: `build/KickCraft_artefacts/Release/VST3/KickCraft.vst3`
Copialo a `C:\Program Files\Common Files\VST3\`.

JUCE es AGPLv3 / licencia comercial: revisalo si lo vas a distribuir.
