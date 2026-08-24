#include "RedstoneEngine.hpp"
#include "World.hpp"
#include <iostream>
#include <queue>
#include <vector>
#include <algorithm>

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

int RedstoneEngine::getRepeaterOutput(World& world, const glm::ivec3& pos, int delayTicks) {
    (void)delayTicks;
    if (isPowered(world, pos)) {
        return 15;
    }
    return 0;
}

bool RedstoneEngine::tryPushPiston(World& world, const glm::ivec3& pistonPos, const glm::ivec3& pushDir, bool isSticky) {
    (void)isSticky;
    const int MAX_PUSH_BLOCKS = 12;
    std::vector<glm::ivec3> blocksToPush;

    glm::ivec3 currentPos = pistonPos + pushDir;

    for (int i = 0; i < MAX_PUSH_BLOCKS; ++i) {
        BlockType type = world.getBlock(currentPos.x, currentPos.y, currentPos.z);
        if (type == BlockType::Air) {
            break; // Air gap reached, pushing is valid
        }

        // Non-pushable blocks (Bedrock, Obsidian, Portals)
        if (type == BlockType::Bedrock || type == BlockType::Obsidian || type == BlockType::NetherPortal || type == BlockType::EndPortal) {
            return false;
        }

        blocksToPush.push_back(currentPos);
        currentPos += pushDir;
    }

    // If max reached and the next block is not air, cannot push
    if (blocksToPush.size() == MAX_PUSH_BLOCKS) {
        if (world.getBlock(currentPos.x, currentPos.y, currentPos.z) != BlockType::Air) {
            return false;
        }
    }

    // Push blocks from furthest to nearest
    for (auto it = blocksToPush.rbegin(); it != blocksToPush.rend(); ++it) {
        glm::ivec3 from = *it;
        glm::ivec3 to = from + pushDir;
        BlockType movedBlock = world.getBlock(from.x, from.y, from.z);
        world.setBlock(to.x, to.y, to.z, movedBlock);
        world.setBlock(from.x, from.y, from.z, BlockType::Air);
    }

    return true;
}

bool RedstoneEngine::tryRetractStickyPiston(World& world, const glm::ivec3& pistonPos, const glm::ivec3& pullDir) {
    glm::ivec3 targetPos = pistonPos + pullDir * 2;
    glm::ivec3 destPos = pistonPos + pullDir;

    BlockType targetBlock = world.getBlock(targetPos.x, targetPos.y, targetPos.z);
    if (targetBlock != BlockType::Air && targetBlock != BlockType::Bedrock && targetBlock != BlockType::Obsidian) {
        world.setBlock(destPos.x, destPos.y, destPos.z, targetBlock);
        world.setBlock(targetPos.x, targetPos.y, targetPos.z, BlockType::Air);
        return true;
    }
    return false;
}

void RedstoneEngine::triggerPistonMechanisms(World& world, const glm::ivec3& sourcePos) {
    glm::ivec3 offsets[6] = {
        {0, 1, 0}, {0, -1, 0},
        {1, 0, 0}, {-1, 0, 0},
        {0, 0, 1}, {0, 0, -1}
    };

    for (const auto& off : offsets) {
        glm::ivec3 neighbor = sourcePos + off;
        BlockType type = world.getBlock(neighbor.x, neighbor.y, neighbor.z);
        if (type == BlockType::Piston) {
            tryPushPiston(world, neighbor, glm::ivec3(0, 1, 0), false);
        } else if (type == BlockType::StickyPiston) {
            tryPushPiston(world, neighbor, glm::ivec3(0, 1, 0), true);
        }
    }
}

void RedstoneEngine::updateRedstoneNetwork(World& world, const glm::ivec3& sourcePos) {
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
    while (!q.empty() && visitedCount < 150) {
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
            } else if (neighborType == BlockType::Piston || neighborType == BlockType::StickyPiston) {
                triggerPistonMechanisms(world, neighbor);
            }
        }
    }
}

}
