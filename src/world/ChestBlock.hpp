#ifndef CHESTBLOCK_HPP
#define CHESTBLOCK_HPP

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>
#include "../inventory/ItemStack.hpp"

namespace Minecraft {

class ChestManager {
public:
    ChestManager();
    ~ChestManager();

    void createChest(const glm::ivec3& pos);
    std::vector<ItemStack>* getChestInventory(const glm::ivec3& pos);
    void setSlot(const glm::ivec3& pos, int slotIndex, BlockType type, int count);

private:
    struct IVec3Hash {
        std::size_t operator()(const glm::ivec3& v) const {
            return std::hash<int>()(v.x) ^ (std::hash<int>()(v.y) << 1) ^ (std::hash<int>()(v.z) << 2);
        }
    };

    std::unordered_map<glm::ivec3, std::vector<ItemStack>, IVec3Hash> m_Chests;
};

}

#endif // CHESTBLOCK_HPP
