#include "HUD.hpp"
#include "FontRenderer.hpp"
#include "../renderer/TextureAtlas.hpp"
#include "../inventory/Inventory.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <cmath>

namespace Minecraft {

HUD::HUD(int windowWidth, int windowHeight)
    : m_Width(windowWidth), m_Height(windowHeight)
{
    if (glCreateShader && glGenVertexArrays) {
        m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
        initBuffers();
        FontRenderer::getInstance().init();
    }
}

HUD::~HUD() {
    if (m_VAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO && glDeleteBuffers) glDeleteBuffers(1, &m_VBO);
}

void HUD::resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    FontRenderer::getInstance().resize(width, height);
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

void HUD::render(int selectedSlot, bool showDebugInfo, float fps, const glm::vec3& playerPos, 
                const glm::vec3& playerDir, bool isFlying, float health, float hunger, 
                const Inventory* inventory, const std::string& biomeName, int lightLevel) {
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

    // 3. Render Item icons & stack counts in hotbar slots
    if (inventory) {
        float slotSize = 44.0f;
        float padding = 4.0f;
        float totalWidth = 9 * slotSize + 8 * padding;
        float startX = (m_Width - totalWidth) / 2.0f;
        float startY = m_Height - slotSize - 15.0f;

        for (int i = 0; i < 9; ++i) {
            const ItemStack& stack = inventory->getSlot(i);
            if (!stack.isEmpty()) {
                float slotX = startX + i * (slotSize + padding);
                
                // Render item thumbnail icon
                glm::vec2 uv = TextureAtlas::getBlockUV(stack.type, Direction::TOP);
                float uvSize = 1.0f / 16.0f;

                m_UIShader->use();
                m_UIShader->setBool("u_UseTexture", true);
                if (glActiveTexture) glActiveTexture(GL_TEXTURE0);
                TextureAtlas::getInstance().bind(0);
                m_UIShader->setInt("u_Texture", 0);
                m_UIShader->setVec4("u_Color", glm::vec4(1.0f));

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(slotX + 6.0f, startY + 6.0f, 0.0f));
                model = glm::scale(model, glm::vec3(slotSize - 12.0f, slotSize - 12.0f, 1.0f));
                m_UIShader->setMat4("u_Projection", projection * model);

                // Update texture coordinates for the quad
                float quad[24] = {
                    0.0f, 1.0f, uv.x, uv.y + uvSize,
                    1.0f, 0.0f, uv.x + uvSize, uv.y,
                    0.0f, 0.0f, uv.x, uv.y,

                    0.0f, 1.0f, uv.x, uv.y + uvSize,
                    1.0f, 1.0f, uv.x + uvSize, uv.y + uvSize,
                    1.0f, 0.0f, uv.x + uvSize, uv.y
                };
                glBindVertexArray(m_VAO);
                glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(quad), quad);
                glDrawArrays(GL_TRIANGLES, 0, 6);

                // Reset standard quad buffer
                float defaultQuad[24] = {
                    0.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, 1.0f, 0.0f, 1.0f,
                    1.0f, 1.0f, 1.0f, 1.0f,
                    1.0f, 0.0f, 1.0f, 0.0f
                };
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(defaultQuad), defaultQuad);
                glBindVertexArray(0);
                TextureAtlas::getInstance().unbind();

                // Render item count number with FontRenderer if count > 1
                if (stack.count > 1) {
                    std::string countStr = std::to_string(stack.count);
                    FontRenderer::getInstance().renderText(countStr, slotX + slotSize - 14.0f, startY + slotSize - 14.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), true);
                }
            }
        }
    }

    // 4. F3 Debug Screen Overlay with Real Text
    if (showDebugInfo) {
        // Draw background box
        renderQuad(glm::vec2(10.0f, 10.0f), glm::vec2(380.0f, 160.0f), glm::vec4(0.0f, 0.0f, 0.0f, 0.70f));
        renderQuad(glm::vec2(12.0f, 12.0f), glm::vec2(376.0f, 2.0f), glm::vec4(0.2f, 0.85f, 0.3f, 0.9f));

        std::string facing = "North (-Z)";
        if (std::abs(playerDir.x) > std::abs(playerDir.z)) {
            facing = (playerDir.x > 0) ? "East (+X)" : "West (-X)";
        } else {
            facing = (playerDir.z > 0) ? "South (+Z)" : "North (-Z)";
        }

        std::ostringstream ss;
        FontRenderer::getInstance().renderText("Minecraft 1:1 C++ Voxel Engine (OpenGL 4.5)", 18.0f, 18.0f, 1.0f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));
        
        ss.str(""); ss << std::fixed << std::setprecision(1) << "FPS: " << fps << " (VSync: Off)";
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 32.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        ss.str(""); ss << std::fixed << std::setprecision(2) << "XYZ: " << playerPos.x << " / " << playerPos.y << " / " << playerPos.z;
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 46.0f, 1.0f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

        int bx = static_cast<int>(std::floor(playerPos.x));
        int by = static_cast<int>(std::floor(playerPos.y));
        int bz = static_cast<int>(std::floor(playerPos.z));
        int cx = bx >> 4;
        int cy = by >> 4;
        int cz = bz >> 4;
        ss.str(""); ss << "Block: " << bx << " " << by << " " << bz << " [Chunk " << cx << " " << cy << " " << cz << "]";
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 60.0f, 1.0f, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f));

        ss.str(""); ss << "Facing: " << facing;
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 74.0f, 1.0f, glm::vec4(0.85f, 0.85f, 0.85f, 1.0f));

        ss.str(""); ss << "Biome: " << biomeName;
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 88.0f, 1.0f, glm::vec4(0.4f, 0.9f, 0.5f, 1.0f));

        ss.str(""); ss << "Client Light: " << lightLevel << " (15 sky, " << lightLevel << " block)";
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 102.0f, 1.0f, glm::vec4(1.0f, 0.9f, 0.4f, 1.0f));

        ss.str(""); ss << "Flight Mode: " << (isFlying ? "ENABLED [Fly]" : "DISABLED [Physics]");
        FontRenderer::getInstance().renderText(ss.str(), 18.0f, 116.0f, 1.0f, isFlying ? glm::vec4(0.3f, 0.8f, 1.0f, 1.0f) : glm::vec4(0.9f, 0.5f, 0.2f, 1.0f));

        FontRenderer::getInstance().renderText("F3: Toggle HUD  F4: Cycle Time  E: Inventory", 18.0f, 134.0f, 1.0f, glm::vec4(0.6f, 0.6f, 0.6f, 1.0f));
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void HUD::renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    m_UIShader->use();
    m_UIShader->setVec3("u_PosOffset", glm::vec3(position, 0.0f));
    m_UIShader->setVec4("u_Color", color);
    m_UIShader->setBool("u_UseTexture", false);

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
