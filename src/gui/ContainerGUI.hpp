#ifndef CONTAINERGUI_HPP
#define CONTAINERGUI_HPP

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../inventory/ItemStack.hpp"
#include "../inventory/Inventory.hpp"
#include "../world/ChestBlock.hpp"
#include "../world/FurnaceBlock.hpp"
#include "../renderer/Shader.hpp"

namespace Minecraft {

enum class ContainerType {
    None,
    Chest,
    Furnace
};

class ContainerGUI {
public:
    ContainerGUI(int windowWidth, int windowHeight);
    ~ContainerGUI();

    void resize(int width, int height);
    void openChest(const glm::ivec3& pos, std::vector<ItemStack>* chestInv);
    void openFurnace(const glm::ivec3& pos, FurnaceData* furnaceData);
    void close();

    bool isOpen() const { return m_ActiveContainer != ContainerType::None; }
    ContainerType getActiveContainer() const { return m_ActiveContainer; }

    void render(Inventory& playerInv);
    bool handleMouseClick(Inventory& playerInv, double mouseX, double mouseY, int button);

private:
    void initBuffers();
    void renderQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
    void renderSlotItem(const ItemStack& stack, float x, float y, float size);

    int m_WindowWidth = 1280;
    int m_WindowHeight = 720;
    ContainerType m_ActiveContainer = ContainerType::None;
    glm::ivec3 m_ActivePos{ 0 };

    std::vector<ItemStack>* m_ActiveChestInv = nullptr;
    FurnaceData* m_ActiveFurnaceData = nullptr;
    int m_SelectedSlot = -1; // -1 none, 0..26 chest / 0..2 furnace, 100..135 player inv

    std::unique_ptr<Shader> m_UIShader;
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};

}

#endif // CONTAINERGUI_HPP
