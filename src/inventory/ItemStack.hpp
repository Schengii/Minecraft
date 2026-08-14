#ifndef ITEMSTACK_HPP
#define ITEMSTACK_HPP

#include "../world/Block.hpp"

namespace Minecraft {

struct ItemStack {
    BlockType type = BlockType::Air;
    int count = 0;
    int maxStackSize = 64;
    int durability = -1; // -1 means no durability limit
    int maxDurability = -1;
    int enchantmentLevel = 0; // 0 = non-enchanted, 1..5 = tier
    int enchantmentType = 0;  // 1 = Sharpness, 2 = Efficiency, 3 = Protection, 4 = Unbreaking

    bool isEmpty() const {
        return type == BlockType::Air || count <= 0;
    }

    void clear() {
        type = BlockType::Air;
        count = 0;
        durability = -1;
        maxDurability = -1;
        enchantmentLevel = 0;
        enchantmentType = 0;
    }
};

}

#endif // ITEMSTACK_HPP
