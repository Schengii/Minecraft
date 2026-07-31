#include "PhysicsEngine.hpp"
#include "../world/World.hpp"
#include <cmath>

namespace Minecraft {

void PhysicsEngine::updatePlayer(World& world, glm::vec3& position, glm::vec3& velocity, bool& isGrounded, bool isFlying, float deltaTime) {
    if (isFlying) {
        position += velocity * deltaTime;
        velocity *= 0.85f; // Dampen flying velocity
        isGrounded = false;
        return;
    }

    // Apply gravity
    velocity.y -= 25.0f * deltaTime;

    // Movement X
    position.x += velocity.x * deltaTime;
    AABB boxX(position - glm::vec3(0.3f, 0.0f, 0.3f), position + glm::vec3(0.3f, 1.8f, 0.3f));
    if (checkCollision(world, boxX)) {
        position.x -= velocity.x * deltaTime;
        velocity.x = 0.0f;
    }

    // Movement Z
    position.z += velocity.z * deltaTime;
    AABB boxZ(position - glm::vec3(0.3f, 0.0f, 0.3f), position + glm::vec3(0.3f, 1.8f, 0.3f));
    if (checkCollision(world, boxZ)) {
        position.z -= velocity.z * deltaTime;
        velocity.z = 0.0f;
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
                if (type != BlockType::Air && type != BlockType::Water) {
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

}
