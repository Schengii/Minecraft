#ifndef HUD_HPP
#define HUD_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>
#include "../renderer/Shader.hpp"
#include "../world/Block.hpp"

namespace Minecraft {

class HUD {
public:
    HUD(int windowWidth, int windowHeight);
    ~HUD();

    void resize(int width, int height);
    void render(int selectedSlot, bool showDebugInfo, float fps, const glm::vec3& playerPos, const glm::vec3& playerDir, bool isFlying);

private:
    void initBuffers();
    void renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void renderCrosshair();
    void renderHotbar(int selectedSlot);

    int m_Width;
    int m_Height;

    std::unique_ptr<Shader> m_UIShader;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};

}

#endif // HUD_HPP
