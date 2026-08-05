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

ItemStack CraftingManager::matchRecipe3x3(const std::array<ItemStack, 9>& grid) {
    int totalItems = 0;
    for (int i = 0; i < 9; ++i) {
        if (!grid[i].isEmpty()) totalItems++;
    }

    if (totalItems == 0) return { BlockType::Air, 0, 64 };

    // 1. Pickaxe Recipes (Top row 0,1,2 = Material; Center vertical 4,7 = Sticks)
    if (totalItems == 5 && grid[4].type == BlockType::Stick && grid[7].type == BlockType::Stick) {
        if (grid[0].type == BlockType::Planks && grid[1].type == BlockType::Planks && grid[2].type == BlockType::Planks) {
            return { BlockType::WoodPickaxe, 1, 1 };
        }
        if (grid[0].type == BlockType::Stone && grid[1].type == BlockType::Stone && grid[2].type == BlockType::Stone) {
            return { BlockType::StonePickaxe, 1, 1 };
        }
        if (grid[0].type == BlockType::IronOre && grid[1].type == BlockType::IronOre && grid[2].type == BlockType::IronOre) {
            return { BlockType::IronPickaxe, 1, 1 };
        }
        if (grid[0].type == BlockType::DiamondOre && grid[1].type == BlockType::DiamondOre && grid[2].type == BlockType::DiamondOre) {
            return { BlockType::DiamondPickaxe, 1, 1 };
        }
    }

    // 1b. Sword Recipes (Vertical column 1, 4 = Material, 7 = Stick)
    if (totalItems == 3 && grid[7].type == BlockType::Stick) {
        if (grid[1].type == BlockType::IronOre && grid[4].type == BlockType::IronOre) {
            return { BlockType::IronSword, 1, 1 };
        }
        if (grid[1].type == BlockType::DiamondOre && grid[4].type == BlockType::DiamondOre) {
            return { BlockType::DiamondSword, 1, 1 };
        }
    }

    // 1c. Axe Recipes (grid 0,1,3 = Planks, 4,7 = Stick)
    if (totalItems == 5 && grid[4].type == BlockType::Stick && grid[7].type == BlockType::Stick) {
        if (grid[0].type == BlockType::Planks && grid[1].type == BlockType::Planks && grid[3].type == BlockType::Planks) {
            return { BlockType::WoodAxe, 1, 1 };
        }
    }

    // 2. Chest Recipe (8 Planks around outer border, grid[4] empty)
    if (totalItems == 8 && grid[4].isEmpty()) {
        bool allPlanks = true;
        int indices[8] = { 0, 1, 2, 3, 5, 6, 7, 8 };
        for (int idx : indices) {
            if (grid[idx].type != BlockType::Planks) allPlanks = false;
        }
        if (allPlanks) return { BlockType::Chest, 1, 64 };
    }

    // 3. TNT Recipe (4 Sand + 1 RedstoneWire/Coal)
    if (totalItems >= 4 && grid[4].type == BlockType::RedstoneWire) {
        return { BlockType::TNT, 1, 64 };
    }

    // 4. Bread Recipe (3 Planks in horizontal row)
    if (totalItems == 3) {
        if (grid[3].type == BlockType::Planks && grid[4].type == BlockType::Planks && grid[5].type == BlockType::Planks) {
            return { BlockType::Bread, 1, 64 };
        }
    }

    // 5. Golden Apple Recipe (1 Apple in middle grid[4], surrounded by 8 GoldOre)
    if (totalItems == 9 && grid[4].type == BlockType::Apple) {
        bool surroundedByGold = true;
        int outerIndices[8] = { 0, 1, 2, 3, 5, 6, 7, 8 };
        for (int idx : outerIndices) {
            if (grid[idx].type != BlockType::GoldOre) surroundedByGold = false;
        }
        if (surroundedByGold) return { BlockType::GoldenApple, 1, 64 };
    }

    return { BlockType::Air, 0, 64 };
}

}
