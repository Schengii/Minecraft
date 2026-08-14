#ifndef CROPSENGINE_HPP
#define CROPSENGINE_HPP

#include "Block.hpp"
#include <unordered_map>
#include <cstdint>
#include <glm/glm.hpp>

namespace Minecraft {

class World;
class ItemEntityManager;

struct CropState {
    int growthStage = 0; // 0 (seedling) to 7 (fully grown)
    float growthTimer = 0.0f;
    bool isHydrated = false;
};

class CropsEngine {
public:
    static bool isCrop(BlockType type);
    static bool plantCrop(World& world, int x, int y, int z, BlockType cropType);
    static bool applyBoneMeal(World& world, int x, int y, int z);
    static void updateCropGrowth(World& world, float deltaTime);

    static int getCropGrowthStage(int x, int y, int z);
    static void harvestCrop(World& world, int x, int y, int z, ItemEntityManager* itemMgr = nullptr);
    static void clear();

private:
    static std::unordered_map<int64_t, CropState> s_Crops;
    static int64_t posKey(int x, int y, int z) {
        return (static_cast<int64_t>(x) & 0x3FFFFFF) |
               ((static_cast<int64_t>(z) & 0x3FFFFFF) << 26) |
               ((static_cast<int64_t>(y) & 0xFFF) << 52);
    }
};

}

#endif // CROPSENGINE_HPP
