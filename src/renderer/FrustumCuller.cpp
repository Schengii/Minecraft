#include "FrustumCuller.hpp"
#include <cmath>

namespace Minecraft {

void FrustumCuller::update(const glm::mat4& vp) {
    // Left plane
    m_Planes[0].normal = glm::vec3(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0]);
    m_Planes[0].distance = vp[3][3] + vp[3][0];

    // Right plane
    m_Planes[1].normal = glm::vec3(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0]);
    m_Planes[1].distance = vp[3][3] - vp[3][0];

    // Bottom plane
    m_Planes[2].normal = glm::vec3(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1]);
    m_Planes[2].distance = vp[3][3] + vp[3][1];

    // Top plane
    m_Planes[3].normal = glm::vec3(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1]);
    m_Planes[3].distance = vp[3][3] - vp[3][1];

    // Near plane
    m_Planes[4].normal = glm::vec3(vp[0][3] + vp[0][2], vp[1][3] + vp[1][2], vp[2][3] + vp[2][2]);
    m_Planes[4].distance = vp[3][3] + vp[3][2];

    // Far plane
    m_Planes[5].normal = glm::vec3(vp[0][3] - vp[0][2], vp[1][3] - vp[1][2], vp[2][3] - vp[2][2]);
    m_Planes[5].distance = vp[3][3] - vp[3][2];

    // Normalize planes
    for (auto& plane : m_Planes) {
        float len = glm::length(plane.normal);
        if (len > 0.0001f) {
            plane.normal /= len;
            plane.distance /= len;
        }
    }
}

bool FrustumCuller::isBoxVisible(const glm::vec3& min, const glm::vec3& max) const {
    for (const auto& plane : m_Planes) {
        glm::vec3 positive = min;
        if (plane.normal.x >= 0) positive.x = max.x;
        if (plane.normal.y >= 0) positive.y = max.y;
        if (plane.normal.z >= 0) positive.z = max.z;

        if (plane.distanceToPoint(positive) < 0.0f) {
            return false;
        }
    }
    return true;
}

}
