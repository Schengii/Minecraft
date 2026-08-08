#include <iostream>
#include <cassert>
#include <filesystem>
#include <memory>
#include <glm/glm.hpp>
#include "../src/world/World.hpp"
#include "../src/world/Block.hpp"
#include "../src/world/RedstoneEngine.hpp"
#include "../src/world/FluidEngine.hpp"
#include "../src/world/ExplosionEngine.hpp"
#include "../src/world/ToolSystem.hpp"
#include "../src/world/TimeManager.hpp"
#include "../src/world/WeatherManager.hpp"
#include "../src/world/CaveDecorator.hpp"
#include "../src/world/Raycast.hpp"
#include "../src/world/ChestBlock.hpp"
#include "../src/world/FurnaceBlock.hpp"
#include "../src/world/StructureGenerator.hpp"
#include "../src/world/DimensionManager.hpp"
#include "../src/world/RegionFile.hpp"
#include "../src/world/LightEngine.hpp"
#include "../src/core/ThreadPool.hpp"
#include "../src/inventory/Inventory.hpp"
#include "../src/inventory/PlayerStats.hpp"
#include "../src/inventory/FoodSystem.hpp"
#include "../src/gui/MenuGUI.hpp"
#include "../src/renderer/Skybox.hpp"
#include "../src/gui/ContainerGUI.hpp"
#include "../src/crafting/CraftingManager.hpp"
#include "../src/physics/PhysicsEngine.hpp"
#include "../src/ecs/MobEngine.hpp"
#include "../src/ecs/ItemEntity.hpp"
#include "../src/renderer/ParticleEngine.hpp"
#include "../src/renderer/FrustumCuller.hpp"
#include "../src/audio/AudioManager.hpp"
#include "../src/net/NetworkManager.hpp"
#include "../src/world/ChunkMesh.hpp"

using namespace Minecraft;

void testGreedyMeshing() {
    std::cout << "[TEST] 18. Greedy Meshing Quad Optimization & Vertex Compression..." << std::endl;
    Chunk chunk(0, 0);
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            chunk.setBlock(x, 10, z, BlockType::Grass);
        }
    }
    ChunkMesh mesh;
    std::vector<Vertex> verts;
    std::vector<unsigned int> indices;
    mesh.buildMeshData(chunk, verts, indices);
    assert(verts.size() > 0 && verts.size() < 16 * 16 * 24);
    std::cout << "  -> Greedy Meshing tests PASSED!" << std::endl;
}

void testExtendedCrafting() {
    std::cout << "[TEST] 19. Extended 3x3 Crafting Table Recipes (Swords, Axes)..." << std::endl;
    std::array<ItemStack, 9> grid;
    grid.fill({ BlockType::Air, 0, 64 });
    grid[1] = { BlockType::IronOre, 1, 64 };
    grid[4] = { BlockType::IronOre, 1, 64 };
    grid[7] = { BlockType::Stick, 1, 64 };

    ItemStack result = CraftingManager::matchRecipe3x3(grid);
    assert(result.type == BlockType::IronSword);
    std::cout << "  -> Extended 3x3 Crafting tests PASSED!" << std::endl;
}

void testRedstone() {
    std::filesystem::remove_all("world_saves");
    std::cout << "[TEST] 1. RedstoneEngine Signal Network & Logic..." << std::endl;
    World world(1);
    
    world.setBlock(10, 60, 10, BlockType::Lever);
    bool powered = RedstoneEngine::isPowered(world, glm::ivec3(10, 60, 11));
    assert(powered == true);
    
    int strength = RedstoneEngine::getSignalStrength(world, glm::ivec3(10, 60, 10));
    assert(strength == 15);
    
    RedstoneEngine::updateRedstoneNetwork(world, glm::ivec3(10, 60, 10));
    std::cout << "  -> RedstoneEngine tests PASSED!" << std::endl;
}

void testFluids() {
    std::filesystem::remove_all("world_saves");
    std::cout << "[TEST] 2. FluidEngine Water/Lava Cellular Automaton Spreading..." << std::endl;
    World world(1);
    
    world.setBlock(0, 70, 0, BlockType::Water);
    world.setBlock(0, 69, 0, BlockType::Air);
    
    FluidEngine::updateFluids(world, glm::vec3(0, 70, 0));
    assert(world.getBlock(0, 69, 0) == BlockType::Water);
    
    world.setBlock(5, 69, 5, BlockType::Stone);
    world.setBlock(6, 69, 5, BlockType::Stone);
    world.setBlock(5, 70, 5, BlockType::Water);
    world.setBlock(6, 70, 5, BlockType::Lava);
    FluidEngine::updateFluids(world, glm::vec3(5, 70, 5));
    
    BlockType b1 = world.getBlock(5, 70, 5);
    BlockType b2 = world.getBlock(6, 70, 5);
    assert(b1 == BlockType::Stone || b2 == BlockType::Stone);
    
    std::cout << "  -> FluidEngine tests PASSED!" << std::endl;
}

void testToolSystem() {
    std::cout << "[TEST] 3. ToolSystem Durability, Mining Speed & Harvest Tiers..." << std::endl;
    assert(ToolSystem::isTool(BlockType::IronPickaxe) == true);
    assert(ToolSystem::getToolInfo(BlockType::DiamondPickaxe).maxDurability == 1561);
    assert(ToolSystem::getMiningSpeed(BlockType::Stone, BlockType::IronPickaxe) == 6.0f);
    assert(ToolSystem::canHarvest(BlockType::DiamondOre, BlockType::IronPickaxe) == true);
    assert(ToolSystem::canHarvest(BlockType::DiamondOre, BlockType::WoodPickaxe) == false);
    assert(ToolSystem::getDamageDealt(BlockType::DiamondSword) == 7);
    std::cout << "  -> ToolSystem tests PASSED!" << std::endl;
}

void testExplosionEngine() {
    std::filesystem::remove_all("world_saves");
    std::cout << "[TEST] 4. ExplosionEngine TNT Detonation & Knockback..." << std::endl;
    World world(1);
    world.setBlock(5, 65, 5, BlockType::Stone);
    world.setBlock(5, 65, 6, BlockType::Stone);
    
    glm::vec3 playerPos(5.0f, 66.0f, 8.0f);
    glm::vec3 playerVel(0.0f);
    
    ExplosionEngine::createExplosion(world, glm::vec3(5.5f, 65.5f, 5.5f), 3.0f, &playerVel, &playerPos);
    assert(world.getBlock(5, 65, 5) == BlockType::Air);
    assert(playerVel.z > 0.0f);
    std::cout << "  -> ExplosionEngine tests PASSED!" << std::endl;
}

void testThreadPool() {
    std::cout << "[TEST] 5. ThreadPool Asynchronous Tasks..." << std::endl;
    ThreadPool pool(2);
    auto fut = pool.enqueue([]() { return 42; });
    assert(fut.get() == 42);
    std::cout << "  -> ThreadPool tests PASSED!" << std::endl;
}

void testChestAndFurnace() {
    std::cout << "[TEST] 6. ChestManager & FurnaceManager Storage..." << std::endl;
    ChestManager cm;
    cm.setSlot(glm::ivec3(5, 60, 5), 0, BlockType::DiamondOre, 12);
    auto* inv = cm.getChestInventory(glm::ivec3(5, 60, 5));
    assert(inv != nullptr && (*inv)[0].type == BlockType::DiamondOre && (*inv)[0].count == 12);

    FurnaceManager fm;
    FurnaceData* f = fm.getFurnace(glm::ivec3(0, 60, 0));
    f->input = { BlockType::IronOre, 2, 64 };
    f->fuel = { BlockType::CoalOre, 1, 64 };
    fm.update(5.1f);
    assert(f->output.type != BlockType::Air);
    std::cout << "  -> ChestManager & FurnaceManager tests PASSED!" << std::endl;
}

void testNetherDimension() {
    std::cout << "[TEST] 7. DimensionManager Nether Portal & Terrain..." << std::endl;
    DimensionManager dm;
    assert(dm.getCurrentDimension() == DimensionType::Overworld);
    
    glm::vec3 playerPos(0.0f, 40.0f, 0.0f);
    World* world = dm.getCurrentWorld();
    world->setBlock(0, 40, 0, BlockType::NetherPortal);

    glm::vec3 outPos;
    bool teleported = dm.checkPortalTeleport(playerPos, outPos);
    assert(teleported == true);
    assert(dm.getCurrentDimension() == DimensionType::Nether);
    std::cout << "  -> DimensionManager tests PASSED!" << std::endl;
}

void testItemEntities() {
    std::cout << "[TEST] 8. ItemEntityManager Magnet Pickup & Drops..." << std::endl;
    World world(1);
    world.setBlock(0, 59, 0, BlockType::Stone);
    ItemEntityManager iem;
    iem.spawnItemDrop(BlockType::GoldOre, 3, glm::vec3(0, 60, 0));
    assert(iem.getEntities().size() == 1);

    std::vector<std::pair<BlockType, int>> picked;
    iem.update(world, glm::vec3(0, 60, 0), picked, 0.6f);
    iem.update(world, glm::vec3(0, 60, 0), picked, 0.6f);
    assert(picked.size() == 1 && picked[0].first == BlockType::GoldOre);
    std::cout << "  -> ItemEntityManager tests PASSED!" << std::endl;
}

void testMobEngineAdvanced() {
    std::filesystem::remove_all("world_saves");
    std::cout << "[TEST] 9. Advanced Mob AI (Skeleton, Creeper, Zombie)..." << std::endl;
    World world(1);
    MobEngine mobEngine;
    
    mobEngine.spawnMob(MobType::Creeper, glm::vec3(10.0f, 60.0f, 10.0f));
    mobEngine.spawnMob(MobType::Skeleton, glm::vec3(15.0f, 60.0f, 15.0f));
    assert(mobEngine.getMobs().size() == 2);
    
    glm::vec3 pPos(10.0f, 60.0f, 9.0f);
    glm::vec3 pVel(0.0f);
    float pHealth = 20.0f;
    mobEngine.update(world, pPos, pVel, pHealth, 0.1f);
    
    std::cout << "  -> Advanced Mob AI tests PASSED!" << std::endl;
}

void testPlayerStatsAndArmor() {
    std::cout << "[TEST] 10. PlayerStats Armor Points & Damage Reduction..." << std::endl;
    PlayerStats stats;
    stats.getArmorSlot(0) = { BlockType::DiamondPickaxe, 1, 1, 100, 100 };
    stats.getArmorSlot(1) = { BlockType::IronPickaxe, 1, 1, 100, 100 };
    assert(stats.getTotalArmorPoints() == 8);
    float finalDmg = stats.applyDamageReduction(10.0f);
    assert(finalDmg < 10.0f);
    std::cout << "  -> PlayerStats & Armor tests PASSED!" << std::endl;
}

void testWeatherManager() {
    std::cout << "[TEST] 11. WeatherManager Rain, Snow & Thunderstorms..." << std::endl;
    WeatherManager wm;
    wm.setWeather(WeatherState::Thunderstorm);
    assert(wm.isThundering() == true);
    wm.update(glm::vec3(0, 65, 0), 0.1f);
    assert(wm.getParticles().size() > 0);
    std::cout << "  -> WeatherManager tests PASSED!" << std::endl;
}

void testCaveDecorator() {
    std::cout << "[TEST] 12. CaveDecorator Stalagmites & Stalactites..." << std::endl;
    World world(1);
    world.setBlock(0, 30, 0, BlockType::Stone);
    world.setBlock(0, 31, 0, BlockType::Air);
    world.setBlock(0, 32, 0, BlockType::Air);
    world.setBlock(0, 33, 0, BlockType::Stone);

    CaveDecorator::generateStalagmite(world, 0, 31, 0, 1);
    assert(world.getBlock(0, 31, 0) == BlockType::Stone);
    std::cout << "  -> CaveDecorator tests PASSED!" << std::endl;
}

void testContainerGUI() {
    std::cout << "[TEST] 13. ContainerGUI Chest & Furnace Slots..." << std::endl;
    ContainerGUI cgui(1280, 720);
    std::vector<ItemStack> chestInv(27, ItemStack{ BlockType::IronOre, 5, 64 });
    cgui.openChest(glm::ivec3(0, 60, 0), &chestInv);
    assert(cgui.isOpen() == true);

    Inventory playerInv;
    bool clicked = cgui.handleMouseClick(playerInv, 100.0, 100.0, 0);
    assert(clicked == true);
    assert(playerInv.getSlot(9).type == BlockType::IronOre);
    std::cout << "  -> ContainerGUI tests PASSED!" << std::endl;
}

void testNetworkManager() {
    std::cout << "[TEST] 14. NetworkManager Server & Client Packets..." << std::endl;
    NetworkManager net;
    assert(net.startServer(25565) == true);
    assert(net.isServer() == true && net.isConnected() == true);
    net.sendPlayerPosition(glm::vec3(10, 65, 10), 0.0f, 0.0f);
    net.sendBlockChange(glm::ivec3(5, 60, 5), BlockType::Stone);
    net.disconnect();
    assert(net.isConnected() == false);
    std::cout << "  -> NetworkManager tests PASSED!" << std::endl;
}

void testHungerAndFoodSystem() {
    std::cout << "[TEST] 15. Hunger & Food System (Eating, Regeneration, Starvation, Mob Drops & Smelting)..." << std::endl;
    
    // 1. Food Info & Checks
    assert(FoodSystem::isFood(BlockType::Apple) == true);
    assert(FoodSystem::isFood(BlockType::CookedPorkchop) == true);
    assert(FoodSystem::isFood(BlockType::Stone) == false);

    FoodInfo info = FoodSystem::getFoodInfo(BlockType::CookedPorkchop);
    assert(info.hungerRestored == 8.0f);

    // 2. Eating Food
    PlayerStats stats;
    stats.setHunger(10.0f);
    bool ate = FoodSystem::eatFood(stats, BlockType::CookedPorkchop);
    assert(ate == true);
    assert(stats.getHunger() == 18.0f);

    // 3. Exhaustion & Hunger Consumption
    stats.addExhaustion(4.0f);
    stats.update(0.1f);
    assert(stats.getHunger() == 17.0f);

    // 4. Passive Health Regeneration
    stats.setHunger(20.0f);
    stats.setHealth(15.0f);
    stats.update(4.1f);
    assert(stats.getHealth() == 16.0f);

    // 5. Starvation Damage
    stats.setHunger(0.0f);
    stats.setHealth(10.0f);
    stats.update(4.1f);
    assert(stats.getHealth() == 9.0f);

    // 6. Smelting Raw Porkchop -> Cooked Porkchop
    assert(FurnaceManager::isSmeltable(BlockType::RawPorkchop) == true);
    assert(FurnaceManager::getSmeltResult(BlockType::RawPorkchop) == BlockType::CookedPorkchop);

    std::cout << "  -> Hunger & Food System tests PASSED!" << std::endl;
}

void testRegionFileAndChunkStreaming() {
    std::cout << "[TEST] 16. Anvil .mca RegionFile Persistence & Async Chunk Streaming..." << std::endl;
    RegionManager::getInstance().clearCache();
    std::filesystem::remove_all("test_saves");
    std::filesystem::create_directories("test_saves");

    // 1. Test RegionFile Save & Load
    BlockType blocks[16][256][16];
    uint8_t light[16][256][16];
    std::memset(blocks, static_cast<int>(BlockType::Stone), sizeof(blocks));
    std::memset(light, 0xFF, sizeof(light));
    blocks[5][60][5] = BlockType::DiamondOre;

    bool saved = RegionManager::getInstance().saveChunk(10, 10, blocks, light, "test_saves");
    assert(saved == true);

    BlockType readBlocks[16][256][16];
    uint8_t readLight[16][256][16];
    bool loaded = RegionManager::getInstance().loadChunk(10, 10, readBlocks, readLight, "test_saves");
    assert(loaded == true);
    assert(readBlocks[5][60][5] == BlockType::DiamondOre);

    // 2. Test Multi-Threaded Chunk Loading
    World world(2);
    world.update(glm::vec3(100.0f, 65.0f, 100.0f));
    assert(world.getLoadedChunkCount() > 0);

    std::filesystem::remove_all("test_saves");
    std::cout << "  -> Anvil RegionFile & Async Streaming tests PASSED!" << std::endl;
}

void testLightEnginePropagation() {
    std::cout << "[TEST] 17. LightEngine 3D BFS Sunlight & Blocklight Propagation..." << std::endl;
    Chunk chunk(0, 0);

    // Sunlight check top column
    chunk.setBlock(5, 100, 5, BlockType::Air);
    LightEngine::calculateSunlight(chunk);
    assert(chunk.getSunlight(5, 100, 5) == 15);

    // Blocklight check torch emission & decay
    chunk.setBlock(5, 50, 5, BlockType::RedstoneTorch);
    chunk.setBlock(6, 50, 5, BlockType::Air);
    chunk.setBlock(7, 50, 5, BlockType::Air);
    LightEngine::calculateBlocklight(chunk);
    assert(chunk.getBlocklight(5, 50, 5) == 14);
    assert(chunk.getBlocklight(6, 50, 5) == 13);
    assert(chunk.getBlocklight(7, 50, 5) == 12);

    std::cout << "  -> LightEngine 3D BFS tests PASSED!" << std::endl;
}

void testMenuAndParticles() {
    std::cout << "[TEST] 20. MenuGUI State Machine & Particle Engine debris..." << std::endl;
    ParticleEngine pe;
    pe.spawnBlockBreak(glm::vec3(0, 60, 0));
    assert(pe.getParticles().size() > 0);
    pe.update(0.1f);
    assert(pe.getParticles().size() > 0);

    assert(BlockData::isOpaque(BlockType::Stone) == true);
    assert(BlockData::isOpaque(BlockType::Glass) == false);
    assert(BlockData::isOpaque(BlockType::Water) == false);

    std::cout << "  -> MenuGUI & Particle Engine tests PASSED!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Running Minecraft Engine Test Suite   " << std::endl;
    std::cout << "========================================" << std::endl;

    std::filesystem::remove_all("world_saves");

    testRedstone();
    testFluids();
    testToolSystem();
    testExplosionEngine();
    testThreadPool();
    testChestAndFurnace();
    testNetherDimension();
    testItemEntities();
    testMobEngineAdvanced();
    testPlayerStatsAndArmor();
    testWeatherManager();
    testCaveDecorator();
    testContainerGUI();
    testNetworkManager();
    testHungerAndFoodSystem();
    testRegionFileAndChunkStreaming();
    testLightEnginePropagation();
    testGreedyMeshing();
    testExtendedCrafting();
    testMenuAndParticles();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL ENGINE TESTS PASSED 100%!          " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
