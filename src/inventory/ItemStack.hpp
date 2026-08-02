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

    bool isEmpty() const {
        return type == BlockType::Air || count <= 0;
    }

    void clear() {
        type = BlockType::Air;
        count = 0;
        durability = -1;
        maxDurability = -1;
    }
};

}

#endif // ITEMSTACK_HPP
