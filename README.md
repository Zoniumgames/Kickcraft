<img width="795" height="545" alt="image" src="https://github.com/user-attachments/assets/e5d54516-e390-4298-8f6e-c3d5ece17e88" />

# KickCraft – Sidechain / Volume Shaper VST3 (Windows, macOS y Linux)

Pon KickCraft en el sonido que quieres "bombear" (bajo, pad, synth...). No necesita señal de bombo:
el efecto se sincroniza solo con el tempo y el compás del DAW.

## Controles
| Control | Función |
|---|---|
| **MIX** | Intensidad del efecto, 0-100 % |
| **RATE** | 1/1, 1/2, 1/4, 1/8, 1/16 = cuántos golpes por compás (1, 2, 4, 8, 16) |
| **OFFSET** | Mueve la posición del efecto dentro del ciclo (0-100 %) |
| **Rejilla de 16 formas** | Curvas de fábrica listas para usar |
| **Barra de presets** | Flechas para navegar, nombre = menú, disquete = guardar preset de usuario, corazón = favorito |
| **Círculo / flecha circular** | Bypass / restablecer curva |

## Curva y forma de onda
- La curva naranja es el volumen durante un ciclo (abajo = silencio, arriba = volumen pleno).
- La onda gris es el audio de entrada y la naranja el audio ya procesado, plegados sobre un ciclo. Se llenan al reproducir.
- Edición opcional: arrastra puntos (Ctrl = snap), doble clic = crear/borrar punto, clic derecho = menú del punto, rueda sobre un segmento = curvatura.

## Offset
- La curva se dibuja ya desplazada según el Offset.
- La línea punteada **START** marca dónde empieza la curva dentro del ciclo.
- La onda de audio, el cursor y la curva comparten el mismo eje de tiempo, así se ve exactamente dónde cae el efecto sobre el sonido.
- Los puntos se pueden seguir editando con el Offset activado.

## Consumo de recursos
- Sin audio (unos 0.4 s de silencio) la onda se limpia sola y la interfaz baja a 4 refrescos por segundo, sin redibujar.
- Con audio, la interfaz se refresca a 30 fps.
- Con la ventana del plugin cerrada, la interfaz no existe y no consume nada.

## Compilar (Windows, macOS y Linux)
Opción A (sin instalar nada): sube la carpeta a un repo de GitHub. El workflow `.github/workflows/build.yml` compila los tres sistemas en paralelo (*Actions > Build KickCraft > Run workflow*) y deja un resultado por sistema en *Artifacts*:

| Artifact | Contenido |
|---|---|
| `KickCraft-Windows` | `VST3/KickCraft.vst3` y `Standalone/KickCraft.exe` |
| `KickCraft-macOS` | `KickCraft-macOS.tar.gz` con VST3, AU y Standalone (universal: Apple Silicon + Intel) |
| `KickCraft-Linux` | `KickCraft-Linux.tar.gz` con VST3 y Standalone |

Opción B (local):

- **Windows:** Visual Studio 2022 ("Desktop development with C++") + CMake:
  ```
  cmake -B build -G "Visual Studio 17 2022" -A x64
  cmake --build build --config Release
  ```
  Resultado: `build/KickCraft_artefacts/Release/VST3/KickCraft.vst3`

- **macOS:** Xcode + CMake:
  ```
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build
  ```

- **Linux:** instala las dependencias y compila:
  ```
  sudo apt install libasound2-dev libfreetype-dev libx11-dev libxrandr-dev \
    libxinerama-dev libxcursor-dev libxext-dev libgl1-mesa-dev libfontconfig1-dev cmake g++
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build
  ```

En macOS y Linux el resultado queda en `build/KickCraft_artefacts/Release/`.

## Instalación
| Sistema | Dónde copiarlo |
|---|---|
| **Windows** | Copia la carpeta `KickCraft.vst3` a `C:\Program Files\Common Files\VST3\` |
| **macOS** | `KickCraft.vst3` a `~/Library/Audio/Plug-Ins/VST3/` y `KickCraft.component` (AU) a `~/Library/Audio/Plug-Ins/Components/` |
| **Linux** | `KickCraft.vst3` a `~/.vst3/` |

**Nota para macOS:** el plugin no está firmado ni notarizado, por lo que macOS lo bloquea. Después de copiarlo, ejecuta en Terminal:
```
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/VST3/KickCraft.vst3
xattr -dr com.apple.quarantine ~/Library/Audio/Plug-Ins/Components/KickCraft.component
```

Después abre tu DAW y haz un rescan de plugins.

JUCE es AGPLv3 / licencia comercial: revísalo si lo vas a distribuir.


## License

KickCraft is free and open-source software licensed under
the GNU Affero General Public License v3.0 (AGPL-3.0).

You are free to use, study, modify, and redistribute this
software under the terms of the license.

Modified versions and forks must comply with the AGPL-3.0.

See the [LICENSE](LICENSE) file for the complete license.

Copyright (C) 2026 Zoniumgames
