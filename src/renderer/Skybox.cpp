#include <glad/glad.h>
#include "Skybox.hpp"
#include <vector>
#include <cmath>
#include <algorithm>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef GL_LEQUAL
#define GL_LEQUAL 0x0203
#endif
#ifndef GL_LESS
#define GL_LESS 0x0201
#endif

namespace Minecraft {

Skybox::Skybox() {
    float skyboxVertices[] = {
        // Positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    if (glGenVertexArrays && glGenBuffers) {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glBindVertexArray(0);
    }

    initCloudBuffers();
}

Skybox::~Skybox() {
    if (m_VAO != 0 && glDeleteVertexArrays) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }
    if (m_CloudVAO != 0 && glDeleteVertexArrays) {
        glDeleteVertexArrays(1, &m_CloudVAO);
        glDeleteBuffers(1, &m_CloudVBO);
    }
}

void Skybox::initCloudBuffers() {
    if (m_CloudsInitialized || !glCreateShader || !glGenVertexArrays) return;

    m_CloudShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");

    // Grid of cloud planes at Y=128
    const float CLOUD_EXTENT = 512.0f;
    float cloudQuad[] = {
        -CLOUD_EXTENT,  CLOUD_EXTENT, 0.0f, 1.0f,
         CLOUD_EXTENT, -CLOUD_EXTENT, 1.0f, 0.0f,
        -CLOUD_EXTENT, -CLOUD_EXTENT, 0.0f, 0.0f,

        -CLOUD_EXTENT,  CLOUD_EXTENT, 0.0f, 1.0f,
         CLOUD_EXTENT,  CLOUD_EXTENT, 1.0f, 1.0f,
         CLOUD_EXTENT, -CLOUD_EXTENT, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_CloudVAO);
    glGenBuffers(1, &m_CloudVBO);

    glBindVertexArray(m_CloudVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_CloudVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cloudQuad), cloudQuad, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
    m_CloudsInitialized = true;
}

void Skybox::render(const glm::mat4& view, const glm::mat4& projection, float timeOfDay) {
    (void)view; (void)projection; (void)timeOfDay;
    if (m_VAO == 0) return;

    glDepthFunc(GL_LEQUAL);
    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::renderClouds(const Camera& camera, float timeOfDay, float totalTime) {
    if (!m_CloudShader || m_CloudVAO == 0) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);

    m_CloudShader->use();
    glm::mat4 proj = camera.getProjectionMatrix(16.0f / 9.0f);
    glm::mat4 view = camera.getViewMatrix();

    // Drifting wind offset
    float driftX = totalTime * 1.5f;
    float driftZ = totalTime * 0.8f;
    glm::vec3 playerPos = camera.getPosition();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(playerPos.x + std::fmod(driftX, 32.0f), 128.0f, playerPos.z + std::fmod(driftZ, 32.0f)));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    m_CloudShader->setMat4("u_Projection", proj * view * model);
    m_CloudShader->setBool("u_UseTexture", false);

    // Calculate Sunset / Day / Night cloud tinting
    glm::vec4 cloudColor(1.0f, 1.0f, 1.0f, 0.65f);
    if (timeOfDay > 12000.0f && timeOfDay < 14000.0f) {
        // Sunset orange/gold
        cloudColor = glm::vec4(1.0f, 0.6f, 0.35f, 0.70f);
    } else if (timeOfDay >= 14000.0f && timeOfDay <= 22000.0f) {
        // Night dark indigo
        cloudColor = glm::vec4(0.2f, 0.25f, 0.4f, 0.50f);
    }

    m_CloudShader->setVec4("u_Color", cloudColor);

    glBindVertexArray(m_CloudVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
}

}
