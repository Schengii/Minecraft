#include "HUD.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace Minecraft {

HUD::HUD(int windowWidth, int windowHeight)
    : m_Width(windowWidth), m_Height(windowHeight)
{
    m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
    initBuffers();
}

HUD::~HUD() {
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
}

void HUD::resize(int width, int height) {
    m_Width = width;
    m_Height = height;
}

void HUD::initBuffers() {
    float quadVertices[] = {
        // Pos      // TexCoords
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

void HUD::render(int selectedSlot, bool showDebugInfo, float fps, const glm::vec3& playerPos, const glm::vec3& playerDir, bool isFlying, float health, float hunger) {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    m_UIShader->use();
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
    m_UIShader->setMat4("u_Projection", projection);

    // 1. Crosshair in screen center
    renderCrosshair();

    // 2. Hotbar at bottom center with health & hunger bars
    renderHotbar(selectedSlot, health, hunger);

    // 3. F3 Debug Screen Overlay
    if (showDebugInfo) {
        // Draw Debug Box Background Top-Left
        renderQuad(glm::vec2(10.0f, 10.0f), glm::vec2(340.0f, 140.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.65f));

        // Draw Debug Status Bar Indicators
        renderQuad(glm::vec2(15.0f, 15.0f), glm::vec2(330.0f, 4.0f), glm::vec4(0.2f, 0.8f, 0.2f, 0.9f));
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void HUD::renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    m_UIShader->use();
    m_UIShader->setVec3("u_PosOffset", glm::vec3(position, 0.0f));
    m_UIShader->setVec4("u_Color", color);
    m_UIShader->setBool("u_UseTexture", false);

    // Calculate projection scale matrix for quad
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));

    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_Width), static_cast<float>(m_Height), 0.0f, -1.0f, 1.0f);
    m_UIShader->setMat4("u_Projection", projection * model);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void HUD::renderCrosshair() {
    float centerX = m_Width / 2.0f;
    float centerY = m_Height / 2.0f;
    float thickness = 2.0f;
    float length = 12.0f;

    glm::vec4 crossColor(1.0f, 1.0f, 1.0f, 0.9f);

    // Vertical line
    renderQuad(glm::vec2(centerX - thickness / 2.0f, centerY - length / 2.0f), glm::vec2(thickness, length), crossColor);
    // Horizontal line
    renderQuad(glm::vec2(centerX - length / 2.0f, centerY - thickness / 2.0f), glm::vec2(length, thickness), crossColor);
}

void HUD::renderHotbar(int selectedSlot, float health, float hunger) {
    float slotSize = 44.0f;
    float padding = 4.0f;
    float totalWidth = 9 * slotSize + 8 * padding;
    float startX = (m_Width - totalWidth) / 2.0f;
    float startY = m_Height - slotSize - 15.0f;

    // 1. Health Bar (10 Red Hearts representing 20 HP)
    float heartY = startY - 20.0f;
    for (int h = 0; h < 10; ++h) {
        float hx = startX + h * 16.0f;
        float heartHP = (h + 1) * 2.0f;
        glm::vec4 heartColor(0.2f, 0.2f, 0.2f, 0.5f); // Empty heart

        if (health >= heartHP) {
            heartColor = glm::vec4(0.9f, 0.15f, 0.15f, 0.95f); // Full red heart
        } else if (health >= heartHP - 1.0f) {
            heartColor = glm::vec4(0.6f, 0.1f, 0.1f, 0.9f); // Half heart
        }
        renderQuad(glm::vec2(hx, heartY), glm::vec2(12.0f, 12.0f), heartColor);
    }

    // 2. Hunger Bar (10 Food Drumsticks representing 20 Hunger)
    for (int f = 0; f < 10; ++f) {
        float fx = startX + totalWidth - (f + 1) * 16.0f;
        float drumstickValue = (f + 1) * 2.0f;
        glm::vec4 drumColor(0.2f, 0.2f, 0.2f, 0.5f); // Empty drumstick

        if (hunger >= drumstickValue) {
            drumColor = glm::vec4(0.75f, 0.45f, 0.15f, 0.95f); // Full drumstick
        } else if (hunger >= drumstickValue - 1.0f) {
            drumColor = glm::vec4(0.45f, 0.25f, 0.1f, 0.85f); // Half drumstick
        }
        renderQuad(glm::vec2(fx, heartY), glm::vec2(12.0f, 12.0f), drumColor);
    }

    // Hotbar background container
    renderQuad(glm::vec2(startX - 6.0f, startY - 6.0f), glm::vec2(totalWidth + 12.0f, slotSize + 12.0f), glm::vec4(0.1f, 0.1f, 0.1f, 0.7f));

    // Render 9 slots
    for (int i = 0; i < 9; ++i) {
        float x = startX + i * (slotSize + padding);
        glm::vec4 slotColor = (i == selectedSlot) ? glm::vec4(1.0f, 0.84f, 0.0f, 0.9f) : glm::vec4(0.3f, 0.3f, 0.3f, 0.6f);
        renderQuad(glm::vec2(x, startY), glm::vec2(slotSize, slotSize), slotColor);

        // Inner slot box
        renderQuad(glm::vec2(x + 2.0f, startY + 2.0f), glm::vec2(slotSize - 4.0f, slotSize - 4.0f), glm::vec4(0.2f, 0.2f, 0.2f, 0.8f));
    }

    // Active Selection Border Frame
    float activeX = startX + selectedSlot * (slotSize + padding);
    renderQuad(glm::vec2(activeX - 3.0f, startY - 3.0f), glm::vec2(slotSize + 6.0f, slotSize + 6.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.95f));
    renderQuad(glm::vec2(activeX, startY), glm::vec2(slotSize, slotSize), glm::vec4(0.25f, 0.25f, 0.25f, 0.9f));
}

}
