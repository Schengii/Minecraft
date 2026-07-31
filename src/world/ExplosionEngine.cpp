#include "ExplosionEngine.hpp"
#include "World.hpp"
#include "Block.hpp"
#include "../audio/AudioManager.hpp"
#include <cmath>
#include <iostream>
#include <vector>

namespace Minecraft {

void ExplosionEngine::createExplosion(World& world, const glm::vec3& center, float radius, glm::vec3* playerVel, const glm::vec3* playerPos) {
    std::cout << "[ExplosionEngine] BOOM at (" << center.x << ", " << center.y << ", " << center.z << ")" << std::endl;
    AudioManager::playSound(SoundEffect::BlockBreak);

    int r = static_cast<int>(std::ceil(radius));
    int cx = static_cast<int>(std::floor(center.x));
    int cy = static_cast<int>(std::floor(center.y));
    int cz = static_cast<int>(std::floor(center.z));

    std::vector<glm::ivec3> tntChain;

    for (int x = cx - r; x <= cx + r; ++x) {
        for (int y = cy - r; y <= cy + r; ++y) {
            for (int z = cz - r; z <= cz + r; ++z) {
                float dist = glm::distance(center, glm::vec3(x + 0.5f, y + 0.5f, z + 0.5f));
                if (dist <= radius) {
                    BlockType type = world.getBlock(x, y, z);
                    if (type != BlockType::Air && type != BlockType::Bedrock) {
                        if (type == BlockType::TNT) {
                            tntChain.push_back(glm::ivec3(x, y, z));
                        }
                        world.setBlock(x, y, z, BlockType::Air);
                    }
                }
            }
        }
    }

    // Chain-reaction explosions
    for (const auto& tntPos : tntChain) {
        createExplosion(world, glm::vec3(tntPos) + glm::vec3(0.5f), radius * 0.8f, playerVel, playerPos);
    }

    // Apply Player Knockback
    if (playerPos && playerVel) {
        float pDist = glm::distance(center, *playerPos);
        if (pDist < radius * 2.0f && pDist > 0.01f) {
            glm::vec3 dir = glm::normalize(*playerPos - center);
            float force = (radius * 2.0f - pDist) * 3.5f;
            *playerVel += dir * force + glm::vec3(0.0f, 4.0f, 0.0f);
        }
    }
}

}
