#include "MobEngine.hpp"
#include "../world/World.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../audio/AudioManager.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

namespace Minecraft {

MobEngine::MobEngine() = default;

void MobEngine::spawnMob(MobType type, const glm::vec3& position) {
    Mob mob;
    mob.type = type;
    mob.position = position;
    mob.health = (type == MobType::Zombie) ? 20.0f : 10.0f;
    mob.maxHealth = mob.health;
    m_Mobs.push_back(mob);
    std::cout << "[MobEngine] Spawned Mob at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void MobEngine::update(World& world, const glm::vec3& playerPos, float deltaTime) {
    for (auto it = m_Mobs.begin(); it != m_Mobs.end(); ) {
        Mob& mob = *it;

        if (mob.health <= 0.0f) {
            std::cout << "[MobEngine] Mob Defeated!" << std::endl;
            AudioManager::playSound(SoundEffect::BlockBreak);
            it = m_Mobs.erase(it);
            continue;
        }

        // AI Logic
        if (mob.type == MobType::Zombie) {
            float dist = glm::distance(mob.position, playerPos);
            if (dist < 20.0f && dist > 1.0f) {
                glm::vec3 dir = glm::normalize(glm::vec3(playerPos.x - mob.position.x, 0.0f, playerPos.z - mob.position.z));
                float speed = 3.5f;
                mob.velocity.x = dir.x * speed;
                mob.velocity.z = dir.z * speed;

                // Auto-Jump obstacle navigation
                glm::vec3 frontCheck = mob.position + dir * 0.6f;
                BlockType frontBlock = world.getBlock(static_cast<int>(std::floor(frontCheck.x)), static_cast<int>(std::floor(mob.position.y)), static_cast<int>(std::floor(frontCheck.z)));
                if (BlockData::isSolid(frontBlock) && mob.isGrounded) {
                    mob.velocity.y = 7.5f;
                    mob.isGrounded = false;
                }
            }
        }

        // Apply Physics & Gravity
        bool dummyInWater = false;
        PhysicsEngine::updatePlayer(world, mob.position, mob.velocity, mob.isGrounded, dummyInWater, false, false, deltaTime);

        ++it;
    }
}

bool MobEngine::checkPlayerAttack(const glm::vec3& playerPos, const glm::vec3& playerDir, float reach, int damage) {
    for (auto& mob : m_Mobs) {
        float dist = glm::distance(playerPos, mob.position);
        if (dist <= reach) {
            glm::vec3 toMob = glm::normalize(mob.position - playerPos);
            float dot = glm::dot(playerDir, toMob);
            if (dot > 0.6f) {
                mob.health -= static_cast<float>(damage);
                mob.velocity += toMob * 5.0f + glm::vec3(0.0f, 3.0f, 0.0f); // Knockback
                AudioManager::playSound(SoundEffect::BlockBreak);
                return true;
            }
        }
    }
    return false;
}

}
