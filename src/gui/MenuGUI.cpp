#include "MenuGUI.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace Minecraft {

MenuGUI::MenuGUI(int windowWidth, int windowHeight) 
    : m_Width(windowWidth), m_Height(windowHeight) {
    
    m_UIShader = std::make_unique<Shader>("assets/shaders/ui.vert", "assets/shaders/ui.frag");
    initBuffers();
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
}

void MenuGUI::renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    if (m_VAO == 0) return;

    m_UIShader->use();
    glm::mat4 projection = glm::ortho(0.0f, (float)m_Width, (float)m_Height, 0.0f, -1.0f, 1.0f);
    m_UIShader->setMat4("projection", projection);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(position, 0.0f));
    model = glm::scale(model, glm::vec3(size, 1.0f));
    m_UIShader->setMat4("model", model);
    m_UIShader->setVec4("color", color);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

bool MenuGUI::isHovered(const glm::vec2& pos, const glm::vec2& size, double mouseX, double mouseY) {
    return mouseX >= pos.x && mouseX <= (pos.x + size.x) && mouseY >= pos.y && mouseY <= (pos.y + size.y);
}

MenuAction MenuGUI::renderMainMenu(double mouseX, double mouseY, bool mouseClicked, std::string& outSeed) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    // Dark overlay background
    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2((float)m_Width, (float)m_Height), glm::vec4(0.1f, 0.12f, 0.15f, 0.95f));

    // Title banner
    glm::vec2 titlePos((m_Width - 400.0f) / 2.0f, 80.0f);
    renderQuad(titlePos, glm::vec2(400.0f, 60.0f), glm::vec4(0.2f, 0.5f, 0.2f, 0.9f));

    // Play Button
    glm::vec2 playPos((m_Width - 300.0f) / 2.0f, 200.0f);
    glm::vec2 btnSize(300.0f, 50.0f);
    bool playHovered = isHovered(playPos, btnSize, mouseX, mouseY);
    renderQuad(playPos, btnSize, playHovered ? glm::vec4(0.3f, 0.7f, 0.3f, 1.0f) : glm::vec4(0.2f, 0.4f, 0.2f, 1.0f));

    // Settings Button
    glm::vec2 setPos((m_Width - 300.0f) / 2.0f, 270.0f);
    bool setHovered = isHovered(setPos, btnSize, mouseX, mouseY);
    renderQuad(setPos, btnSize, setHovered ? glm::vec4(0.5f, 0.5f, 0.7f, 1.0f) : glm::vec4(0.3f, 0.3f, 0.5f, 1.0f));

    // Quit Button
    glm::vec2 quitPos((m_Width - 300.0f) / 2.0f, 340.0f);
    bool quitHovered = isHovered(quitPos, btnSize, mouseX, mouseY);
    renderQuad(quitPos, btnSize, quitHovered ? glm::vec4(0.8f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.5f, 0.2f, 0.2f, 1.0f));

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
    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2((float)m_Width, (float)m_Height), glm::vec4(0.0f, 0.0f, 0.0f, 0.6f));

    glm::vec2 btnSize(300.0f, 50.0f);

    // Resume Button
    glm::vec2 resPos((m_Width - 300.0f) / 2.0f, 180.0f);
    bool resHovered = isHovered(resPos, btnSize, mouseX, mouseY);
    renderQuad(resPos, btnSize, resHovered ? glm::vec4(0.3f, 0.7f, 0.3f, 1.0f) : glm::vec4(0.2f, 0.4f, 0.2f, 1.0f));

    // Settings Button
    glm::vec2 setPos((m_Width - 300.0f) / 2.0f, 250.0f);
    bool setHovered = isHovered(setPos, btnSize, mouseX, mouseY);
    renderQuad(setPos, btnSize, setHovered ? glm::vec4(0.5f, 0.5f, 0.7f, 1.0f) : glm::vec4(0.3f, 0.3f, 0.5f, 1.0f));

    // Save & Quit Button
    glm::vec2 quitPos((m_Width - 300.0f) / 2.0f, 320.0f);
    bool quitHovered = isHovered(quitPos, btnSize, mouseX, mouseY);
    renderQuad(quitPos, btnSize, quitHovered ? glm::vec4(0.8f, 0.3f, 0.3f, 1.0f) : glm::vec4(0.5f, 0.2f, 0.2f, 1.0f));

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

    renderQuad(glm::vec2(0.0f, 0.0f), glm::vec2((float)m_Width, (float)m_Height), glm::vec4(0.1f, 0.1f, 0.15f, 0.95f));

    glm::vec2 btnSize(300.0f, 50.0f);

    // Render Distance Toggle
    glm::vec2 rdPos((m_Width - 300.0f) / 2.0f, 180.0f);
    bool rdHovered = isHovered(rdPos, btnSize, mouseX, mouseY);
    renderQuad(rdPos, btnSize, rdHovered ? glm::vec4(0.4f, 0.4f, 0.6f, 1.0f) : glm::vec4(0.2f, 0.2f, 0.4f, 1.0f));

    // FOV Toggle
    glm::vec2 fovPos((m_Width - 300.0f) / 2.0f, 250.0f);
    bool fovHovered = isHovered(fovPos, btnSize, mouseX, mouseY);
    renderQuad(fovPos, btnSize, fovHovered ? glm::vec4(0.4f, 0.4f, 0.6f, 1.0f) : glm::vec4(0.2f, 0.2f, 0.4f, 1.0f));

    // Back Button
    glm::vec2 backPos((m_Width - 300.0f) / 2.0f, 350.0f);
    bool backHovered = isHovered(backPos, btnSize, mouseX, mouseY);
    renderQuad(backPos, btnSize, backHovered ? glm::vec4(0.7f, 0.4f, 0.4f, 1.0f) : glm::vec4(0.5f, 0.2f, 0.2f, 1.0f));

    glEnable(GL_DEPTH_TEST);

    if (mouseClicked) {
        if (rdHovered) {
            renderDistance = (renderDistance >= 16) ? 4 : renderDistance + 4;
        } else if (fovHovered) {
            fov = (fov >= 110.0f) ? 60.0f : fov + 15.0f;
        } else if (backHovered) {
            return MenuAction::CloseSettings;
        }
    }

    return MenuAction::None;
}

}
