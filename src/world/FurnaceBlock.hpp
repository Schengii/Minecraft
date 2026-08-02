#ifndef FURNACEBLOCK_HPP
#define FURNACEBLOCK_HPP

#include <glm/glm.hpp>
#include <unordered_map>
#include "../inventory/ItemStack.hpp"

namespace Minecraft {

struct FurnaceData {
    ItemStack input{ BlockType::Air, 0, 64 };
    ItemStack fuel{ BlockType::Air, 0, 64 };
    ItemStack output{ BlockType::Air, 0, 64 };
    float cookTimer = 0.0f;
    float cookTimeMax = 5.0f; // 5 seconds per item
    float fuelBurnTimer = 0.0f;
};

class FurnaceManager {
public:
    FurnaceManager();
    ~FurnaceManager();

    FurnaceData* getFurnace(const glm::ivec3& pos);
    void update(float deltaTime);

private:
    struct IVec3Hash {
        std::size_t operator()(const glm::ivec3& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^ (std::hash<int>()(v.z) << 2);
        }
    };

    std::unordered_map<glm::ivec3, FurnaceData, IVec3Hash> m_Furnaces;

    bool isSmeltable(BlockType input);
    BlockType getSmeltResult(BlockType input);
    bool isFuel(BlockType fuel);
};

}

#endif // FURNACEBLOCK_HPP
