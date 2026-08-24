#include "MobEngine.hpp"
#include "ItemEntity.hpp"
#include "../world/World.hpp"
#include "../world/ExplosionEngine.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../audio/AudioManager.hpp"
#include <cmath>
#include <iostream>
#include <algorithm>
#include <queue>
#include <unordered_map>
#include <unordered_set>

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
    else if (type == MobType::EnderDragon) mob.health = 200.0f;
    else if (type == MobType::IronGolem) mob.health = 100.0f;
    else if (type == MobType::Villager) mob.health = 20.0f;
    else mob.health = 10.0f; // Pig / Cow
    mob.maxHealth = mob.health;
    m_Mobs.push_back(mob);
}

std::deque<glm::ivec3> MobEngine::findPath3D(World& world, const glm::ivec3& start, const glm::ivec3& target, int maxSteps) {
    std::deque<glm::ivec3> path;
    if (start == target) return path;

    struct Node {
        glm::ivec3 pos;
        float gCost;
        float hCost;
        float fCost() const { return gCost + hCost; }
    };

    auto comp = [](const Node& a, const Node& b) { return a.fCost() > b.fCost(); };
    std::priority_queue<Node, std::vector<Node>, decltype(comp)> openSet(comp);

    auto hashPos = [](const glm::ivec3& v) -> int64_t {
        return (static_cast<int64_t>(v.x) & 0x3FFFFFF) |
               ((static_cast<int64_t>(v.z) & 0x3FFFFFF) << 26) |
               ((static_cast<int64_t>(v.y) & 0xFFF) << 52);
    };

    std::unordered_map<int64_t, glm::ivec3> cameFrom;
    std::unordered_map<int64_t, float> gScore;

    int64_t startKey = hashPos(start);
    gScore[startKey] = 0.0f;
    openSet.push({ start, 0.0f, glm::distance(glm::vec3(start), glm::vec3(target)) });

    glm::ivec3 neighborDeltas[10] = {
        {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1},       // Cardinal
        {1, 1, 0}, {-1, 1, 0}, {0, 1, 1}, {0, 1, -1},       // Step-up jump
        {0, -1, 0}, {0, -2, 0}                               // Step-down drop
    };

    int steps = 0;
    bool found = false;
    glm::ivec3 closest = start;
    float closestDist = glm::distance(glm::vec3(start), glm::vec3(target));

    while (!openSet.empty() && steps < maxSteps) {
        Node current = openSet.top();
        openSet.pop();
        steps++;

        float d = glm::distance(glm::vec3(current.pos), glm::vec3(target));
        if (d < closestDist) {
            closestDist = d;
            closest = current.pos;
        }

        if (glm::distance(glm::vec3(current.pos), glm::vec3(target)) <= 1.5f) {
            closest = current.pos;
            found = true;
            break;
        }

        for (const auto& delta : neighborDeltas) {
            glm::ivec3 neighbor = current.pos + delta;
            if (neighbor.y < 0 || neighbor.y >= CHUNK_SIZE_Y) continue;

            // Check if foot is non-solid and head is non-solid, and block below foot is solid
            BlockType footBlock = world.getBlock(neighbor.x, neighbor.y, neighbor.z);
            BlockType headBlock = world.getBlock(neighbor.x, neighbor.y + 1, neighbor.z);
            BlockType groundBlock = world.getBlock(neighbor.x, neighbor.y - 1, neighbor.z);

            if (BlockData::isSolid(footBlock) || BlockData::isSolid(headBlock)) continue;
            if (!BlockData::isSolid(groundBlock)) continue;

            float tentativeG = current.gCost + glm::distance(glm::vec3(current.pos), glm::vec3(neighbor));
            int64_t nKey = hashPos(neighbor);

            if (gScore.find(nKey) == gScore.end() || tentativeG < gScore[nKey]) {
                gScore[nKey] = tentativeG;
                cameFrom[nKey] = current.pos;
                openSet.push({ neighbor, tentativeG, glm::distance(glm::vec3(neighbor), glm::vec3(target)) });
            }
        }
    }

    if (found || steps > 0) {
        glm::ivec3 curr = closest;
        int64_t currKey = hashPos(curr);
        while (currKey != startKey && cameFrom.find(currKey) != cameFrom.end()) {
            path.push_front(curr);
            curr = cameFrom[currKey];
            currKey = hashPos(curr);
        }
    }

    return path;
}

void MobEngine::update(World& world, glm::vec3& playerPos, glm::vec3& playerVel, float& playerHealth, float deltaTime, ItemEntityManager* itemMgr) {
    // 1. Update Mobs
    for (auto it = m_Mobs.begin(); it != m_Mobs.end(); ) {
        Mob& mob = *it;

        if (mob.hurtTime > 0.0f) {
            mob.hurtTime -= deltaTime * 3.0f;
            if (mob.hurtTime < 0.0f) mob.hurtTime = 0.0f;
        }

        if (mob.attackCooldown > 0.0f) {
            mob.attackCooldown -= deltaTime;
        }

        if (mob.health <= 0.0f) {
            if (itemMgr) {
                if (mob.type == MobType::Pig) {
                    itemMgr->spawnItemDrop(BlockType::RawPorkchop, 1 + rand() % 2, mob.position);
                } else if (mob.type == MobType::Zombie) {
                    itemMgr->spawnItemDrop(BlockType::Apple, 1, mob.position);
                } else if (mob.type == MobType::Villager) {
                    itemMgr->spawnItemDrop(BlockType::Emerald, 1, mob.position);
                } else if (mob.type == MobType::IronGolem) {
                    itemMgr->spawnItemDrop(BlockType::IronOre, 3 + rand() % 3, mob.position);
                }
            }
            AudioManager::playSound3D(SoundEffect::MobHit, mob.position, playerPos, glm::vec3(0, 0, -1));
            it = m_Mobs.erase(it);
            continue;
        }

        float distToPlayer = glm::distance(mob.position, playerPos);

        // Calculate Yaw rotation towards player
        glm::vec3 lookVec = playerPos - mob.position;
        mob.yaw = std::atan2(lookVec.x, lookVec.z);

        // --- Zombie AI ---
        if (mob.type == MobType::Zombie) {
            if (distToPlayer < 22.0f && distToPlayer > 1.2f) {
                glm::vec3 dir = glm::normalize(glm::vec3(playerPos.x - mob.position.x, 0.0f, playerPos.z - mob.position.z));
                float speed = 3.5f;

                glm::vec3 frontCheck = mob.position + dir * 0.6f;
                int fx = static_cast<int>(std::floor(frontCheck.x));
                int fy = static_cast<int>(std::floor(mob.position.y));
                int fz = static_cast<int>(std::floor(frontCheck.z));
                BlockType frontBlock = world.getBlock(fx, fy, fz);
                BlockType headBlock = world.getBlock(fx, fy + 1, fz);

                if (BlockData::isSolid(frontBlock)) {
                    if (!BlockData::isSolid(headBlock) && mob.isGrounded) {
                        mob.velocity.y = 7.5f;
                        mob.isGrounded = false;
                    } else {
                        glm::vec3 sideDir = glm::normalize(glm::vec3(-dir.z, 0.0f, dir.x));
                        dir = glm::normalize(dir + sideDir * 0.8f);
                    }
                }

                mob.velocity.x = dir.x * speed;
                mob.velocity.z = dir.z * speed;
                mob.limbSwing += deltaTime * 8.0f;
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
                mob.velocity.x = dir.x * 2.8f;
                mob.velocity.z = dir.z * 2.8f;
                mob.limbSwing += deltaTime * 6.0f;
            } else if (distToPlayer <= 16.0f && mob.attackCooldown <= 0.0f) {
                // Shoot Arrow projectile
                ArrowEntity arrow;
                arrow.position = mob.position + glm::vec3(0.0f, 1.4f, 0.0f);
                glm::vec3 aimDir = glm::normalize((playerPos + glm::vec3(0, 1, 0)) - arrow.position);
                arrow.velocity = aimDir * 18.0f + glm::vec3(0.0f, 2.0f, 0.0f);
                arrow.active = true;
                m_Arrows.push_back(arrow);
                mob.attackCooldown = 2.0f;
                AudioManager::playSound3D(SoundEffect::ArrowShoot, mob.position, playerPos, glm::vec3(0, 0, -1));
            }
        }
        // --- Creeper AI ---
        else if (mob.type == MobType::Creeper) {
            if (distToPlayer < 18.0f && distToPlayer > 2.5f) {
                glm::vec3 dir = glm::normalize(glm::vec3(playerPos.x - mob.position.x, 0.0f, playerPos.z - mob.position.z));
                mob.velocity.x = dir.x * 3.2f;
                mob.velocity.z = dir.z * 3.2f;
                mob.fuseTimer = 0.0f;
                mob.limbSwing += deltaTime * 7.0f;
            } else if (distToPlayer <= 2.8f) {
                mob.velocity.x = 0.0f;
                mob.velocity.z = 0.0f;
                mob.fuseTimer += deltaTime;
                if (mob.fuseTimer >= 1.5f) {
                    ExplosionEngine::createExplosion(world, mob.position, 3.5f);
                    AudioManager::playSound3D(SoundEffect::Explosion, mob.position, playerPos, glm::vec3(0, 0, -1));
                    it = m_Mobs.erase(it);
                    continue;
                }
            }
        }
        // --- Ender Dragon Boss AI ---
        else if (mob.type == MobType::EnderDragon) {
            glm::vec3 target = playerPos + glm::vec3(0, 5, 0);
            glm::vec3 dir = glm::normalize(target - mob.position);
            mob.velocity = dir * 10.0f;
            mob.position += mob.velocity * deltaTime;

            if (distToPlayer < 4.0f && mob.attackCooldown <= 0.0f) {
                playerHealth -= 10.0f;
                playerVel += dir * 12.0f + glm::vec3(0, 6, 0);
                mob.attackCooldown = 2.0f;
                AudioManager::playSound(SoundEffect::MobHit);
            }
            ++it;
            continue;
        }

        // Apply Mob Gravity & Physics
        mob.velocity.y -= 25.0f * deltaTime;
        mob.position.y += mob.velocity.y * deltaTime;

        int mx = static_cast<int>(std::floor(mob.position.x));
        int my = static_cast<int>(std::floor(mob.position.y));
        int mz = static_cast<int>(std::floor(mob.position.z));

        if (BlockData::isSolid(world.getBlock(mx, my, mz))) {
            mob.position.y = std::floor(mob.position.y) + 1.0f;
            mob.velocity.y = 0.0f;
            mob.isGrounded = true;
        } else {
            mob.isGrounded = false;
        }

        mob.position.x += mob.velocity.x * deltaTime;
        mob.position.z += mob.velocity.z * deltaTime;

        ++it;
    }

    // 2. Update Arrows
    for (auto it = m_Arrows.begin(); it != m_Arrows.end(); ) {
        ArrowEntity& arrow = *it;
        arrow.velocity.y -= 9.8f * deltaTime;
        arrow.position += arrow.velocity * deltaTime;

        if (glm::distance(arrow.position, playerPos + glm::vec3(0, 1, 0)) < 1.0f) {
            playerHealth -= 4.0f;
            playerVel += glm::normalize(arrow.velocity) * 3.0f;
            AudioManager::playSound(SoundEffect::MobHit);
            it = m_Arrows.erase(it);
            continue;
        }

        if (ArrowEntityCollisionCheck(world, arrow.position)) {
            it = m_Arrows.erase(it);
            continue;
        }

        ++it;
    }
}

bool MobEngine::checkPlayerAttack(const glm::vec3& playerPos, const glm::vec3& playerDir, float reach, int damage, ItemEntityManager* itemMgr) {
    (void)itemMgr;
    for (auto& mob : m_Mobs) {
        glm::vec3 toMob = (mob.position + glm::vec3(0, 1, 0)) - playerPos;
        float dist = glm::length(toMob);
        if (dist <= reach) {
            glm::vec3 dirToMob = glm::normalize(toMob);
            float dot = glm::dot(playerDir, dirToMob);
            if (dot > 0.65f) {
                mob.health -= damage;
                mob.hurtTime = 1.0f; // Visual red flash
                mob.velocity += playerDir * 6.0f + glm::vec3(0, 4, 0); // Knockback
                AudioManager::playSound3D(SoundEffect::MobHit, mob.position, playerPos, glm::vec3(0, 0, -1));
                return true;
            }
        }
    }
    return false;
}

}
