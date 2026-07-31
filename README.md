# Minecraft C++ Voxel Engine (1:1 Remake)

Eine hoch-performante, modulare Voxel-Engine in C++20 und OpenGL 4.5 nach dem Vorbild von Minecraft.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)

## Features & Architektur

- **C++20 & OpenGL 4.5 Core Profile** für maximale Hardware-Performance.
- **Automatisiertes Dependency Management**: CMake `FetchContent` lädt GLFW, GLM, GLAD, EnTT und FastNoiseLite beim Build automatisch.
- **Voxel Engine & Chunk System**:
  - Chunks mit 16x256x16 Blöcken.
  - **Culled Face Meshing**: Verhindert das Zeichnen verdeckter Voxel-Flächen für extrem hohe Bildraten.
  - Prozedurale Terrain-Generierung mit FastNoiseLite (Hügel, Berge, Höhlen, Biome).
- **FPS 3D-Kamera & Steuerung**: W/A/S/D Navigation, Mauseingabe mit Pitch/Yaw Clamping, Smooth Fly/Walk Modus.
- **Entity Component System (ECS)**: EnTT Integration für performante Trennung von Spieldaten (Transform, Velocity, Rendering) und Systemen (Physik, Input, Animation).

## Quick Start & Build

### Voraussetzungen
- C++20 fähiger Compiler (MSVC / GCC / Clang)
- CMake >= 3.20
- Internetverbindung beim ersten Build (FetchContent lädt Abhängigkeiten herunter)

### Kompilieren & Ausführen (Windows / PowerShell)

```powershell
# 1. Build-Ordner erstellen & CMake konfigurieren
cmake -B build -S .

# 2. Projekt kompilieren
cmake --build build --config Release

# 3. Spiel ausführen
.\build\Release\Minecraft.exe
```

## Steuerung

| Taste | Aktion |
|-------|--------|
| `W / A / S / D` | Kamera nach vorne, links, hinten, rechts bewegen |
| `Space / L-Shift` | Nach oben / unten fliegen |
| `Maus bewegen` | Umsehen (Pitch / Yaw) |
| `Escape` | Mauszeiger freigeben / Spiel beenden |
| `F3` | Debug-Informationen / Stats |

## Dokumentation

Die vollständige System- und Erweiterungsdokumentation befindet sich in [`docs/ARCHITECTURE.md`](file:///c:/Users/sche-/Desktop/Programmieren%20Projekte/Minecraft/docs/ARCHITECTURE.md).
