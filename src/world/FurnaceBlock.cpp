#include "FurnaceBlock.hpp"

namespace Minecraft {

FurnaceManager::FurnaceManager() = default;
FurnaceManager::~FurnaceManager() = default;

FurnaceData* FurnaceManager::getFurnace(const glm::ivec3& pos) {
    auto it = m_Furnaces.find(pos);
    if (it != m_Furnaces.end()) {
        return &it->second;
    }
    m_Furnaces[pos] = FurnaceData{};
    return &m_Furnaces[pos];
}

bool FurnaceManager::isSmeltable(BlockType input) {
    return input == BlockType::IronOre || input == BlockType::GoldOre || input == BlockType::Sand || input == BlockType::OakLog;
}

BlockType FurnaceManager::getSmeltResult(BlockType input) {
    if (input == BlockType::IronOre) return BlockType::Stone; // Refined Metal Block / Ingot
    if (input == BlockType::GoldOre) return BlockType::GoldOre;
    if (input == BlockType::Sand) return BlockType::Glass;
    if (input == BlockType::OakLog) return BlockType::CoalOre;
    return BlockType::Air;
}

bool FurnaceManager::isFuel(BlockType fuel) {
    return fuel == BlockType::CoalOre || fuel == BlockType::OakLog || fuel == BlockType::Planks || fuel == BlockType::Stick;
}

void FurnaceManager::update(float deltaTime) {
    for (auto& [pos, furnace] : m_Furnaces) {
        if (isSmeltable(furnace.input.type) && furnace.input.count > 0) {
            if (furnace.fuelBurnTimer <= 0.0f && isFuel(furnace.fuel.type) && furnace.fuel.count > 0) {
                furnace.fuel.count--;
                if (furnace.fuel.count == 0) furnace.fuel.type = BlockType::Air;
                furnace.fuelBurnTimer = 10.0f; // 10s fuel burn time
            }

            if (furnace.fuelBurnTimer > 0.0f) {
                furnace.fuelBurnTimer -= deltaTime;
                furnace.cookTimer += deltaTime;
                if (furnace.cookTimer >= furnace.cookTimeMax) {
                    furnace.cookTimer = 0.0f;
                    furnace.input.count--;
                    if (furnace.input.count == 0) furnace.input.type = BlockType::Air;

                    BlockType result = getSmeltResult(furnace.input.type);
                    furnace.output.type = result;
                    furnace.output.count++;
                }
            }
        } else {
            furnace.cookTimer = 0.0f;
        }
    }
}

}
