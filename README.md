# Minecraft C++ Voxel Engine (1:1 Remake)

Eine hoch-performante, modulare Voxel-Engine in C++20 und OpenGL 4.5 nach dem Vorbild von Minecraft.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)

## Feature-Übersicht & Stand

### 💾 Welt-Speichersystem (Save / Load Persistence)
- **Binäre Chunk-Serialisierung (`SaveSystem`)**: Modifizierte Chunks werden beim Beenden automatisch in `world_saves/chunk_X_Z.bin` gespeichert und beim Start geladen. Gebaute und abgebaute Strukturen bleiben dauerhaft erhalten!

### 🏔️ 3D Höhlensysteme, Erze & Unterwelt
- **3D Noise Caves**: Unterirdische Tunnelgänge, Grotten und tief liegende Lava-Seen (Y < 10).
- **Voxel-Erzadern in Steinschichten**:
  - 💎 **Diamant-Erz** (`DiamondOre`, Y: 1 bis 16)
  - 🟡 **Gold-Erz** (`GoldOre`, Y: 5 bis 30)
  - ⚪ **Eisen-Erz** (`IronOre`, Y: 5 bis 45)
  - ⬛ **Kohle-Erz** (`CoalOre`, Y: 5 bis 60)

### 💡 Smooth Lighting & Ambient Occlusion (AO)
- **Vertex Ambient Occlusion**: Weiche Eck- und Kanten-Schatten an angrenzenden Voxel-Blöcken für plastischen Tiefeneffekt.

### 🔊 Audio Subsystem & Sound Effects
- **Synthesizer & Audio Feedback**: Soundeffekt-Trigger beim Abbauen/Platzieren von Blöcken und Springen.

### 🎮 Gameplay & Raycasting
- **Voxel Raycasting (DDA Algorithm)**: Linksklick zum Abbauen, Rechtsklick zum Platzieren von Blöcken.
- **Hotbar Slot-Auswahl (`1-9`)**: Schnellzugriff auf Gras, Erde, Stein, Holzstamm, Blätter, Bretter, Glas, Sand, Bedrock & Erze.

### 🖥️ GUI, HUD & F3 Debug Screen
- **Fadenkreuz (Crosshair)** & **2D Hotbar** mit visueller Slot-Hervorhebung.
- **`F3` Debug Screen**: Live-Anzeige von FPS, Frametime, XYZ-Position, Chunk-Koordinaten & Flug-/Physik-Status.

---

## Steuerung & Tastatur-Layout

| Taste / Eingabe | Aktion |
|-----------------|--------|
| `W / A / S / D` | Vorwärts, links, rückwärts, rechts bewegen |
| `Space` | Springen (im Laufmodus) / Nach oben fliegen (im Flugmodus) |
| `L-Shift` | Nach unten fliegen (im Flugmodus) |
| `F` | Umschalten zwischen Flugmodus & Voxel-Physik |
| `F3` | Debug-Bildschirm (FPS, Koordinaten, Facing) ein-/ausblenden |
| `1 - 9` | Hotbar Slot wählen |
| `Linksklick` | Block unter Fadenkreuz abbauen (inkl. Sound) |
| `Rechtsklick` | Ausgewählten Block platzieren (inkl. Sound) |
| `Maus bewegen` | Umsehen (Pitch / Yaw) |
| `Escape` | Spiel beenden (Automatisches Speichern der Welt) |

---

## Build & Ausführung

### Kompilieren (PowerShell / Terminal)

```powershell
# 1. Build-Ordner erstellen & CMake Konfiguration
cmake -B build -S .

# 2. Projekt kompilieren
cmake --build build --config Release

# 3. Spiel starten
.\build\Release\Minecraft.exe
```

## Architektur

Vollständige Dokumentation der C++ Engine-Module befindet sich in [`docs/ARCHITECTURE.md`](file:///c:/Users/sche-/Desktop/Programmieren%20Projekte/Minecraft/docs/ARCHITECTURE.md).
