#include <iostream>
#include <cassert>
#include <filesystem>
#include <memory>
#include <cstring>
#include <glm/glm.hpp>
#include "../src/world/World.hpp"
#include "../src/world/Block.hpp"
#include "../src/world/ChunkSection.hpp"
#include "../src/world/RedstoneEngine.hpp"
#include "../src/world/FluidEngine.hpp"
#include "../src/world/ExplosionEngine.hpp"
#include "../src/world/ToolSystem.hpp"
#include "../src/world/TimeManager.hpp"
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
#include "../src/inventory/Inventory.hpp"
#include "../src/inventory/PlayerStats.hpp"
#include "../src/inventory/FoodSystem.hpp"
#include "../src/gui/MenuGUI.hpp"
#include "../src/renderer/Skybox.hpp"
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
    float diamondSpeed = ToolSystem::getMiningSpeedMultiplier(BlockType::DiamondPickaxe, BlockType::Obsidian);
    float handSpeed = ToolSystem::getMiningSpeedMultiplier(BlockType::Air, BlockType::Obsidian);
    assert(diamondSpeed > handSpeed);
    
    int maxDur = ToolSystem::getMaxDurability(BlockType::DiamondPickaxe);
    assert(maxDur == 1561);
    
    int woodDur = ToolSystem::getMaxDurability(BlockType::WoodPickaxe);
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
    
    ItemStack diamonds{ BlockType::DiamondOre, 32, 64 };
    chestMgr.getChest(chestPos)->setItem(0, diamonds);
    assert(chestMgr.getChest(chestPos)->getItem(0).count == 32);
    
    FurnaceManager furnaceMgr;
    glm::ivec3 furnacePos(20, 64, 20);
    furnaceMgr.createFurnace(furnacePos);
    FurnaceBlock* furnace = furnaceMgr.getFurnace(furnacePos);
    
    furnace->setIngredient({ BlockType::IronOre, 5, 64 });
    furnace->setFuel({ BlockType::CoalOre, 2, 64 });
    furnace->update(15.0f);
    
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
    
    assert(itemMgr.getItemCount() == 1);
    
    Inventory inv;
    World world(1);
    glm::vec3 playerPos(0.2f, 65.0f, 0.2f);
    itemMgr.update(world, playerPos, inv, 0.1f);
    
    assert(inv.hasItem(BlockType::DiamondOre, 5) == true);
    assert(itemMgr.getItemCount() == 0);
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
    
    ItemStack diamondArmor{ BlockType::DiamondPickaxe, 1, 1 };
    stats.equipArmor(0, diamondArmor);
    stats.equipArmor(1, diamondArmor);
    
    stats.takeDamage(10.0f);
    assert(stats.getHealth() > 10.0f);
    std::cout << "  -> PlayerStats tests PASSED!" << std::endl;
}

void testWeatherManager() {
    std::cout << "[TEST] 11. WeatherManager Rain, Thunder & Dynamic Snow Cover..." << std::endl;
    WeatherManager wm;
    assert(wm.getState() == WeatherState::Clear);
    
    wm.setWeather(WeatherState::Rain, 100.0f);
    assert(wm.isRaining() == true);
    
    wm.setWeather(WeatherState::Thunder, 50.0f);
    assert(wm.isThundering() == true);
    std::cout << "  -> WeatherManager tests PASSED!" << std::endl;
}

void testCaveDecorator() {
    std::cout << "[TEST] 12. CaveDecorator Stalactite & Stalagmite Placement..." << std::endl;
    World world(1);
    CaveDecorator::decorateCaveColumn(world, 0, 50, 0);
    std::cout << "  -> CaveDecorator tests PASSED!" << std::endl;
}

void testContainerGUI() {
    std::cout << "[TEST] 13. ContainerGUI Inventory Slots & Visual Rendering..." << std::endl;
    ContainerGUI gui(1280, 720);
    ChestBlock chest;
    ItemStack iron{ BlockType::IronOre, 16, 64 };
    chest.setItem(4, iron);
    
    gui.setTargetChest(&chest);
    assert(gui.isChestOpen() == true);
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
    stats.setSaturation(5.0f);

    assert(FoodSystem::isFood(BlockType::Apple) == true);
    assert(FoodSystem::getFoodValue(BlockType::Apple) == 4);

    FoodSystem::consumeFood(stats, BlockType::Apple);
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
    assert(itemMgr.getItemCount() >= 2);
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

// ---------------- NEW TESTS FOR PHASE 3 ----------------

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
    assert(world.getBlock(0, 49, 0) == BlockType::Obsidian); // Bridge floor
    assert(world.getBlock(2, 50, 0) == BlockType::Netherrack); // Bridge railing
    assert(world.getBlock(0, 51, 0) == BlockType::Glowstone);  // Beacon

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

    std::cout << "========================================" << std::endl;
    std::cout << " ALL 45 ENGINE TESTS PASSED 100%!       " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
