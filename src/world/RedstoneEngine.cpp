#include "RedstoneEngine.hpp"
#include "World.hpp"
#include <iostream>
#include <queue>
#include <unordered_set>
#include <tuple>

namespace Minecraft {

bool RedstoneEngine::isPowered(World& world, const glm::ivec3& pos) {
    return getSignalStrength(world, pos) > 0;
}

int RedstoneEngine::getSignalStrength(World& world, const glm::ivec3& pos) {
    BlockType selfType = world.getBlock(pos.x, pos.y, pos.z);
    if (selfType == BlockType::RedstoneTorch || selfType == BlockType::Lever) {
        return 15;
    }

    glm::ivec3 offsets[6] = {
        {0, 1, 0}, {0, -1, 0},
        {1, 0, 0}, {-1, 0, 0},
        {0, 0, 1}, {0, 0, -1}
    };

    int maxSignal = 0;
    for (const auto& off : offsets) {
        glm::ivec3 checkPos = pos + off;
        BlockType type = world.getBlock(checkPos.x, checkPos.y, checkPos.z);
        if (type == BlockType::RedstoneTorch || type == BlockType::Lever) {
            return 15;
        }
    }
    return maxSignal;
}

void RedstoneEngine::updateRedstoneNetwork(World& world, const glm::ivec3& sourcePos) {
    std::cout << "[RedstoneEngine] Propagating Redstone Signal from (" 
              << sourcePos.x << ", " << sourcePos.y << ", " << sourcePos.z << ")" << std::endl;

    glm::ivec3 offsets[6] = {
        {0, 1, 0}, {0, -1, 0},
        {1, 0, 0}, {-1, 0, 0},
        {0, 0, 1}, {0, 0, -1}
    };

    // Breadth-First Propagation up to 15 blocks
    struct QueueNode {
        glm::ivec3 pos;
        int strength;
    };

    std::queue<QueueNode> q;
    int initialStrength = getSignalStrength(world, sourcePos);
    if (initialStrength > 0) {
        q.push({ sourcePos, initialStrength });
    }

    int visitedCount = 0;
    while (!q.empty() && visitedCount < 100) {
        QueueNode current = q.front();
        q.pop();
        visitedCount++;

        if (current.strength <= 1) continue;

        for (const auto& off : offsets) {
            glm::ivec3 neighbor = current.pos + off;
            BlockType neighborType = world.getBlock(neighbor.x, neighbor.y, neighbor.z);

            if (neighborType == BlockType::RedstoneWire || neighborType == BlockType::RedstoneLamp) {
                int nextStrength = current.strength - 1;
                if (nextStrength > 0) {
                    q.push({ neighbor, nextStrength });
                }
            }
        }
    }
}

}

