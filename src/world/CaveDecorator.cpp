#include "CaveDecorator.hpp"
#include "World.hpp"

namespace Minecraft {

void CaveDecorator::decorateCaveColumn(World& world, int x, int z, int minY, int maxY) {
    for (int y = minY; y < maxY; ++y) {
        // Find cave floor
        if (world.getBlock(x, y, z) == BlockType::Air && world.getBlock(x, y - 1, z) == BlockType::Stone) {
            if (rand() % 25 == 0) {
                generateStalagmite(world, x, y, z, 2 + rand() % 3);
            }
        }
        // Find cave ceiling
        if (world.getBlock(x, y, z) == BlockType::Air && world.getBlock(x, y + 1, z) == BlockType::Stone) {
            if (rand() % 25 == 0) {
                generateStalactite(world, x, y, z, 2 + rand() % 3);
            }
        }
    }
}

void CaveDecorator::generateStalagmite(World& world, int x, int y, int z, int height) {
    for (int i = 0; i < height; ++i) {
        if (world.getBlock(x, y + i, z) == BlockType::Air) {
            world.setBlock(x, y + i, z, BlockType::Stone);
        }
    }
}

void CaveDecorator::generateStalactite(World& world, int x, int y, int z, int height) {
    for (int i = 0; i < height; ++i) {
        if (world.getBlock(x, y - i, z) == BlockType::Air) {
            world.setBlock(x, y - i, z, BlockType::Stone);
        }
    }
}

}
