# Minecraft C++ OpenGL Voxel Engine (1:1 Remake & Modern Voxel Engine)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)](https://cmake.org/)
[![Build & Test](https://github.com/Schengii/Minecraft/actions/workflows/build.yml/badge.svg)](.github/workflows/build.yml)

A high-performance, modular 3D voxel game engine written in **C++20** and **OpenGL 4.5**, modeled after Minecraft. Features high-throughput multithreaded chunk generation, greedy meshing, cellular automaton redstone logic, fluid dynamics, Anvil region saving, entity AI, dynamic lighting, and spatial audio.

---

## 🌟 Feature Overview & State

### 🔴 Redstone Engine (`RedstoneEngine`)
- **Signal Propagation & Logic Gates**:
  - 🔴 **Redstone Wire (`RedstoneWire`)**: Signal transport across blocks with signal decay (0–15 strength).
  - 🕯️ **Redstone Torch (`RedstoneTorch`)**: Infinite power source & signal inverter.
  - 🎛️ **Lever (`Lever`)**: Interactive state toggling on right-click.
  - 💡 **Redstone Lamp (`RedstoneLamp`)**: Automatic visual light emission upon active redstone power.

### 🌌 Multi-Dimension System (`DimensionManager`)
- 🌍 **Overworld**: Multi-biome world with dynamic weather, cave networks, vegetation, and day-night cycle.
- 🔥 **The Nether (`DimensionType::Nether`)**: Bedrock ceilings/floors, vast lava oceans, Soul Sand, Netherrack, Glowstone stalactites and working Nether Portals.
- 🐉 **The End (`DimensionType::TheEnd`)**: Floating End Stone islands, towering Obsidian Pillars with glowing crystals, central exit End Portal, custom void skybox, and **Ender Dragon Boss AI** (200 HP, 3D aerial flight, swooping dive attacks, heavy knockback).

### 🌾 Agricultural Crops & Farming Engine (`CropsEngine`)
- Cultivation of **Wheat**, **Carrots**, and **Potatoes** (`WheatCrop`, `CarrotCrop`, `PotatoCrop`).
- **Growth Stages (0–7)**: Realistic growth progression accelerated by adjacent water soil irrigation and bone meal fertilization.
- Drop yield drops on harvesting fully mature crops.

### 🚂 Rail Network & Vehicle Physics (`PhysicsEngine` & `CraftingManager`)
- **Rails & Powered Rails**: Craftable 16x rail tracks and velocity-boosted powered rails.
- **Vehicles (`Minecart` & `Boat`)**:
  - Minecarts with smooth rail track snapping, momentum preservation, and powered rail acceleration.
  - Wooden Boats with water buoyancy, drag, and gliding mechanics.

### 🛡️ Armor Suite & Extended 3x3 Crafting (`CraftingManager` & `PlayerStats`)
- Full armor sets (Helmets, Chestplates, Leggings, Boots) crafted with Iron and Diamond in 3x3 Crafting Tables.
- Dynamic Damage Reduction (up to 80%) based on total equipped armor points and item durability wear.

### 🏔️ Greedy Meshing & Frustum Culling (`ChunkMesh` & `FrustumCuller`)
- Quad-merging quad face optimization reducing vertex memory footprint and draw calls by up to **80%**.
- **Camera Frustum Culling**: 6-plane AABB chunk visibility testing eliminating off-screen chunk render overhead.
- **Two-Pass Mesh Rendering**: Separate transparent render passes for water, bamboo, and glass blocks eliminating depth-sorting artifacts.

### ☀️ Dynamic Time & Sky System (`Skybox` & `TimeManager`)
- 24,000-tick daily cycle with dynamic sky color interpolation, directional sun/moon movement, and custom skybox.
- Key shortcuts `F4` (cycle time) and `T` (timelapse fast-forward).

### 🌊 Water Physics & Swimming (`FluidEngine`)
- Cellular automaton liquid spreading for water and lava. Buoyancy mechanics and underwater fog shader.

### 🎒 GUI, Main Menu & Pause System (`MenuGUI` & `HUD`)
- **Main Menu**: Start new worlds with custom Seed input, adjust graphics settings, or quit.
- **Pause Menu (`Esc`)**: In-game pause menu with Resume, Settings, and Save & Exit.
- **Settings GUI**: Live adjustment of FOV (60°–110°), Render Distance (4–16 Chunks), and VSync.
- **Inventory & Crafting**: 36-slot inventory, hotbar, and 2x2 / 3x3 Crafting Table integration.

### 💾 Region Save File System (`SaveSystem` & `RegionFile`)
- Binary chunk serialization (`.mca` Anvil format) with high-speed disk I/O.

### 🧟 Entity AI & 3D Mob Animations (`MobEngine`)
- Entity pathfinding for Zombies, Skeletons (arrow projectiles), Creepers (fuse countdown & explosion), and passive Mobs.
- 3D limb walking animations and hit feedback.

### 💥 Particle Debris Engine (`ParticleEngine`)
- 3D block fragment particles spawned on block destruction with velocity and gravity physics.

---

## ⌨️ Controls & Keybindings

| Key / Input | Action |
|-------------|--------|
| `Escape` | Open Pause Menu / Close UI / Back |
| `E` | Open / Close Inventory & Crafting GUI |
| `W / A / S / D` | Movement (Forward, Left, Backward, Right) |
| `Space` | Jump / Swim upward in water / Ascend in Fly Mode |
| `L-Shift` | Descend / Sink in Fly Mode |
| `F` | Toggle Fly Mode vs Physics Mode |
| `F3` | Toggle Debug HUD (FPS, Position, Facing, Biome) |
| `F4` | Cycle Time of Day (Day -> Sunset -> Night -> Sunrise) |
| `1 - 9` | Select Hotbar Item |
| `Left Click` | Break block under crosshair / Attack Mob |
| `Right Click` | Place block / Interact (Lever, Chest, Furnace) |
| `Mouse Move` | Look around (Pitch / Yaw) |

---

## 🏗️ Build & Compilation

### Requirements
- **Compiler**: C++20 compatible (MSVC 2019+, GCC 10+, Clang 11+)
- **CMake**: Version >= 3.20
- **Libraries** (automatically fetched via CMake): GLFW 3.4, GLM 1.0.1, EnTT 3.13.2, OpenGL 4.5 Core

### Build Instructions (PowerShell / Command Prompt / Terminal)

```bash
# 1. Clone repository
git clone https://github.com/Schengii/Minecraft.git
cd Minecraft

# 2. Configure project
cmake -B build -S .

# 3. Build executable
cmake --build build --config Release

# 4. Run main game
./build/Release/Minecraft

# 5. Run automated unit tests
./build/Release/TestEngine
```

---

## 📐 Architecture & Modules

Full system architecture and module documentation is available in [`docs/ARCHITECTURE.md`](file:///c:/Users/sche-/Desktop/Programmieren%20Projekte/Minecraft/docs/ARCHITECTURE.md).

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

---

## 📜 License

This project is open-source software licensed under the [MIT License](LICENSE).
