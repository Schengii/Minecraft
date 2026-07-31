#ifndef CRAFTINGMANAGER_HPP
#define CRAFTINGMANAGER_HPP

#include "../inventory/ItemStack.hpp"
#include <array>

namespace Minecraft {

class CraftingManager {
public:
    static ItemStack matchRecipe2x2(const std::array<ItemStack, 4>& grid);
    static ItemStack matchRecipe3x3(const std::array<ItemStack, 9>& grid);
};

}

#endif // CRAFTINGMANAGER_HPP
