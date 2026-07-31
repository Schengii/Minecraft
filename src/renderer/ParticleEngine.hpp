#ifndef PARTICLEENGINE_HPP
#define PARTICLEENGINE_HPP

#include <glm/glm.hpp>
#include <vector>

namespace Minecraft {

struct Particle {
    glm::vec3 position{ 0.0f };
    glm::vec3 velocity{ 0.0f };
    glm::vec4 color{ 1.0f };
    float size = 0.1f;
    float life = 1.0f;
};

class ParticleEngine {
public:
    ParticleEngine();

    void spawnBlockBreak(const glm::vec3& blockPos);
    void spawnPrecipitation(const glm::vec3& playerPos, bool isSnow);
    void update(float deltaTime);

    const std::vector<Particle>& getParticles() const { return m_Particles; }

private:
    std::vector<Particle> m_Particles;
};

}

#endif // PARTICLEENGINE_HPP
