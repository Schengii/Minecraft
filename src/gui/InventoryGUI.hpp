#ifndef INVENTORYGUI_HPP
#define INVENTORYGUI_HPP

#include "../inventory/Inventory.hpp"
#include "../renderer/Shader.hpp"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <memory>

namespace Minecraft {

class InventoryGUI {
public:
    InventoryGUI(int windowWidth, int windowHeight);
    ~InventoryGUI();

    void resize(int width, int height);
    void render(Inventory& inventory, bool isOpen);
    void handleMouseClick(Inventory& inventory, double mouseX, double mouseY, int button);

private:
    void initBuffers();
    void renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);

    int m_Width;
    int m_Height;
    int m_SelectedSlotIndex = -1;

    std::unique_ptr<Shader> m_UIShader;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};

}

#endif // INVENTORYGUI_HPP
