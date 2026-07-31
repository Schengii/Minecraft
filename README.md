# Minecraft C++ Voxel Engine (1:1 Remake)

Eine hoch-performante, modulare Voxel-Engine in C++20 und OpenGL 4.5 nach dem Vorbild von Minecraft.

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)
![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)

## Feature-Übersicht & Stand

### ☀️ Dynamischer Tag/Nacht-Zyklus (`TimeManager`)
- **24.000 Ticks Tageszyklus**:
  - Dynamische **Sonnen- und Mondstandsberechnung** (360-Grad Lichtquellenrotation im Raum).
  - Continuous **Himmelsfarb-Interpolation**: Morgenrot, Taghimmel (`Sky Blue`), Abendrot & tiefblaue Nacht.
  - **Dynamisches Umgebungslicht (`u_AmbientLight`)**: Helles Tageslicht (`0.5f`) & sanftes Mondlicht (`0.10f`).
  - **`F4` Taste**: Schnell-Umschaltung zwischen Tag, Dämmerung & Nacht.
  - **`T` Taste**: Zeitraffer / Zeitvorspulen.

### 🎒 Inventar-System, Item-Stacks & 2x2 Crafting (`E` Taste)
- **36-Slot Spieler-Inventar (`Inventory`)**: 9 Hotbar-Slots + 27 Hauptinventar-Slots.
- **`E` Taste**: Öffnen und Schließen des Inventar-GUI Screens.
- **Crafting Rezept-Manager (`CraftingManager`)**: Eichenholz -> Holzbretter -> Werkbank & Stöcke.

### 💾 Welt-Speichersystem (Save / Load Persistence)
- **Binäre Chunk-Serialisierung (`SaveSystem`)**: Modifizierte Chunks werden beim Beenden in `world_saves/chunk_X_Z.bin` gespeichert und beim Start geladen.

### 🏔️ 3D Höhlensysteme, Erze & Unterwelt
- **3D Noise Caves**: Unterirdische Tunnelgänge, Grotten und tief liegende Lava-Seen (Y < 10).
- **Voxel-Erzadern**: Diamant-, Gold-, Eisen- und Kohle-Erzadern in tiefen Steinschichten.

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
| `Space` | Springen (im Laufmodus) / Nach oben fliegen (im Flugmodus) |
| `L-Shift` | Nach unten fliegen (im Flugmodus) |
| `F` | Umschalten zwischen Flugmodus & Voxel-Physik |
| `F3` | Debug-Bildschirm (FPS, Koordinaten, Facing) ein-/ausblenden |
| `1 - 9` | Hotbar Slot wählen |
| `Linksklick` | Block unter Fadenkreuz abbauen / Item im Inventar bewegen |
| `Rechtsklick` | Ausgewählten Block platzieren |
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
