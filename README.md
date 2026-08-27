# Minecraft C++ OpenGL Voxel Engine (1:1 Remake & Modern Voxel Engine)

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)](https://en.cppreference.com/w/cpp/20)
[![OpenGL](https://img.shields.io/badge/OpenGL-4.5-green.svg)](https://www.opengl.org/)
[![CMake](https://img.shields.io/badge/CMake-%3E%3D3.20-orange.svg)](https://cmake.org/)
[![Build & Test](https://github.com/Schengii/Minecraft/actions/workflows/build.yml/badge.svg)](.github/workflows/build.yml)

A high-performance, modular 3D voxel game engine written in **C++20** and **OpenGL 4.5**, modeled after Minecraft. Features high-throughput multithreaded chunk generation, $16 \times 16 \times 16$ sub-chunk section slicing, asynchronous CPU meshing with exact atlas UV tiling, Amanatides & Woo 3D DDA fast voxel raycasting, real-time dynamic shadow mapping with 3x3 PCF filtering, procedural pixel-art texture atlas generation, 3D hierarchical mob models, 3-headed Wither boss mechanics, multiplayer remote player models with nametags, client-side prediction with dead-reckoning interpolation, screen-space ambient occlusion (SSAO), procedural skybox clouds at $Y=128$, PBR specular & Fresnel water shaders, environmental player hazards (oxygen/drowning, fire/lava burning, fall damage), animal breeding & feeding mechanics, pig & horse mount saddling and steering, dynamic fluid flow level height calculations, Nether Bastion remnants, Elytra aerodynamic gliding physics, End City towers, atmospheric distance fog with sun in-scattering, mouse scroll hotbar selection, right-click stack splitting, infinite water source generation, console command engine, first-person hand animations, bitmap typography rendering, interactive container GUIs (chests and animated furnaces), continuous mining with tool speed multipliers, non-blocking UDP socket multiplayer networking, vertex ambient occlusion (smooth lighting), ACES filmic tone mapping & bloom post-processing, data-driven modding engine, brewing stand & status effect systems, cellular automaton redstone, hopper item transfer & comparator reading, piston mechanics, 3D A* mob pathfinding, fluid dynamics, Anvil region saving, and material-based spatial audio.

---

## 🌟 Feature Overview & State

### 🎯 Fast Voxel Traversal DDA Raycasting (`Raycast.cpp`)
- **Amanatides & Woo 3D Grid DDA**: Exact grid traversal algorithm with zero sampling gaps and sub-voxel boundary intersection precision.
- **Accurate Hit Surface Normals**: Guarantees mathematically exact face normal calculations for block placement and mining across chunk boundaries.

---

### ☀️ Real-Time Shadow Mapping & PCF Filtering (`block.vert` & `block.frag`)
- **Directional Light-Space Shadows**: Dynamic shadow depth map generated from the moving sun orientation.
- **3x3 Percentage-Closer Filtering (PCF)**: Smooth, anti-aliased soft shadow edges with slope-scaled adaptive depth bias preventing shadow acne.

---

### 🌑 Screen-Space Ambient Occlusion (SSAO) (`postprocess.frag` & `PostProcessing.cpp`)
- **Contact Shadow Occlusion**: Post-processing screen-space curvature and depth-gradient sampling producing localized contact darkening on block recesses and corners.

---

### 🌐 Client-Side Prediction & Dead-Reckoning Interpolation (`NetworkManager.cpp`)
- **Velocity-Compensated Dead-Reckoning**: Predicts remote entity movement trajectories ($\vec{p}_{\text{pred}} = \vec{p}_{\text{target}} + \vec{v} \cdot \Delta t$) with smooth alpha blending to prevent network stutter.
- **Shortest-Path Angle Interpolation**: Seamless heading yaw and pitch smoothing across the 360-degree boundary.

---

### 🌫️ Atmospheric Distance Fog & Sun In-Scattering (`block.frag`)
- **Exponential Squared Distance Fog**: $1.0 - e^{-(\text{dist} \times \text{density})^2}$ smoothly dissolves distant chunk edges into the horizon and skybox without clipping pop-in.
- **Sun Ray In-Scattering**: Computes forward Rayleigh/Mie solar scattering $\max(-\vec{v} \cdot \vec{l}, 0)^4$ adding warm golden volumetric glows when looking toward the sun.

---

### 🐎 Mount Saddling & Steered Riding (`MobEngine` & `PlayerStats`)
- **Saddle Equipment**: Right-clicking an adult pig or cow with a saddle equips the mount with full ridable capabilities.
- **Player-Steered Velocity**: Mounted players directly steer the animal with accelerated directional velocity ($6.0\,\text{m/s}$).

---

### 🦅 Elytra Aerodynamic Gliding Physics (`PhysicsEngine.cpp` & `PlayerStats.cpp`)
- **Pitch-to-Thrust Translation**: Translates downward gravitational potential energy into forward horizontal aerodynamic thrust ($\vec{v}_{\text{horiz}} += \vec{d}_{\text{look}} \times 14.0\,\text{m/s}^2$).
- **Cushioned Descent**: Clamps maximum downward glide velocity to $-2.5\,\text{m/s}$ for smooth panoramic aerial travel.

---

### 🐷 Animal Breeding & Feeding Mechanics (`MobEngine.hpp` & `MobEngine.cpp`)
- **Food Acceptance & Love Mode**: Feeding pigs (carrots, potatoes, apples) or cows (wheat, apples) initiates a 30-second breeding state with hearts.
- **Pair Reproduction & Baby Spawning**: When two animals in love meet ($\text{dist} < 3.5\text{m}$), they spawn a miniature baby animal with an age growth timer ($300\text{s}$).

---

### 🌊 Dynamic Fluid Flow Levels & Infinite Sources (`FluidEngine.cpp`)
- **Fluid Height Leveling**: Computes distance from source ($1 \dots 7$), supplying proportional surface heights ($h = 1.0 - \text{level} \times 0.12$).
- **2-Source Infinite Well Formation**: Air blocks with a solid floor and $\ge 2$ horizontally adjacent water source blocks automatically form new infinite water source blocks.
- **Lava + Water Obsidian Reaction**: Flowing water touching stationary lava blocks instantly transforms them into solid Obsidian.

---

### 🏛️ Advanced Structures: Nether Bastions, End Cities & Strongholds (`StructureGenerator`)
- **Nether Bastion Remnants**: $11 \times 8 \times 11$ fortified fortresses with gold deposits, central treasure vault chest, and magma cube spawners.
- **End City Towers**: $7 \times 14 \times 7$ Obsidian/Stone Brick fortresses in The End with floor dividers, floating overhang balconies, Shulker spawners, and loot chests.
- **Stronghold Portal Chamber**: $9 \times 6 \times 9$ Stone Brick dungeon featuring suspended $3 \times 3$ End Portal frame blocks around a central lava pool, silverfish spawner, and iron bar archways.
- **Ocean Ruins & Desert Pyramids**: Submerged underwater structures and multi-room sandstone temples with TNT basement traps.

---

### 🔲 Tiled Greedy Meshing & Zero UV Distortion (`ChunkMesh.cpp`)
- **Per-Block UV Sub-Sprite Clamping**: Merged coplanar rectangular faces emit tiled sub-quads mapped precisely to $[u, u+1/16] \times [v, v+1/16]$ atlas texture tiles.
- **Bilinear AO Interpolation**: Preserves smooth per-vertex ambient occlusion gradients across merged surfaces without texture stretching or atlas bleeding.

---

### 🫁 Player Survival Mechanics & Environmental Hazards (`PlayerStats` & `PhysicsEngine`)
- **Oxygen & Drowning**: 300-tick air gauge that depletes when submerged underwater ($30\,\text{ticks/sec}$), causing $1.0\,\text{HP}$ damage every 1.25 seconds when out of air. Surfaces instantly recover oxygen.
- **Lava & Fire Burning**: Immersion in lava or contact with fire ignites the player for 100 ticks ($1.0\,\text{HP}$ per second). Immersion in water immediately extinguishes all burning ticks.
- **Fall Damage & Armor Protection**: Accumulates fall distance while airborne. Impacts above 3 blocks deal $1.0\,\text{HP}$ damage per block fallen, mitigated by equipped diamond/iron/gold/leather armor damage reduction ratios.

---

### 🖱️ Mouse Scroll Wheel & Inventory Stack-Splitting (`Input`, `InventoryGUI` & `ContainerGUI`)
- **Mouse Wheel Hotbar Selection**: Smooth hotbar slot cycling ($0 \dots 8$) via GLFW scroll events.
- **Right-Click Half-Stack Pickup**: Right-clicking an item stack in inventory or chest picks up $\lfloor \text{count} / 2 \rfloor$ items.
- **Single-Item Drop & Distribution**: Right-clicking a destination slot while holding an item stack deposits exactly 1 item.

---

### 💻 In-Game Console Command Engine (`CommandParser.hpp` & `Application.cpp`)
- **Full Console Command Parser**:
  - `/gamemode <creative|survival>`: Toggles flight mode and survival physics.
  - `/time set <day|noon|sunset|night|midnight|sunrise|ticks>`: Real-time celestial time manipulation.
  - `/tp <x> <y> <z>`: Direct player coordinate teleportation.
  - `/give <item> [count]`: Adds items and tools directly into player inventory.
  - `/weather <clear|rain|thunder>`: Changes atmospheric precipitation and lightning states.
  - `/heal` & `/kill`: Instantly restores or zeroes player health and oxygen.

---

### 🔴 Redstone Comparator & Hopper Automation (`RedstoneEngine`)
- **Redstone Comparator (`BlockType::Comparator`)**: Measures chest/furnace/hopper container inventory fullness and outputs proportional redstone analog power ($0 \dots 15$).
- **Hopper Item Transfers (`BlockType::Hopper`)**: Automatically extracts and deposits items between connected containers on 20Hz ticks; locks and pauses transfer when powered by redstone.

---

### 🌿 Biome Colormap Blending (`Biome.hpp` & `Biome.cpp`)
- **Dynamic Grass & Foliage Colors**: Calculates authentic RGB tinting based on biome temperature and humidity:
  - 🌴 Jungle: Vibrant emerald green `(0.35, 0.80, 0.22)`.
  - 🏜️ Desert: Parched olive-yellow `(0.75, 0.71, 0.38)`.
  - 🌲 Taiga: Dark pine teal-green `(0.40, 0.65, 0.50)`.
  - 🐸 Swamp: Murky olive-brown `(0.42, 0.52, 0.27)`.

---

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
- **27-Slot Chest GUI**: Interactive chest interface supporting full stack transfers, slot click dragging, right-click half stack pickup, and audio feedback.
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
| `1 - 9` / `Mouse Wheel` | Select Hotbar Item Slot |
| `Left Click (Hold)` | Continuous Mining / Attack Mob |
| `Right Click` | Place block / Interact (Chest, Furnace, Lever, Food, Redstone) / Half-Stack Pick / Single Item Deposit |
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
nmake Minecraft
```

### Running the Engine
```powershell
.\build\Minecraft.exe
```

### Running Automated Engine Unit Tests
```powershell
nmake TestEngine
.\build\TestEngine.exe
```
> **100% Pass Rate**: All **76 automated test suites** execute and validate every engine subsystem across DDA raycasting, PCF shadow mapping, Screen-Space Ambient Occlusion (SSAO), dead-reckoning client prediction, volumetric distance fog, animal breeding, mount riding & saddling, dynamic fluid levels, Nether Bastions, Elytra gliding aerodynamics, End City structures, player hazards (oxygen, lava, fall damage), mouse scroll selection, inventory stack-splitting, infinite water fluids, console commands, Redstone Comparators, Hopper automation, Biome colormaps, Stronghold dungeons, mob AI, procedural clouds, and UDP socket networking.

---

## 📄 License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
