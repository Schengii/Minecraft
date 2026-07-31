#ifndef RAYCAST_HPP
#define RAYCAST_HPP

#include <glm/glm.hpp>

namespace Minecraft {

class World;

struct RaycastResult {
    bool hit = false;
    glm::ivec3 blockPos{ 0 };
    glm::ivec3 previousPos{ 0 };
    glm::vec3 normal{ 0.0f };
    float distance = 0.0f;
};

class Raycast {
public:
    static RaycastResult raycast(World& world, const glm::vec3& origin, const glm::vec3& direction, float maxDistance = 6.0f);
};

}

#endif // RAYCAST_HPP
