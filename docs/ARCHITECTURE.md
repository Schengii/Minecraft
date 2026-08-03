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
- **`Chunk`**: 16x256x16 3D-Array von Blöcken. Verwaltet Block-Zustände und Lichtwerte (`m_Light`).
- **`ChunkMesh`**: Generiert optimierte Meshes unter Verwendung von **Culled Face Meshing** und dynamischer Gesichtsausleuchtung (Sonnen- & Blocklicht).
- **`LightEngine`**: 3D Zellularer Automat / BFS-Algorithmus zur Berechnung von Sonnenlicht-Säulen und Blocklicht-Ausbreitung (Fackeln, Lava, Glowstone 0–15).
- **`RegionFile` (`.mca`)**: Anvil Region-Dateiformat (32x32 Chunks pro Region) für Hochgeschwindigkeits-Binary-Disk-I/O.
- **`World`**: Koordiniert asynchrones Chunk-Streaming auf `ThreadPool`-Worker-Threads sowie dynamisches Laden/Entladen um den Spieler.

### ECS Subsystem (`src/ecs/`)
- Verwendet **EnTT** als Entity Component System:
  - **Components**: `TransformComponent`, `VelocityComponent`, `BoundingBoxComponent`, `PlayerComponent`, `MeshComponent`.
  - **Systems**: `MovementSystem`, `RenderSystem`, `PhysicsSystem`, `CollisionSystem`.

### Physics Subsystem (`src/physics/`)
- **`AABB`**: Axis-Aligned Bounding Box für Voxel-Präzise Kollisionserkennung zwischen Entitäten (Spieler, Mobs, Items) und der Blockwelt.

### Inventory & Player Subsystem (`src/inventory/`)
- **`PlayerStats`**: Verwaltet Gesundheit (20 HP), Hunger (20 Punkte), Erschöpfung (Exhaustion), Rüstungspunkte und Zeitstempel für passive Regeneration und Verhungern-Schaden.
- **`FoodSystem`**: Nahrungsmittel-Metadaten (Apfel, Brot, Rohes/Gebratenes Schweinefleisch, Goldener Apfel) und Ess-Mechaniken mit automatischer Lebens- & Hungerwiederherstellung.
- **`Inventory`**: 36-Slot Inventar des Spielers mit Hotbar-Management.

---

## 3. Erweiterungs-Roadmap & Modding

- [x] **Multi-Threading Chunk Loading**: Hintergründige Weltgenerierung auf Worker-Threads.
- [x] **Anvil File Format Persistence**: Speichern & Laden der Welt-Chunks in Region-Dateien (`.mca`).
- [x] **Dynamic Light Engine**: 3D BFS Sonnenlicht- & Blocklicht-Ausbreitung (Fackeln, Lava 0–15).
- [ ] **Advanced Rendering**: SSAO, Wasser-Refraktion & Shader-Packs.
- [x] **Redstone Logic & Fluid Dynamics**: Zellularer Automat für Wasser/Lava-Fluss und Signalübertragung.
- [x] **Crafting & GUI System**: Skalierbares HUD, Inventar, Crafting Grid und Chest-Container.
