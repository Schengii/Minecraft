#include "PhysicsEngine.hpp"
#include "../world/World.hpp"
#include <cmath>
#include <algorithm>

namespace Minecraft {

bool PhysicsEngine::isPointInWater(World& world, const glm::vec3& point) {
    int x = static_cast<int>(std::floor(point.x));
    int y = static_cast<int>(std::floor(point.y));
    int z = static_cast<int>(std::floor(point.z));
    BlockType type = world.getBlock(x, y, z);
    return type == BlockType::Water;
}

bool PhysicsEngine::isPointInLava(World& world, const glm::vec3& point) {
    int x = static_cast<int>(std::floor(point.x));
    int y = static_cast<int>(std::floor(point.y));
    int z = static_cast<int>(std::floor(point.z));
    BlockType type = world.getBlock(x, y, z);
    return type == BlockType::Lava;
}

bool PhysicsEngine::isHeadUnderwater(World& world, const glm::vec3& position) {
    return isPointInWater(world, position + glm::vec3(0.0f, 1.6f, 0.0f));
}

void PhysicsEngine::updatePlayer(World& world, glm::vec3& position, glm::vec3& velocity, bool& isGrounded, bool& inWater, bool isFlying, bool isSneaking, float deltaTime, bool isGliding, const glm::vec3& lookDir) {
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

    if (isGliding && !isGrounded) {
        // Elytra Gliding Aerodynamics: Cushion gravity & translate pitch to forward horizontal thrust
        velocity.y = std::max(velocity.y - 4.5f * deltaTime, -2.5f);
        velocity.x += lookDir.x * 14.0f * deltaTime;
        velocity.z += lookDir.z * 14.0f * deltaTime;
        float hSpeed = std::sqrt(velocity.x * velocity.x + velocity.z * velocity.z);
        if (hSpeed > 18.0f) {
            velocity.x = (velocity.x / hSpeed) * 18.0f;
            velocity.z = (velocity.z / hSpeed) * 18.0f;
        }
    } else {
        // Normal Gravity
        velocity.y -= 25.0f * deltaTime;
    }

    // Movement X with Auto Step-Up & Sneak Edge Check
    glm::vec3 nextPosX = position;
    nextPosX.x += velocity.x * deltaTime;
    AABB boxX(nextPosX - glm::vec3(0.3f, 0.0f, 0.3f), nextPosX + glm::vec3(0.3f, 1.8f, 0.3f));
    bool collideX = checkCollision(world, boxX);

    // Auto Step-Up over slabs / 0.5-block steps
    if (collideX && isGrounded) {
        glm::vec3 stepPos = nextPosX + glm::vec3(0.0f, 0.6f, 0.0f);
        AABB stepBox(stepPos - glm::vec3(0.3f, 0.0f, 0.3f), stepPos + glm::vec3(0.3f, 1.8f, 0.3f));
        if (!checkCollision(world, stepBox)) {
            nextPosX.y += 0.6f;
            collideX = false;
        }
    }

    if (isSneaking && isGrounded && !collideX) {
        // Prevent walking off ledge when sneaking
        AABB footCheck(nextPosX - glm::vec3(0.3f, 0.2f, 0.3f), nextPosX + glm::vec3(0.3f, 0.0f, 0.3f));
        if (!checkCollision(world, footCheck)) {
            collideX = true;
        }
    }

    if (collideX) {
        velocity.x = 0.0f;
    } else {
        position.x = nextPosX.x;
        position.y = nextPosX.y;
    }

    // Movement Z with Auto Step-Up & Sneak Edge Check
    glm::vec3 nextPosZ = position;
    nextPosZ.z += velocity.z * deltaTime;
    AABB boxZ(nextPosZ - glm::vec3(0.3f, 0.0f, 0.3f), nextPosZ + glm::vec3(0.3f, 1.8f, 0.3f));
    bool collideZ = checkCollision(world, boxZ);

    if (collideZ && isGrounded) {
        glm::vec3 stepPos = nextPosZ + glm::vec3(0.0f, 0.6f, 0.0f);
        AABB stepBox(stepPos - glm::vec3(0.3f, 0.0f, 0.3f), stepPos + glm::vec3(0.3f, 1.8f, 0.3f));
        if (!checkCollision(world, stepBox)) {
            nextPosZ.y += 0.6f;
            collideZ = false;
        }
    }

    if (isSneaking && isGrounded && !collideZ) {
        AABB footCheck(nextPosZ - glm::vec3(0.3f, 0.2f, 0.3f), nextPosZ + glm::vec3(0.3f, 0.0f, 0.3f));
        if (!checkCollision(world, footCheck)) {
            collideZ = true;
        }
    }

    if (collideZ) {
        velocity.z = 0.0f;
    } else {
        position.z = nextPosZ.z;
        position.y = nextPosZ.y;
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
                    AABB blockBox(glm::vec3(x, y, z), glm::vec3(x + 1.0f, y + 1.0f, z + 1.0f));
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
    int bx = static_cast<int>(std::floor(position.x));
    int by = static_cast<int>(std::floor(position.y));
    int bz = static_cast<int>(std::floor(position.z));

    BlockType railCheck = world.getBlock(bx, by, bz);
    bool onRail = (railCheck == BlockType::Rail || railCheck == BlockType::PoweredRail);

    if (onRail) {
        if (railCheck == BlockType::PoweredRail) {
            velocity *= 1.15f; // Powered speed boost
        }
        velocity.y = 0.0f;
    } else {
        velocity.y -= 15.0f * deltaTime; // Normal gravity off-rail
    }

    position += velocity * deltaTime;
    velocity.x *= 0.95f;
    velocity.z *= 0.95f;
}

void PhysicsEngine::updateBoat(World& world, glm::vec3& position, glm::vec3& velocity, float deltaTime) {
    bool onWater = isPointInWater(world, position);

    if (onWater) {
        velocity.y = 0.0f; // Buoyancy on surface
        position += velocity * deltaTime;
        velocity.x *= 0.92f;
        velocity.z *= 0.92f;
    } else {
        velocity.y -= 15.0f * deltaTime;
        position += velocity * deltaTime;
        velocity.x *= 0.70f;
        velocity.z *= 0.70f;
    }
}

}
