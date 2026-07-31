#include "FluidEngine.hpp"
#include "World.hpp"
#include "Block.hpp"
#include <vector>
#include <cmath>

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

                // 1. Water + Lava interaction -> Stone
                glm::ivec3 dirs[4] = { {1, 0, 0}, {-1, 0, 0}, {0, 0, 1}, {0, 0, -1} };
                for (const auto& d : dirs) {
                    BlockType neighbor = world.getBlock(x + d.x, y + d.y, z + d.z);
                    if ((current == BlockType::Water && neighbor == BlockType::Lava) ||
                        (current == BlockType::Lava && neighbor == BlockType::Water)) {
                        changes.push_back({ x + d.x, y + d.y, z + d.z, BlockType::Stone });
                    }
                }

                // 2. Flow Downward
                BlockType below = world.getBlock(x, y - 1, z);
                if (below == BlockType::Air) {
                    changes.push_back({ x, y - 1, z, current });
                } else if (below != BlockType::Air && below != BlockType::Water && below != BlockType::Lava) {
                    // 3. Flow Horizontally if below is solid
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

    // Apply changes (limit batch size for smooth performance)
    size_t applyCount = std::min(changes.size(), static_cast<size_t>(16));
    for (size_t i = 0; i < applyCount; ++i) {
        world.setBlock(changes[i].x, changes[i].y, changes[i].z, changes[i].type);
    }
}

}

