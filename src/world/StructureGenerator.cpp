#include "StructureGenerator.hpp"
#include "World.hpp"

namespace Minecraft {

void StructureGenerator::generateTree(World& world, int x, int y, int z, bool isBirch) {
    int height = 4 + (rand() % 2);
    BlockType logType = isBirch ? BlockType::BirchLog : BlockType::OakLog;

    // Log Trunk
    for (int i = 0; i < height; ++i) {
        world.setBlock(x, y + i, z, logType);
    }

    // Leaves Canopy
    for (int lx = -2; lx <= 2; ++lx) {
        for (int lz = -2; lz <= 2; ++lz) {
            for (int ly = height - 2; ly <= height + 1; ++ly) {
                if (abs(lx) == 2 && abs(lz) == 2 && ly == height + 1) continue;
                if (world.getBlock(x + lx, y + ly, z + lz) == BlockType::Air) {
                    world.setBlock(x + lx, y + ly, z + lz, BlockType::Leaves);
                }
            }
        }
    }
}

void StructureGenerator::generateDungeon(World& world, int x, int y, int z) {
    // 5x5 Cobblestone Dungeon with Chest
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            for (int dy = 0; dy <= 3; ++dy) {
                if (dy == 0 || dy == 3 || abs(dx) == 2 || abs(dz) == 2) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Stone);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air);
                }
            }
        }
    }
    // Place Chest in center
    world.setBlock(x, y + 1, z, BlockType::Chest);
}

void StructureGenerator::generateDesertTemple(World& world, int x, int y, int z) {
    for (int dx = -3; dx <= 3; ++dx) {
        for (int dz = -3; dz <= 3; ++dz) {
            for (int dy = 0; dy <= 4; ++dy) {
                if (dy == 0 || abs(dx) == 3 || abs(dz) == 3) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Sand);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air);
                }
            }
        }
    }
    world.setBlock(x, y + 1, z, BlockType::TNT);
}

void StructureGenerator::generateNetherPortalFrame(World& world, int x, int y, int z) {
    // 4x5 Obsidian Portal Frame
    for (int px = 0; px < 4; ++px) {
        for (int py = 0; py < 5; ++py) {
            if (px == 0 || px == 3 || py == 0 || py == 4) {
                world.setBlock(x + px, y + py, z, BlockType::Obsidian);
            } else {
                world.setBlock(x + px, y + py, z, BlockType::NetherPortal);
            }
        }
    }
}

}
