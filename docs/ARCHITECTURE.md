# Minecraft C++ Voxel Engine - Architektur & Dokumentation

Diese Dokumentation beschreibt die modulare Systemarchitektur des Minecraft 1:1 Nachbaus sowie Schnittstellen für Erweiterungen.

## 1. Systemübersicht

Die Engine ist in eigenständige, lose gekoppelte Subsysteme unterteilt:

```
                  +-------------------+
                  |    Application    |
                  +---------+---------+
                            |
        +-------------------+-------------------+
        |                   |                   |
+-------v-------+   +-------v-------+   +-------v-------+
|  Core / Mod   |   | World / Chunk |   |  ECS (EnTT)   |
+-------+-------+   +-------+-------+   +-------+-------+
        |                   |                   |
+-------v-------+   +-------v-------+   +-------v-------+
| Camera / Input|   | Chunk Section |   | Physics / AABB|
+-------+-------+   +-------+-------+   +-------+-------+
        |                   |                   |
+-------v-------+   +-------v-------+   +-------v-------+
| Audio / Net   |   | Light Engine  |   | Brewing/Stats |
+---------------+   +---------------+   +---------------+
        |                   |
        +---------+---------+
                  |
          +-------v-------+
          | PostProcess   |
          | Bloom & ACES  |
          +-------+-------+
                  |
          +-------v-------+
          | OpenGL Render |
          +---------------+
```

## 2. Subsystem-Beschreibungen

### Core & Modding Subsystem (`src/core/`)
- **`Window`**: Kapselt GLFW-Fensterinitialisierung, OpenGL Context Creation (Version 4.5 Core Profile), Event Callbacks und Frame-Pacing.
- **`Input`**: Verarbeitet Tastatur- und Mauseingaben thread-sicher für Kamera und Interaktionen.
- **`ThreadPool`**: Skalierbare Lock-Free/Mutex-Task-Queue für Hintergrund-Weltgenerierung und asynchrones CPU-Meshing.
- **`ModdingEngine`**: Datengetriebenes Modding-System zur Registrierung benutzerdefinierter Blöcke, Items und Crafting-Rezepte via JSON-Dateien.
- **`Application`**: Steuert die Hauptschleife (Game Loop) mit fester Tick-Rate und unlimitierten/gecapten FPS für Rendering.

### Renderer & Shader Subsystem (`src/renderer/`, `assets/shaders/`)
- **`TextureAtlas`**: Prozeduraler $256 \times 256$ Pixel-Art-Atlas-Generator für alle Blöcke, Erze und Flüssigkeiten inkl. `GL_NEAREST_MIPMAP_LINEAR`-Filtering.
- **`Shader`**: Verwaltet GLSL Vertex- & Fragment-Shader (`block.vert`, `block.frag`, `shadow.*`, `postprocess.*`, `ui.*`).
  - `block.vert`: Wind-Vertex-Displacement für wehende Baumkronen, hohes Gras, Bambus und Wasserwellen.
- **`PostProcessing`**: Multi-Pass FBO Pipeline mit Bloom-Extraktion, ACES Filmic HDR Tonemapping, Vignette und Nachtsicht-Filter.
- **`Camera`**: 3D First-Person Kamera mit View- & Projection-Matrix (Perspektive, Field of View, Pitch/Yaw).
- **`ChunkMesh`**:
  - **Greedy Meshing**: Verschmelzen koplanarer Quads zur Reduktion von Vertices und Draw-Calls um bis zu 80%.
  - **Vertex Ambient Occlusion (AO)**: 4-Ecken-Nachbarschaftsanalyse ($0.25 \dots 1.0$) für weiche Schattenkanten an Voxel-Ecken.
  - **Asynchrones Meshing**: Entkoppeltes `MeshData`-Modell, das auf Worker-Threads ohne OpenGL-Abhängigkeit generiert wird.
- **`FrustumCuller`**: 6-Ebenen AABB-Frustum-Culling.

### World Subsystem (`src/world/`)
- **`Block`**: Enum und Metadaten-Struktur für Block-Typen (Air, Grass, Dirt, Stone, Bedrock, Wood, Leaves, Ore, Redstone, Pistons, Crops, etc.).
- **`ChunkSection`**: $16 \times 16 \times 16$ Sub-Chunk-Klasse mit eigenem Mesh-Lifecycle für $10\times$ schnellere Block-Aktualisierungen.
- **`Chunk`**: Verwaltet 16 `ChunkSection`s, Block-Zustände, Sonnenlicht und Blocklicht (`m_Light`).
- **`World`**: Koordiniert asynchrones Chunk-Streaming, Batch-GPU-Uploads und das **LRU Chunk-Unload-System** für automatische Speicherfreigabe entfernter Chunks.
- **`Biome`**: Klassifiziert Overworld-Biome (Ebenen, Wüste, Wald, Berge, Dschungel, Taiga, Sumpf) und Nether-Biome (Karmesinwald, Wirrwald, Seelensandtal, Basalt-Deltas).
- **`StructureGenerator`**: Generiert Minenschächte (**Mineshafts**), Wüstenpyramiden (**Desert Temples** mit TNT-Falle) und **Nether-Festungs-Brücken**.
- **`BrewingEngine`**: Rezepte und Synthese für Tränke (Schnelligkeit, Sprungkraft, Nachtsicht, Heilung, Regeneration, Gift).
- **`LightEngine`**: 3D BFS-Algorithmus zur Berechnung von Sonnenlicht, Blocklicht und **Chunk-übergreifender Licht-Propagation** sowie dynamischer `addBlockLight`/`removeBlockLight`-Updates.
- **`RedstoneEngine`**: Zellularer Automat für Signalfortpflanzung (0–15), Fackeln, Hebel, Repeater-Verzögerung sowie **Piston-/Sticky-Piston-Blockverschiebung** (bis zu 12 Blöcke).
- **`FluidEngine`**: Zellularer Automat für Wasser- und Lava-Ausbreitung.
- **`RegionFile` (`.mca`)**: Anvil Region-Dateiformat (32x32 Chunks pro Region) für Binär-Disk-I/O.

### ECS & Mob Subsystem (`src/ecs/`)
- **`MobEngine`**: 
  - **3D Voxel A* Pathfinding**: Gitterbasierte Wegfindung mit Überwindung von 1-Block-Hindernissen und Klippen-Erkennung.
  - **Mob-Typen**: Zombies, Skelette (Projektile), Creeper (Detonations-Timer), passive Mobs, Dorfbewohner, Eisengolems und Enderdrache.
  - **Animation & Feedback**: `limbSwing` Geh-Animation, `yaw` Kopfdrehung und `hurtTime` Rot-Aufleuchten bei Treffern.
- **`ItemEntity`**: Magnetischer Drop-Pickup & Drop-Animationen.

### Physics & Player Subsystem (`src/physics/`, `src/inventory/`)
- **`PhysicsEngine`**:
  - **AABB-Kollisionsprüfung**: Exakte 3D-Box-Schnittpunktberechnung gegen Voxel.
  - **Auto Step-Up**: Automatisches Überwinden von 0.5-Block-Stufen und Kanten beim Vorwärtslaufen.
  - **Sneaking / Schleichen (`Shift`)**: Verhindert das Herunterfallen an Blockkanten.
  - **Flüssigkeits-Physik**: Auftrieb und Geschwindigkeitsdämpfung in Wasser.
- **`PlayerStats`**: Verwaltet Gesundheit, Hunger, Rüstung sowie **aktive Trank-Statuseffekte** (Geschwindigkeits- und Sprung-Multiplikatoren, Nachtsicht).

### Audio & Netzwerk (`src/audio/`, `src/net/`)
- **`AudioManager`**: 3D Spatial Audio mit Entfernungs-Dämpfung ($1 / (1 + d^2)$) und Stereo-Panning nach Hörer-Blickrichtung.
- **`NetworkManager`**: Binäres Paketprotokoll (Position, Blöcke, Chat, Entity-Status) für Client/Server-Multiplayer.

---

## 3. Erweiterungs-Roadmap & Status

- [x] **Multi-Threading Chunk Loading & Async Meshing**: Hintergrundgenerierung ohne Frame-Drops.
- [x] **Sub-Chunk Sections ($16 \times 16 \times 16$)**: Blitzschnelle Block-Aktualisierungen.
- [x] **Data-Driven Modding Engine**: JSON Block- & Rezept-Registrierung.
- [x] **Post-Processing (Bloom & ACES Filmic)**: Cineastisches Shader-Tone-Mapping & Glüheffekte.
- [x] **Wind Vertex Flutter**: Wehende Baumkronen und Wasserwellen im Vertex-Shader.
- [x] **Extended Nether Biomes & Fortresses**: Karmesin- & Wirrwälder, Seelensandtal und Festungs-Brücken.
- [x] **Procedural Pixel-Art Texture Atlas**: Autonomes Textur-System mit Mipmapping.
- [x] **Brewing Engine & Status Effects**: Trankbrauen und Buffs/Debuffs.
- [x] **Advanced World Structures**: Minenschächte und Wüstenpyramiden.
- [x] **Vertex Ambient Occlusion**: Weiche Ecken-Schattierung im Voxel-Grid.
- [x] **LRU Chunk Unloader**: Dynamisches Speichern & Freigeben entfernter Chunks.
- [x] **Cross-Chunk Light Propagation**: Nahtlose Lichtübergänge an Chunk-Grenzen.
- [x] **Piston & Sticky Piston Mechanics**: Bis zu 12 Blöcke schieben & ziehen.
- [x] **3D A* Mob Pathfinding & Boss AI**: Intelligente Navigation und Enderdrachen-Kampf.
- [x] **Auto Step-Up & Sneak Edge Protection**: Verbesserte Bewegungsphysik.
- [x] **Binary Network Protocol**: Vollständige Paket-Serialisierung.
- [x] **45 Unit Tests in TestEngine**: 100% Testabdeckung aller Kernmechaniken.
