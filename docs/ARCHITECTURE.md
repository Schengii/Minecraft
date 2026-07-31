# Minecraft C++ Voxel Engine - Architektur & Dokumentation

Diese Dokumentation beschreibt die modulare Systemarchitektur des Minecraft 1:1 Nachbaus sowie Schnittstellen für zukünftige Erweiterungen.

## 1. Systemübersicht

Die Engine ist in eigenständige Subsysteme unterteilt:

```
                  +-------------------+
                  |    Application    |
                  +---------+---------+
                            |
        +-------------------+-------------------+
        |                   |                   |
+-------v-------+   +-------v-------+   +-------v-------+
|  Core (GLFW)  |   | World / Chunk |   |  ECS (EnTT)   |
+-------+-------+   +-------+-------+   +-------+-------+
        |                   |                   |
+-------v-------+   +-------v-------+   +-------v-------+
| Camera / Input|   | Chunk Mesher  |   | Physics / AABB|
+-------+-------+   +-------+-------+   +-------+-------+
        |                   |
        +---------+---------+
                  |
          +-------v-------+
          | OpenGL Render |
          +---------------+
```

## 2. Subsystem-Beschreibungen

### Core Subsystem (`src/core/`)
- **`Window`**: Kapselt GLFW-Fensterinitialisierung, OpenGL Context Creation (Version 4.5 Core Profile), Event Callbacks und Frame-Pacing.
- **`Input`**: Verarbeitet Tastatur- und Mauseingaben thread-sicher für Kamera und Interaktionen.
- **`Application`**: Steuert die Hauptschleife (Game Loop) mit fester Tick-Rate (20 TPS für Logik/Weltupdates) und unlimitierten/gecapten FPS für Rendering.

### Renderer Subsystem (`src/renderer/`)
- **`Shader`**: Verwaltet GLSL Vertex- & Fragment-Shader, inkl. Dynamic Compilation & Uniform-Caching.
- **`Camera`**: 3D First-Person Kamera mit View- & Projection-Matrix (Perspektive, Field of View, Pitch/Yaw).
- **`Mesh`**: Abstraktion von OpenGL Vertex Array Objects (VAO), Vertex Buffer Objects (VBO) und Element Buffer Objects (EBO).
- **`Texture`**: Lädt und bindet 2D-Texturen & Textur-Atlanten via `stb_image`.

### World Subsystem (`src/world/`)
- **`Block`**: Enum und Metadaten-Struktur für Block-Typen (Air, Grass, Dirt, Stone, Bedrock, Wood, Leaves, etc.).
- **`Chunk`**: 16x256x16 3D-Array von Blöcken. Verwaltet Block-Zustände und Lichtwerte.
- **`ChunkMesh`**: Generiert optimierte Meshes unter Verwendung von **Culled Face Meshing** (Nachbarblock-Prüfung zur Entfernung unsichtbarer Flächen) zur Maximierung der Bildrate.
- **`World`**: Koordiniert Chunk-Laden, Prozedurale Weltgenerierung (Perlin/Simplex Noise) und Block-Interaktionen (Setzen/Zerstören).

### ECS Subsystem (`src/ecs/`)
- Verwendet **EnTT** als Entity Component System:
  - **Components**: `TransformComponent`, `VelocityComponent`, `BoundingBoxComponent`, `PlayerComponent`, `MeshComponent`.
  - **Systems**: `MovementSystem`, `RenderSystem`, `PhysicsSystem`, `CollisionSystem`.

### Physics Subsystem (`src/physics/`)
- **`AABB`**: Axis-Aligned Bounding Box für Voxel-Präzise Kollisionserkennung zwischen Entitäten (Spieler, Mobs, Items) und der Blockwelt.

---

## 3. Erweiterungs-Roadmap & Modding

- [ ] **Multi-Threading Chunk Loading**: Hintergründige Weltgenerierung auf Worker-Threads.
- [ ] **Anvil File Format Persistence**: Speichern & Laden der Welt-Chunks in Region-Dateien (`.mca`).
- [ ] **Advanced Rendering**: Shadow Mapping, Ambient Occlusion (SSAO / Vertex AO), Wasser-Refraktion & Shader-Packs.
- [ ] **Redstone Logic & Fluid Dynamics**: Zellularer Automat für Wasser/Lava-Fluss und Signalübertragung.
- [ ] **Crafting & GUI System**: Skalierbares HUD, Inventar, Crafting Grid und Chest-Container.
