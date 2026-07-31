#include "Inventory.hpp"
#include "../crafting/CraftingManager.hpp"
#include <iostream>

namespace Minecraft {

Inventory::Inventory() {
    // Fill hotbar with default blocks for creative play
    m_Slots[0] = { BlockType::Grass, 64, 64 };
    m_Slots[1] = { BlockType::Dirt, 64, 64 };
    m_Slots[2] = { BlockType::Stone, 64, 64 };
    m_Slots[3] = { BlockType::OakLog, 64, 64 };
    m_Slots[4] = { BlockType::Leaves, 64, 64 };
    m_Slots[5] = { BlockType::Planks, 64, 64 };
    m_Slots[6] = { BlockType::Glass, 64, 64 };
    m_Slots[7] = { BlockType::Sand, 64, 64 };
    m_Slots[8] = { BlockType::Bedrock, 64, 64 };
}

ItemStack& Inventory::getSlot(int index) {
    if (index < 0 || index >= 36) return m_Slots[0];
    return m_Slots[index];
}

const ItemStack& Inventory::getSlot(int index) const {
    if (index < 0 || index >= 36) return m_Slots[0];
    return m_Slots[index];
}

bool Inventory::addItem(BlockType type, int count) {
    if (type == BlockType::Air) return false;

    // Try stacking into existing non-full slots
    for (int i = 0; i < 36; ++i) {
        if (m_Slots[i].type == type && m_Slots[i].count < m_Slots[i].maxStackSize) {
            int addable = m_Slots[i].maxStackSize - m_Slots[i].count;
            int added = std::min(addable, count);
            m_Slots[i].count += added;
            count -= added;
            if (count <= 0) return true;
        }
    }

    // Put into first empty slot
    for (int i = 0; i < 36; ++i) {
        if (m_Slots[i].isEmpty()) {
            m_Slots[i].type = type;
            m_Slots[i].count = std::min(64, count);
            return true;
        }
    }

    return false;
}

void Inventory::setSlot(int index, BlockType type, int count) {
    if (index >= 0 && index < 36) {
        m_Slots[index].type = type;
        m_Slots[index].count = count;
    }
}

void Inventory::swapSlots(int indexA, int indexB) {
    if (indexA >= 0 && indexA < 36 && indexB >= 0 && indexB < 36) {
        std::swap(m_Slots[indexA], m_Slots[indexB]);
    }
}

ItemStack& Inventory::getCraftingInput(int index) {
    if (index < 0 || index >= 4) return m_CraftingInput[0];
    return m_CraftingInput[index];
}

ItemStack& Inventory::getCraftingOutput() {
    return m_CraftingOutput;
}

void Inventory::updateCraftingRecipe() {
    m_CraftingOutput = CraftingManager::matchRecipe2x2(m_CraftingInput);
}

}
