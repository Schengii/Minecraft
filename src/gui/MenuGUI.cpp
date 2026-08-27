#include "MenuGUI.hpp"
#include "FontRenderer.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>

namespace Minecraft {

MenuGUI::MenuGUI(int windowWidth, int windowHeight) 
    : m_Width(windowWidth), m_Height(windowHeight) {
    
    m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
    initBuffers();
    FontRenderer::getInstance().init();
}

MenuGUI::~MenuGUI() {
    if (m_VAO != 0) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
    }
}

void MenuGUI::initBuffers() {
    float vertices[] = {
        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f,
        0.0f, 0.0f,  0.0f, 0.0f,

        0.0f, 1.0f,  0.0f, 1.0f,
        1.0f, 1.0f,  1.0f, 1.0f,
        1.0f, 0.0f,  1.0f, 0.0f
    };

    if (glGenVertexArrays) {
        glGenVertexArrays(1, &m_VAO);
        glGenBuffers(1, &m_VBO);

        glBindVertexArray(m_VAO);
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
}

void MenuGUI::resize(int width, int height) {
    m_Width = width;
    m_Height = height;
    FontRenderer::getInstance().resize(width, height);
}

void MenuGUI::renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    if (m_VAO == 0) return;

    m_UIShader->use();
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

bool MenuGUI::isHovered(const glm::vec2& pos, const glm::vec2& size, double mouseX, double mouseY) {
    return mouseX >= pos.x && mouseX <= (pos.x + size.x) && mouseY >= pos.y && mouseY <= (pos.y + size.y);
}

MenuAction MenuGUI::renderMainMenu(double mouseX, double mouseY, bool mouseClicked, std::string& outSeed) {
    (void)outSeed;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Dark overlay background
    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2((float)m_Width, (float)m_Height), glm::vec4(0.08f, 0.10f, 0.14f, 0.96f));

    // Title banner
    glm::vec2 titlePos((m_Width - 420.0f) / 2.0f, 70.0f);
    renderQuad(titlePos, glm::vec2(420.0f, 65.0f), glm::vec4(0.15f, 0.18f, 0.22f, 0.95f));
    renderQuad(titlePos + glm::vec2(2.0f), glm::vec2(416.0f, 61.0f), glm::vec4(0.20f, 0.24f, 0.30f, 0.95f));
    FontRenderer::getInstance().renderText("MINECRAFT C++", titlePos.x + 85.0f, titlePos.y + 18.0f, 2.2f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));
    FontRenderer::getInstance().renderText("1:1 Modern Voxel Engine", titlePos.x + 115.0f, titlePos.y + 44.0f, 1.0f, glm::vec4(0.7f, 0.8f, 0.9f, 1.0f));

    // Play Button
    glm::vec2 btnSize(320.0f, 48.0f);
    glm::vec2 playPos((m_Width - 320.0f) / 2.0f, 180.0f);
    bool playHovered = isHovered(playPos, btnSize, mouseX, mouseY);
    renderQuad(playPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), playHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(playPos, btnSize, playHovered ? glm::vec4(0.35f, 0.65f, 0.35f, 1.0f) : glm::vec4(0.25f, 0.45f, 0.25f, 1.0f));
    FontRenderer::getInstance().renderText("Singleplayer (Play World)", playPos.x + 45.0f, playPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // Settings Button
    glm::vec2 setPos((m_Width - 320.0f) / 2.0f, 245.0f);
    bool setHovered = isHovered(setPos, btnSize, mouseX, mouseY);
    renderQuad(setPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), setHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(setPos, btnSize, setHovered ? glm::vec4(0.4f, 0.45f, 0.65f, 1.0f) : glm::vec4(0.28f, 0.32f, 0.48f, 1.0f));
    FontRenderer::getInstance().renderText("Options / Settings", setPos.x + 75.0f, setPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // Quit Button
    glm::vec2 quitPos((m_Width - 320.0f) / 2.0f, 310.0f);
    bool quitHovered = isHovered(quitPos, btnSize, mouseX, mouseY);
    renderQuad(quitPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), quitHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(quitPos, btnSize, quitHovered ? glm::vec4(0.75f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.5f, 0.2f, 0.2f, 1.0f));
    FontRenderer::getInstance().renderText("Quit Game", quitPos.x + 115.0f, quitPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // Footer Info
    FontRenderer::getInstance().renderText("C++20 & OpenGL 4.5 Core Profile Engine", (m_Width - 280.0f) / 2.0f, m_Height - 30.0f, 1.0f, glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    glEnable(GL_DEPTH_TEST);

    if (mouseClicked) {
        if (playHovered) return MenuAction::StartNewWorld;
        if (setHovered) return MenuAction::OpenSettings;
        if (quitHovered) return MenuAction::QuitGame;
    }

    return MenuAction::None;
}

MenuAction MenuGUI::renderPauseMenu(double mouseX, double mouseY, bool mouseClicked) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Semi-transparent background
    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2((float)m_Width, (float)m_Height), glm::vec4(0.0f, 0.0f, 0.0f, 0.65f));

    FontRenderer::getInstance().renderText("Game Paused", (m_Width - 160.0f) / 2.0f, 110.0f, 1.8f, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    glm::vec2 btnSize(320.0f, 48.0f);

    // Resume Button
    glm::vec2 resPos((m_Width - 320.0f) / 2.0f, 170.0f);
    bool resHovered = isHovered(resPos, btnSize, mouseX, mouseY);
    renderQuad(resPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), resHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(resPos, btnSize, resHovered ? glm::vec4(0.35f, 0.65f, 0.35f, 1.0f) : glm::vec4(0.25f, 0.45f, 0.25f, 1.0f));
    FontRenderer::getInstance().renderText("Back to Game", resPos.x + 95.0f, resPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // Settings Button
    glm::vec2 setPos((m_Width - 320.0f) / 2.0f, 235.0f);
    bool setHovered = isHovered(setPos, btnSize, mouseX, mouseY);
    renderQuad(setPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), setHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(setPos, btnSize, setHovered ? glm::vec4(0.4f, 0.45f, 0.65f, 1.0f) : glm::vec4(0.28f, 0.32f, 0.48f, 1.0f));
    FontRenderer::getInstance().renderText("Options...", setPos.x + 110.0f, setPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // Save & Quit Button
    glm::vec2 quitPos((m_Width - 320.0f) / 2.0f, 300.0f);
    bool quitHovered = isHovered(quitPos, btnSize, mouseX, mouseY);
    renderQuad(quitPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), quitHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(quitPos, btnSize, quitHovered ? glm::vec4(0.75f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.5f, 0.2f, 0.2f, 1.0f));
    FontRenderer::getInstance().renderText("Save and Quit to Title", quitPos.x + 55.0f, quitPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    glEnable(GL_DEPTH_TEST);

    if (mouseClicked) {
        if (resHovered) return MenuAction::ResumeGame;
        if (setHovered) return MenuAction::OpenSettings;
        if (quitHovered) return MenuAction::QuitGame;
    }

    return MenuAction::None;
}

MenuAction MenuGUI::renderSettingsMenu(double mouseX, double mouseY, bool mouseClicked, int& renderDistance, float& fov, bool& vsync) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2((float)m_Width, (float)m_Height), glm::vec4(0.1f, 0.1f, 0.15f, 0.96f));

    FontRenderer::getInstance().renderText("Settings & Video Options", (m_Width - 280.0f) / 2.0f, 80.0f, 1.8f, glm::vec4(1.0f, 0.85f, 0.2f, 1.0f));

    glm::vec2 btnSize(340.0f, 48.0f);

    // Render Distance Toggle
    glm::vec2 rdPos((m_Width - 340.0f) / 2.0f, 160.0f);
    bool rdHovered = isHovered(rdPos, btnSize, mouseX, mouseY);
    renderQuad(rdPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), rdHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(rdPos, btnSize, rdHovered ? glm::vec4(0.4f, 0.4f, 0.6f, 1.0f) : glm::vec4(0.25f, 0.25f, 0.4f, 1.0f));
    std::ostringstream ssRD;
    ssRD << "Render Distance: " << renderDistance << " chunks";
    FontRenderer::getInstance().renderText(ssRD.str(), rdPos.x + 35.0f, rdPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // FOV Toggle
    glm::vec2 fovPos((m_Width - 340.0f) / 2.0f, 225.0f);
    bool fovHovered = isHovered(fovPos, btnSize, mouseX, mouseY);
    renderQuad(fovPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), fovHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(fovPos, btnSize, fovHovered ? glm::vec4(0.4f, 0.4f, 0.6f, 1.0f) : glm::vec4(0.25f, 0.25f, 0.4f, 1.0f));
    std::ostringstream ssFOV;
    ssFOV << "Field of View: " << static_cast<int>(fov);
    FontRenderer::getInstance().renderText(ssFOV.str(), fovPos.x + 75.0f, fovPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // VSync Toggle
    glm::vec2 vsyncPos((m_Width - 340.0f) / 2.0f, 290.0f);
    bool vsyncHovered = isHovered(vsyncPos, btnSize, mouseX, mouseY);
    renderQuad(vsyncPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), vsyncHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(vsyncPos, btnSize, vsyncHovered ? glm::vec4(0.4f, 0.4f, 0.6f, 1.0f) : glm::vec4(0.25f, 0.25f, 0.4f, 1.0f));
    std::string vsyncStr = vsync ? "VSync: ON" : "VSync: OFF";
    FontRenderer::getInstance().renderText(vsyncStr, vsyncPos.x + 105.0f, vsyncPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    // Done / Back Button
    glm::vec2 backPos((m_Width - 340.0f) / 2.0f, 365.0f);
    bool backHovered = isHovered(backPos, btnSize, mouseX, mouseY);
    renderQuad(backPos - glm::vec2(2.0f), btnSize + glm::vec2(4.0f), backHovered ? glm::vec4(1.0f, 1.0f, 1.0f, 0.9f) : glm::vec4(0.1f, 0.1f, 0.1f, 0.9f));
    renderQuad(backPos, btnSize, backHovered ? glm::vec4(0.7f, 0.4f, 0.4f, 1.0f) : glm::vec4(0.5f, 0.25f, 0.25f, 1.0f));
    FontRenderer::getInstance().renderText("Done", backPos.x + 140.0f, backPos.y + 16.0f, 1.2f, glm::vec4(1.0f));

    glEnable(GL_DEPTH_TEST);

    if (mouseClicked) {
        if (rdHovered) {
            renderDistance = (renderDistance >= 16) ? 4 : renderDistance + 4;
        } else if (fovHovered) {
            fov = (fov >= 110.0f) ? 60.0f : fov + 15.0f;
        } else if (vsyncHovered) {
            vsync = !vsync;
        } else if (backHovered) {
            return MenuAction::CloseSettings;
        }
    }

    return MenuAction::None;
}

}
