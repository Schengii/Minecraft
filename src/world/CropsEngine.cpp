#include "CropsEngine.hpp"
#include "World.hpp"
#include "../ecs/ItemEntity.hpp"
#include <algorithm>

namespace Minecraft {

std::unordered_map<int64_t, CropState> CropsEngine::s_Crops;

bool CropsEngine::isCrop(BlockType type) {
    return type == BlockType::WheatCrop || type == BlockType::CarrotCrop || type == BlockType::PotatoCrop;
}

bool CropsEngine::plantCrop(World& world, int x, int y, int z, BlockType cropType) {
    if (world.getBlock(x, y - 1, z) != BlockType::Dirt && world.getBlock(x, y - 1, z) != BlockType::Grass) {
        return false;
    }
    if (world.getBlock(x, y, z) != BlockType::Air) {
        return false;
    }

    world.setBlock(x, y, z, cropType);
    CropState state;
    state.growthStage = 0;
    state.growthTimer = 0.0f;
    state.isHydrated = false;

    // Check adjacent water blocks for soil hydration
    for (int dx = -4; dx <= 4; ++dx) {
        for (int dz = -4; dz <= 4; ++dz) {
            if (world.getBlock(x + dx, y - 1, z + dz) == BlockType::Water) {
                state.isHydrated = true;
                break;
            }
        }
        if (state.isHydrated) break;
    }

    s_Crops[posKey(x, y, z)] = state;
    return true;
}

bool CropsEngine::applyBoneMeal(World& world, int x, int y, int z) {
    auto it = s_Crops.find(posKey(x, y, z));
    if (it != s_Crops.end()) {
        it->second.growthStage = std::min(7, it->second.growthStage + 2 + rand() % 3);
        return true;
    }
    return false;
}

void CropsEngine::updateCropGrowth(World& world, float deltaTime) {
    for (auto& [key, state] : s_Crops) {
        if (state.growthStage >= 7) continue;

        // Hydrated crops grow 3x faster
        float rate = state.isHydrated ? 3.0f : 1.0f;
        state.growthTimer += deltaTime * rate;

        if (state.growthTimer >= 10.0f) {
            state.growthTimer = 0.0f;
            state.growthStage++;
        }
    }
}

int CropsEngine::getCropGrowthStage(int x, int y, int z) {
    auto it = s_Crops.find(posKey(x, y, z));
    if (it != s_Crops.end()) {
        return it->second.growthStage;
    }
    return 0;
}

void CropsEngine::harvestCrop(World& world, int x, int y, int z, ItemEntityManager* itemMgr) {
    auto it = s_Crops.find(posKey(x, y, z));
    if (it != s_Crops.end()) {
        BlockType crop = world.getBlock(x, y, z);
        int stage = it->second.growthStage;

        if (itemMgr) {
            if (stage >= 7) {
                if (crop == BlockType::WheatCrop) {
                    itemMgr->spawnItemDrop(BlockType::Bread, 1, glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f));
                    itemMgr->spawnItemDrop(BlockType::Stick, 2, glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f));
                } else if (crop == BlockType::CarrotCrop) {
                    itemMgr->spawnItemDrop(BlockType::Apple, 2, glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f));
                } else if (crop == BlockType::PotatoCrop) {
                    itemMgr->spawnItemDrop(BlockType::Apple, 2, glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f));
                }
            } else {
                itemMgr->spawnItemDrop(BlockType::Stick, 1, glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f));
            }
        }
        world.setBlock(x, y, z, BlockType::Air);
        s_Crops.erase(it);
    }
}

void CropsEngine::clear() {
    s_Crops.clear();
}

}
