#ifndef FONTRENDERER_HPP
#define FONTRENDERER_HPP

#include <string>
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include "../renderer/Shader.hpp"

namespace Minecraft {

class FontRenderer {
public:
    static FontRenderer& getInstance();
    ~FontRenderer();

    void init();
    void resize(int windowWidth, int windowHeight);

    // Renders 2D text at (x, y) with optional drop shadow
    void renderText(const std::string& text, float x, float y, float scale = 1.0f, 
                    const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), bool dropShadow = true);

    float getTextWidth(const std::string& text, float scale = 1.0f) const;
    float getTextHeight(float scale = 1.0f) const;

private:
    FontRenderer();
    void generateFontAtlas();

    GLuint m_FontTexture = 0;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    std::unique_ptr<Shader> m_Shader;

    int m_WindowWidth = 1280;
    int m_WindowHeight = 720;
    bool m_IsInitialized = false;
};

}

#endif // FONTRENDERER_HPP
