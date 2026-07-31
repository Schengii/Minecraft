#ifndef FRUSTUMCULLER_HPP
#define FRUSTUMCULLER_HPP

#include <glm/glm.hpp>
#include <array>

namespace Minecraft {

struct Plane {
    glm::vec3 normal{ 0.0f };
    float distance = 0.0f;

    float distanceToPoint(const glm::vec3& point) const {
        return glm::dot(normal, point) + distance;
    }
};

class FrustumCuller {
public:
    void update(const glm::mat4& viewProjection);
    bool isBoxVisible(const glm::vec3& min, const glm::vec3& max) const;

private:
    std::array<Plane, 6> m_Planes;
};

}

#endif // FRUSTUMCULLER_HPP
