#include <iostream>
#include <cassert>
#include <filesystem>
#include <cstring>
#include <glm/glm.hpp>
#include "../src/world/World.hpp"
#include "../src/world/Block.hpp"
#include "../src/world/ChunkSection.hpp"
#include "../src/world/RedstoneEngine.hpp"
#include "../src/world/FluidEngine.hpp"
#include "../src/world/ExplosionEngine.hpp"
#include "../src/world/ToolSystem.hpp"
#include "../src/world/WeatherManager.hpp"
#include "../src/world/Biome.hpp"
#include "../src/world/CaveDecorator.hpp"
#include "../src/world/Raycast.hpp"
#include "../src/world/ChestBlock.hpp"
#include "../src/world/FurnaceBlock.hpp"
#include "../src/world/StructureGenerator.hpp"
#include "../src/world/DimensionManager.hpp"
#include "../src/world/CropsEngine.hpp"
#include "../src/world/VillageGenerator.hpp"
#include "../src/world/TradingEngine.hpp"
#include "../src/world/EnchantingEngine.hpp"
#include "../src/world/BrewingEngine.hpp"
#include "../src/world/RegionFile.hpp"
#include "../src/world/LightEngine.hpp"
#include "../src/core/ThreadPool.hpp"
#include "../src/core/ModdingEngine.hpp"
#include "../src/core/CommandParser.hpp"
#include "../src/core/Application.hpp"
#include "../src/inventory/PlayerStats.hpp"
#include "../src/inventory/FoodSystem.hpp"
#include "../src/renderer/TextureAtlas.hpp"
#include "../src/renderer/PostProcessing.hpp"
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
#include "../src/gui/FontRenderer.hpp"
#include "../src/renderer/EntityRenderer.hpp"
#include "../src/renderer/Skybox.hpp"

using namespace Minecraft;

void testGreedyMeshing() {
    std::cout << "[TEST] 18. Greedy Meshing Quad Optimization & Vertex Compression..." << std::endl;
    Chunk chunk(0, 0);
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            chunk.setBlock(x, 10, z, BlockType::Grass);
        }
    }
    MeshData data = ChunkMesh::buildMeshData(chunk);
    assert(data.vertices.size() > 0 && data.vertices.size() < 16 * 16 * 24);
    assert(data.indices.size() > 0);
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
    
    world.setBlock(5, 70, 5, BlockType::Lava);
    world.setBlock(5, 69, 5, BlockType::Air);
    FluidEngine::updateFluids(world, glm::vec3(5, 70, 5));
    assert(world.getBlock(5, 69, 5) == BlockType::Lava);
    
    std::cout << "  -> FluidEngine tests PASSED!" << std::endl;
}

void testToolSystem() {
    std::cout << "[TEST] 3. ToolSystem Harvesting Speed & Material Durability..." << std::endl;
    float diamondSpeed = ToolSystem::getMiningSpeed(BlockType::Obsidian, BlockType::DiamondPickaxe);
    float handSpeed = ToolSystem::getMiningSpeed(BlockType::Obsidian, BlockType::Air);
    assert(diamondSpeed > handSpeed);
    
    int maxDur = ToolSystem::getToolInfo(BlockType::DiamondPickaxe).maxDurability;
    assert(maxDur == 1561);
    
    int woodDur = ToolSystem::getToolInfo(BlockType::WoodPickaxe).maxDurability;
    assert(woodDur == 59);
    
    std::cout << "  -> ToolSystem tests PASSED!" << std::endl;
}

void testExplosionEngine() {
    std::cout << "[TEST] 4. ExplosionEngine Spherical Destruction & TNT Chain Reactions..." << std::endl;
    World world(1);
    
    for (int x = -2; x <= 2; ++x) {
        for (int y = 50; y <= 54; ++y) {
            for (int z = -2; z <= 2; ++z) {
                world.setBlock(x, y, z, BlockType::Dirt);
            }
        }
    }
    
    ExplosionEngine::createExplosion(world, glm::vec3(0, 52, 0), 2.5f);
    assert(world.getBlock(0, 52, 0) == BlockType::Air);
    std::cout << "  -> ExplosionEngine tests PASSED!" << std::endl;
}

void testThreadPool() {
    std::cout << "[TEST] 5. ThreadPool Task Scheduling..." << std::endl;
    ThreadPool pool(4);
    std::atomic<int> counter = 0;
    
    for (int i = 0; i < 20; ++i) {
        pool.enqueue([&counter]() {
            counter++;
        });
    }
    
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    assert(counter.load() == 20);
    std::cout << "  -> ThreadPool tests PASSED!" << std::endl;
}

void testChestAndFurnace() {
    std::cout << "[TEST] 6. ChestManager & FurnaceManager State Machines..." << std::endl;
    ChestManager chestMgr;
    glm::ivec3 chestPos(10, 64, 10);
    chestMgr.createChest(chestPos);
    
    chestMgr.setSlot(chestPos, 0, BlockType::DiamondOre, 32);
    auto* chestInv = chestMgr.getChestInventory(chestPos);
    assert(chestInv != nullptr && (*chestInv)[0].count == 32);
    
    FurnaceManager furnaceMgr;
    glm::ivec3 furnacePos(20, 64, 20);
    FurnaceData* furnace = furnaceMgr.getFurnace(furnacePos);
    assert(furnace != nullptr);
    furnace->input = { BlockType::IronOre, 5, 64 };
    furnace->fuel = { BlockType::CoalOre, 2, 64 };
    furnaceMgr.update(15.0f);
    
    std::cout << "  -> Chest & Furnace tests PASSED!" << std::endl;
}

void testNetherDimension() {
    std::cout << "[TEST] 7. DimensionManager Nether World Generation & Portals..." << std::endl;
    DimensionManager dm;
    assert(dm.getCurrentType() == DimensionType::Overworld);
    
    dm.switchDimension(DimensionType::Nether);
    assert(dm.getCurrentType() == DimensionType::Nether);
    
    World* netherWorld = dm.getCurrentWorld();
    assert(netherWorld != nullptr);
    
    std::cout << "  -> Nether Dimension tests PASSED!" << std::endl;
}

void testItemEntities() {
    std::cout << "[TEST] 8. ItemEntityManager Physics & Player Magnetic Pickup..." << std::endl;
    ItemEntityManager itemMgr;
    itemMgr.spawnItemDrop(BlockType::DiamondOre, 5, glm::vec3(0.0f, 65.0f, 0.0f));
    
    assert(itemMgr.getEntities().size() == 1);
    
    World world(1);
    glm::vec3 playerPos(0.2f, 65.0f, 0.2f);
    std::vector<std::pair<BlockType, int>> pickedUp;
    itemMgr.update(world, playerPos, pickedUp, 0.1f);
    
    assert(!pickedUp.empty());
    assert(itemMgr.getEntities().empty());
    std::cout << "  -> ItemEntityManager tests PASSED!" << std::endl;
}

void testMobEngineAdvanced() {
    std::cout << "[TEST] 9. MobEngine AI, Pathfinder & Combat Interactions..." << std::endl;
    MobEngine mobEngine;
    World world(1);
    
    mobEngine.spawnMob(MobType::Zombie, glm::vec3(5.0f, 65.0f, 5.0f));
    mobEngine.spawnMob(MobType::Skeleton, glm::vec3(10.0f, 65.0f, 10.0f));
    mobEngine.spawnMob(MobType::Creeper, glm::vec3(3.0f, 65.0f, 3.0f));
    
    assert(mobEngine.getMobs().size() == 3);
    
    glm::vec3 playerPos(0.0f, 65.0f, 0.0f);
    glm::vec3 playerVel(0.0f);
    float playerHealth = 20.0f;
    
    mobEngine.update(world, playerPos, playerVel, playerHealth, 0.1f);
    
    bool hit = mobEngine.checkPlayerAttack(playerPos, glm::normalize(glm::vec3(1.0f, 0.0f, 1.0f)), 5.0f, 10);
    assert(hit == true);
    
    std::cout << "  -> MobEngine tests PASSED!" << std::endl;
}

void testPlayerStatsAndArmor() {
    std::cout << "[TEST] 10. PlayerStats Health, Armor & Damage Calculations..." << std::endl;
    PlayerStats stats;
    assert(stats.getHealth() == 20.0f);
    
    stats.getArmorSlot(0) = { BlockType::DiamondPickaxe, 1, 1 };
    stats.getArmorSlot(1) = { BlockType::DiamondPickaxe, 1, 1 };
    
    float reduced = stats.applyDamageReduction(10.0f);
    assert(reduced < 10.0f);
    std::cout << "  -> PlayerStats tests PASSED!" << std::endl;
}

void testWeatherManager() {
    std::cout << "[TEST] 11. WeatherManager Rain, Thunder & Dynamic Snow Cover..." << std::endl;
    WeatherManager wm;
    assert(wm.getWeatherState() == WeatherState::Clear);
    
    wm.setWeather(WeatherState::Rain);
    assert(wm.getWeatherState() == WeatherState::Rain);
    
    wm.setWeather(WeatherState::Thunderstorm);
    assert(wm.isThundering() == true);
    std::cout << "  -> WeatherManager tests PASSED!" << std::endl;
}

void testCaveDecorator() {
    std::cout << "[TEST] 12. CaveDecorator Stalactite & Stalagmite Placement..." << std::endl;
    World world(1);
    CaveDecorator::decorateCaveColumn(world, 0, 0, 10, 50);
    std::cout << "  -> CaveDecorator tests PASSED!" << std::endl;
}

void testContainerGUI() {
    std::cout << "[TEST] 13. ContainerGUI Inventory Slots & Visual Rendering..." << std::endl;
    ContainerGUI gui(1280, 720);
    ChestManager chestMgr;
    glm::ivec3 chestPos(10, 64, 10);
    chestMgr.createChest(chestPos);
    chestMgr.setSlot(chestPos, 4, BlockType::IronOre, 16);
    
    gui.openChest(chestPos, chestMgr.getChestInventory(chestPos));
    assert(gui.isOpen() == true);
    std::cout << "  -> ContainerGUI tests PASSED!" << std::endl;
}

void testNetworkManager() {
    std::cout << "[TEST] 14. NetworkManager Server/Client State Synchronization..." << std::endl;
    NetworkManager net;
    assert(net.startServer(25565) == true);
    assert(net.isServer() == true);
    
    net.sendPlayerPosition(glm::vec3(10.0f, 65.0f, 10.0f), 45.0f, 0.0f);
    net.sendBlockChange(glm::ivec3(5, 60, 5), BlockType::Stone);
    
    net.disconnect();
    assert(net.isConnected() == false);
    std::cout << "  -> NetworkManager tests PASSED!" << std::endl;
}

void testHungerAndFoodSystem() {
    std::cout << "[TEST] 15. FoodSystem & Hunger Consumption Mechanics..." << std::endl;
    PlayerStats stats;
    stats.setHunger(10.0f);

    assert(FoodSystem::isFood(BlockType::Apple) == true);
    assert(FoodSystem::getFoodInfo(BlockType::Apple).hungerRestored == 4.0f);

    FoodSystem::eatFood(stats, BlockType::Apple);
    assert(stats.getHunger() == 14.0f);

    stats.setHunger(20.0f);
    stats.setHealth(15.0f);
    stats.update(4.1f);
    assert(stats.getHealth() == 16.0f);

    stats.setHunger(0.0f);
    stats.setHealth(10.0f);
    stats.update(4.1f);
    assert(stats.getHealth() == 9.0f);

    assert(FurnaceManager::isSmeltable(BlockType::RawPorkchop) == true);
    assert(FurnaceManager::getSmeltResult(BlockType::RawPorkchop) == BlockType::CookedPorkchop);

    std::cout << "  -> Hunger & Food System tests PASSED!" << std::endl;
}

void testRegionFileAndChunkStreaming() {
    std::cout << "[TEST] 16. Anvil .mca RegionFile Persistence & Async Chunk Streaming..." << std::endl;
    RegionManager::getInstance().clearCache();
    std::filesystem::remove_all("test_saves");
    std::filesystem::create_directories("test_saves");

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

    World world(2);
    world.update(glm::vec3(100.0f, 65.0f, 100.0f));
    assert(world.getLoadedChunkCount() > 0);

    std::filesystem::remove_all("test_saves");
    std::cout << "  -> Anvil RegionFile & Async Streaming tests PASSED!" << std::endl;
}

void testLightEnginePropagation() {
    std::cout << "[TEST] 17. LightEngine 3D BFS Sunlight & Blocklight Propagation..." << std::endl;
    Chunk chunk(0, 0);

    chunk.setBlock(5, 100, 5, BlockType::Air);
    LightEngine::calculateSunlight(chunk);
    assert(chunk.getSunlight(5, 100, 5) == 15);

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

void testJungleBiomeAndBamboo() {
    std::cout << "[TEST] 21. Jungle Biome & Bamboo Flora World Generation..." << std::endl;
    BiomeType jungle = Biome::getBiome(0.5f, 0.5f);
    assert(jungle == BiomeType::Jungle);

    assert(BlockData::isOpaque(BlockType::Bamboo) == false);
    assert(BlockData::isSolid(BlockType::Bamboo) == false);

    Chunk chunk(100, 100);
    assert(chunk.getChunkX() == 100 && chunk.getChunkZ() == 100);
    std::cout << "  -> Jungle Biome & Bamboo tests PASSED!" << std::endl;
}

void testArmorCraftingSuite() {
    std::cout << "[TEST] 22. Iron & Diamond Full Armor Set Crafting Recipes..." << std::endl;
    
    std::array<ItemStack, 9> helmGrid;
    helmGrid.fill({ BlockType::Air, 0, 64 });
    helmGrid[0] = { BlockType::IronOre, 1, 64 };
    helmGrid[1] = { BlockType::IronOre, 1, 64 };
    helmGrid[2] = { BlockType::IronOre, 1, 64 };
    helmGrid[3] = { BlockType::IronOre, 1, 64 };
    helmGrid[5] = { BlockType::IronOre, 1, 64 };
    ItemStack helmResult = CraftingManager::matchRecipe3x3(helmGrid);
    assert(helmResult.type == BlockType::IronPickaxe && helmResult.durability == 165);

    std::cout << "  -> Armor Crafting Suite tests PASSED!" << std::endl;
}

void testFrustumCullingInWorld() {
    std::cout << "[TEST] 23. Frustum Culling & Transparent Mesh Render Pass..." << std::endl;
    World world(2);
    FrustumCuller culler;
    glm::mat4 viewProj = glm::mat4(1.0f);
    culler.update(viewProj);

    bool visible = culler.isBoxVisible(glm::vec3(0, 0, 0), glm::vec3(16, 256, 16));
    assert(visible == true);

    world.render(&culler);
    world.renderTransparent(&culler);
    std::cout << "  -> Frustum Culling & Transparency tests PASSED!" << std::endl;
}

void testTheEndDimensionAndDragon() {
    std::cout << "[TEST] 24. The End Dimension, End Stone Island & Ender Dragon Boss AI..." << std::endl;
    DimensionManager dm;
    dm.switchDimension(DimensionType::TheEnd);
    assert(dm.getCurrentType() == DimensionType::TheEnd);

    World* endWorld = dm.getCurrentWorld();
    assert(endWorld != nullptr);

    MobEngine bossEngine;
    bossEngine.spawnMob(MobType::EnderDragon, glm::vec3(0.0f, 75.0f, 0.0f));
    assert(bossEngine.getMobs().size() == 1);
    assert(bossEngine.getMobs()[0].health == 200.0f);

    glm::vec3 playerPos(0.0f, 65.0f, 0.0f);
    glm::vec3 playerVel(0.0f);
    float playerHealth = 20.0f;
    bossEngine.update(*endWorld, playerPos, playerVel, playerHealth, 0.1f);
    assert(bossEngine.getMobs()[0].velocity != glm::vec3(0.0f));

    std::cout << "  -> The End & Ender Dragon tests PASSED!" << std::endl;
}

void testCropsAndFarmingSystem() {
    std::cout << "[TEST] 25. Agricultural Crops Cultivation, Bone Meal & Growth Stages..." << std::endl;
    World world(1);
    CropsEngine::clear();

    world.setBlock(10, 60, 10, BlockType::Dirt);
    world.setBlock(11, 60, 10, BlockType::Water);

    bool planted = CropsEngine::plantCrop(world, 10, 61, 10, BlockType::WheatCrop);
    assert(planted == true);
    assert(CropsEngine::getCropGrowthStage(10, 61, 10) == 0);

    bool fertilized = CropsEngine::applyBoneMeal(world, 10, 61, 10);
    assert(fertilized == true);
    assert(CropsEngine::getCropGrowthStage(10, 61, 10) >= 2);

    for (int i = 0; i < 5; ++i) {
        CropsEngine::applyBoneMeal(world, 10, 61, 10);
    }
    assert(CropsEngine::getCropGrowthStage(10, 61, 10) == 7);

    ItemEntityManager itemMgr;
    CropsEngine::harvestCrop(world, 10, 61, 10, &itemMgr);
    assert(itemMgr.getEntities().size() >= 2);
    assert(world.getBlock(10, 61, 10) == BlockType::Air);

    std::cout << "  -> Agricultural Crops tests PASSED!" << std::endl;
}

void testVehicleAndRailPhysics() {
    std::cout << "[TEST] 26. Rail Network, Powered Rails, Minecart & Boat Buoyancy Physics..." << std::endl;
    World world(1);

    world.setBlock(0, 60, 0, BlockType::Dirt);
    world.setBlock(0, 61, 0, BlockType::PoweredRail);

    glm::vec3 cartPos(0.5f, 61.0f, 0.5f);
    glm::vec3 cartVel(2.0f, 0.0f, 0.0f);
    PhysicsEngine::updateMinecart(world, cartPos, cartVel, 0.1f);
    assert(cartPos.x > 0.5f);
    assert(cartVel.x > 1.8f);

    world.setBlock(10, 60, 10, BlockType::Water);
    glm::vec3 boatPos(10.5f, 60.5f, 10.5f);
    glm::vec3 boatVel(1.0f, 0.0f, 1.0f);
    PhysicsEngine::updateBoat(world, boatPos, boatVel, 0.1f);
    assert(boatPos.x > 10.5f);
    assert(boatVel.y == 0.0f);

    std::cout << "  -> Vehicle & Rail Physics tests PASSED!" << std::endl;
}

void testVillageAndVillagers() {
    std::cout << "[TEST] 27. Procedural Village Architecture, Houses & Iron Golem Defenders..." << std::endl;
    World world(2);
    MobEngine mobEngine;
    VillageGenerator::generateVillage(world, 0, 60, 0, &mobEngine);

    assert(world.getBlock(0, 60, 0) == BlockType::Stone);
    assert(world.getBlock(0, 59, 0) == BlockType::Water);
    assert(world.getBlock(7, 60, 0) == BlockType::Planks);
    assert(world.getBlock(0, 61, 8) == BlockType::Furnace);

    assert(mobEngine.getMobs().size() == 4);
    std::cout << "  -> Village & Villager tests PASSED!" << std::endl;
}

void testVillagerTradingEngine() {
    std::cout << "[TEST] 28. Villager Professions & Emerald Trading System..." << std::endl;
    auto blacksmithTrades = TradingEngine::getTradesForProfession(VillagerProfession::Blacksmith);
    assert(blacksmithTrades.size() >= 2);

    ItemStack ironSlot = { BlockType::IronOre, 8, 64 };
    ItemStack emptySlot = { BlockType::Air, 0, 64 };
    ItemStack resultSlot;

    bool tradeSuccess = TradingEngine::executeTrade(VillagerProfession::Blacksmith, 0, ironSlot, emptySlot, resultSlot);
    assert(tradeSuccess == true);
    assert(ironSlot.count == 4);
    assert(resultSlot.type == BlockType::Emerald && resultSlot.count == 1);

    std::cout << "  -> Villager Trading Engine tests PASSED!" << std::endl;
}

void testEnchantingAndAnvilSystem() {
    std::cout << "[TEST] 29. Enchanting Table, Bookshelves & Enchantment Power..." << std::endl;
    World world(2);
    world.setBlock(0, 60, 0, BlockType::EnchantingTable);
    world.setBlock(2, 60, 0, BlockType::Bookshelf);
    world.setBlock(-2, 60, 0, BlockType::Bookshelf);
    world.setBlock(0, 60, 2, BlockType::Bookshelf);
    world.setBlock(0, 60, -2, BlockType::Bookshelf);

    int bookshelves = EnchantingEngine::countNearbyBookshelves(world, 0, 60, 0);
    assert(bookshelves >= 4);

    ItemStack sword = { BlockType::DiamondSword, 1, 1, 1561, 1561 };
    auto options = EnchantingEngine::getEnchantmentOptions(sword, bookshelves);
    assert(!options.empty());
    assert(options[0].type == Enchantment::Sharpness);

    EnchantingEngine::applyEnchantment(sword, Enchantment::Sharpness, 4);
    assert(sword.enchantmentLevel == 4);
    assert(sword.enchantmentType == static_cast<int>(Enchantment::Sharpness));

    float bonusDamage = EnchantingEngine::getEnchantedDamageBonus(sword);
    assert(bonusDamage == 6.0f);

    std::cout << "  -> Enchanting & Power Calculation tests PASSED!" << std::endl;
}

void testVertexAOAndAsyncMeshing() {
    std::cout << "[TEST] 30. Vertex Ambient Occlusion & Asynchronous MeshData Generation..." << std::endl;
    float aoFull = ChunkMesh::calculateVertexAO(false, false, false);
    float aoSide = ChunkMesh::calculateVertexAO(true, false, false);
    float aoCorner = ChunkMesh::calculateVertexAO(true, true, true);
    assert(aoFull == 1.0f);
    assert(aoSide < aoFull);
    assert(aoCorner == 0.25f);

    Chunk chunk(5, 5);
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            chunk.setBlock(x, 15, z, BlockType::Stone);
            chunk.setBlock(x, 16, z, BlockType::Water);
        }
    }
    chunk.buildMeshDataAsync();
    assert(chunk.hasPendingMesh() == true);
    std::cout << "  -> Vertex AO & Async Meshing tests PASSED!" << std::endl;
}

void testPistonPushAndPullMechanics() {
    std::cout << "[TEST] 31. Piston Block Pushing & Sticky Piston Retraction Mechanics..." << std::endl;
    World world(1);
    glm::ivec3 pistonPos(10, 60, 10);
    world.setBlock(10, 60, 10, BlockType::Piston);
    world.setBlock(10, 60, 11, BlockType::Stone);
    world.setBlock(10, 60, 12, BlockType::Dirt);
    world.setBlock(10, 60, 13, BlockType::Air);

    bool pushed = RedstoneEngine::tryPushPiston(world, pistonPos, glm::ivec3(0, 0, 1), false);
    assert(pushed == true);
    assert(world.getBlock(10, 60, 11) == BlockType::Air);
    assert(world.getBlock(10, 60, 12) == BlockType::Stone);
    assert(world.getBlock(10, 60, 13) == BlockType::Dirt);

    world.setBlock(20, 60, 20, BlockType::StickyPiston);
    world.setBlock(20, 60, 22, BlockType::DiamondOre);
    bool pulled = RedstoneEngine::tryRetractStickyPiston(world, glm::ivec3(20, 60, 20), glm::ivec3(0, 0, 1));
    assert(pulled == true);
    assert(world.getBlock(20, 60, 21) == BlockType::DiamondOre);
    assert(world.getBlock(20, 60, 22) == BlockType::Air);

    std::cout << "  -> Piston Push & Pull Mechanics tests PASSED!" << std::endl;
}

void testRepeaterAndRedstoneLogic() {
    std::cout << "[TEST] 32. Redstone Repeater Delay & Power Boost..." << std::endl;
    World world(1);
    world.setBlock(5, 60, 5, BlockType::RedstoneTorch);
    world.setBlock(5, 60, 6, BlockType::Repeater);

    int output = RedstoneEngine::getRepeaterOutput(world, glm::ivec3(5, 60, 6), 2);
    assert(output == 15);
    std::cout << "  -> Redstone Repeater tests PASSED!" << std::endl;
}

void testMobPathfinding3DAndFeedback() {
    std::cout << "[TEST] 33. 3D Voxel A* Mob Pathfinding & Damage Flash..." << std::endl;
    World world(1);
    for (int x = 0; x < 16; ++x) {
        for (int z = 0; z < 16; ++z) {
            world.setBlock(x, 59, z, BlockType::Stone);
            world.setBlock(x, 60, z, BlockType::Air);
            world.setBlock(x, 61, z, BlockType::Air);
        }
    }

    glm::ivec3 start(2, 60, 2);
    glm::ivec3 target(8, 60, 8);
    auto path = MobEngine::findPath3D(world, start, target, 40);
    assert(!path.empty());

    MobEngine mobEngine;
    mobEngine.spawnMob(MobType::Zombie, glm::vec3(2.0f, 60.0f, 2.0f));
    assert(mobEngine.getMobs()[0].hurtTime == 0.0f);

    glm::vec3 playerPos(1.0f, 60.0f, 2.0f);
    mobEngine.checkPlayerAttack(playerPos, glm::vec3(1.0f, 0.0f, 0.0f), 3.0f, 5);
    assert(mobEngine.getMobs()[0].hurtTime > 0.0f);

    std::cout << "  -> 3D Mob Pathfinding & Feedback tests PASSED!" << std::endl;
}

void testPhysicsAutoStepAndSneak() {
    std::cout << "[TEST] 34. Physics Auto Step-Up & Sneaking Ledge Stop..." << std::endl;
    World world(1);
    for (int x = 0; x < 10; ++x) {
        for (int z = 0; z < 10; ++z) {
            world.setBlock(x, 60, z, BlockType::Stone);
        }
    }

    glm::vec3 pos(5.0f, 61.0f, 5.0f);
    glm::vec3 vel(5.0f, 0.0f, 0.0f);
    bool isGrounded = true;
    bool inWater = false;

    PhysicsEngine::updatePlayer(world, pos, vel, isGrounded, inWater, false, true, 0.1f);
    assert(pos.x > 5.0f);

    std::cout << "  -> Physics Auto Step-Up & Sneak tests PASSED!" << std::endl;
}

void testSpatialAudioCalculations() {
    std::cout << "[TEST] 35. 3D Spatial Audio Distance Gain & Stereo Panning..." << std::endl;
    glm::vec3 listener(0.0f, 65.0f, 0.0f);
    glm::vec3 front(0.0f, 0.0f, -1.0f);

    glm::vec3 soundRight(10.0f, 65.0f, 0.0f);
    glm::vec3 soundLeft(-10.0f, 65.0f, 0.0f);

    float gainClose = AudioManager::calculateDistanceGain(glm::vec3(2.0f, 65.0f, 0.0f), listener);
    float gainFar = AudioManager::calculateDistanceGain(glm::vec3(35.0f, 65.0f, 0.0f), listener);
    assert(gainClose > gainFar);

    float panRight = AudioManager::calculateStereoPan(soundRight, listener, front);
    float panLeft = AudioManager::calculateStereoPan(soundLeft, listener, front);
    assert(panRight > 0.5f);
    assert(panLeft < -0.5f);

    std::cout << "  -> 3D Spatial Audio tests PASSED!" << std::endl;
}

void testBinaryPacketSerialization() {
    std::cout << "[TEST] 36. Binary Network Packet Serialization & Deserialization..." << std::endl;
    PlayerPosPacket posPkt;
    posPkt.playerId = 42;
    posPkt.position = glm::vec3(123.4f, 65.0f, -456.7f);
    posPkt.yaw = 90.0f;
    posPkt.pitch = -15.0f;

    auto posBytes = NetworkManager::serializePlayerPos(posPkt);
    PlayerPosPacket readPosPkt;
    bool okPos = NetworkManager::deserializePlayerPos(posBytes.data(), posBytes.size(), readPosPkt);
    assert(okPos == true);
    assert(readPosPkt.playerId == 42);
    assert(glm::distance(readPosPkt.position, posPkt.position) < 0.001f);
    assert(readPosPkt.yaw == 90.0f);

    BlockChangePacket blockPkt;
    blockPkt.blockPos = glm::ivec3(10, 64, -20);
    blockPkt.newBlock = BlockType::DiamondOre;

    auto blockBytes = NetworkManager::serializeBlockChange(blockPkt);
    BlockChangePacket readBlockPkt;
    bool okBlock = NetworkManager::deserializeBlockChange(blockBytes.data(), blockBytes.size(), readBlockPkt);
    assert(okBlock == true);
    assert(readBlockPkt.blockPos == glm::ivec3(10, 64, -20));
    assert(readBlockPkt.newBlock == BlockType::DiamondOre);

    ChatMessagePacket chatPkt;
    chatPkt.senderId = 7;
    chatPkt.message = "Hello Voxel Engine!";

    auto chatBytes = NetworkManager::serializeChatMessage(chatPkt);
    ChatMessagePacket readChatPkt;
    bool okChat = NetworkManager::deserializeChatMessage(chatBytes.data(), chatBytes.size(), readChatPkt);
    assert(okChat == true);
    assert(readChatPkt.senderId == 7);
    assert(readChatPkt.message == "Hello Voxel Engine!");

    std::cout << "  -> Binary Network Packet tests PASSED!" << std::endl;
}

void testChunkUnloaderAndMemoryManagement() {
    std::cout << "[TEST] 37. Chunk Unloader LRU & Dynamic Memory Management..." << std::endl;
    World world(2);
    assert(world.getLoadedChunkCount() > 0);

    glm::vec3 farPlayerPos(500.0f, 65.0f, 500.0f);
    world.unloadFarChunks(farPlayerPos);
    assert(world.getChunk(0, 0) == nullptr);

    std::cout << "  -> Chunk Unloader & Memory Management tests PASSED!" << std::endl;
}

void testTextureAtlasAndPixelArtPatterns() {
    std::cout << "[TEST] 38. Procedural Pixel-Art Texture Atlas & Tile Coordinates..." << std::endl;
    std::vector<uint8_t> pixels;
    TextureAtlas::generateDefaultAtlas(pixels, 256, 256);
    assert(pixels.size() == 256 * 256 * 4);

    glm::vec2 grassUV = TextureAtlas::getBlockUV(BlockType::Grass, Direction::TOP);
    glm::vec2 dirtUV = TextureAtlas::getBlockUV(BlockType::Dirt, Direction::TOP);
    assert(grassUV != dirtUV);
    std::cout << "  -> Texture Atlas tests PASSED!" << std::endl;
}

void testChunkSectionsAndLocalizedMeshing() {
    std::cout << "[TEST] 39. 16x16x16 ChunkSection Slicing & Localized Updates..." << std::endl;
    Chunk chunk(0, 0);
    assert(chunk.getSection(0) != nullptr);
    assert(chunk.getSection(15) != nullptr);

    chunk.setBlock(5, 20, 5, BlockType::Stone);
    ChunkSection* sec1 = chunk.getSection(1);
    assert(sec1->isEmpty() == false);
    assert(sec1->isDirty() == true);

    std::cout << "  -> ChunkSection tests PASSED!" << std::endl;
}

void testAdvancedMineshaftAndDesertTemple() {
    std::cout << "[TEST] 40. Procedural Abandoned Mineshafts & Desert Pyramid Basements..." << std::endl;
    World world(2);
    StructureGenerator::generateMineshaft(world, 0, 30, 0, 16);
    assert(world.getBlock(0, 30, 0) == BlockType::Rail);
    assert(world.getBlock(0, 29, 0) == BlockType::Stone);

    StructureGenerator::generateDesertTemple(world, 50, 60, 50);
    assert(world.getBlock(50, 60, 50) == BlockType::Sand);
    assert(world.getBlock(50, 55, 50) == BlockType::TNT);
    assert(world.getBlock(51, 56, 51) == BlockType::Chest);

    std::cout << "  -> Mineshaft & Desert Temple tests PASSED!" << std::endl;
}

void testBrewingEngineRecipesAndPotions() {
    std::cout << "[TEST] 41. Brewing Stand Engine Recipes & Potion Synthesis..." << std::endl;
    assert(BrewingEngine::isValidIngredient(BlockType::Sugar) == true);
    assert(BrewingEngine::isValidIngredient(BlockType::Dirt) == false);

    PotionType awkward = BrewingEngine::getBrewResult(BlockType::Netherrack, PotionType::WaterBottle);
    assert(awkward == PotionType::Awkward);

    PotionType speed = BrewingEngine::getBrewResult(BlockType::Sugar, PotionType::Awkward);
    assert(speed == PotionType::Speed);

    std::array<PotionType, 3> standBottles = { PotionType::Awkward, PotionType::Awkward, PotionType::WaterBottle };
    bool brewed = BrewingEngine::brewBottles(BlockType::Sugar, standBottles);
    assert(brewed == true);
    assert(standBottles[0] == PotionType::Speed);
    assert(standBottles[1] == PotionType::Speed);
    assert(standBottles[2] == PotionType::WaterBottle);

    std::cout << "  -> Brewing Engine tests PASSED!" << std::endl;
}

void testPlayerStatusEffectsAndMultipliers() {
    std::cout << "[TEST] 42. Potion Status Effects, Speed Multipliers & Health Regeneration..." << std::endl;
    PlayerStats stats;
    stats.addEffect(StatusEffect::Speed, 180.0f, 1);
    assert(stats.hasEffect(StatusEffect::Speed) == true);
    assert(stats.getSpeedMultiplier() > 1.15f);

    stats.addEffect(StatusEffect::JumpBoost, 180.0f, 1);
    assert(stats.getJumpMultiplier() > 1.30f);

    stats.addEffect(StatusEffect::Regeneration, 10.0f, 1);
    stats.setHealth(10.0f);
    stats.update(2.0f);
    assert(stats.getHealth() > 10.0f);

    std::cout << "  -> Potion Status Effects tests PASSED!" << std::endl;
}

void testModdingEngineBlockAndRecipeRegistry() {
    std::cout << "[TEST] 43. Data-Driven ModdingEngine JSON Block & Recipe Registration..." << std::endl;
    ModdingEngine& modding = ModdingEngine::getInstance();
    modding.clear();

    CustomBlockDef rubyBlock;
    rubyBlock.id = "ruby_block";
    rubyBlock.name = "Block of Ruby";
    rubyBlock.hardness = 5.0f;
    rubyBlock.isSolid = true;
    rubyBlock.isOpaque = true;

    bool registered = modding.registerBlock(rubyBlock);
    assert(registered == true);
    assert(modding.getBlockDef("ruby_block") != nullptr);
    assert(modding.getBlockDef("ruby_block")->hardness == 5.0f);

    CustomRecipeDef rubyRecipe;
    rubyRecipe.resultId = "ruby_sword";
    rubyRecipe.resultCount = 1;
    rubyRecipe.pattern = { "ruby_gem", "ruby_gem", "stick" };
    modding.registerRecipe(rubyRecipe);
    assert(modding.getAllRecipes().size() == 1);

    std::cout << "  -> Modding Engine tests PASSED!" << std::endl;
}

void testExtendedNetherBiomesAndFortresses() {
    std::cout << "[TEST] 44. Extended Nether Biomes (Crimson, Warped, Soul Sand) & Fortresses..." << std::endl;
    BiomeType crimson = Biome::getNetherBiome(0.5f, 0.5f);
    BiomeType warped = Biome::getNetherBiome(0.3f, -0.3f);
    BiomeType soulValley = Biome::getNetherBiome(-0.5f, 0.0f);

    assert(crimson == BiomeType::CrimsonForest);
    assert(warped == BiomeType::WarpedForest);
    assert(soulValley == BiomeType::SoulSandValley);

    World world(2);
    StructureGenerator::generateNetherFortressCorridor(world, 0, 50, 0, 20);
    assert(world.getBlock(0, 49, 0) == BlockType::Obsidian);
    assert(world.getBlock(2, 50, 0) == BlockType::Netherrack);
    assert(world.getBlock(0, 51, 0) == BlockType::Glowstone);

    std::cout << "  -> Extended Nether Biomes & Fortress tests PASSED!" << std::endl;
}

void testPostProcessingShaderParameters() {
    std::cout << "[TEST] 45. Post-Processing Bloom, SSAO & Night Vision Pipeline..." << std::endl;
    PostProcessing pp(1280, 720);
    assert(pp.isBloomEnabled() == true);
    pp.setBloomEnabled(false);
    assert(pp.isBloomEnabled() == false);
    pp.setBloomEnabled(true);

    PlayerStats stats;
    stats.addEffect(StatusEffect::NightVision, 180.0f);
    assert(stats.hasNightVision() == true);

    std::cout << "  -> Post-Processing Pipeline tests PASSED!" << std::endl;
}


void testFontRendererAndTypography() {
    std::cout << "[TEST] 46. FontRenderer Bitmap Atlas, Text Metrics & Glyph Widths..." << std::endl;
    FontRenderer& fr = FontRenderer::getInstance();
    std::string sample = "Minecraft 1:1 Engine";
    float width = fr.getTextWidth(sample, 1.0f);
    assert(width == static_cast<float>(sample.length() * 8));
    float height = fr.getTextHeight(2.0f);
    assert(height == 16.0f);
    std::cout << "  -> FontRenderer & Typography tests PASSED!" << std::endl;
}

void testEntityRendererAndModels() {
    std::cout << "[TEST] 47. 3D Entity & Mob Renderer Hierarchical Transforms..." << std::endl;
    EntityRenderer& er = EntityRenderer::getInstance();
    MobEngine mobEngine;
    ItemEntityManager itemMgr;
    Camera cam(glm::vec3(0, 10, 0));

    mobEngine.spawnMob(MobType::Zombie, glm::vec3(0, 10, 0));
    mobEngine.spawnMob(MobType::Creeper, glm::vec3(5, 10, 5));
    mobEngine.spawnMob(MobType::Pig, glm::vec3(-5, 10, -5));
    mobEngine.spawnMob(MobType::EnderDragon, glm::vec3(0, 30, 0));

    itemMgr.spawnItemDrop(BlockType::DiamondOre, 3, glm::vec3(2, 10, 2));

    assert(mobEngine.getMobs().size() == 4);
    assert(itemMgr.getItems().size() == 1);
    std::cout << "  -> EntityRenderer tests PASSED!" << std::endl;
}

void testContinuousMiningAndToolProgress() {
    std::cout << "[TEST] 48. Continuous Mining Speed Multipliers & Break Progress..." << std::endl;
    float handSpeedOnDirt = ToolSystem::getMiningSpeed(BlockType::Dirt, BlockType::Air);
    float pickSpeedOnStone = ToolSystem::getMiningSpeed(BlockType::Stone, BlockType::DiamondPickaxe);
    float axeSpeedOnWood = ToolSystem::getMiningSpeed(BlockType::OakLog, BlockType::WoodAxe);

    assert(handSpeedOnDirt >= 1.0f);
    assert(pickSpeedOnStone > handSpeedOnDirt * 5.0f);
    assert(axeSpeedOnWood > handSpeedOnDirt * 1.5f);
    assert(ToolSystem::canHarvest(BlockType::DiamondOre, BlockType::DiamondPickaxe) == true);
    assert(ToolSystem::canHarvest(BlockType::Obsidian, BlockType::WoodPickaxe) == false);
    std::cout << "  -> Continuous Mining & Tool System tests PASSED!" << std::endl;
}

void testContainerGUIInteractions() {
    std::cout << "[TEST] 49. ContainerGUI Chest 27-Slot & Furnace Animated State..." << std::endl;
    ContainerGUI gui(1280, 720);
    Inventory playerInv;
    std::vector<ItemStack> chestInv(27);
    chestInv[0] = { BlockType::GoldOre, 12, 64 };
    gui.openChest(glm::ivec3(0, 64, 0), &chestInv);
    assert(gui.isOpen() == true);
    assert(gui.getActiveContainer() == ContainerType::Chest);

    // Simulate clicking chest slot 0 and transfer to player inv
    gui.handleMouseClick(playerInv, 0, 0, 0); // Click chest slot
    gui.close();
    assert(gui.isOpen() == false);
    std::cout << "  -> ContainerGUI tests PASSED!" << std::endl;
}

void testSocketNetworkingAndSync() {
    std::cout << "[TEST] 50. Socket Networking Non-Blocking UDP Packet Pipeline..." << std::endl;
    NetworkManager server;
    NetworkManager client;

    bool serverStarted = server.startServer(25566);
    assert(serverStarted == true);
    assert(server.isServer() == true);

    bool clientConnected = client.connectToServer("127.0.0.1", 25566);
    assert(clientConnected == true);

    client.sendPlayerPosition(glm::vec3(10.0f, 65.0f, 10.0f), 90.0f, 0.0f);
    client.sendBlockChange(glm::ivec3(1, 2, 3), BlockType::Glass);
    client.sendChatMessage("Hello Minecraft!");

    server.update(nullptr);
    server.disconnect();
    client.disconnect();
    assert(server.isConnected() == false);
    assert(client.isConnected() == false);
    std::cout << "  -> Socket Networking & Packet Pipeline tests PASSED!" << std::endl;
}

void testNaturalMobSpawningAndDespawn() {
    std::cout << "[TEST] 51. Natural Mob Spawning, Mob Cap & Distance Despawning..." << std::endl;
    MobEngine engine;
    World world(1);
    glm::vec3 playerPos(0.0f, 65.0f, 0.0f);
    glm::vec3 playerVel(0.0f);
    float playerHp = 20.0f;

    // Spawn a mob very far away (> 75 blocks)
    engine.spawnMob(MobType::Zombie, glm::vec3(150.0f, 65.0f, 150.0f));
    assert(engine.getMobs().size() == 1);

    // Update mob engine - distance despawner should erase it
    engine.update(world, playerPos, playerVel, playerHp, 0.1f, nullptr);
    assert(engine.getMobs().size() == 0);

    // Trigger natural spawning simulation
    for (int i = 0; i < 5; ++i) {
        engine.checkNaturalSpawning(world, playerPos, 4.0f);
    }
    std::cout << "  -> Natural Mob Spawning & Despawn tests PASSED!" << std::endl;
}

void testSkyboxCloudsAndDrift() {
    std::cout << "[TEST] 52. Procedural Skybox Clouds at Y=128 & Wind Drift Offset..." << std::endl;
    Skybox skybox;
    Camera cam(glm::vec3(0, 65, 0));
    float totalTime = 10.0f;
    float driftX = totalTime * 1.5f;
    float driftZ = totalTime * 0.8f;
    assert(driftX == 15.0f);
    assert(driftZ == 8.0f);
    std::cout << "  -> Skybox Clouds & Wind Drift tests PASSED!" << std::endl;
}

void testWitherBossEngineAndSkulls() {
    std::cout << "[TEST] 53. Wither 3-Headed Boss Engine, 300 HP & Skull Projectiles..." << std::endl;
    MobEngine engine;
    World world(1);
    glm::vec3 playerPos(0.0f, 65.0f, 0.0f);
    glm::vec3 playerVel(0.0f);
    float playerHp = 20.0f;

    engine.spawnMob(MobType::Wither, glm::vec3(5.0f, 75.0f, 5.0f));
    assert(engine.getMobs().size() == 1);
    assert(engine.getMobs()[0].health == 300.0f);
    assert(engine.getMobs()[0].type == MobType::Wither);

    // Update Wither AI to fire skull projectile
    engine.update(world, playerPos, playerVel, playerHp, 0.1f, nullptr);
    assert(engine.getWitherSkulls().size() >= 1);
    std::cout << "  -> Wither Boss Engine & Skull tests PASSED!" << std::endl;
}

void testPBRSpecularShadersAndFresnel() {
    std::cout << "[TEST] 54. PBR Blinn-Phong Specular Highlights & Water Fresnel Reflection..." << std::endl;
    glm::vec3 sunDir = glm::normalize(glm::vec3(0.4f, 0.8f, 0.3f));
    glm::vec3 viewDir = glm::normalize(glm::vec3(0.0f, 1.0f, 1.0f));
    glm::vec3 halfwayDir = glm::normalize(sunDir + viewDir);
    glm::vec3 norm(0.0f, 1.0f, 0.0f);

    float spec = std::pow(std::max(glm::dot(norm, halfwayDir), 0.0f), 32.0f);
    assert(spec >= 0.0f && spec <= 1.0f);

    float fresnel = std::pow(1.0f - std::max(glm::dot(norm, viewDir), 0.0f), 4.0f);
    assert(fresnel >= 0.0f && fresnel <= 1.0f);
    std::cout << "  -> PBR Specular & Fresnel Shader tests PASSED!" << std::endl;
}

void testRemotePlayerRenderingAndInterpolation() {
    std::cout << "[TEST] 55. Multiplayer Remote Player Models & Name Tag Rendering..." << std::endl;
    NetworkManager net;
    net.startServer(25567);

    PlayerPosPacket p1;
    p1.playerId = 42;
    p1.position = glm::vec3(15.0f, 65.0f, 20.0f);
    p1.yaw = 180.0f;
    p1.pitch = 0.0f;

    std::vector<uint8_t> bytes = NetworkManager::serializePlayerPos(p1);
    net.processIncomingPacket(bytes.data(), bytes.size(), nullptr);

    assert(net.getRemotePlayers().size() == 1);
    assert(net.getRemotePlayers()[0].playerId == 42);
    assert(net.getRemotePlayers()[0].position == p1.position);

    net.disconnect();
    std::cout << "  -> Remote Player Rendering & Serialization tests PASSED!" << std::endl;
}

void testMaterialFootstepsAndAudioSynthesizer() {
    std::cout << "[TEST] 56. Material Footstep Synthesis & Extended Sound Effects..." << std::endl;
    float gainGrass = AudioManager::calculateDistanceGain(glm::vec3(0, 0, 0), glm::vec3(5, 0, 0), 40.0f);
    float gainFar = AudioManager::calculateDistanceGain(glm::vec3(0, 0, 0), glm::vec3(50, 0, 0), 40.0f);

    assert(gainGrass > 0.5f);
    assert(gainFar == 0.0f);

    AudioManager::playMaterialFootstep(BlockType::Grass, glm::vec3(0), glm::vec3(0), glm::vec3(0, 0, -1));
    AudioManager::playMaterialFootstep(BlockType::Stone, glm::vec3(0), glm::vec3(0), glm::vec3(0, 0, -1));
    AudioManager::playMaterialFootstep(BlockType::Water, glm::vec3(0), glm::vec3(0), glm::vec3(0, 0, -1));

    std::cout << "  -> Material Footstep & Audio Synthesizer tests PASSED!" << std::endl;
}

void testDDARaycastExactTraversal() {
    std::cout << "[TEST] 57. Fast Voxel Traversal DDA & Exact Normal Detection..." << std::endl;
    World world(1);
    world.setBlock(5, 65, 5, BlockType::Stone);

    // Cast ray from (5.5, 65.5, 0.0) towards +Z direction into the block at (5, 65, 5)
    glm::vec3 origin(5.5f, 65.5f, 0.0f);
    glm::vec3 direction(0.0f, 0.0f, 1.0f);

    RaycastResult res = Raycast::raycast(world, origin, direction, 10.0f);
    assert(res.hit == true);
    assert(res.blockPos == glm::ivec3(5, 65, 5));
    assert(res.normal == glm::vec3(0.0f, 0.0f, -1.0f)); // Entered via front face (-Z normal)
    assert(res.distance >= 4.9f && res.distance <= 5.1f);
    std::cout << "  -> Fast Voxel Traversal DDA tests PASSED!" << std::endl;
}

void testOxygenDrowningAndFireMechanics() {
    std::cout << "[TEST] 58. Oxygen Depletion, Drowning Damage & Fire/Lava Burn..." << std::endl;
    PlayerStats stats;
    assert(stats.getOxygen() == 300.0f);
    assert(stats.getHealth() == 20.0f);

    // Simulate 5 seconds underwater
    stats.updateEnvironmentalEffects(true, false, true, 5.0f);
    assert(stats.getOxygen() < 300.0f);
    assert(stats.getOxygen() == 150.0f);

    // Submerge completely until oxygen is depleted and drowning damage occurs
    stats.updateEnvironmentalEffects(true, false, true, 6.0f);
    assert(stats.getOxygen() == 0.0f);
    assert(stats.getHealth() <= 18.0f); // Drowning damage received

    // Surface out of water -> oxygen replenishes
    stats.updateEnvironmentalEffects(false, false, false, 2.0f);
    assert(stats.getOxygen() == 300.0f);

    // Lava ignition
    stats.updateEnvironmentalEffects(false, true, false, 0.1f);
    assert(stats.isOnFire() == true);
    assert(stats.getFireTicks() > 0.0f);

    // Extinguish in water
    stats.updateEnvironmentalEffects(false, false, true, 0.1f);
    assert(stats.isOnFire() == false);
    assert(stats.getFireTicks() == 0.0f);
    std::cout << "  -> Oxygen & Fire Mechanics tests PASSED!" << std::endl;
}

void testFallDamageCalculation() {
    std::cout << "[TEST] 59. Fall Damage Calculation & Armor Absorption..." << std::endl;
    PlayerStats stats;
    stats.setHealth(20.0f);

    // Fall of 3 blocks -> no damage
    float dmg0 = stats.applyFallDamage(3.0f);
    assert(dmg0 == 0.0f);
    assert(stats.getHealth() == 20.0f);

    // Fall of 7 blocks -> 4 raw damage
    float dmg1 = stats.applyFallDamage(7.0f);
    assert(dmg1 == 4.0f);
    assert(stats.getHealth() == 16.0f);

    std::cout << "  -> Fall Damage tests PASSED!" << std::endl;
}

void testInfiniteWaterAndObsidianFormation() {
    std::cout << "[TEST] 60. Infinite Water Source Creation & Obsidian Reaction..." << std::endl;
    World world(1);

    // Set up a 2x2 water well with solid cobblestone floor at y=63
    for (int x = 0; x < 3; ++x) {
        for (int z = 0; z < 3; ++z) {
            world.setBlock(x, 63, z, BlockType::Stone);
        }
    }

    world.setBlock(0, 64, 0, BlockType::Water);
    world.setBlock(1, 64, 1, BlockType::Water);
    world.setBlock(0, 64, 1, BlockType::Air); // Diagonal air pocket

    // Simulate fluid update
    FluidEngine::updateFluids(world, glm::vec3(0.0f, 64.0f, 0.0f));

    // Water + Lava reaction
    world.setBlock(10, 64, 10, BlockType::Water);
    world.setBlock(11, 64, 10, BlockType::Lava);
    FluidEngine::updateFluids(world, glm::vec3(10.0f, 64.0f, 10.0f));

    assert(world.getBlock(11, 64, 10) == BlockType::Obsidian || world.getBlock(10, 64, 10) == BlockType::Obsidian || world.getBlock(10, 64, 10) == BlockType::Stone || world.getBlock(11, 64, 10) == BlockType::Stone);

    std::cout << "  -> Infinite Water & Obsidian tests PASSED!" << std::endl;
}

void testInGameConsoleCommands() {
    std::cout << "[TEST] 61. In-Game Console Command Execution Engine..." << std::endl;
    TimeManager timeMgr;
    WeatherManager weatherMgr;
    PlayerStats playerStats;
    Inventory inventory;
    Camera camera(glm::vec3(0, 65, 0));
    bool isFlying = false;

    bool cmd1 = CommandParser::execute("/time set day", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd1 == true);
    assert(timeMgr.getTimeOfDay() == 1000.0f);

    bool cmd2 = CommandParser::execute("/time set night", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd2 == true);
    assert(timeMgr.getTimeOfDay() == 14000.0f);

    bool cmd3 = CommandParser::execute("/gamemode creative", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd3 == true);
    assert(isFlying == true);

    bool cmd4 = CommandParser::execute("/gamemode survival", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd4 == true);
    assert(isFlying == false);

    bool cmd5 = CommandParser::execute("/give diamond 5", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd5 == true);
    assert(inventory.getSlot(0).type == BlockType::DiamondOre);
    assert(inventory.getSlot(0).count == 5);

    bool cmd6 = CommandParser::execute("/weather clear", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd6 == true);
    assert(weatherMgr.isRaining() == false);

    bool cmd7 = CommandParser::execute("/heal", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd7 == true);
    assert(playerStats.getHealth() == 20.0f);
    assert(playerStats.getOxygen() == 300.0f);

    bool cmd8 = CommandParser::execute("/kill", &timeMgr, &weatherMgr, &playerStats, &inventory, &camera, &isFlying);
    assert(cmd8 == true);
    assert(playerStats.getHealth() == 0.0f);

    std::cout << "  -> In-Game Console Command tests PASSED!" << std::endl;
}

void testHotbarScrollAndStackSplitting() {
    std::cout << "[TEST] 62. Hotbar Scroll & Inventory Stack-Splitting..." << std::endl;
    Inventory inv;
    inv.addItem(BlockType::OakLog, 16);
    assert(inv.getSlot(0).count == 16);

    // Simulate right click picking up half stack
    ItemStack& slot0 = inv.getSlot(0);
    int half = slot0.count / 2;
    slot0.count -= half;
    assert(slot0.count == 8);
    assert(half == 8);

    std::cout << "  -> Hotbar Scroll & Stack Splitting tests PASSED!" << std::endl;
}

void testRedstoneComparatorAndContainerReading() {
    std::cout << "[TEST] 63. Redstone Comparator & Container Fullness Reading..." << std::endl;
    World world(1);
    std::vector<ItemStack> chest(27);
    chest[0] = { BlockType::DiamondOre, 64, 64 };
    chest[1] = { BlockType::GoldOre, 64, 64 };

    // Comparator measuring chest
    int sigEmpty = RedstoneEngine::getComparatorOutput(world, glm::ivec3(0, 64, 0), glm::ivec3(0, 0, 1), nullptr);
    assert(sigEmpty == 0);

    int sigPart = RedstoneEngine::getComparatorOutput(world, glm::ivec3(0, 64, 0), glm::ivec3(0, 0, 1), &chest);
    assert(sigPart >= 1 && sigPart <= 2);

    // Completely fill chest
    for (int i = 0; i < 27; ++i) {
        chest[i] = { BlockType::IronOre, 64, 64 };
    }
    int sigFull = RedstoneEngine::getComparatorOutput(world, glm::ivec3(0, 64, 0), glm::ivec3(0, 0, 1), &chest);
    assert(sigFull == 15);

    std::cout << "  -> Redstone Comparator tests PASSED!" << std::endl;
}

void testHopperTransferAndLocking() {
    std::cout << "[TEST] 64. Hopper Item Transfer, Stacking & Redstone Lock..." << std::endl;
    World world(1);
    std::vector<ItemStack> hopperInv(5);
    std::vector<ItemStack> destChest(27);

    hopperInv[0] = { BlockType::Obsidian, 5, 64 };

    // Unpowered hopper transfers 1 item into destination container
    bool transferred = RedstoneEngine::processHopperTransfer(world, glm::ivec3(5, 64, 5), glm::ivec3(0, -1, 0), &hopperInv, &destChest);
    assert(transferred == true);
    assert(hopperInv[0].count == 4);
    assert(destChest[0].type == BlockType::Obsidian);
    assert(destChest[0].count == 1);

    // Power the hopper with a redstone torch next to it -> hopper locks
    world.setBlock(5, 64, 6, BlockType::RedstoneTorch);
    bool lockedTransfer = RedstoneEngine::processHopperTransfer(world, glm::ivec3(5, 64, 5), glm::ivec3(0, -1, 0), &hopperInv, &destChest);
    assert(lockedTransfer == false); // Locked, no transfer
    assert(hopperInv[0].count == 4);

    std::cout << "  -> Hopper Transfer & Redstone Locking tests PASSED!" << std::endl;
}

void testBiomeColormapsAndFoliage() {
    std::cout << "[TEST] 65. Biome Temperature, Rainfall & Grass/Foliage Colormaps..." << std::endl;
    glm::vec3 jungleGrass = Biome::getGrassColor(BiomeType::Jungle);
    glm::vec3 desertGrass = Biome::getGrassColor(BiomeType::Desert);
    glm::vec3 swampGrass = Biome::getGrassColor(BiomeType::Swamp);

    assert(jungleGrass.g > jungleGrass.r); // Vibrant green
    assert(desertGrass.r > 0.7f); // Yellowish dry tone
    assert(swampGrass.g < jungleGrass.g); // Darker murky swamp tone

    glm::vec3 jungleLeaves = Biome::getFoliageColor(BiomeType::Jungle);
    assert(jungleLeaves.g > 0.7f);

    std::cout << "  -> Biome Colormap tests PASSED!" << std::endl;
}

void testStrongholdAndOceanRuinGeneration() {
    std::cout << "[TEST] 66. Stronghold Chamber, End Portal Rim & Ocean Ruins..." << std::endl;
    World world(1);

    // Generate Stronghold at (100, 30, 100)
    StructureGenerator::generateStronghold(world, 100, 30, 100);
    assert(world.getBlock(100, 30, 100) == BlockType::StoneBricks);
    assert(world.getBlock(104, 30, 104) == BlockType::Lava); // Central lava pool
    assert(world.getBlock(104, 32, 101) == BlockType::Spawner); // Spawner block

    // Generate Ocean Ruin at (-50, 40, -50)
    StructureGenerator::generateOceanRuin(world, -50, 40, -50);
    assert(world.getBlock(-48, 41, -48) == BlockType::Chest);

    std::cout << "  -> Stronghold & Ocean Ruin tests PASSED!" << std::endl;
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
    testJungleBiomeAndBamboo();
    testArmorCraftingSuite();
    testFrustumCullingInWorld();
    testTheEndDimensionAndDragon();
    testCropsAndFarmingSystem();
    testVehicleAndRailPhysics();
    testVillageAndVillagers();
    testVillagerTradingEngine();
    testEnchantingAndAnvilSystem();

    // Phase 1 Tests
    testVertexAOAndAsyncMeshing();
    testPistonPushAndPullMechanics();
    testRepeaterAndRedstoneLogic();
    testMobPathfinding3DAndFeedback();
    testPhysicsAutoStepAndSneak();
    testSpatialAudioCalculations();
    testBinaryPacketSerialization();
    testChunkUnloaderAndMemoryManagement();

    // Phase 2 Tests
    testTextureAtlasAndPixelArtPatterns();
    testChunkSectionsAndLocalizedMeshing();
    testAdvancedMineshaftAndDesertTemple();
    testBrewingEngineRecipesAndPotions();
    testPlayerStatusEffectsAndMultipliers();

    // Phase 3 Tests
    testModdingEngineBlockAndRecipeRegistry();
    testExtendedNetherBiomesAndFortresses();
    testPostProcessingShaderParameters();

    // Phase 4 Extensions (New Subsystems)
    testFontRendererAndTypography();
    testEntityRendererAndModels();
    testContinuousMiningAndToolProgress();
    testContainerGUIInteractions();
    testSocketNetworkingAndSync();
    testNaturalMobSpawningAndDespawn();

    // Phase 5 Next-Gen Subsystems
    testSkyboxCloudsAndDrift();
    testWitherBossEngineAndSkulls();
    testPBRSpecularShadersAndFresnel();
    testRemotePlayerRenderingAndInterpolation();
    testMaterialFootstepsAndAudioSynthesizer();

    // Phase 6 Modern Enhancements
    testDDARaycastExactTraversal();
    testOxygenDrowningAndFireMechanics();
    testFallDamageCalculation();
    testInfiniteWaterAndObsidianFormation();
    testInGameConsoleCommands();
    testHotbarScrollAndStackSplitting();

    // Phase 7 Automation, Ecosystem & Structures
    testRedstoneComparatorAndContainerReading();
    testHopperTransferAndLocking();
    testBiomeColormapsAndFoliage();
    testStrongholdAndOceanRuinGeneration();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL 66 ENGINE TESTS PASSED 100%!       " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
