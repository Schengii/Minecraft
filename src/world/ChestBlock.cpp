#include "ChestBlock.hpp"

namespace Minecraft {

ChestManager::ChestManager() = default;
ChestManager::~ChestManager() = default;

void ChestManager::createChest(const glm::ivec3& pos) {
    if (m_Chests.find(pos) == m_Chests.end()) {
        m_Chests[pos] = std::vector<ItemStack>(27, ItemStack{ BlockType::Air, 0, 64 });
    }
}

std::vector<ItemStack>* ChestManager::getChestInventory(const glm::ivec3& pos) {
    auto it = m_Chests.find(pos);
    if (it != m_Chests.end()) {
        return &it->second;
    }
    createChest(pos);
    return &m_Chests[pos];
}

void ChestManager::setSlot(const glm::ivec3& pos, int slotIndex, BlockType type, int count) {
    auto* inv = getChestInventory(pos);
    if (inv && slotIndex >= 0 && slotIndex < static_cast<int>(inv->size())) {
        (*inv)[slotIndex].type = type;
        (*inv)[slotIndex].count = count;
    }
}

}
