#include "Raycast.hpp"
#include "World.hpp"
#include <cmath>

namespace Minecraft {

RaycastResult Raycast::raycast(World& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance) {
    RaycastResult result;

    glm::vec3 rayDir = glm::normalize(direction);
    float step = 0.05f;

    glm::vec3 currentPos = origin;
    glm::ivec3 lastBlockPos = glm::ivec3(std::floor(origin.x), std::floor(origin.y), std::floor(origin.z));

    for (float dist = 0.0f; dist <= maxDistance; dist += step) {
        currentPos = origin + rayDir * dist;

        glm::ivec3 blockPos(
            static_cast<int>(std::floor(currentPos.x)),
            static_cast<int>(std::floor(currentPos.y)),
            static_cast<int>(std::floor(currentPos.z))
        );

        BlockType type = world.getBlock(blockPos.x, blockPos.y, blockPos.z);
        if (type != BlockType::Air && type != BlockType::Water) {
            result.hit = true;
            result.blockPos = blockPos;
            result.previousPos = lastBlockPos;
            result.distance = dist;

            // Calculate hit face normal
            glm::vec3 diff = glm::vec3(blockPos) + glm::vec3(0.5f) - (currentPos - rayDir * step);
            if (std::abs(diff.x) > std::abs(diff.y) && std::abs(diff.x) > std::abs(diff.z)) {
                result.normal = glm::vec3(diff.x > 0 ? -1.0f : 1.0f, 0.0f, 0.0f);
            } else if (std::abs(diff.y) > std::abs(diff.z)) {
                result.normal = glm::vec3(0.0f, diff.y > 0 ? -1.0f : 1.0f, 0.0f);
            } else {
                result.normal = glm::vec3(0.0f, 0.0f, diff.z > 0 ? -1.0f : 1.0f);
            }

            return result;
        }

        lastBlockPos = blockPos;
    }

    return result;
}

}
