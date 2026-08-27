# Minecraft C++ OpenGL Voxel Engine (1:1 Remake & Modern Voxel Engine)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)](https://cmake.org/)
[![Build & Test](https://github.com/Schengii/Minecraft/actions/workflows/build.yml/badge.svg)](.github/workflows/build.yml)

A high-performance, modular 3D voxel game engine written in **C++20** and **OpenGL 4.5**, modeled after Minecraft. Features high-throughput multithreaded chunk generation, $16 \times 16 \times 16$ sub-chunk section slicing, asynchronous CPU meshing, procedural pixel-art texture atlas generation, 3D hierarchical mob models, 3-headed Wither boss mechanics, multiplayer remote player models with nametags, procedural skybox clouds at $Y=128$, PBR specular & Fresnel water shaders, first-person hand animations, bitmap typography rendering, interactive container GUIs (chests and animated furnaces), continuous mining with tool speed multipliers, non-blocking UDP socket multiplayer networking, vertex ambient occlusion (smooth lighting), ACES filmic tone mapping & bloom post-processing, data-driven modding engine, brewing stand & status effect systems, cellular automaton redstone & piston mechanics, 3D A* mob pathfinding, fluid dynamics, Anvil region saving, and material-based spatial audio.

---

## 🌟 Feature Overview & State

### ☁️ Procedural Dynamic Clouds (`Skybox`)
- **Atmospheric Clouds at $Y=128$**: Infinite drifting 2.5D/3D procedural cloud plane scrolling with continuous wind vectors ($v_x = 1.5, v_z = 0.8$).
- **Time-of-Day Color Grading**: Automatic sunset gold/orange tinting ($t \in [12000, 14000]$) and night dark indigo lighting ($t \in [14000, 22000]$) blended into the cloud layer.

---

### ✨ PBR Specular & Fresnel Highlights (`block.frag`)
- **Blinn-Phong Specular Reflections**: Real-time sun glint based on normal vectors, light direction, and view camera angle.
- **Fresnel Water Translucency**: Angle-dependent surface reflectance ($F_0 + (1-F_0)(1-\cos\theta)^4$) for realistic ocean, river, and ice rendering.

---

### ☠️ Wither 3-Headed Boss Engine (`MobEngine` & `EntityRenderer`)
- **3-Headed Geometric Box Model**: Dark floating ribcage body with hovering sine motion and independent left/right side-head yaw rotations.
- **Boss AI & Explosive Skulls**: 300 HP hovering combat AI firing ballistic Wither Skulls every 2.2 seconds that trigger spherical block destruction on impact.
- **Nether Star Drops**: High-value boss drop upon defeat.

---

### 👥 Multiplayer Remote Players & 3D Nametags (`NetworkManager` & `EntityRenderer`)
- **Remote Player 3D Models**: Renders complete Steve character models (cyan shirt, blue pants, skin tones) with synchronized limb swing animations.
- **3D Floating Nametags**: Renders player names/IDs above head level facing the camera.
- **UDP Socket Pipeline**: Non-blocking UDP packet transmission with binary serialization for coordinates, yaw/pitch, and block edits.

---

### 👾 3D Entity & Mob Renderer (`EntityRenderer`)
- **Hierarchical Mob Models**: 3D geometric box-model rendering with real-time limb-swing animations, head rotation tracking, and hurt-flash red tinting for:
  - 🧟 **Zombie**: Forward-raised arms, dynamic limb swinging, and damage feedback.
  - 🏹 **Skeleton**: Ranged humanoid model shooting 3D ballistic arrow projectiles.
  - 💥 **Creeper**: 4-legged quadruped model with pulsing white swelling animation during fuse countdown.
  - 🐷 **Pig & Cow**: Horizontal quadruped body with walking leg cycles and drops.
  - 👨‍🌾 **Villager**: Humanoid model with trading interaction support.
  - 🦾 **Iron Golem**: Heavy village defender model with high-damage knockback.
  - 🐉 **Ender Dragon**: Multi-part boss model with flapping wings ($\sin(\text{time} \times 8)$), tail segments, and 3D aerial pathfinding.
- **First-Person Hand & Held Item Model**: Dynamic view-space arm and held block/tool rendering featuring walk bobbing and attack/mining swing arcs.
- **Item Drops & Arrow Projectiles**: 3D spinning and bobbing voxel miniature block entities with magnetic player attraction, plus velocity-oriented ballistic arrows.

---

### 🔤 Procedural Bitmap Typography Engine (`FontRenderer`)
- **Procedural 8x8 ASCII Font Atlas**: Generates a $128 \times 128$ texture atlas from binary pixel matrices covering all standard printable ASCII characters (32–126).
- **Text Formatting & Shadows**: Dynamic string batching with drop shadows, color styling, and scale adjustments for HUD, F3 debug screen, inventory counts, and menus.

---

### 📦 Interactive Containers & Crafting (`ContainerGUI` & `InventoryGUI`)
- **27-Slot Chest GUI**: Interactive chest interface supporting full stack transfers, slot click dragging, and audio feedback.
- **Animated Smelting Furnace**: Interactive furnace screen with dynamic burning fire flame gauge and green smelting progress arrow.
- **2x2 & 3x3 Crafting Grids**: Real-time recipe matching for tools, weapons, armors, and utility blocks.

---

### ⛏️ Continuous Mining & Tool System (`ToolSystem`)
- **Block Hardness & Tool Multipliers**: Continuous left-click mining with hardness formulas (Dirt $0.5\text{s}$, Stone $1.5\text{s}$, Obsidian $5.0\text{s}$). Pickaxes, axes, and shovels dramatically accelerate harvest speed according to tier (Wood, Stone, Iron, Diamond).
- **Debris Particles & Breaking Decals**: Mining generates real-time particle sparks and block-break debris.

---

### 🌲 Natural Mob Spawning & World Ecology (`MobEngine`)
- **Radius-Based Spawner**: Spawns passive animals on grass surfaces during daylight and hostile monsters in dark areas ($r = 24 \dots 48$ blocks).
- **Mob Cap & Distance Despawning**: Strict mob cap limit (24 mobs) and automatic despawning of entities further than 72 blocks from the player.

---

### 🎨 Visual & Shader Pipeline (`ChunkMesh`, `TextureAtlas` & Shaders)
- **Pixel-Art Texture Atlas**: Integrated $256 \times 256$ texture atlas generator with authentic pixel art patterns for all voxel blocks, ores, flora, and liquids with `GL_NEAREST_MIPMAP_LINEAR` filtering.
- **Smooth Lighting / Vertex AO**: Dynamic per-vertex ambient occlusion calculation ($0.25 \dots 1.0$) based on 3-voxel corner neighborhood checks.
- **Waving Foliage & Wind Flutter**: Real-time vertex displacement in `block.vert` simulating wind sway for tree canopies, tall grass, crops, and water ripples.
- **Post-Processing (Bloom & ACES Filmic)**: Multi-pass FBO pipeline in `postprocess.frag` with Bloom extraction, ACES HDR tonemapping, Vignette, and Night Vision luminance amplification.
- **Asynchronous CPU Meshing**: Worker threads generate vertex/index data without OpenGL context locks; main thread performs bounded batch GPU buffer uploads.

---

### 🧩 Sub-Chunk Vertical Slicing (`ChunkSection`)
- **$16 \times 16 \times 16$ Chunk Sections**: Chunks are partitioned into 16 vertical sections. Block edits only trigger re-meshing for the affected sub-chunk, reducing modification meshing CPU overhead by **93%**.

---

### 🔌 Data-Driven Modding & Addon Registry (`ModdingEngine`)
- **Custom Blocks & Recipes**: Dynamic runtime registration of custom block definitions (hardness, light emission, solid/opacity flags) and custom crafting recipes via JSON manifests.

---

### ⚗️ Brewing Stand Engine & Status Effects (`BrewingEngine` & `PlayerStats`)
- **Brewing Stand Recipes**: Synthesis of Awkward Potions, Swiftness (Speed I), Leaping (Jump Boost I), Night Vision, Healing (Instant Health), Regeneration, and Poison.
- **Dynamic Status Modifiers**: Live acceleration of player movement velocity, jump height elevation, continuous health regeneration, and poison damage tick timers.

---

### 🏛️ Advanced World Structures & Nether Biomes (`Biome` & `StructureGenerator`)
- **Nether Biomes**: Crimson Forest, Warped Forest, Soul Sand Valley, and Basalt Deltas.
- **Nether Fortress Corridors**: Obsidian bridge corridors with Netherrack railings and overhead Glowstone beacons.
- **Abandoned Mineshafts**: 3D underground tunnels with oak support arches, rail track networks, cobblestone floors, and redstone wall torches.
- **Desert Pyramid Temples**: Sandstone pyramids featuring hidden underground basement shafts with 4 corner loot chests and a central TNT floor trap.

---

### 🔴 Redstone & Piston Engine (`RedstoneEngine`)
- **Signal Propagation & Logic Gates**:
  - 🔴 **Redstone Wire (`RedstoneWire`)**: Signal transport across blocks with signal decay (0–15 strength).
  - 🕯️ **Redstone Torch (`RedstoneTorch`)**: Infinite power source & signal inverter.
  - 🎛️ **Lever (`Lever`)**: Interactive state toggling on right-click.
  - 💡 **Redstone Lamp (`RedstoneLamp`)**: Automatic visual light emission upon active redstone power.
  - ⏱️ **Redstone Repeater**: Signal delay and full power boost to 15.
- **Pistons & Sticky Pistons**: Mechanical shifting of up to 12 connected blocks and sticky retraction pulling adjacent blocks.

---

## ⌨️ Controls & Keybindings

| Key / Input | Action |
|-------------|--------|
| `Escape` | Open Pause Menu / Close UI / Back |
| `E` | Open / Close Inventory & Crafting GUI |
| `W / A / S / D` | Movement (Forward, Left, Backward, Right) |
| `Space` | Jump / Swim upward in water / Ascend in Fly Mode |
| `L-Shift` | Sneak (prevents falling off ledges) / Descend in Fly Mode |
| `L-Ctrl` | Sprint (increases movement speed and FOV) |
| `F` | Toggle Fly Mode vs Physics Mode |
| `F3` | Toggle Debug HUD (FPS, Position, Chunk, Facing, Biome, Light) |
| `F4` | Cycle Time of Day (Day -> Sunset -> Night -> Sunrise) |
| `T` | Fast Forward Time |
| `1 - 9` | Select Hotbar Item Slot |
| `Left Click (Hold)` | Continuous Mining / Attack Mob |
| `Right Click` | Place block / Interact (Chest, Furnace, Lever, Food, Redstone) |
| `Mouse Move` | Look around (Pitch / Yaw) |

---

## 🏗️ Build & Compilation

### Requirements
- **Compiler**: C++20 compatible (MSVC 2019+, GCC 10+, Clang 11+)
- **CMake**: Version >= 3.20
- **Libraries** (automatically fetched via CMake): GLFW 3.4, GLM 1.0.1, EnTT 3.13.2, OpenGL 4.5 Core

### Windows Build (MSVC)
```powershell
mkdir build
cd build
cmake .. -G "NMake Makefiles" -DCMAKE_BUILD_TYPE=Release
nmake
```

### Running the Engine
```powershell
.\build\Minecraft.exe
```

### Running Automated Engine Unit Tests
```powershell
.\build\TestEngine.exe
```
> **100% Pass Rate**: All **56 automated test suites** execute and validate every engine subsystem across physics, lighting, world generation, mob AI, GUI, clouds, shaders, and networking.

---

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
