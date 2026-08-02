#include "ItemEntity.hpp"
#include "../world/World.hpp"
#include "../audio/AudioManager.hpp"
#include <iostream>

namespace Minecraft {

ItemEntityManager::ItemEntityManager() = default;
ItemEntityManager::~ItemEntityManager() = default;

void ItemEntityManager::spawnItemDrop(BlockType type, int count, const glm::vec3& position) {
    if (type == BlockType::Air) return;

    ItemDropEntity item;
    item.itemType = type;
    item.count = count;
    item.position = position + glm::vec3(0.0f, 0.3f, 0.0f);
    item.velocity = glm::vec3(
        (rand() % 100 - 50) * 0.02f,
        2.5f,
        (rand() % 100 - 50) * 0.02f
    );
    item.rotation = static_cast<float>(rand() % 360);
    m_Items.push_back(item);
}

void ItemEntityManager::update(World& world, const glm::vec3& playerPos, std::vector<std::pair<BlockType, int>>& pickedUpItems, float deltaTime) {
    for (auto it = m_Items.begin(); it != m_Items.end(); ) {
        ItemDropEntity& item = *it;
        item.rotation += 90.0f * deltaTime;
        if (item.pickupDelay > 0.0f) {
            item.pickupDelay -= deltaTime;
        }

        // Apply Gravity
        item.velocity.y -= 9.81f * deltaTime;
        item.position += item.velocity * deltaTime;

        // Ground Collision check
        int blockX = static_cast<int>(std::floor(item.position.x));
        int blockY = static_cast<int>(std::floor(item.position.y));
        int blockZ = static_cast<int>(std::floor(item.position.z));

        if (BlockData::isSolid(world.getBlock(blockX, blockY, blockZ))) {
            item.position.y = static_cast<float>(blockY + 1);
            item.velocity = glm::vec3(0.0f);
        }

        // Magnetic Pull towards player when nearby
        float dist = glm::distance(item.position, playerPos);
        if (item.pickupDelay <= 0.0f && dist < 3.5f) {
            float moveStep = 6.0f * deltaTime;
            if (moveStep >= dist || dist < 1.5f) {
                pickedUpItems.push_back({ item.itemType, item.count });
                AudioManager::playSound(SoundEffect::BlockPlace);
                it = m_Items.erase(it);
                continue;
            } else {
                glm::vec3 pullDir = glm::normalize(playerPos - item.position);
                item.position += pullDir * moveStep;
            }
        }
        ++it;
    }
}

}
