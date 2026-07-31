#include "ToolSystem.hpp"

namespace Minecraft {

bool ToolSystem::isTool(BlockType type) {
    switch (type) {
        case BlockType::WoodPickaxe:
        case BlockType::StonePickaxe:
        case BlockType::IronPickaxe:
        case BlockType::DiamondPickaxe:
        case BlockType::WoodAxe:
        case BlockType::IronSword:
        case BlockType::DiamondSword:
            return true;
        default:
            return false;
    }
}

ToolInfo ToolSystem::getToolInfo(BlockType type) {
    switch (type) {
        case BlockType::WoodPickaxe:    return { ToolCategory::Pickaxe, ToolTier::Wood, 59, 2.0f };
        case BlockType::StonePickaxe:   return { ToolCategory::Pickaxe, ToolTier::Stone, 131, 4.0f };
        case BlockType::IronPickaxe:    return { ToolCategory::Pickaxe, ToolTier::Iron, 250, 6.0f };
        case BlockType::DiamondPickaxe: return { ToolCategory::Pickaxe, ToolTier::Diamond, 1561, 8.0f };
        case BlockType::WoodAxe:        return { ToolCategory::Axe, ToolTier::Wood, 59, 2.0f };
        case BlockType::IronSword:      return { ToolCategory::Sword, ToolTier::Iron, 250, 6.0f };
        case BlockType::DiamondSword:   return { ToolCategory::Sword, ToolTier::Diamond, 1561, 8.0f };
        default:                        return { ToolCategory::None, ToolTier::None, 0, 1.0f };
    }
}

float ToolSystem::getMiningSpeed(BlockType block, BlockType tool) {
    ToolInfo info = getToolInfo(tool);

    if (block == BlockType::Stone || block == BlockType::CoalOre || block == BlockType::IronOre || block == BlockType::GoldOre || block == BlockType::DiamondOre) {
        if (info.category == ToolCategory::Pickaxe) return info.speedMultiplier;
    } else if (block == BlockType::OakLog || block == BlockType::BirchLog || block == BlockType::Planks || block == BlockType::CraftingTable) {
        if (info.category == ToolCategory::Axe) return info.speedMultiplier;
    }

    return 1.0f;
}

bool ToolSystem::canHarvest(BlockType block, BlockType tool) {
    ToolInfo info = getToolInfo(tool);

    if (block == BlockType::DiamondOre || block == BlockType::GoldOre) {
        return info.category == ToolCategory::Pickaxe && static_cast<int>(info.tier) >= static_cast<int>(ToolTier::Iron);
    }
    if (block == BlockType::IronOre) {
        return info.category == ToolCategory::Pickaxe && static_cast<int>(info.tier) >= static_cast<int>(ToolTier::Stone);
    }
    if (block == BlockType::CoalOre || block == BlockType::Stone) {
        return info.category == ToolCategory::Pickaxe && static_cast<int>(info.tier) >= static_cast<int>(ToolTier::Wood);
    }

    return true;
}

int ToolSystem::getDamageDealt(BlockType tool) {
    ToolInfo info = getToolInfo(tool);
    if (info.category == ToolCategory::Sword) {
        if (info.tier == ToolTier::Diamond) return 7;
        if (info.tier == ToolTier::Iron) return 6;
    }
    return 1;
}

}
