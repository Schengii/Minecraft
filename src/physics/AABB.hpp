#ifndef AABB_HPP
#define AABB_HPP

#include <glm/glm.hpp>

namespace Minecraft {

struct AABB {
    glm::vec3 min;
    glm::vec3 max;

    AABB(const glm::vec3& minVal, const glm::vec3& maxVal)
        : min(minVal), max(maxVal) {}

    bool intersects(const AABB& other) const {
        return (min.x <= other.max.x && max.x >= other.min.x) &&
               (min.y <= other.max.y && max.y >= other.min.y) &&
               (min.z <= other.max.z && max.z >= other.min.z);
    }
};

}

#endif // AABB_HPP
