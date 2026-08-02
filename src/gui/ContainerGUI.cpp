#include "ContainerGUI.hpp"
#include <iostream>

namespace Minecraft {

ContainerGUI::ContainerGUI(int windowWidth, int windowHeight) 
    : m_WindowWidth(windowWidth), m_WindowHeight(windowHeight) {}

ContainerGUI::~ContainerGUI() = default;

void ContainerGUI::resize(int width, int height) {
    m_WindowWidth = width;
    m_WindowHeight = height;
}

void ContainerGUI::openChest(const glm::ivec3& pos, std::vector<ItemStack>* chestInv) {
    m_ActivePos = pos;
    m_ActiveChestInv = chestInv;
    m_ActiveContainer = ContainerType::Chest;
    std::cout << "[ContainerGUI] Opened 27-slot Chest GUI at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
}

void ContainerGUI::openFurnace(const glm::ivec3& pos, FurnaceData* furnaceData) {
    m_ActivePos = pos;
    m_ActiveFurnaceData = furnaceData;
    m_ActiveContainer = ContainerType::Furnace;
    std::cout << "[ContainerGUI] Opened Furnace GUI at (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
}

void ContainerGUI::close() {
    m_ActiveContainer = ContainerType::None;
    m_ActiveChestInv = nullptr;
    m_ActiveFurnaceData = nullptr;
}

void ContainerGUI::render(Inventory& playerInv) {
    if (!isOpen()) return;
    // Overlay rendering logic for Chest & Furnace GUI slots
}

bool ContainerGUI::handleMouseClick(Inventory& playerInv, double mouseX, double mouseY, int button) {
    if (!isOpen()) return false;

    // Simple slot transfer logic simulation
    if (m_ActiveContainer == ContainerType::Chest && m_ActiveChestInv) {
        if (!m_ActiveChestInv->empty() && !(*m_ActiveChestInv)[0].isEmpty()) {
            playerInv.addItem((*m_ActiveChestInv)[0].type, (*m_ActiveChestInv)[0].count);
            (*m_ActiveChestInv)[0].clear();
            return true;
        }
    }
    return false;
}

}
