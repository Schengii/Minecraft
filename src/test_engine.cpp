#include "world/World.hpp"
#include "world/Block.hpp"
#include "world/RedstoneEngine.hpp"
#include "world/FluidEngine.hpp"
#include "world/TimeManager.hpp"
#include "world/Raycast.hpp"
#include "inventory/Inventory.hpp"
#include "crafting/CraftingManager.hpp"
#include "physics/PhysicsEngine.hpp"
#include "audio/AudioManager.hpp"
#include <iostream>
#include <cassert>

using namespace Minecraft;

void testRedstone() {
    std::cout << "[TEST] 1. RedstoneEngine Signal Network & Logic..." << std::endl;
    World world(1);
    
    // Place a Lever at (10, 60, 10)
    world.setBlock(10, 60, 10, BlockType::Lever);
    
    // Check if adjacent block is powered
    bool powered = RedstoneEngine::isPowered(world, glm::ivec3(10, 60, 11));
    std::cout << "  - Powered check at (10,60,11): " << (powered ? "TRUE" : "FALSE") << std::endl;
    assert(powered == true);
    
    int strength = RedstoneEngine::getSignalStrength(world, glm::ivec3(10, 60, 10));
    std::cout << "  - Signal strength at lever (10,60,10): " << strength << std::endl;
    assert(strength == 15);
    
    RedstoneEngine::updateRedstoneNetwork(world, glm::ivec3(10, 60, 10));
    std::cout << "  -> RedstoneEngine tests PASSED!" << std::endl;
}

void testFluids() {
    std::cout << "[TEST] 2. FluidEngine Water/Lava Cellular Automaton Spreading..." << std::endl;
    World world(1);
    
    world.setBlock(0, 70, 0, BlockType::Water);
    world.setBlock(0, 69, 0, BlockType::Air);
    
    FluidEngine::updateFluids(world, glm::vec3(0, 70, 0));
    BlockType belowType = world.getBlock(0, 69, 0);
    std::cout << "  - Fluid flow down at (0,69,0): " << (belowType == BlockType::Water ? "WATER" : "NOT WATER") << std::endl;
    assert(belowType == BlockType::Water);
    
    // Test Water + Lava contact -> Stone
    world.setBlock(5, 70, 5, BlockType::Water);
    world.setBlock(6, 70, 5, BlockType::Lava);
    FluidEngine::updateFluids(world, glm::vec3(5, 70, 5));
    BlockType reactType = world.getBlock(6, 70, 5);
    std::cout << "  - Water + Lava reaction at (6,70,5): " << (reactType == BlockType::Stone ? "STONE" : "OTHER") << std::endl;
    assert(reactType == BlockType::Stone || world.getBlock(5, 70, 5) == BlockType::Stone);
    
    std::cout << "  -> FluidEngine tests PASSED!" << std::endl;
}

void testCraftingAndInventory() {
    std::cout << "[TEST] 3. Inventory & 2x2 Crafting Recipes..." << std::endl;
    Inventory inv;
    
    // Recipe 1: 1 OakLog -> 4 Planks
    inv.getCraftingInput(0) = { BlockType::OakLog, 1, 64 };
    inv.updateCraftingRecipe();
    assert(inv.getCraftingOutput().type == BlockType::Planks);
    assert(inv.getCraftingOutput().count == 4);
    
    // Recipe 2: 4 Planks -> CraftingTable
    inv.getCraftingInput(0) = { BlockType::Planks, 1, 64 };
    inv.getCraftingInput(1) = { BlockType::Planks, 1, 64 };
    inv.getCraftingInput(2) = { BlockType::Planks, 1, 64 };
    inv.getCraftingInput(3) = { BlockType::Planks, 1, 64 };
    inv.updateCraftingRecipe();
    assert(inv.getCraftingOutput().type == BlockType::CraftingTable);
    assert(inv.getCraftingOutput().count == 1);
    
    std::cout << "  -> Inventory & Crafting tests PASSED!" << std::endl;
}

void testPhysicsAndRaycast() {
    std::cout << "[TEST] 4. PhysicsEngine & DDA Raycasting..." << std::endl;
    World world(1);
    world.setBlock(0, 60, 5, BlockType::Stone);
    
    // Raycast towards (0, 60, 5)
    RaycastResult hit = Raycast::raycast(world, glm::vec3(0, 60, 0), glm::vec3(0, 0, 1), 10.0f);
    assert(hit.hit == true);
    assert(hit.blockPos == glm::ivec3(0, 60, 5));
    
    // Physics update sneaking edge check
    glm::vec3 pos(0.5f, 61.0f, 0.5f);
    glm::vec3 vel(5.0f, 0.0f, 0.0f);
    bool isGrounded = true;
    bool inWater = false;
    PhysicsEngine::updatePlayer(world, pos, vel, isGrounded, inWater, false, true, 0.016f);
    
    std::cout << "  -> Physics & Raycast tests PASSED!" << std::endl;
}

void testTimeManagerAndAudio() {
    std::cout << "[TEST] 5. TimeManager & Audio System..." << std::endl;
    TimeManager tm;
    tm.setTimeOfDay(6000.0f); // Noon
    assert(tm.getSkyColor().r > 0.4f); // Daylight blue sky
    
    tm.setTimeOfDay(18000.0f); // Midnight
    assert(tm.getSkyColor().r < 0.1f); // Dark night sky
    
    AudioManager::playSound(SoundEffect::BlockBreak);
    AudioManager::playSound(SoundEffect::Jump);
    std::cout << "  -> TimeManager & Audio tests PASSED!" << std::endl;
}

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << " Running Minecraft Engine Test Suite   " << std::endl;
    std::cout << "========================================" << std::endl;

    testRedstone();
    testFluids();
    testCraftingAndInventory();
    testPhysicsAndRaycast();
    testTimeManagerAndAudio();

    std::cout << "========================================" << std::endl;
    std::cout << " ALL ENGINE TESTS PASSED SUCCESSFULLY! " << std::endl;
    std::cout << "========================================" << std::endl;
    return 0;
}
