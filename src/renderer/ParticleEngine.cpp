#include "ParticleEngine.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>

namespace Minecraft {

ParticleEngine::ParticleEngine() = default;

void ParticleEngine::spawnBlockBreak(const glm::vec3& blockPos) {
    for (int i = 0; i < 12; ++i) {
        Particle p;
        float rx = (rand() % 100 / 100.0f - 0.5f) * 0.8f;
        float ry = (rand() % 100 / 100.0f) * 0.8f;
        float rz = (rand() % 100 / 100.0f - 0.5f) * 0.8f;

        p.position = blockPos + glm::vec3(0.5f) + glm::vec3(rx, ry, rz);
        p.velocity = glm::vec3(rx * 4.0f, 3.0f + ry * 2.0f, rz * 4.0f);
        p.color = glm::vec4(0.55f, 0.45f, 0.35f, 0.9f);
        p.size = 0.08f;
        p.life = 0.6f + (rand() % 40 / 100.0f);
        m_Particles.push_back(p);
    }
}

void ParticleEngine::spawnPrecipitation(const glm::vec3& playerPos, bool isSnow) {
    for (int i = 0; i < 4; ++i) {
        Particle p;
        float rx = (rand() % 1000 / 100.0f - 5.0f);
        float rz = (rand() % 1000 / 100.0f - 5.0f);

        p.position = playerPos + glm::vec3(rx, 12.0f, rz);
        if (isSnow) {
            p.velocity = glm::vec3((rand() % 100 / 100.0f - 0.5f) * 0.5f, -3.0f, (rand() % 100 / 100.0f - 0.5f) * 0.5f);
            p.color = glm::vec4(0.95f, 0.95f, 1.0f, 0.85f);
            p.size = 0.06f;
        } else {
            p.velocity = glm::vec3(0.0f, -14.0f, 0.0f);
            p.color = glm::vec4(0.4f, 0.6f, 0.9f, 0.7f);
            p.size = 0.04f;
        }
        p.life = 1.2f;
        m_Particles.push_back(p);
    }
}

void ParticleEngine::update(float deltaTime) {
    for (auto it = m_Particles.begin(); it != m_Particles.end(); ) {
        it->life -= deltaTime;
        if (it->life <= 0.0f) {
            it = m_Particles.erase(it);
        } else {
            it->velocity.y -= 9.81f * deltaTime; // Gravity
            it->position += it->velocity * deltaTime;
            ++it;
        }
    }
}

}
