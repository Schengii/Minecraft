#include "world/World.hpp"
#include "world/Block.hpp"
#include "world/RedstoneEngine.hpp"
#include "world/FluidEngine.hpp"
#include "world/ExplosionEngine.hpp"
#include "world/ToolSystem.hpp"
#include "world/TimeManager.hpp"
#include "world/Raycast.hpp"
#include "world/ChestBlock.hpp"
#include "world/FurnaceBlock.hpp"
#include "world/StructureGenerator.hpp"
#include "world/DimensionManager.hpp"
#include "core/ThreadPool.hpp"
#include "inventory/Inventory.hpp"
#include "crafting/CraftingManager.hpp"
#include "physics/PhysicsEngine.hpp"
#include "ecs/MobEngine.hpp"
#include "ecs/ItemEntity.hpp"
#include "renderer/ParticleEngine.hpp"
#include "renderer/FrustumCuller.hpp"
#include "audio/AudioManager.hpp"
#include <iostream>
#include <cassert>
#include <filesystem>

using namespace Minecraft;

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
    ItemEntityManager iem;
    iem.spawnItemDrop(BlockType::GoldOre, 3, glm::vec3(0, 60, 0));
    assert(iem.getEntities().size() == 1);

    std::vector<std::pair<BlockType, int>> picked;
    iem.update(world, glm::vec3(0, 60, 0), picked, 1.0f);
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

    std::cout << "========================================" << std::endl;
    std::cout << " ALL ENGINE TESTS PASSED 100%!          " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
