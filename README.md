# Minecraft C++ Voxel Engine (1:1 Remake)

Eine hoch-performante, modulare Voxel-Engine in C++20 und OpenGL 4.5 nach dem Vorbild von Minecraft.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)

## Feature-Übersicht & Stand

### 🎮 Gameplay & Interaktionen
- **Voxel Raycasting (DDA Algorithm)**:
  - **Linksklick**: Abbauen des angeblickten Blocks unter dem Fadenkreuz (`BlockType::Air`).
  - **Rechtsklick**: Platzieren eines neuen Blocks an der angrenzenden Blockfläche.
  - **Hotbar Slot-Auswahl (`1-9`)**: Schnellzugriff auf Gras, Erde, Stein, Holzstamm, Blätter, Holzbretter, Glas, Sand und Bedrock.

### 🖥️ GUI, HUD & F3 Debug Screen
- **Zentriertes Fadenkreuz (Crosshair)**: Dynamisches UI-Overlay in Bildschirmmitte.
- **2D Hotbar**: 9-Slot Auswahlleiste mit visueller Hervorhebung des aktiven Slots.
- **`F3` Debug Overlay**: Ein- und Ausblendung von Live-Engine-Daten:
  - Framerate (FPS) & Frametime.
  - Spieler-Position (`XYZ`) & Chunk-Koordinaten.
  - Blickrichtung (Pitch / Yaw).
  - Flug- vs. Physik-Laufmodus Indicator.

### 🏔️ Weltgenerierung & Meshing
- **FastNoiseLite Integration**: Simplex Noise für Höhenlandschaften, Hügel und prozedurale **Eichenbäume** (Holzstamm + Blätterdach).
- **Culled Face Meshing**: Entfernung verdeckter Block-Innenflächen zur Reduktion der Polycount um >80%.
- **Textur-Atlas Mapping**: Dynamic Pro-Face UV-Berechnung für Gras (Oben/Seite/Unten), Erde, Stein, Holz, Blätter, Sand, Bretter, Glas & Bedrock.

### ⚡ Physik & Steuerung
- **AABB-Kollisionsabfrage**: Bounding-Box Kollision zwischen Spieler (0.6 x 1.8 m) und Voxel-Welt.
- **Gravitation & Springen**: Realistische Schwerkraft & Sprungimpuls (`Space`).
- **`F`-Taste**: Umschalten zwischen **Flugmodus** (freie 3D-Kamera) und **Laufmodus** (Voxel-Physik).

---

## Steuerung & Tastatur-Layout

| Taste / Eingabe | Aktion |
|-----------------|--------|
| `W / A / S / D` | Vorwärts, links, rückwärts, rechts bewegen |
| `Space` | Springen (im Laufmodus) / Nach oben fliegen (im Flugmodus) |
| `L-Shift` | Nach unten fliegen (im Flugmodus) |
| `F` | Umschalten zwischen Flugmodus & Voxel-Physik |
| `F3` | Debug-Bildschirm (FPS, Koordinaten, Facing) ein-/ausblenden |
| `1 - 9` | Hotbar Slot wählen (Gras, Erde, Stein, Holz, Blätter, Bretter, Glas, Sand, Bedrock) |
| `Linksklick` | Block unter Fadenkreuz abbauen |
| `Rechtsklick` | Ausgewählten Block platzieren |
| `Maus bewegen` | Umsehen (Pitch / Yaw) |
| `Escape` | Spiel beenden |

---

## Build & Ausführung

### Voraussetzungen
- C++20 fähiger Compiler (MSVC / GCC / Clang)
- CMake >= 3.20

### Kompilieren (PowerShell / Terminal)

```powershell
# 1. Build-Ordner erstellen & CMake Konfiguration
cmake -B build -S .

# 2. Projekt kompilieren
cmake --build build --config Release

# 3. Spiel starten
.\build\Release\Minecraft.exe
```

## Dokumentation & Entwicklungs-Plan

- [`docs/ARCHITECTURE.md`](file:///c:/Users/sche-/Desktop/Programmieren%20Projekte/Minecraft/docs/ARCHITECTURE.md): Detaillierte Systemarchitektur & Erweiterungspunkte.
