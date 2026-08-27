#include "StructureGenerator.hpp"
#include "World.hpp"
#include <cmath>

namespace Minecraft {

void StructureGenerator::generateTree(World& world, int x, int y, int z, bool isBirch) {
    int height = 4 + (rand() % 2);
    BlockType logType = isBirch ? BlockType::BirchLog : BlockType::OakLog;

    for (int i = 0; i < height; ++i) {
        world.setBlock(x, y + i, z, logType);
    }

    for (int lx = -2; lx <= 2; ++lx) {
        for (int lz = -2; lz <= 2; ++lz) {
            for (int ly = height - 2; ly <= height + 1; ++ly) {
                if (std::abs(lx) == 2 && std::abs(lz) == 2 && ly == height + 1) continue;
                if (world.getBlock(x + lx, y + ly, z + lz) == BlockType::Air) {
                    world.setBlock(x + lx, y + ly, z + lz, BlockType::Leaves);
                }
            }
        }
    }
}

void StructureGenerator::generateDungeon(World& world, int x, int y, int z) {
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            for (int dy = 0; dy <= 3; ++dy) {
                if (dy == 0 || dy == 3 || std::abs(dx) == 2 || std::abs(dz) == 2) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Stone);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air);
                }
            }
        }
    }
    world.setBlock(x, y + 1, z, BlockType::Chest);
}

void StructureGenerator::generateDesertTemple(World& world, int x, int y, int z) {
    for (int dx = -3; dx <= 3; ++dx) {
        for (int dz = -3; dz <= 3; ++dz) {
            for (int dy = 0; dy <= 4; ++dy) {
                if (dy == 0 || std::abs(dx) == 3 || std::abs(dz) == 3) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Sand);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air);
                }
            }
        }
    }

    for (int by = 1; by <= 4; ++by) {
        world.setBlock(x, y - by, z, BlockType::Air);
    }
    world.setBlock(x, y - 5, z, BlockType::TNT);
    world.setBlock(x + 1, y - 5, z, BlockType::TNT);
    world.setBlock(x - 1, y - 5, z, BlockType::TNT);

    world.setBlock(x + 1, y - 4, z + 1, BlockType::Chest);
    world.setBlock(x - 1, y - 4, z + 1, BlockType::Chest);
    world.setBlock(x + 1, y - 4, z - 1, BlockType::Chest);
    world.setBlock(x - 1, y - 4, z - 1, BlockType::Chest);
}

void StructureGenerator::generateMineshaft(World& world, int startX, int startY, int startZ, int length) {
    for (int i = 0; i < length; ++i) {
        int z = startZ + i;

        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = 0; dy <= 2; ++dy) {
                world.setBlock(startX + dx, startY + dy, z, BlockType::Air);
            }
            world.setBlock(startX + dx, startY - 1, z, BlockType::Stone);
        }

        world.setBlock(startX, startY, z, BlockType::Rail);

        if (i % 4 == 0) {
            world.setBlock(startX - 1, startY, z, BlockType::OakLog);
            world.setBlock(startX - 1, startY + 1, z, BlockType::OakLog);
            world.setBlock(startX + 1, startY, z, BlockType::OakLog);
            world.setBlock(startX + 1, startY + 1, z, BlockType::OakLog);
            world.setBlock(startX - 1, startY + 2, z, BlockType::Planks);
            world.setBlock(startX, startY + 2, z, BlockType::Planks);
            world.setBlock(startX + 1, startY + 2, z, BlockType::Planks);
            world.setBlock(startX, startY + 1, z - 1, BlockType::RedstoneTorch);
        }
    }
}

void StructureGenerator::generateNetherFortressCorridor(World& world, int x, int y, int z, int length) {
    for (int i = 0; i < length; ++i) {
        int curZ = z + i;
        // 5-wide bridge with Netherrack base and Obsidian trim
        for (int dx = -2; dx <= 2; ++dx) {
            world.setBlock(x + dx, y - 1, curZ, BlockType::Obsidian);
            for (int dy = 0; dy <= 3; ++dy) {
                world.setBlock(x + dx, y + dy, curZ, BlockType::Air);
            }
            if (std::abs(dx) == 2) {
                world.setBlock(x + dx, y, curZ, BlockType::Netherrack); // Bridge railing
            }
        }

        // Support pillars every 5 blocks down into the abyss
        if (i % 5 == 0) {
            for (int py = 1; py <= 10; ++py) {
                world.setBlock(x - 2, y - py, curZ, BlockType::Netherrack);
                world.setBlock(x + 2, y - py, curZ, BlockType::Netherrack);
            }
            world.setBlock(x, y + 1, curZ, BlockType::Glowstone); // Overhead beacon
        }
    }
}

void StructureGenerator::generateNetherPortalFrame(World& world, int x, int y, int z) {
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

void StructureGenerator::generateStronghold(World& world, int x, int y, int z) {
    // 9x6x9 Stone Bricks Chamber
    for (int dx = 0; dx < 9; ++dx) {
        for (int dy = 0; dy < 6; ++dy) {
            for (int dz = 0; dz < 9; ++dz) {
                if (dx == 0 || dx == 8 || dz == 0 || dz == 8 || dy == 0 || dy == 5) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::StoneBricks);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air);
                }
            }
        }
    }

    // Central 3x3 Lava Pool at floor
    for (int dx = 3; dx <= 5; ++dx) {
        for (int dz = 3; dz <= 5; ++dz) {
            world.setBlock(x + dx, y, z + dz, BlockType::Lava);
        }
    }

    // End Portal Frame Rim at Y=1 around lava pool
    for (int dx = 2; dx <= 6; ++dx) {
        for (int dz = 2; dz <= 6; ++dz) {
            if ((dx == 2 || dx == 6 || dz == 2 || dz == 6) && !(dx == 2 && dz == 2) && !(dx == 2 && dz == 6) && !(dx == 6 && dz == 2) && !(dx == 6 && dz == 6)) {
                world.setBlock(x + dx, y + 1, z + dz, BlockType::EndPortalFrame);
            }
        }
    }

    // Silverfish Spawner and Iron Bars Archway
    world.setBlock(x + 4, y + 2, z + 1, BlockType::Spawner);
    world.setBlock(x + 3, y + 1, z, BlockType::IronBars);
    world.setBlock(x + 5, y + 1, z, BlockType::IronBars);
    world.setBlock(x + 1, y + 1, z + 1, BlockType::Chest);
}

void StructureGenerator::generateOceanRuin(World& world, int x, int y, int z) {
    // 5x4x5 Prismarine / Stone Brick underwater ruin
    for (int dx = 0; dx < 5; ++dx) {
        for (int dy = 0; dy < 4; ++dy) {
            for (int dz = 0; dz < 5; ++dz) {
                if ((dx == 0 || dx == 4 || dz == 0 || dz == 4) && (rand() % 4 != 0)) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::StoneBricks);
                } else if (dy == 0) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Sand);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Water);
                }
            }
        }
    }
    world.setBlock(x + 2, y + 1, z + 2, BlockType::Chest);
}

}
