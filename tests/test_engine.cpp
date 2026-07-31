#include "world/World.hpp"
#include "world/Block.hpp"
#include "world/RedstoneEngine.hpp"
#include "world/FluidEngine.hpp"
#include "world/ExplosionEngine.hpp"
#include "world/ToolSystem.hpp"
#include "world/TimeManager.hpp"
#include "world/Raycast.hpp"
#include "inventory/Inventory.hpp"
#include "crafting/CraftingManager.hpp"
#include "physics/PhysicsEngine.hpp"
#include "ecs/MobEngine.hpp"
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
    
    // Test Water + Lava contact -> Stone (with solid ground underneath)
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
    assert(playerVel.z > 0.0f); // Pushed away from explosion center
    std::cout << "  -> ExplosionEngine tests PASSED!" << std::endl;
}

void testCrafting3x3() {
    std::cout << "[TEST] 5. 3x3 Crafting Recipes (Pickaxes, Chest, TNT)..." << std::endl;
    std::array<ItemStack, 9> grid3x3;
    
    // Test 3x3 Pickaxe Recipe
    grid3x3[0] = { BlockType::Planks, 1, 64 };
    grid3x3[1] = { BlockType::Planks, 1, 64 };
    grid3x3[2] = { BlockType::Planks, 1, 64 };
    grid3x3[4] = { BlockType::Stick, 1, 64 };
    grid3x3[7] = { BlockType::Stick, 1, 64 };
    
    ItemStack output = CraftingManager::matchRecipe3x3(grid3x3);
    assert(output.type == BlockType::WoodPickaxe);
    
    // Test 3x3 Chest Recipe
    std::array<ItemStack, 9> chestGrid;
    int chestIndices[8] = { 0, 1, 2, 3, 5, 6, 7, 8 };
    for (int idx : chestIndices) chestGrid[idx] = { BlockType::Planks, 1, 64 };
    
    ItemStack chestOutput = CraftingManager::matchRecipe3x3(chestGrid);
    assert(chestOutput.type == BlockType::Chest);
    
    std::cout << "  -> 3x3 Crafting tests PASSED!" << std::endl;
}

void testMobEngine() {
    std::filesystem::remove_all("world_saves");
    std::cout << "[TEST] 6. MobEngine AI Navigation, Pathfinding & Combat..." << std::endl;
    World world(1);
    MobEngine mobEngine;
    
    mobEngine.spawnMob(MobType::Zombie, glm::vec3(10.0f, 60.0f, 10.0f));
    assert(mobEngine.getMobs().size() == 1);
    
    // Update mob towards player at (2.0f, 60.0f, 10.0f)
    mobEngine.update(world, glm::vec3(2.0f, 60.0f, 10.0f), 0.1f);
    assert(mobEngine.getMobs()[0].velocity.x < 0.0f); // Moving left towards player
    
    // Test Combat attack
    glm::vec3 playerPos(10.0f, 60.0f, 10.0f);
    bool hit = mobEngine.checkPlayerAttack(playerPos, glm::vec3(-1.0f, 0.0f, 0.0f), 5.0f, 7);
    assert(hit == true);
    
    std::cout << "  -> MobEngine tests PASSED!" << std::endl;
}

void testParticleAndFrustum() {
    std::cout << "[TEST] 7. ParticleEngine & FrustumCuller..." << std::endl;
    ParticleEngine pe;
    pe.spawnBlockBreak(glm::vec3(0, 60, 0));
    assert(pe.getParticles().size() > 0);
    pe.update(0.1f);
    
    FrustumCuller fc;
    glm::mat4 identity(1.0f);
    fc.update(identity);
    bool visible = fc.isBoxVisible(glm::vec3(-1, -1, -1), glm::vec3(1, 1, 1));
    assert(visible == true);
    
    std::cout << "  -> ParticleEngine & FrustumCuller tests PASSED!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Running Minecraft Tier-2 Engine Tests " << std::endl;
    std::cout << "========================================" << std::endl;

    std::filesystem::remove_all("world_saves");

    testRedstone();
    testFluids();
    testToolSystem();
    testExplosionEngine();
    testCrafting3x3();
    testMobEngine();
    testParticleAndFrustum();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL TIER-2 ENGINE TESTS PASSED 100%!   " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
