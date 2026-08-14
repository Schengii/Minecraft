#include "VillageGenerator.hpp"
#include "World.hpp"
#include "../ecs/MobEngine.hpp"
#include <cmath>

namespace Minecraft {

void VillageGenerator::generateVillage(World& world, int originX, int originY, int originZ, MobEngine* mobEngine) {
    // 1. Central Town Well
    generateWell(world, originX, originY, originZ);

    // 2. Village Houses
    generateHouse(world, originX + 7, originY, originZ);
    generateHouse(world, originX - 7, originY, originZ);

    // 3. Blacksmith Forge
    generateBlacksmith(world, originX, originY, originZ + 8);

    // 4. Village Crop Farmland
    generateFarmPlot(world, originX, originY, originZ - 8);

    // 5. Spawn Villagers and Iron Golem Defender
    if (mobEngine) {
        mobEngine->spawnMob(MobType::Villager, glm::vec3(originX + 7.5f, originY + 1.0f, originZ + 0.5f));
        mobEngine->spawnMob(MobType::Villager, glm::vec3(originX - 6.5f, originY + 1.0f, originZ + 0.5f));
        mobEngine->spawnMob(MobType::Villager, glm::vec3(originX + 0.5f, originY + 1.0f, originZ + 8.5f));
        mobEngine->spawnMob(MobType::IronGolem, glm::vec3(originX + 2.0f, originY + 1.0f, originZ + 2.0f));
    }
}

void VillageGenerator::generateWell(World& world, int x, int y, int z) {
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dz = -1; dz <= 1; ++dz) {
            world.setBlock(x + dx, y, z + dz, BlockType::Stone);
            world.setBlock(x + dx, y - 1, z + dz, BlockType::Water);
        }
    }
    world.setBlock(x - 1, y + 1, z - 1, BlockType::OakLog);
    world.setBlock(x + 1, y + 1, z - 1, BlockType::OakLog);
    world.setBlock(x - 1, y + 1, z + 1, BlockType::OakLog);
    world.setBlock(x + 1, y + 1, z + 1, BlockType::OakLog);
    for (int dx = -1; dx <= 1; ++dx) {
        for (int dz = -1; dz <= 1; ++dz) {
            world.setBlock(x + dx, y + 2, z + dz, BlockType::Planks);
        }
    }
}

void VillageGenerator::generateHouse(World& world, int x, int y, int z) {
    // 5x5 Wooden House with Planks floor, Wood frame, Glass windows, and Door
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            world.setBlock(x + dx, y, z + dz, BlockType::Planks); // Floor

            for (int dy = 1; dy <= 3; ++dy) {
                if (abs(dx) == 2 && abs(dz) == 2) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::OakLog);
                } else if (abs(dx) == 2 || abs(dz) == 2) {
                    if (dy == 2 && (dx == 0 || dz == 0)) {
                        world.setBlock(x + dx, y + dy, z + dz, BlockType::Glass); // Window
                    } else {
                        world.setBlock(x + dx, y + dy, z + dz, BlockType::Planks);
                    }
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air); // Interior
                }
            }
            // Roof
            world.setBlock(x + dx, y + 4, z + dz, BlockType::OakLog);
        }
    }
    // Entrance
    world.setBlock(x, y + 1, z - 2, BlockType::Air);
    world.setBlock(x, y + 2, z - 2, BlockType::Air);
}

void VillageGenerator::generateBlacksmith(World& world, int x, int y, int z) {
    // Stone foundation, Furnace, Crafting Table, Chest
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            world.setBlock(x + dx, y, z + dz, BlockType::Stone);
            for (int dy = 1; dy <= 3; ++dy) {
                if (abs(dx) == 2 || abs(dz) == 2) {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Stone);
                } else {
                    world.setBlock(x + dx, y + dy, z + dz, BlockType::Air);
                }
            }
            world.setBlock(x + dx, y + 4, z + dz, BlockType::Stone);
        }
    }
    // Workshop interior
    world.setBlock(x - 1, y + 1, z, BlockType::Furnace);
    world.setBlock(x + 1, y + 1, z, BlockType::CraftingTable);
    world.setBlock(x, y + 1, z + 1, BlockType::Chest);
    world.setBlock(x, y + 1, z - 2, BlockType::Air); // Doorway
}

void VillageGenerator::generateFarmPlot(World& world, int x, int y, int z) {
    for (int dx = -2; dx <= 2; ++dx) {
        for (int dz = -2; dz <= 2; ++dz) {
            if (dx == 0 && dz == 0) {
                world.setBlock(x + dx, y, z + dz, BlockType::Water);
                world.setBlock(x + dx, y + 1, z + dz, BlockType::Air);
            } else {
                world.setBlock(x + dx, y, z + dz, BlockType::Dirt);
                world.setBlock(x + dx, y + 1, z + dz, BlockType::WheatCrop);
            }
        }
    }
}

}
