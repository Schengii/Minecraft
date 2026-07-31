#ifndef BLOCK_HPP
#define BLOCK_HPP

#include <cstdint>
#include <glm/glm.hpp>

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
    Planks,
    CoalOre,
    IronOre,
    GoldOre,
    DiamondOre,
    Lava,
    CraftingTable,
    Stick,
    Cactus,
    Snow,
    BirchLog
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

    static bool isOpaque(BlockType type) {
        return type != BlockType::Air && type != BlockType::Glass && type != BlockType::Water && type != BlockType::Lava;
    }

    static bool isSolid(BlockType type) {
        return type != BlockType::Air && type != BlockType::Water && type != BlockType::Lava;
    }

    static glm::vec2 getTextureUV(BlockType type, Direction face) {
        int tileIndex = 0;

        switch (type) {
            case BlockType::Grass:
                if (face == TOP) tileIndex = 0;
                else if (face == BOTTOM) tileIndex = 2;
                else tileIndex = 1;
                break;
            case BlockType::Dirt:
                tileIndex = 2;
                break;
            case BlockType::Stone:
                tileIndex = 3;
                break;
            case BlockType::Bedrock:
                tileIndex = 4;
                break;
            case BlockType::OakLog:
                if (face == TOP || face == BOTTOM) tileIndex = 6;
                else tileIndex = 5;
                break;
            case BlockType::Leaves:
                tileIndex = 7;
                break;
            case BlockType::Sand:
                tileIndex = 8;
                break;
            case BlockType::Planks:
                tileIndex = 9;
                break;
            case BlockType::Glass:
                tileIndex = 10;
                break;
            case BlockType::CoalOre:
                tileIndex = 11;
                break;
            case BlockType::IronOre:
                tileIndex = 12;
                break;
            case BlockType::GoldOre:
                tileIndex = 13;
                break;
            case BlockType::DiamondOre:
                tileIndex = 14;
                break;
            case BlockType::Lava:
                tileIndex = 15;
                break;
            case BlockType::CraftingTable:
                tileIndex = 6;
                break;
            case BlockType::Stick:
                tileIndex = 9;
                break;
            case BlockType::Cactus:
                tileIndex = 7;
                break;
            case BlockType::Snow:
                tileIndex = 0;
                break;
            case BlockType::BirchLog:
                tileIndex = 5;
                break;
            default:
                tileIndex = 2;
                break;
        }

        float tileWidth = 1.0f / 16.0f;
        float u = (tileIndex % 16) * tileWidth;
        float v = (tileIndex / 16) * tileWidth;

        return glm::vec2(u, v);
    }
};

}

#endif // BLOCK_HPP
