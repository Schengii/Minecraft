#ifndef MENU_GUI_HPP
#define MENU_GUI_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include "../renderer/Shader.hpp"

namespace Minecraft {

enum class MenuAction {
    None,
    StartNewWorld,
    LoadWorld,
    OpenSettings,
    CloseSettings,
    ResumeGame,
    QuitGame
};

class MenuGUI {
public:
    MenuGUI(int windowWidth, int windowHeight);
    ~MenuGUI();

    void resize(int width, int height);
    MenuAction renderMainMenu(double mouseX, double mouseY, bool mouseClicked, std::string& outSeed);
    MenuAction renderPauseMenu(double mouseX, double mouseY, bool mouseClicked);
    MenuAction renderSettingsMenu(double mouseX, double mouseY, bool mouseClicked, int& renderDistance, float& fov, bool& vsync);

private:
    void initBuffers();
    void renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    bool isHovered(const glm::vec2& pos, const glm::vec2& size, double mouseX, double mouseY);

    int m_Width;
    int m_Height;

    std::unique_ptr<Shader> m_UIShader;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};

}

#endif // MENU_GUI_HPP
