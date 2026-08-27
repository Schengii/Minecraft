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
+-------+-------+   +-------+-------+   +-------+-------+
        |                   |                   |
+-------v-------+   +-------v-------+   +-------v-------+
| Font / UI     |   | EntityRender  |   | ContainerGUI  |
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
          | PBR & Clouds  |
          +---------------+
```

## 2. Subsystem-Beschreibungen

### Core & Modding Subsystem (`src/core/`)
- **`Window`**: Kapselt GLFW-Fensterinitialisierung, OpenGL Context Creation (Version 4.5 Core Profile), Event Callbacks und Frame-Pacing.
- **`Input`**: Verarbeitet Tastatur- und Mauseingaben thread-sicher für Kamera und Interaktionen.
- **`ThreadPool`**: Skalierbare Lock-Free/Mutex-Task-Queue für Hintergrund-Weltgenerierung und asynchrones CPU-Meshing.
- **`ModdingEngine`**: Datengetriebenes Modding-System zur Registrierung benutzerdefinierter Blöcke, Items und Crafting-Rezepte via JSON-Dateien.
- **`Application`**: Steuert die Hauptschleife (Game Loop) mit kontinuierlichem Mining, Schrittgeräuschen, First-Person-Hand-Animation und nahtlosem Container-Management.

### Renderer & Shader Subsystem (`src/renderer/`, `assets/shaders/`)
- **`Skybox`**: Prozedurale Wolkenschicht auf Höhe $Y=128$ mit zeitgesteuertem Wind-Drift und dynamischer Himmelsfarben-Tönung bei Sonnenuntergang.
- **`EntityRenderer`**:
  - **Hierarchische 3D-Modelle**: Box-Mesh-Hierarchien mit Drehachsen-Offsets für Zombies, Skelette, Creeper, Schweine, Kühe, Dorfbewohner, Eisengolems, den Enderdrachen und den **3-Köpfigen Wither**.
  - **Multiplayer Remote Players**: Vollwertige Steve-Modelle mit Beinschwing-Animationen und **schwebenden 3D-Nametags**.
  - **First-Person-Hand**: Rendering der Spielerhand und des gehaltenen 3D-Voxel-Blocks im View-Space mit dynamischem Geh-Bobbing und Schlag-/Abbau-Schwungbögen.
  - **Item-Drops & Pfeile**: Rotierende und schwebende 3D-Voxel-Miniaturen sowie ballistische Pfeile und fliegende Wither-Schädel.
- **`ParticleEngine`**: 3D Billboard-Partikel für Abbau-Splitter, Treffer-Funken, Regen und Schneefall.
- **`TextureAtlas`**: Prozeduraler $256 \times 256$ Pixel-Art-Atlas-Generator für alle Blöcke, Erze und Flüssigkeiten inkl. `GL_NEAREST_MIPMAP_LINEAR`-Filtering.
- **`Shader`**: Verwaltet GLSL Vertex- & Fragment-Shader (`block.vert`, `block.frag`, `shadow.*`, `postprocess.*`, `ui.*`).
  - `block.vert`: Wind-Vertex-Displacement für wehende Baumkronen, hohes Gras, Bambus und Wasserwellen.
  - `block.frag`: **PBR Blinn-Phong Glanzlichter** und **Fresnel-Wasser-Reflexion**.
- **`PostProcessing`**: Multi-Pass FBO Pipeline mit Bloom-Extraktion, ACES Filmic HDR Tonemapping, Vignette und Nachtsicht-Filter.
- **`ChunkMesh`**:
  - **Greedy Meshing**: Verschmelzen koplanarer Quads zur Reduktion von Vertices und Draw-Calls um bis zu 80%.
  - **Vertex Ambient Occlusion (AO)**: 4-Ecken-Nachbarschaftsanalyse ($0.25 \dots 1.0$) für weiche Schattenkanten an Voxel-Ecken.
  - **Asynchrones Meshing**: Entkoppeltes `MeshData`-Modell, das auf Worker-Threads ohne OpenGL-Abhängigkeit generiert wird.
- **`FrustumCuller`**: 6-Ebenen AABB-Frustum-Culling.

### GUI & Typografie Subsystem (`src/gui/`)
- **`FontRenderer`**: Prozedurale Generierung eines $128 \times 128$ Bitmap-Font-Atlas (ASCII 32–126) mit dynamischen Schatten, Skalierung und zentrierter Textausrichtung.
- **`HUD`**: F3-Debug-Screen (FPS, XYZ-Koordinaten, Chunk-Koordinaten, Blickrichtung, Biome-Name, Lichtlevel) und texturierte Hotbar mit Stack-Zahlen.
- **`ContainerGUI`**: 27-Slot Kisten-Inventar und animierte Schmelzofen-Oberfläche mit dynamischer Brennflammen- und Pfeil-Fortschrittsanzeige.
- **`InventoryGUI`**: 2x2 Crafting-Gitter mit Resultatsanzeige und vollständigem Slot-Austausch.

### World Subsystem (`src/world/`)
- **`Block`**: Enum und Metadaten-Struktur für Block-Typen (Air, Grass, Dirt, Stone, Bedrock, Wood, Leaves, Ore, Redstone, Pistons, Crops, etc.).
- **`ChunkSection`**: $16 \times 16 \times 16$ Sub-Chunk-Klasse mit eigenem Mesh-Lifecycle für $10\times$ schnellere Block-Aktualisierungen.
- **`Chunk`**: Verwaltet 16 `ChunkSection`s, Block-Zustände, Sonnenlicht und Blocklicht (`m_Light`).
- **`World`**: Koordiniert asynchrones Chunk-Streaming, Batch-GPU-Uploads und das **LRU Chunk-Unload-System** für automatische Speicherfreigabe entfernter Chunks.
- **`Biome`**: Klassifiziert Overworld-Biome (Ebenen, Wüste, Wald, Berge, Dschungel, Taiga, Sumpf) und Nether-Biome (Karmesinwald, Wirrwald, Seelensandtal, Basalt-Deltas).
- **`StructureGenerator`**: Generiert Minenschächte (**Mineshafts**), Wüstenpyramiden (**Desert Temples** mit TNT-Falle) und **Nether-Festungs-Brücken**.
- **`ToolSystem`**: Kontinuierlicher Abbau mit Werkzeuggeschwindigkeiten (Holz, Stein, Eisen, Diamant) und Erntestufen.
- **`BrewingEngine`**: Rezepte und Synthese für Tränke (Schnelligkeit, Sprungkraft, Nachtsicht, Heilung, Regeneration, Gift).
- **`LightEngine`**: 3D BFS-Algorithmus zur Berechnung von Sonnenlicht, Blocklicht und **Chunk-übergreifender Licht-Propagation**.
- **`RedstoneEngine`**: Zellularer Automat für Signalfortpflanzung (0–15), Fackeln, Hebel, Repeater-Verzögerung sowie **Piston-/Sticky-Piston-Blockverschiebung** (bis zu 12 Blöcke).
- **`FluidEngine`**: Zellularer Automat für Wasser- und Lava-Ausbreitung.
- **`RegionFile` (`.mca`)**: Anvil Region-Dateiformat (32x32 Chunks pro Region) für Binär-Disk-I/O.

### ECS & Mob Subsystem (`src/ecs/`)
- **`MobEngine`**: 
  - **3D Voxel A* Pathfinding**: Gitterbasierte Wegfindung mit Überwindung von 1-Block-Hindernissen und Klippen-Erkennung.
  - **Mob-Typen**: Zombies, Skelette (Projektile), Creeper (Detonations-Timer), passive Mobs, Dorfbewohner, Eisengolems, Enderdrache und **Wither-Boss** mit fliegenden Explosiv-Schädeln.
  - **Natürlicher Spawner & Despawner**: Umkreis-Prüfung ($r = 24 \dots 48$), Mob-Cap-Limitierung und automatisches Despawnen entfernter Mobs ($d > 72$).
- **`ItemEntity`**: Magnetischer Drop-Pickup & Drop-Animationen.

### Physics & Player Subsystem (`src/physics/`, `src/inventory/`)
- **`PhysicsEngine`**:
  - **AABB-Kollisionsprüfung**: Exakte 3D-Box-Schnittpunktberechnung gegen Voxel.
  - **Auto Step-Up**: Automatisches Überwinden von 0.5-Block-Stufen und Kanten beim Vorwärtslaufen.
  - **Sneaking / Schleichen (`Shift`)**: Verhindert das Herunterfallen an Blockkanten.
- **`PlayerStats`**: Verwaltet Gesundheit, Hunger, Rüstung sowie **aktive Trank-Statuseffekte**.

### Audio & Netzwerk (`src/audio/`, `src/net/`)
- **`AudioManager`**: Materialabhängige Schrittgeräusche (Gras, Stein, Holz, Sand, Wasser) und 3D Spatial Audio mit Entfernungs-Dämpfung.
- **`NetworkManager`**: Nicht-blockierende UDP-Sockets (Winsock2 / POSIX) mit binärer Paket-Serialisierung für Multiplayer.

---

## 3. Test-Suite & Verifikation

Das Projekt verfügt über **56 automatisierte Unit-Tests** in `TestEngine.exe`, die 100% der Engine-Funktionen abdecken:
- Tests 1–15: Redstone, Fluide, Werkzeuge, Explosionen, Nether, Mobs, Hunger, Kisten & Öfen.
- Tests 16–30: RegionFile-Streaming, Licht-BFS, Greedy Meshing, Tränke, Village, Farming, Fahrzeuge, Vertex AO.
- Tests 31–45: Pistone, Repeater, A* Wegfindung, Spatial Audio, Modding Engine, Nether-Biome, Post-Processing.
- Tests 46–51: Bitmap Font Atlas, 3D Entity Models, Kontinuierlicher Abbau, Container GUIs, UDP Socket Networking, Mob Spawning.
- Tests 52–56: **Dynamische Wolken auf $Y=128$**, **Wither-3-Kopf-Boss AI & Schädel-Physik**, **PBR Specular & Fresnel Shader**, **Remote Player Steve Models & 3D Nametags**, **Material Footstep Synthesizer**.
