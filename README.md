# Minecraft C++ OpenGL Voxel Engine (1:1 Remake & Modern Voxel Engine)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)](https://cmake.org/)
[![Build & Test](https://github.com/Schengii/Minecraft/actions/workflows/build.yml/badge.svg)](.github/workflows/build.yml)

A high-performance, modular 3D voxel game engine written in **C++20** and **OpenGL 4.5**, modeled after Minecraft. Features high-throughput multithreaded chunk generation, $16 \times 16 \times 16$ sub-chunk section slicing, asynchronous CPU meshing, procedural pixel-art texture atlas generation, vertex ambient occlusion (smooth lighting), ACES filmic tone mapping & bloom post-processing, wind vertex displacement, data-driven modding engine, brewing stand & status effect systems, cellular automaton redstone & piston mechanics, 3D A* mob pathfinding, fluid dynamics, Anvil region saving, spatial audio, and binary networking protocol.

---

## 🌟 Feature Overview & State

### 🎨 Visual & Shader Pipeline (`ChunkMesh`, `TextureAtlas` & Shaders)
- **Pixel-Art Texture Atlas**: Integrated $256 \times 256$ texture atlas generator with authentic pixel art patterns for all voxel blocks, ores, flora, and liquids with `GL_NEAREST_MIPMAP_LINEAR` filtering.
- **Smooth Lighting / Vertex AO**: Dynamic per-vertex ambient occlusion calculation ($0.25 \dots 1.0$) based on 3-voxel corner neighborhood checks ($S_1, S_2, C$).
- **Waving Foliage & Wind Flutter**: Real-time vertex displacement in `block.vert` simulating wind sway for tree canopies, tall grass, crops, and water ripples.
- **Post-Processing (Bloom & ACES Filmic)**: Multi-pass FBO pipeline in `postprocess.frag` with Bloom extraction, ACES HDR tonemapping, Vignette, and Night Vision luminance amplification.
- **Asynchronous CPU Meshing**: Worker threads generate vertex/index data without OpenGL context locks; main thread performs bounded batch GPU buffer uploads.

### 🧩 Sub-Chunk Vertical Slicing (`ChunkSection`)
- **$16 \times 16 \times 16$ Chunk Sections**: Chunks are partitioned into 16 vertical sections. Block edits only trigger re-meshing for the affected sub-chunk, reducing modification meshing CPU overhead by **93%**.

### 🔌 Data-Driven Modding & Addon Registry (`ModdingEngine`)
- **Custom Blocks & Recipes**: Dynamic runtime registration of custom block definitions (hardness, light emission, solid/opacity flags) and custom crafting recipes via JSON manifests.

### ⚗️ Brewing Stand Engine & Status Effects (`BrewingEngine` & `PlayerStats`)
- **Brewing Stand Recipes**: Synthesis of Awkward Potions, Swiftness (Speed I), Leaping (Jump Boost I), Night Vision, Healing (Instant Health), Regeneration, and Poison.
- **Dynamic Status Modifiers**: Live acceleration of player movement velocity, jump height elevation, continuous health regeneration, and poison damage tick timers.

### 🏛️ Advanced World Structures & Nether Biomes (`Biome` & `StructureGenerator`)
- **Nether Biomes**: Crimson Forest, Warped Forest, Soul Sand Valley, and Basalt Deltas.
- **Nether Fortress Corridors**: Obsidian bridge corridors with Netherrack railings and overhead Glowstone beacons.
- **Abandoned Mineshafts**: 3D underground tunnels with oak support arches, rail track networks, cobblestone floors, and redstone wall torches.
- **Desert Pyramid Temples**: 7x7 sandstone pyramids featuring hidden underground basement shafts with 4 corner loot chests and a central TNT floor trap.

### 💾 LRU Chunk Unloader & Memory Management (`World`)
- **Dynamic Distance-Based Unloader**: Chunks further than `RenderDistance + 2` chunks away from the player are automatically serialized to disk (`.mca` Anvil format) and memory is freed.

### 💡 Cross-Chunk Light Propagation (`LightEngine`)
- **Chunk-Boundary Light Flood**: BFS light propagation extends across chunk borders ($X=0, 15$ and $Z=0, 15$) eliminating harsh light seams.
- **Dynamic Light Ingestion**: Real-time `addBlockLight` and `removeBlockLight` BFS updates when placing torches, lamps, and lava.

### 🔴 Redstone & Piston Engine (`RedstoneEngine`)
- **Signal Propagation & Logic Gates**:
  - 🔴 **Redstone Wire (`RedstoneWire`)**: Signal transport across blocks with signal decay (0–15 strength).
  - 🕯️ **Redstone Torch (`RedstoneTorch`)**: Infinite power source & signal inverter.
  - 🎛️ **Lever (`Lever`)**: Interactive state toggling on right-click.
  - 💡 **Redstone Lamp (`RedstoneLamp`)**: Automatic visual light emission upon active redstone power.
  - ⏱️ **Redstone Repeater**: Signal delay and full power boost to 15.
- **Pistons & Sticky Pistons**:
  - Mechanical shifting of up to 12 connected blocks in push direction.
  - Sticky Piston retraction mechanism pulling adjacent blocks.

### 👾 3D Voxel A* Mob Pathfinding & AI (`MobEngine`)
- **A* Pathfinding**: 3D voxel grid navigation enabling mobs (Zombies, Skeletons, Creepers) to pathfind around walls, ascend 1-block steps, and navigate complex terrain.
- **Animation & Hit Feedback**: Limb swinging animations (`limbSwing`), head rotation (`yaw`), and visual hit-flash feedback (`hurtTime`).
- **Boss Mechanics**: Ender Dragon 3D aerial flight AI with 200 HP and heavy knockback swoops.

### 🎛️ Advanced Player Physics & Movement (`PhysicsEngine`)
- **Auto Step-Up**: Smooth elevation over 0.5-block steps and ledges without manual jumping.
- **Sneak Ledge Prevention (`Shift`)**: Clamps horizontal velocity at block edges to prevent accidental falling.
- **Water Physics**: Buoyancy, swimming drag, and underwater fog shaders.

### 🔊 3D Spatial Audio Engine (`AudioManager`)
- **Spatial Attenuation & Panning**: Inverse-square distance gain calculations and stereo panning based on listener facing vectors.
- Master volume controls and multi-channel sound event triggering.

### 🌐 Binary Network Packet Protocol (`NetworkManager`)
- **Binary Serialization**: High-speed packet encoders and decoders for `PlayerPosPacket`, `BlockChangePacket`, `ChatMessagePacket`, and `EntityState`.

---

## ⌨️ Controls & Keybindings

| Key / Input | Action |
|-------------|--------|
| `Escape` | Open Pause Menu / Close UI / Back |
| `E` | Open / Close Inventory & Crafting GUI |
| `W / A / S / D` | Movement (Forward, Left, Backward, Right) |
| `Space` | Jump / Swim upward in water / Ascend in Fly Mode |
| `L-Shift` | Sneak (prevents falling off edges) / Descend in Fly Mode |
| `F` | Toggle Fly Mode vs Physics Mode |
| `F3` | Toggle Debug HUD (FPS, Position, Facing, Biome) |
| `F4` | Cycle Time of Day (Day -> Sunset -> Night -> Sunrise) |
| `1 - 9` | Select Hotbar Item |
| `Left Click` | Break block under crosshair / Attack Mob |
| `Right Click` | Place block / Interact (Lever, Chest, Furnace, Brewing Stand) |
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

# 5. Run automated unit tests (45 comprehensive test suites)
./build/Release/TestEngine
```

---

## 📐 Architecture & Modules

Full system architecture and module documentation is available in [`docs/ARCHITECTURE.md`](file:///c:/Users/sche-/Desktop/Programmieren%20Projekte/Minecraft/docs/ARCHITECTURE.md).

---

## 📜 License

This project is open-source software licensed under the [MIT License](LICENSE).
