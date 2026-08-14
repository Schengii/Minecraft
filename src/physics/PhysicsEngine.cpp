#include "PhysicsEngine.hpp"
#include "../world/World.hpp"
#include <cmath>

namespace Minecraft {

bool PhysicsEngine::isPointInWater(World& world, const glm::vec3& point) {
    int x = static_cast<int>(std::floor(point.x));
    int y = static_cast<int>(std::floor(point.y));
    int z = static_cast<int>(std::floor(point.z));
    BlockType type = world.getBlock(x, y, z);
    return type == BlockType::Water;
}

void PhysicsEngine::updatePlayer(World& world, glm::vec3& position, glm::vec3& velocity, bool& isGrounded, bool& inWater, bool isFlying, bool isSneaking, float deltaTime) {
    if (isFlying) {
        position += velocity * deltaTime;
        velocity *= 0.85f;
        isGrounded = false;
        inWater = false;
        return;
    }

    if (isSneaking) {
        velocity.x *= 0.35f;
        velocity.z *= 0.35f;
    }

    inWater = isPointInWater(world, position + glm::vec3(0.0f, 0.9f, 0.0f));

    if (inWater) {
        // Water physics: Buoyancy & Swimming drag
        velocity.y -= 4.0f * deltaTime; // Reduced gravity in water
        if (velocity.y < -3.0f) velocity.y = -3.0f; // Terminal velocity in water

        velocity.x *= 0.85f;
        velocity.z *= 0.85f;

        position += velocity * deltaTime;
        isGrounded = false;
        return;
    }

    // Normal Gravity
    velocity.y -= 25.0f * deltaTime;

    // Movement X
    glm::vec3 nextPosX = position;
    nextPosX.x += velocity.x * deltaTime;
    AABB boxX(nextPosX - glm::vec3(0.3f, 0.0f, 0.3f), nextPosX + glm::vec3(0.3f, 1.8f, 0.3f));
    bool collideX = checkCollision(world, boxX);
    if (isSneaking && isGrounded && !collideX) {
        // Check if foot ground below next position is solid
        AABB footCheck(nextPosX - glm::vec3(0.3f, 0.1f, 0.3f), nextPosX + glm::vec3(0.3f, 0.0f, 0.3f));
        if (!checkCollision(world, footCheck)) collideX = true;
    }
    if (collideX) {
        velocity.x = 0.0f;
    } else {
        position.x = nextPosX.x;
    }

    // Movement Z
    glm::vec3 nextPosZ = position;
    nextPosZ.z += velocity.z * deltaTime;
    AABB boxZ(nextPosZ - glm::vec3(0.3f, 0.0f, 0.3f), nextPosZ + glm::vec3(0.3f, 1.8f, 0.3f));
    bool collideZ = checkCollision(world, boxZ);
    if (isSneaking && isGrounded && !collideZ) {
        AABB footCheck(nextPosZ - glm::vec3(0.3f, 0.1f, 0.3f), nextPosZ + glm::vec3(0.3f, 0.0f, 0.3f));
        if (!checkCollision(world, footCheck)) collideZ = true;
    }
    if (collideZ) {
        velocity.z = 0.0f;
    } else {
        position.z = nextPosZ.z;
    }

    // Movement Y
    position.y += velocity.y * deltaTime;
    AABB boxY(position - glm::vec3(0.3f, 0.0f, 0.3f), position + glm::vec3(0.3f, 1.8f, 0.3f));
    if (checkCollision(world, boxY)) {
        if (velocity.y < 0.0f) {
            isGrounded = true;
        }
        position.y -= velocity.y * deltaTime;
        velocity.y = 0.0f;
    } else {
        isGrounded = false;
    }

    velocity.x *= 0.8f;
    velocity.z *= 0.8f;
}

bool PhysicsEngine::checkCollision(World& world, const AABB& playerBox) {
    int minX = static_cast<int>(std::floor(playerBox.min.x));
    int maxX = static_cast<int>(std::floor(playerBox.max.x));
    int minY = static_cast<int>(std::floor(playerBox.min.y));
    int maxY = static_cast<int>(std::floor(playerBox.max.y));
    int minZ = static_cast<int>(std::floor(playerBox.min.z));
    int maxZ = static_cast<int>(std::floor(playerBox.max.z));

    for (int x = minX; x <= maxX; ++x) {
        for (int y = minY; y <= maxY; ++y) {
            for (int z = minZ; z <= maxZ; ++z) {
                BlockType type = world.getBlock(x, y, z);
                if (BlockData::isSolid(type)) {
                    AABB blockBox(glm::vec3(x, y, z), glm::vec3(x + 1, y + 1, z + 1));
                    if (playerBox.intersects(blockBox)) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void PhysicsEngine::updateMinecart(World& world, glm::vec3& position, glm::vec3& velocity, float deltaTime) {
    int px = static_cast<int>(std::floor(position.x));
    int py = static_cast<int>(std::floor(position.y));
    int pz = static_cast<int>(std::floor(position.z));

    BlockType onBlock = world.getBlock(px, py, pz);
    BlockType belowBlock = world.getBlock(px, py - 1, pz);

    if (onBlock == BlockType::PoweredRail || belowBlock == BlockType::PoweredRail) {
        // Boost velocity on powered rail
        velocity *= 1.5f;
    } else if (onBlock == BlockType::Rail || belowBlock == BlockType::Rail) {
        // Low friction rolling on standard rail
        velocity.x *= 0.98f;
        velocity.z *= 0.98f;
    } else {
        // High ground friction off-rail
        velocity.x *= 0.70f;
        velocity.z *= 0.70f;
    }

    position += velocity * deltaTime;
}

void PhysicsEngine::updateBoat(World& world, glm::vec3& position, glm::vec3& velocity, float deltaTime) {
    int px = static_cast<int>(std::floor(position.x));
    int py = static_cast<int>(std::floor(position.y));
    int pz = static_cast<int>(std::floor(position.z));

    bool inWater = (world.getBlock(px, py, pz) == BlockType::Water || world.getBlock(px, py - 1, pz) == BlockType::Water);

    if (inWater) {
        // Water buoyancy & smooth glide
        velocity.y = 0.0f;
        position.y = std::floor(position.y) + 0.2f;
        velocity.x *= 0.96f;
        velocity.z *= 0.96f;
    } else {
        // Gravity on land
        velocity.y -= 18.0f * deltaTime;
        velocity.x *= 0.80f;
        velocity.z *= 0.80f;
    }

    position += velocity * deltaTime;
}

}
