#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <cstdint>
#include <array>

namespace Minecraft {

enum class BlockType : uint8_t {
    Air = 0,
    Grass,
    Dirt,
    Stone,
    Bedrock,
    OakLog,
    Leaves,
    Glass,
    Water,
    Sand,
    Planks
};

enum Direction {
    TOP = 0,
    BOTTOM,
    NORTH,
    SOUTH,
    EAST,
    WEST
};

struct BlockData {
    BlockType type = BlockType::Air;
    uint8_t lightLevel = 15;

    bool isOpaque() const {
        return type != BlockType::Air && type != BlockType::Glass && type != BlockType::Water;
    }

    bool isSolid() const {
        return type != BlockType::Air && type != BlockType::Water;
    }
};

}

#endif // BLOCK_HPP
