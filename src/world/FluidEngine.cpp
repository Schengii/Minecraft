#include "FluidEngine.hpp"
#include "World.hpp"
#include "Block.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace Minecraft {

void FluidEngine::updateFluids(World& world, const glm::vec3& playerPos) {
    int px = static_cast<int>(std::floor(playerPos.x));
    int py = static_cast<int>(std::floor(playerPos.y));
    int pz = static_cast<int>(std::floor(playerPos.z));

    int range = 24; // Update fluids within 24 blocks of player

    struct FluidChange {
        int x, y, z;
        BlockType type;
    };
    std::vector<FluidChange> changes;

    for (int x = px - range; x <= px + range; ++x) {
        for (int y = std::max(1, py - 16); y <= std::min(250, py + 16); ++y) {
            for (int z = pz - range; z <= pz + range; ++z) {
                BlockType current = world.getBlock(x, y, z);
                if (current != BlockType::Water && current != BlockType::Lava) continue;

                // 1. Water + Lava interaction -> Obsidian / Stone
                glm::ivec3 dirs[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
                for (const auto& d : dirs) {
                    BlockType neighbor = world.getBlock(x + d.x, y + d.y, z + d.z);
                    if (current == BlockType::Water && neighbor == BlockType::Lava) {
                        changes.push_back({ x + d.x, y + d.y, z + d.z, BlockType::Obsidian });
                    } else if (current == BlockType::Lava && neighbor == BlockType::Water) {
                        changes.push_back({ x, y, z, BlockType::Stone });
                    }
                }

                // 2. Infinite Water Source Generation: Air block with >= 2 water neighbors & solid floor
                for (const auto& d : dirs) {
                    int nx = x + d.x;
                    int ny = y + d.y;
                    int nz = z + d.z;
                    if (world.getBlock(nx, ny, nz) == BlockType::Air) {
                        BlockType floor = world.getBlock(nx, ny - 1, nz);
                        if (BlockData::isSolid(floor)) {
                            int waterNeighbors = 0;
                            for (const auto& d2 : dirs) {
                                if (world.getBlock(nx + d2.x, ny, nz + d2.z) == BlockType::Water) {
                                    waterNeighbors++;
                                }
                            }
                            if (waterNeighbors >= 2) {
                                changes.push_back({ nx, ny, nz, BlockType::Water });
                            }
                        }
                    }
                }

                // 3. Flow Downward
                BlockType below = world.getBlock(x, y - 1, z);
                if (below == BlockType::Air) {
                    changes.push_back({ x, y - 1, z, current });
                } else if (below != BlockType::Air && below != BlockType::Water && below != BlockType::Lava) {
                    // 4. Flow Horizontally if below is solid
                    for (const auto& d : dirs) {
                        BlockType side = world.getBlock(x + d.x, y, z + d.z);
                        if (side == BlockType::Air) {
                            changes.push_back({ x + d.x, y, z + d.z, current });
                        }
                    }
                }
            }
        }
    }

    // Sort changes by distance to playerPos so nearby fluids update first
    std::sort(changes.begin(), changes.end(), [&playerPos](const FluidChange& a, const FluidChange& b) {
        float distA = glm::distance(playerPos, glm::vec3(a.x, a.y, a.z));
        float distB = glm::distance(playerPos, glm::vec3(b.x, b.y, b.z));
        return distA < distB;
    });

    // Apply changes (limit batch size for smooth performance)
    size_t applyCount = std::min(changes.size(), static_cast<size_t>(128));
    for (size_t i = 0; i < applyCount; ++i) {
        world.setBlock(changes[i].x, changes[i].y, changes[i].z, changes[i].type);
    }
}

int FluidEngine::getFluidLevel(World& world, int x, int y, int z) {
    BlockType current = world.getBlock(x, y, z);
    if (current != BlockType::Water && current != BlockType::Lava) return 0;
    if (world.getBlock(x, y + 1, z) == current) return 0; // Submerged under another fluid block

    glm::ivec3 dirs[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
    int minNeighborLevel = 8;
    for (const auto& d : dirs) {
        BlockType neighbor = world.getBlock(x + d.x, y, z + d.z);
        if (neighbor == current) {
            minNeighborLevel = std::min(minNeighborLevel, 1);
        }
    }
    return (minNeighborLevel == 8) ? 0 : minNeighborLevel;
}

float FluidEngine::getFluidHeight(World& world, int x, int y, int z) {
    BlockType current = world.getBlock(x, y, z);
    if (current != BlockType::Water && current != BlockType::Lava) return 0.0f;
    if (world.getBlock(x, y + 1, z) == current) return 1.0f;

    int level = getFluidLevel(world, x, y, z);
    float h = 1.0f - static_cast<float>(level) * 0.12f;
    return std::clamp(h, 0.15f, 1.0f);
}

}

