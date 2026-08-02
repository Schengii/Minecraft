#ifndef CONTAINERGUI_HPP
#define CONTAINERGUI_HPP

#include <vector>
#include <glm/glm.hpp>
#include "../inventory/ItemStack.hpp"
#include "../inventory/Inventory.hpp"
#include "../world/ChestBlock.hpp"
#include "../world/FurnaceBlock.hpp"

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
    int m_WindowWidth = 1280;
    int m_WindowHeight = 720;
    ContainerType m_ActiveContainer = ContainerType::None;
    glm::ivec3 m_ActivePos{ 0 };

    std::vector<ItemStack>* m_ActiveChestInv = nullptr;
    FurnaceData* m_ActiveFurnaceData = nullptr;
    ItemStack m_HeldItem{ BlockType::Air, 0, 64 };
};

}

#endif // CONTAINERGUI_HPP
