#include "CraftingManager.hpp"

namespace Minecraft {

ItemStack CraftingManager::matchRecipe2x2(const std::array<ItemStack, 4>& grid) {
    int countLogs = 0;
    int countPlanks = 0;
    int totalItems = 0;

    for (int i = 0; i < 4; ++i) {
        if (!grid[i].isEmpty()) {
            totalItems++;
            if (grid[i].type == BlockType::OakLog) countLogs++;
            if (grid[i].type == BlockType::Planks) countPlanks++;
        }
    }

    // Recipe 1: 1 Oak Log -> 4 Planks
    if (totalItems == 1 && countLogs == 1) {
        return { BlockType::Planks, 4, 64 };
    }

    // Recipe 2: 4 Planks -> 1 CraftingTable
    if (totalItems == 4 && countPlanks == 4) {
        return { BlockType::CraftingTable, 1, 64 };
    }

    // Recipe 3: 2 Planks (vertically stacked) -> 4 Sticks
    if (totalItems == 2 && countPlanks == 2) {
        if ((!grid[0].isEmpty() && !grid[2].isEmpty()) || (!grid[1].isEmpty() && !grid[3].isEmpty())) {
            return { BlockType::Stick, 4, 64 };
        }
    }

    return { BlockType::Air, 0, 64 };
}

}
