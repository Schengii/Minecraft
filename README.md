# Minecraft C++ Voxel Engine (1:1 Remake)

Eine hoch-performante, modulare Voxel-Engine in C++20 und OpenGL 4.5 nach dem Vorbild von Minecraft.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)

## Feature-Übersicht & Stand

### 🔴 Redstone Logik & Schaltungen (`RedstoneEngine`)
- **Signal-Ausbreitung & Interaktion**:
  - 🔴 **Redstone-Leitung (`RedstoneWire`)**: Transportiert Redstone-Signale über Voxel-Blöcke.
  - 🕯️ **Redstone-Fackel (`RedstoneTorch`)**: Ständige Energiequelle & Signal-Inverter.
  - 🎛️ **Hebel / Schalter (`Lever`)**: Interaktives Ein- und Ausschalten von Schaltungen per Rechtsklick.
  - 💡 **Redstone-Lampe (`RedstoneLamp`)**: Leuchtet automatisch bei anliegendem Redstone-Signal auf.

### 🌍 Multi-Biome System & Prozedurale Strukturen (`Biome`)
- Dynamisches Biome-Noise (Ebenen, Wüste mit Kakteen, Waldbiome mit Birken, Verschneite Berggipfel).

### 🌊 Wasser-Physik, Schwimmen & Unterwasser-Nebel
- Flüssigkeits-Physik, Auftrieb, Schwimmen (`Space`) & Unterwasser-Nebelschader.

### ☀️ Dynamischer Tag/Nacht-Zyklus (`TimeManager`)
- 24.000 Ticks Tageszyklus mit flüssiger Himmelsfarb-Interpolation & Tasten `F4`/`T`.

### 🎒 Inventar-System, Item-Stacks & 2x2 Crafting (`E` Taste)
- 36-Slot Spieler-Inventar, Abbau-Item Drops & Crafting Rezept-Manager.

### 💾 Welt-Speichersystem (Save / Load Persistence)
- Binäre Chunk-Serialisierung (`SaveSystem`) in `world_saves/chunk_X_Z.bin`.

### 🏔️ 3D Höhlensysteme, Erze & Unterwelt
- 3D Noise Caves, Lava-Seen & Erzadern (Diamant, Gold, Eisen, Kohle).

### 💡 Smooth Lighting & Ambient Occlusion (AO)
- Weiche Eck- und Kanten-Schatten an angrenzenden Voxel-Blöcken für plastischen Tiefeneffekt.

---

## Steuerung & Tastatur-Layout

| Taste / Eingabe | Aktion |
|-----------------|--------|
| `E` | Inventar- & Crafting-GUI öffnen / schließen |
| `F4` | Tageszeit umschalten (Tag -> Dämmerung -> Nacht -> Morgenrot) |
| `T` | Zeit im Spiel vorspulen (Zeitraffer) |
| `W / A / S / D` | Vorwärts, links, rückwärts, rechts bewegen |
| `Space` | Springen (am Land) / Nach oben schwimmen (im Wasser) / Steigen (im Flugmodus) |
| `L-Shift` | Sinken / Nach unten fliegen (im Flugmodus) |
| `F` | Umschalten zwischen Flugmodus & Voxel-Physik |
| `F3` | Debug-Bildschirm (FPS, Koordinaten, Facing) ein-/ausblenden |
| `1 - 9` | Hotbar Slot wählen (inkl. Redstone-Staub, Fackeln, Hebel & Lampen) |
| `Linksklick` | Block unter Fadenkreuz abbauen / Item im Inventar bewegen |
| `Rechtsklick` | Ausgewählten Block platzieren / Hebel umschalten |
| `Maus bewegen` | Umsehen (Pitch / Yaw) |
| `Escape` | Inventar schließen oder Spiel beenden |

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
