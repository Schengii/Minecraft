#ifndef PARTICLEENGINE_HPP
#define PARTICLEENGINE_HPP

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include "../renderer/Camera.hpp"
#include "../renderer/Shader.hpp"

namespace Minecraft {

struct Particle {
    glm::vec3 position{ 0.0f };
    glm::vec3 velocity{ 0.0f };
    glm::vec4 color{ 1.0f };
    float size = 0.1f;
    float life = 1.0f;
    float maxLife = 1.0f;
};

class ParticleEngine {
public:
    ParticleEngine();
    ~ParticleEngine();

    void init();
    void spawnBlockBreak(const glm::vec3& blockPos);
    void spawnPrecipitation(const glm::vec3& playerPos, bool isSnow);
    void spawnHitCrit(const glm::vec3& targetPos);
    void spawnExplosionDebris(const glm::vec3& explosionPos, int count = 25);
    void clear();
    void update(float deltaTime);
    void render(const Camera& camera);

    const std::vector<Particle>& getParticles() const { return m_Particles; }

private:
    void initBuffers();

    std::vector<Particle> m_Particles;
    std::unique_ptr<Shader> m_Shader;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    bool m_IsInitialized = false;
};

}

#endif // PARTICLEENGINE_HPP
