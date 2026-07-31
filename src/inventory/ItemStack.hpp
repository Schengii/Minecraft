#ifndef ITEMSTACK_HPP
#define ITEMSTACK_HPP

#include "../world/Block.hpp"

namespace Minecraft {

struct ItemStack {
    BlockType type = BlockType::Air;
    int count = 0;
    int maxStackSize = 64;

    bool isEmpty() const {
        return type == BlockType::Air || count <= 0;
    }

    void clear() {
        type = BlockType::Air;
        count = 0;
    }
};

}

#endif // ITEMSTACK_HPP
