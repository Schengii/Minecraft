#ifndef ENCHANTINGENGINE_HPP
#define ENCHANTINGENGINE_HPP

#include "../inventory/ItemStack.hpp"
#include <vector>
#include <string>

namespace Minecraft {

class World;

enum class Enchantment {
    None = 0,
    Sharpness = 1,    // Increases sword melee damage (+1.5 damage per tier)
    Efficiency = 2,   // Increases tool mining speed (+25% per tier)
    Protection = 3,   // Increases armor damage reduction (+4% per tier)
    Unbreaking = 4    // Reduces durability damage chance
};

struct EnchantmentOption {
    Enchantment type;
    int level = 1;
    int costLapisOrEmeralds = 1;
    int requiredBookshelves = 0;
};

class EnchantingEngine {
public:
    static int countNearbyBookshelves(World& world, int tableX, int tableY, int tableZ);
    static std::vector<EnchantmentOption> getEnchantmentOptions(const ItemStack& item, int bookshelfCount);
    static bool applyEnchantment(ItemStack& item, Enchantment ench, int level);
    static float getEnchantedDamageBonus(const ItemStack& weapon);
    static float getEnchantedSpeedBonus(const ItemStack& tool);
    static float getEnchantedDefenseBonus(const ItemStack& armor);
};

}

#endif // ENCHANTINGENGINE_HPP
