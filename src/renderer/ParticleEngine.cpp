#include <glad/glad.h>
#include "ParticleEngine.hpp"
#include <cmath>
#include <cstdlib>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

ParticleEngine::ParticleEngine() {
    init();
}

ParticleEngine::~ParticleEngine() {
    if (m_VAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO && glDeleteBuffers) glDeleteBuffers(1, &m_VBO);
}

void ParticleEngine::init() {
    if (m_IsInitialized || !glCreateShader || !glGenVertexArrays) return;
    m_Shader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
    initBuffers();
    m_IsInitialized = true;
}

void ParticleEngine::initBuffers() {
    if (glGenVertexArrays && glGenBuffers) {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        // Pre-allocate buffer for dynamic particle billboard quads (max 1024 particles)
        glBufferData(GL_ARRAY_BUFFER, 1024 * 6 * 4 * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

        glEnableVertexAttribArray(0); // Pos (vec2)
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

        glEnableVertexAttribArray(1); // TexCoord (vec2)
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindVertexArray(0);
    }
}

void ParticleEngine::spawnBlockBreak(const glm::vec3& blockPos) {
    for (int i = 0; i < 16; ++i) {
        Particle p;
        float rx = (rand() % 100 / 100.0f - 0.5f) * 0.8f;
        float ry = (rand() % 100 / 100.0f) * 0.8f;
        float rz = (rand() % 100 / 100.0f - 0.5f) * 0.8f;

        p.position = blockPos + glm::vec3(0.5f) + glm::vec3(rx, ry, rz);
        p.velocity = glm::vec3(rx * 4.5f, 3.5f + ry * 2.5f, rz * 4.5f);
        p.color = glm::vec4(0.55f, 0.45f, 0.35f, 0.95f);
        p.size = 0.08f;
        p.life = 0.7f + (rand() % 40 / 100.0f);
        p.maxLife = p.life;
        m_Particles.push_back(p);
    }
}

void ParticleEngine::spawnPrecipitation(const glm::vec3& playerPos, bool isSnow) {
    for (int i = 0; i < 6; ++i) {
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
            p.color = glm::vec4(0.4f, 0.6f, 0.9f, 0.75f);
            p.size = 0.04f;
        }
        p.life = 1.2f;
        p.maxLife = 1.2f;
        m_Particles.push_back(p);
    }
}

void ParticleEngine::spawnHitCrit(const glm::vec3& targetPos) {
    for (int i = 0; i < 8; ++i) {
        Particle p;
        float rx = (rand() % 100 / 100.0f - 0.5f) * 0.6f;
        float ry = (rand() % 100 / 100.0f) * 0.6f;
        float rz = (rand() % 100 / 100.0f - 0.5f) * 0.6f;

        p.position = targetPos + glm::vec3(rx, ry + 1.0f, rz);
        p.velocity = glm::vec3(rx * 3.0f, 2.0f + ry * 2.0f, rz * 3.0f);
        p.color = glm::vec4(1.0f, 0.84f, 0.0f, 1.0f); // Golden spark
        p.size = 0.06f;
        p.life = 0.5f;
        p.maxLife = 0.5f;
        m_Particles.push_back(p);
    }
}

void ParticleEngine::spawnExplosionDebris(const glm::vec3& explosionPos, int count) {
    for (int i = 0; i < count; ++i) {
        Particle p;
        float rx = (rand() % 200 / 100.0f - 1.0f);
        float ry = (rand() % 200 / 100.0f - 0.5f);
        float rz = (rand() % 200 / 100.0f - 1.0f);

        p.position = explosionPos + glm::vec3(rx * 0.5f, ry * 0.5f, rz * 0.5f);
        p.velocity = glm::vec3(rx * 8.0f, 6.0f + ry * 5.0f, rz * 8.0f);
        p.color = (i % 2 == 0) ? glm::vec4(1.0f, 0.45f, 0.1f, 1.0f) : glm::vec4(0.3f, 0.3f, 0.3f, 0.9f); // Flame or smoke
        p.size = 0.12f;
        p.life = 0.8f + (rand() % 50 / 100.0f);
        p.maxLife = p.life;
        m_Particles.push_back(p);
    }
}

void ParticleEngine::clear() {
    m_Particles.clear();
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

void ParticleEngine::render(const Camera& camera) {
    if (m_Particles.empty() || m_VAO == 0 || !m_Shader) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    m_Shader->use();
    glm::mat4 proj = camera.getProjectionMatrix(16.0f / 9.0f);
    glm::mat4 view = camera.getViewMatrix();

    // Render using standard single billboard quad with transforms
    float quadVertices[24] = {
        -0.5f,  0.5f, 0.0f, 1.0f,
         0.5f, -0.5f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.0f, 0.0f,

        -0.5f,  0.5f, 0.0f, 1.0f,
         0.5f,  0.5f, 1.0f, 1.0f,
         0.5f, -0.5f, 1.0f, 0.0f
    };

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_DYNAMIC_DRAW);

    m_Shader->setBool("u_UseTexture", false);

    for (const auto& p : m_Particles) {
        float alpha = std::clamp(p.life / p.maxLife, 0.0f, 1.0f);
        glm::vec4 pColor = p.color;
        pColor.a *= alpha;

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, p.position);
        // Billboard: Face the camera directly
        model[0][0] = view[0][0]; model[0][1] = view[1][0]; model[0][2] = view[2][0];
        model[1][0] = view[0][1]; model[1][1] = view[1][1]; model[1][2] = view[2][1];
        model[2][0] = view[0][2]; model[2][1] = view[1][2]; model[2][2] = view[2][2];
        model = glm::scale(model, glm::vec3(p.size));

        m_Shader->setMat4("u_Projection", proj * view * model);
        m_Shader->setVec4("u_Color", pColor);

        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    glBindVertexArray(0);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

}
