#include "EntityRenderer.hpp"
#include "TextureAtlas.hpp"
#include "../gui/FontRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <cmath>

namespace Minecraft {

EntityRenderer& EntityRenderer::getInstance() {
    static EntityRenderer instance;
    return instance;
}

EntityRenderer::EntityRenderer() {
    init();
}

EntityRenderer::~EntityRenderer() {
    if (m_CubeVAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_CubeVAO);
    if (m_CubeVBO && glDeleteBuffers) glDeleteBuffers(1, &m_CubeVBO);
}

void EntityRenderer::init() {
    if (m_IsInitialized || !glCreateShader || !glGenVertexArrays) return;

    m_EntityShader = std::make_unique<Shader>("assets/shaders/block.vert", "assets/shaders/block.frag");
    initCubeBuffers();
    m_IsInitialized = true;
}

void EntityRenderer::initCubeBuffers() {
    float cubeVertices[] = {
        // Front face (+Z)
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,  1.0f, 1.0f,

        // Back face (-Z)
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,  1.0f, 1.0f,

        // Left face (-X)
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f,

        // Right face (+X)
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,

        // Top face (+Y)
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,

        // Bottom face (-Y)
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,  1.0f, 1.0f
    };

    if (glGenVertexArrays && glGenBuffers) {
        glGenVertexArrays(1, &m_CubeVAO);
        glGenBuffers(1, &m_CubeVBO);

        glBindVertexArray(m_CubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

        GLsizei stride = 10 * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(9 * sizeof(float)));

        glBindVertexArray(0);
    }
}

void EntityRenderer::renderBox(const glm::mat4& viewProj, const glm::vec3& position, 
                               const glm::vec3& rotation, const glm::vec3& scale, 
                               const glm::vec3& pivot, const glm::vec4& color, 
                               BlockType texType) {
    (void)texType;
    if (m_CubeVAO == 0 || !m_EntityShader) return;

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
    model = glm::translate(model, -pivot);
    model = glm::scale(model, scale);

    m_EntityShader->setMat4("u_Model", model);

    glBindVertexArray(m_CubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}

void EntityRenderer::renderHumanoid(const glm::mat4& viewProj, const Mob& mob, const glm::vec4& skinColor, const glm::vec4& clothesColor) {
    float swing = std::sin(mob.limbSwing) * 35.0f;
    glm::vec3 pos = mob.position;

    // Head
    renderBox(viewProj, pos + glm::vec3(0.0f, 1.6f, 0.0f), glm::vec3(mob.pitch, mob.yaw, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f), skinColor);

    // Body
    renderBox(viewProj, pos + glm::vec3(0.0f, 1.05f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.5f, 0.7f, 0.25f), glm::vec3(0.0f), clothesColor);

    // Arms
    if (mob.type == MobType::Zombie) {
        renderBox(viewProj, pos + glm::vec3(-0.35f, 1.25f, 0.0f), glm::vec3(-90.0f, mob.yaw, 0.0f), glm::vec3(0.2f, 0.65f, 0.2f), glm::vec3(0.0f, 0.25f, 0.0f), skinColor);
        renderBox(viewProj, pos + glm::vec3(0.35f, 1.25f, 0.0f), glm::vec3(-90.0f, mob.yaw, 0.0f), glm::vec3(0.2f, 0.65f, 0.2f), glm::vec3(0.0f, 0.25f, 0.0f), skinColor);
    } else {
        renderBox(viewProj, pos + glm::vec3(-0.35f, 1.25f, 0.0f), glm::vec3(swing, mob.yaw, 0.0f), glm::vec3(0.2f, 0.65f, 0.2f), glm::vec3(0.0f, 0.25f, 0.0f), skinColor);
        renderBox(viewProj, pos + glm::vec3(0.35f, 1.25f, 0.0f), glm::vec3(-swing, mob.yaw, 0.0f), glm::vec3(0.2f, 0.65f, 0.2f), glm::vec3(0.0f, 0.25f, 0.0f), skinColor);
    }

    // Legs
    renderBox(viewProj, pos + glm::vec3(-0.15f, 0.45f, 0.0f), glm::vec3(-swing, mob.yaw, 0.0f), glm::vec3(0.22f, 0.7f, 0.22f), glm::vec3(0.0f, 0.35f, 0.0f), clothesColor * 0.8f);
    renderBox(viewProj, pos + glm::vec3(0.15f, 0.45f, 0.0f), glm::vec3(swing, mob.yaw, 0.0f), glm::vec3(0.22f, 0.7f, 0.22f), glm::vec3(0.0f, 0.35f, 0.0f), clothesColor * 0.8f);
}

void EntityRenderer::renderCreeper(const glm::mat4& viewProj, const Mob& mob) {
    float swing = std::sin(mob.limbSwing) * 28.0f;
    glm::vec3 pos = mob.position;
    glm::vec4 color = (mob.fuseTimer > 0.0f && static_cast<int>(mob.fuseTimer * 10) % 2 == 0) ? glm::vec4(1.0f, 1.0f, 1.0f, 1.0f) : glm::vec4(0.15f, 0.75f, 0.2f, 1.0f);

    renderBox(viewProj, pos + glm::vec3(0.0f, 1.4f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.5f, 0.5f, 0.5f), glm::vec3(0.0f), color);
    renderBox(viewProj, pos + glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.45f, 0.7f, 0.25f), glm::vec3(0.0f), color);
    renderBox(viewProj, pos + glm::vec3(-0.16f, 0.25f, 0.16f), glm::vec3(swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), color * 0.85f);
    renderBox(viewProj, pos + glm::vec3(0.16f, 0.25f, 0.16f), glm::vec3(-swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), color * 0.85f);
    renderBox(viewProj, pos + glm::vec3(-0.16f, 0.25f, -0.16f), glm::vec3(-swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), color * 0.85f);
    renderBox(viewProj, pos + glm::vec3(0.16f, 0.25f, -0.16f), glm::vec3(swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), color * 0.85f);
}

void EntityRenderer::renderQuadruped(const glm::mat4& viewProj, const Mob& mob, const glm::vec4& bodyColor) {
    float swing = std::sin(mob.limbSwing) * 30.0f;
    glm::vec3 pos = mob.position;

    renderBox(viewProj, pos + glm::vec3(0.0f, 0.65f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.6f, 0.5f, 0.9f), glm::vec3(0.0f), bodyColor);
    renderBox(viewProj, pos + glm::vec3(0.0f, 0.85f, 0.55f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.4f, 0.4f, 0.4f), glm::vec3(0.0f), bodyColor * 1.05f);
    renderBox(viewProj, pos + glm::vec3(-0.2f, 0.25f, 0.3f), glm::vec3(swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), bodyColor * 0.9f);
    renderBox(viewProj, pos + glm::vec3(0.2f, 0.25f, 0.3f), glm::vec3(-swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), bodyColor * 0.9f);
    renderBox(viewProj, pos + glm::vec3(-0.2f, 0.25f, -0.3f), glm::vec3(-swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), bodyColor * 0.9f);
    renderBox(viewProj, pos + glm::vec3(0.2f, 0.25f, -0.3f), glm::vec3(swing, mob.yaw, 0.0f), glm::vec3(0.18f, 0.45f, 0.18f), glm::vec3(0.0f), bodyColor * 0.9f);
}

void EntityRenderer::renderDragon(const glm::mat4& viewProj, const Mob& mob, float totalTime) {
    float wingFlap = std::sin(totalTime * 8.0f) * 45.0f;
    glm::vec3 pos = mob.position;
    glm::vec4 dragonColor(0.12f, 0.12f, 0.15f, 1.0f);

    renderBox(viewProj, pos, glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(1.5f, 1.2f, 3.5f), glm::vec3(0.0f), dragonColor);
    renderBox(viewProj, pos + glm::vec3(0.0f, 0.6f, 2.2f), glm::vec3(-15.0f, mob.yaw, 0.0f), glm::vec3(0.9f, 0.9f, 1.4f), glm::vec3(0.0f), dragonColor * 1.1f);
    renderBox(viewProj, pos + glm::vec3(-1.8f, 0.5f, 0.0f), glm::vec3(0.0f, mob.yaw, wingFlap), glm::vec3(2.5f, 0.15f, 1.8f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec4(0.2f, 0.1f, 0.25f, 1.0f));
    renderBox(viewProj, pos + glm::vec3(1.8f, 0.5f, 0.0f), glm::vec3(0.0f, mob.yaw, -wingFlap), glm::vec3(2.5f, 0.15f, 1.8f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec4(0.2f, 0.1f, 0.25f, 1.0f));
    renderBox(viewProj, pos + glm::vec3(0.0f, 0.0f, -2.5f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.6f, 0.6f, 2.5f), glm::vec3(0.0f), dragonColor);
}

void EntityRenderer::renderWither(const glm::mat4& viewProj, const Mob& mob, float totalTime) {
    float hover = std::sin(totalTime * 4.0f) * 0.15f;
    glm::vec3 pos = mob.position + glm::vec3(0.0f, hover, 0.0f);
    glm::vec4 witherColor(0.14f, 0.14f, 0.16f, 1.0f);

    // Central Body & Spine Ribcage
    renderBox(viewProj, pos + glm::vec3(0.0f, 1.1f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.9f, 0.35f, 0.35f), glm::vec3(0.0f), witherColor);
    renderBox(viewProj, pos + glm::vec3(0.0f, 0.6f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.35f, 0.7f, 0.25f), glm::vec3(0.0f), witherColor * 0.9f);
    renderBox(viewProj, pos + glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, mob.yaw, 0.0f), glm::vec3(0.25f, 0.45f, 0.2f), glm::vec3(0.0f), witherColor * 0.8f);

    // Central Head (0.6 x 0.6 x 0.6)
    renderBox(viewProj, pos + glm::vec3(0.0f, 1.65f, 0.0f), glm::vec3(mob.pitch, mob.yaw, 0.0f), glm::vec3(0.6f, 0.6f, 0.6f), glm::vec3(0.0f), witherColor * 1.1f);

    // Left Head (smaller, looking slightly outward)
    renderBox(viewProj, pos + glm::vec3(-0.65f, 1.5f, 0.0f), glm::vec3(mob.pitch, mob.yaw - 20.0f, 0.0f), glm::vec3(0.45f, 0.45f, 0.45f), glm::vec3(0.0f), witherColor * 1.05f);

    // Right Head (smaller, looking slightly outward)
    renderBox(viewProj, pos + glm::vec3(0.65f, 1.5f, 0.0f), glm::vec3(mob.pitch, mob.yaw + 20.0f, 0.0f), glm::vec3(0.45f, 0.45f, 0.45f), glm::vec3(0.0f), witherColor * 1.05f);
}

void EntityRenderer::render(const Camera& camera, const MobEngine& mobEngine, 
                            const ItemEntityManager& itemMgr, float totalTime,
                            const NetworkManager* netManager) {
    if (!m_EntityShader) return;

    m_EntityShader->use();
    glm::mat4 proj = camera.getProjectionMatrix(16.0f / 9.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 viewProj = proj * view;

    m_EntityShader->setMat4("u_Projection", proj);
    m_EntityShader->setMat4("u_View", view);
    m_EntityShader->setVec3("u_SunDirection", glm::vec3(0.4f, 0.8f, 0.3f));
    m_EntityShader->setVec3("u_SunColor", glm::vec3(1.0f, 0.95f, 0.9f));
    m_EntityShader->setVec3("u_SkyColor", glm::vec3(0.5f, 0.7f, 1.0f));
    m_EntityShader->setFloat("u_AmbientLight", 0.65f);
    m_EntityShader->setBool("u_IsUnderwater", false);
    m_EntityShader->setBool("u_HasHandheldLight", false);

    if (glActiveTexture) glActiveTexture(GL_TEXTURE0);
    TextureAtlas::getInstance().bind(0);
    m_EntityShader->setInt("u_Texture", 0);

    // 1. Render all Mobs
    for (const auto& mob : mobEngine.getMobs()) {
        glm::vec4 tint = (mob.hurtTime > 0.0f) ? glm::vec4(1.0f, 0.2f, 0.2f, 1.0f) : glm::vec4(1.0f);

        switch (mob.type) {
            case MobType::Zombie:
                renderHumanoid(viewProj, mob, glm::vec4(0.2f, 0.65f, 0.3f, 1.0f) * tint, glm::vec4(0.2f, 0.3f, 0.8f, 1.0f) * tint);
                break;
            case MobType::Skeleton:
                renderHumanoid(viewProj, mob, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f) * tint, glm::vec4(0.7f, 0.7f, 0.7f, 1.0f) * tint);
                break;
            case MobType::Creeper:
                renderCreeper(viewProj, mob);
                break;
            case MobType::Pig:
                renderQuadruped(viewProj, mob, glm::vec4(0.95f, 0.65f, 0.65f, 1.0f) * tint);
                break;
            case MobType::Cow:
                renderQuadruped(viewProj, mob, glm::vec4(0.45f, 0.35f, 0.25f, 1.0f) * tint);
                break;
            case MobType::Villager:
                renderHumanoid(viewProj, mob, glm::vec4(0.8f, 0.6f, 0.45f, 1.0f) * tint, glm::vec4(0.55f, 0.35f, 0.2f, 1.0f) * tint);
                break;
            case MobType::IronGolem:
                renderHumanoid(viewProj, mob, glm::vec4(0.85f, 0.85f, 0.8f, 1.0f) * tint, glm::vec4(0.8f, 0.8f, 0.75f, 1.0f) * tint);
                break;
            case MobType::EnderDragon:
                renderDragon(viewProj, mob, totalTime);
                break;
            case MobType::Wither:
                renderWither(viewProj, mob, totalTime);
                break;
        }
    }

    // 2. Render Dropped Items (spinning & floating)
    for (const auto& item : itemMgr.getItems()) {
        float bob = std::sin(totalTime * 4.0f) * 0.08f;
        glm::vec3 itemPos = item.position + glm::vec3(0.0f, 0.15f + bob, 0.0f);
        renderBox(viewProj, itemPos, glm::vec3(0.0f, item.rotation, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), glm::vec3(0.0f), glm::vec4(1.0f), item.itemType);
    }

    // 3. Render Flying Arrows
    for (const auto& arrow : mobEngine.getArrows()) {
        if (!arrow.active) continue;
        float yaw = glm::degrees(std::atan2(arrow.velocity.x, arrow.velocity.z));
        float pitch = glm::degrees(std::atan2(arrow.velocity.y, glm::length(glm::vec2(arrow.velocity.x, arrow.velocity.z))));
        renderBox(viewProj, arrow.position, glm::vec3(-pitch, yaw, 0.0f), glm::vec3(0.06f, 0.06f, 0.6f), glm::vec3(0.0f), glm::vec4(0.6f, 0.4f, 0.2f, 1.0f));
    }

    // 4. Render Wither Skulls
    for (const auto& skull : mobEngine.getWitherSkulls()) {
        if (!skull.active) continue;
        renderBox(viewProj, skull.position, glm::vec3(0.0f, totalTime * 90.0f, 0.0f), glm::vec3(0.35f, 0.35f, 0.35f), glm::vec3(0.0f), glm::vec4(0.1f, 0.1f, 0.12f, 1.0f));
    }

    // 5. Render Remote Multiplayer Players
    if (netManager) {
        renderRemotePlayers(camera, *netManager, totalTime);
    }

    TextureAtlas::getInstance().unbind();
}

void EntityRenderer::renderRemotePlayers(const Camera& camera, const NetworkManager& netManager, float totalTime) {
    glm::mat4 proj = camera.getProjectionMatrix(16.0f / 9.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 viewProj = proj * view;

    for (const auto& remote : netManager.getRemotePlayers()) {
        Mob playerMob;
        playerMob.position = remote.position;
        playerMob.yaw = remote.yaw;
        playerMob.pitch = remote.pitch;
        playerMob.limbSwing = totalTime * 4.0f;

        // Render Steve (cyan shirt, blue pants, skin head/arms)
        renderHumanoid(viewProj, playerMob, glm::vec4(0.85f, 0.65f, 0.5f, 1.0f), glm::vec4(0.15f, 0.65f, 0.75f, 1.0f));
    }
}

void EntityRenderer::renderFirstPersonHand(const Camera& camera, BlockType heldItem, 
                                          float swingProgress, float walkBobbing, float totalTime) {
    (void)totalTime;
    if (m_CubeVAO == 0 || !m_EntityShader) return;

    glDisable(GL_CULL_FACE);
    glClear(GL_DEPTH_BUFFER_BIT); // Always draw hand on top of scene

    m_EntityShader->use();
    glm::mat4 proj = camera.getProjectionMatrix(16.0f / 9.0f);
    glm::mat4 view = glm::mat4(1.0f); // Draw in view space

    m_EntityShader->setMat4("u_Projection", proj);
    m_EntityShader->setMat4("u_View", view);
    m_EntityShader->setVec3("u_SunDirection", glm::vec3(0.4f, 0.8f, 0.3f));
    m_EntityShader->setVec3("u_SunColor", glm::vec3(1.0f, 0.95f, 0.9f));
    m_EntityShader->setVec3("u_SkyColor", glm::vec3(0.5f, 0.7f, 1.0f));
    m_EntityShader->setFloat("u_AmbientLight", 0.75f);
    m_EntityShader->setBool("u_IsUnderwater", false);
    m_EntityShader->setBool("u_HasHandheldLight", false);

    if (glActiveTexture) glActiveTexture(GL_TEXTURE0);
    TextureAtlas::getInstance().bind(0);
    m_EntityShader->setInt("u_Texture", 0);

    // Calculate First-Person Hand & Item Position
    float bobX = std::sin(walkBobbing) * 0.03f;
    float bobY = std::abs(std::cos(walkBobbing)) * 0.03f;
    float swingAngle = std::sin(swingProgress * 3.14159f) * 45.0f;

    glm::vec3 handPos(0.42f + bobX, -0.38f + bobY - std::sin(swingProgress * 3.14159f) * 0.15f, -0.65f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, handPos);
    model = glm::rotate(model, glm::radians(-swingAngle), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(35.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(-25.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    if (heldItem != BlockType::Air) {
        model = glm::scale(model, glm::vec3(0.28f, 0.28f, 0.28f));
    } else {
        model = glm::scale(model, glm::vec3(0.18f, 0.55f, 0.18f));
    }

    m_EntityShader->setMat4("u_Model", model);

    glBindVertexArray(m_CubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);

    TextureAtlas::getInstance().unbind();
    glEnable(GL_CULL_FACE);
}

}
