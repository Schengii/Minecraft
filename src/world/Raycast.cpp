#include "Raycast.hpp"
#include "World.hpp"
#include <cmath>
#include <algorithm>

namespace Minecraft {

RaycastResult Raycast::raycast(World& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
    RaycastResult result;
    if (glm::length(direction) < 1e-6f) return result;

    glm::vec3 rayDir = glm::normalize(direction);

    glm::ivec3 currentPos(
        static_cast<int>(std::floor(origin.x)),
        static_cast<int>(std::floor(origin.y)),
        static_cast<int>(std::floor(origin.z))
    );

    int stepX = (rayDir.x > 0.0f) ? 1 : ((rayDir.x < 0.0f) ? -1 : 0);
    int stepY = (rayDir.y > 0.0f) ? 1 : ((rayDir.y < 0.0f) ? -1 : 0);
    int stepZ = (rayDir.z > 0.0f) ? 1 : ((rayDir.z < 0.0f) ? -1 : 0);

    float tDeltaX = (std::abs(rayDir.x) > 1e-6f) ? std::abs(1.0f / rayDir.x) : 1e30f;
    float tDeltaY = (std::abs(rayDir.y) > 1e-6f) ? std::abs(1.0f / rayDir.y) : 1e30f;
    float tDeltaZ = (std::abs(rayDir.z) > 1e-6f) ? std::abs(1.0f / rayDir.z) : 1e30f;

    float tMaxX = (stepX > 0) ? (std::floor(origin.x) + 1.0f - origin.x) * tDeltaX :
                  ((stepX < 0) ? (origin.x - std::floor(origin.x)) * tDeltaX : 1e30f);
    float tMaxY = (stepY > 0) ? (std::floor(origin.y) + 1.0f - origin.y) * tDeltaY :
                  ((stepY < 0) ? (origin.y - std::floor(origin.y)) * tDeltaY : 1e30f);
    float tMaxZ = (stepZ > 0) ? (std::floor(origin.z) + 1.0f - origin.z) * tDeltaZ :
                  ((stepZ < 0) ? (origin.z - std::floor(origin.z)) * tDeltaZ : 1e30f);

    // Initial check for block at origin
    BlockType startType = world.getBlock(currentPos.x, currentPos.y, currentPos.z);
    if (startType != BlockType::Air && startType != BlockType::Water) {
        result.hit = true;
        result.blockPos = currentPos;
        result.previousPos = currentPos;
        result.normal = -rayDir;
        result.distance = 0.0f;
        return result;
    }

    glm::ivec3 lastBlockPos = currentPos;
    glm::vec3 hitNormal(0.0f);
    float currentDist = 0.0f;

    while (currentDist <= maxDistance) {
        lastBlockPos = currentPos;

        if (tMaxX < tMaxY) {
            if (tMaxX < tMaxZ) {
                currentDist = tMaxX;
                tMaxX += tDeltaX;
                currentPos.x += stepX;
                hitNormal = glm::vec3(-stepX, 0.0f, 0.0f);
            } else {
                currentDist = tMaxZ;
                tMaxZ += tDeltaZ;
                currentPos.z += stepZ;
                hitNormal = glm::vec3(0.0f, 0.0f, -stepZ);
            }
        } else {
            if (tMaxY < tMaxZ) {
                currentDist = tMaxY;
                tMaxY += tDeltaY;
                currentPos.y += stepY;
                hitNormal = glm::vec3(0.0f, -stepY, 0.0f);
            } else {
                currentDist = tMaxZ;
                tMaxZ += tDeltaZ;
                currentPos.z += stepZ;
                hitNormal = glm::vec3(0.0f, 0.0f, -stepZ);
            }
        }

        if (currentDist > maxDistance) break;

        BlockType type = world.getBlock(currentPos.x, currentPos.y, currentPos.z);
        if (type != BlockType::Air && type != BlockType::Water) {
            result.hit = true;
            result.blockPos = currentPos;
            result.previousPos = lastBlockPos;
            result.normal = hitNormal;
            result.distance = currentDist;
            return result;
        }
    }

    return result;
}

}
