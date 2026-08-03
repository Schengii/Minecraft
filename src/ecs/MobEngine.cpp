#include "MobEngine.hpp"
#include "ItemEntity.hpp"
#include "../world/World.hpp"
#include "../world/ExplosionEngine.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../audio/AudioManager.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>

namespace Minecraft {

static bool ArrowEntityCollisionCheck(World& world, const glm::vec3& pos) {
    int x = static_cast<int>(std::floor(pos.x));
    int y = static_cast<int>(std::floor(pos.y));
    int z = static_cast<int>(std::floor(pos.z));
    return BlockData::isSolid(world.getBlock(x, y, z));
}

MobEngine::MobEngine() = default;

void MobEngine::spawnMob(MobType type, const glm::vec3& position) {
    Mob mob;
    mob.type = type;
    mob.position = position;
    if (type == MobType::Zombie) mob.health = 20.0f;
    else if (type == MobType::Skeleton) mob.health = 20.0f;
    else if (type == MobType::Creeper) mob.health = 20.0f;
    else mob.health = 10.0f; // Pig / Cow
    mob.maxHealth = mob.health;
    m_Mobs.push_back(mob);
    std::cout << "[MobEngine] Spawned Mob at (" << position.x << ", " << position.y << ", " << position.z << ")" << std::endl;
}

void MobEngine::update(World& world, glm::vec3& playerPos, glm::vec3& playerVel, float& playerHealth, float deltaTime, ItemEntityManager* itemMgr) {
    // 1. Update Mobs
    for (auto it = m_Mobs.begin(); it != m_Mobs.end(); ) {
        Mob& mob = *it;

        if (mob.health <= 0.0f) {
            std::cout << "[MobEngine] Mob Defeated!" << std::endl;
            if (itemMgr) {
                if (mob.type == MobType::Pig) {
                    itemMgr->spawnItemDrop(BlockType::RawPorkchop, 1 + rand() % 2, mob.position);
                } else if (mob.type == MobType::Zombie) {
                    itemMgr->spawnItemDrop(BlockType::Apple, 1, mob.position);
                }
            }
            AudioManager::playSound3D(SoundEffect::MobHit, mob.position, playerPos, glm::vec3(0, 0, -1));
            it = m_Mobs.erase(it);
            continue;
        }

        float distToPlayer = glm::distance(mob.position, playerPos);

        // --- Zombie AI ---
        if (mob.type == MobType::Zombie) {
            if (distToPlayer < 22.0f && distToPlayer > 1.2f) {
                glm::vec3 dir = glm::normalize(glm::vec3(playerPos.x - mob.position.x, 0.0f, playerPos.z - mob.position.z));
                float speed = 3.5f;
                mob.velocity.x = dir.x * speed;
                mob.velocity.z = dir.z * speed;

                glm::vec3 frontCheck = mob.position + dir * 0.6f;
                BlockType frontBlock = world.getBlock(static_cast<int>(std::floor(frontCheck.x)), static_cast<int>(std::floor(mob.position.y)), static_cast<int>(std::floor(frontCheck.z)));
                if (BlockData::isSolid(frontBlock) && mob.isGrounded) {
                    mob.velocity.y = 7.5f;
                    mob.isGrounded = false;
                }
            } else if (distToPlayer <= 1.5f && mob.attackCooldown <= 0.0f) {
                playerHealth -= 3.0f;
                playerVel += glm::normalize(playerPos - mob.position) * 4.0f + glm::vec3(0, 2, 0);
                mob.attackCooldown = 1.0f;
                AudioManager::playSound(SoundEffect::MobHit);
            }
        }
        // --- Skeleton AI ---
        else if (mob.type == MobType::Skeleton) {
            if (distToPlayer < 24.0f && distToPlayer > 8.0f) {
                glm::vec3 dir = glm::normalize(glm::vec3(playerPos.x - mob.position.x, 0.0f, playerPos.z - mob.position.z));
                mob.velocity.x = dir.x * 3.0f;
                mob.velocity.z = dir.z * 3.0f;
            } else if (distToPlayer <= 14.0f && mob.attackCooldown <= 0.0f) {
                // Shoot Arrow towards player
                glm::vec3 shootDir = glm::normalize(playerPos + glm::vec3(0, 1.2f, 0) - mob.position);
                ArrowEntity arrow;
                arrow.position = mob.position + glm::vec3(0, 1.2f, 0);
                arrow.velocity = shootDir * 18.0f;
                m_Arrows.push_back(arrow);
                mob.attackCooldown = 2.0f;
                AudioManager::playSound3D(SoundEffect::ArrowShoot, mob.position, playerPos, glm::vec3(0, 0, -1));
            }
        }
        // --- Creeper AI ---
        else if (mob.type == MobType::Creeper) {
            if (distToPlayer < 20.0f) {
                glm::vec3 dir = glm::normalize(glm::vec3(playerPos.x - mob.position.x, 0.0f, playerPos.z - mob.position.z));
                mob.velocity.x = dir.x * 3.2f;
                mob.velocity.z = dir.z * 3.2f;

                if (distToPlayer < 3.2f) {
                    mob.fuseTimer += deltaTime;
                    if (mob.fuseTimer >= 1.5f) {
                        ExplosionEngine::createExplosion(world, mob.position, 4.5f, &playerVel, &playerPos);
                        AudioManager::playSound3D(SoundEffect::Explosion, mob.position, playerPos, glm::vec3(0, 0, -1));
                        it = m_Mobs.erase(it);
                        continue;
                    } else if (mob.fuseTimer < 0.1f) {
                        AudioManager::playSound3D(SoundEffect::CreeperFuse, mob.position, playerPos, glm::vec3(0, 0, -1));
                    }
                } else {
                    mob.fuseTimer = std::max(0.0f, mob.fuseTimer - deltaTime);
                }
            }
        }
        // --- Passive Animal Wandering ---
        else if (mob.type == MobType::Pig || mob.type == MobType::Cow) {
            if (rand() % 200 == 0) {
                mob.velocity.x = (rand() % 100 - 50) * 0.04f;
                mob.velocity.z = (rand() % 100 - 50) * 0.04f;
            }
        }

        if (mob.attackCooldown > 0.0f) mob.attackCooldown -= deltaTime;

        // Apply Physics
        bool dummyInWater = false;
        PhysicsEngine::updatePlayer(world, mob.position, mob.velocity, mob.isGrounded, dummyInWater, false, false, deltaTime);

        ++it;
    }

    // 2. Update Arrows
    for (auto it = m_Arrows.begin(); it != m_Arrows.end(); ) {
        ArrowEntity& arrow = *it;
        arrow.position += arrow.velocity * deltaTime;
        arrow.velocity.y -= 4.0f * deltaTime;

        float dist = glm::distance(arrow.position, playerPos);
        if (dist < 1.2f) {
            playerHealth -= 4.0f;
            playerVel += glm::normalize(arrow.velocity) * 3.0f;
            AudioManager::playSound(SoundEffect::MobHit);
            it = m_Arrows.erase(it);
        } else if (arrow.position.y < 0.0f || ArrowEntityCollisionCheck(world, arrow.position)) {
            it = m_Arrows.erase(it);
        } else {
            ++it;
        }
    }
}

bool MobEngine::checkPlayerAttack(const glm::vec3& playerPos, const glm::vec3& playerDir, float reach, int damage, ItemEntityManager* itemMgr) {
    for (auto& mob : m_Mobs) {
        float dist = glm::distance(playerPos, mob.position);
        if (dist <= reach) {
            glm::vec3 toMob = glm::normalize(mob.position - playerPos);
            float dot = glm::dot(playerDir, toMob);
            if (dot > 0.6f) {
                mob.health -= static_cast<float>(damage);
                mob.velocity += toMob * 5.0f + glm::vec3(0.0f, 3.0f, 0.0f); // Knockback
                AudioManager::playSound3D(SoundEffect::MobHit, mob.position, playerPos, playerDir);
                return true;
            }
        }
    }
    return false;
}

}
