#include "EnchantingEngine.hpp"
#include "World.hpp"
#include "ToolSystem.hpp"
#include <algorithm>

namespace Minecraft {

int EnchantingEngine::countNearbyBookshelves(World& world, int tableX, int tableY, int tableZ) {
    int count = 0;
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            for (int dy = 0; dy <= 1; ++dy) {
                if (abs(dx) == 2 || abs(dz) == 2) {
                    if (world.getBlock(tableX + dx, tableY + dy, tableZ + dz) == BlockType::Bookshelf) {
                        count++;
                    }
                }
            }
        }
    }
    return std::min(15, count);
}

std::vector<EnchantmentOption> EnchantingEngine::getEnchantmentOptions(const ItemStack& item, int bookshelfCount) {
    std::vector<EnchantmentOption> options;
    if (item.isEmpty()) return options;

    int maxTier = 1 + bookshelfCount / 4; // Up to Tier 4-5 with 15 bookshelves

    if (item.type == BlockType::IronSword || item.type == BlockType::DiamondSword) {
        options.push_back({ Enchantment::Sharpness, std::min(5, maxTier), 1, 0 });
        options.push_back({ Enchantment::Unbreaking, std::min(3, maxTier), 2, 4 });
    } else if (ToolSystem::isTool(item.type)) {
        options.push_back({ Enchantment::Efficiency, std::min(5, maxTier), 1, 0 });
        options.push_back({ Enchantment::Unbreaking, std::min(3, maxTier), 2, 4 });
    } else {
        // Armor (Helmet, Chestplate, Leggings, Boots represented with Pickaxe durability slots)
        options.push_back({ Enchantment::Protection, std::min(4, maxTier), 1, 0 });
        options.push_back({ Enchantment::Unbreaking, std::min(3, maxTier), 2, 4 });
    }

    return options;
}

bool EnchantingEngine::applyEnchantment(ItemStack& item, Enchantment ench, int level) {
    if (item.isEmpty()) return false;
    item.enchantmentType = static_cast<int>(ench);
    item.enchantmentLevel = level;
    return true;
}

float EnchantingEngine::getEnchantedDamageBonus(const ItemStack& weapon) {
    if (weapon.enchantmentType == static_cast<int>(Enchantment::Sharpness)) {
        return weapon.enchantmentLevel * 1.5f;
    }
    return 0.0f;
}

float EnchantingEngine::getEnchantedSpeedBonus(const ItemStack& tool) {
    if (tool.enchantmentType == static_cast<int>(Enchantment::Efficiency)) {
        return tool.enchantmentLevel * 0.35f;
    }
    return 0.0f;
}

float EnchantingEngine::getEnchantedDefenseBonus(const ItemStack& armor) {
    if (armor.enchantmentType == static_cast<int>(Enchantment::Protection)) {
        return armor.enchantmentLevel * 0.04f; // +4% protection per level
    }
    return 0.0f;
}

}
