#include "RedstoneEngine.hpp"
#include "World.hpp"
#include <iostream>

namespace Minecraft {

void RedstoneEngine::updateRedstoneNetwork(World& world, const glm::ivec3& sourcePos) {
    (void)sourcePos;
    std::cout << "[RedstoneEngine] Signal Network Updated." << std::endl;
}

bool RedstoneEngine::isPowered(World& world, const glm::ivec3& pos) {
    // Check 6 adjacent blocks for RedstoneTorch or Lever
    glm::ivec3 offsets[6] = {
        {0, 1, 0}, {0, -1, 0},
        {1, 0, 0}, {-1, 0, 0},
        {0, 0, 1}, {0, 0, -1}
    };

    for (const auto& off : offsets) {
        glm::ivec3 checkPos = pos + off;
        BlockType type = world.getBlock(checkPos.x, checkPos.y, checkPos.z);
        if (type == BlockType::RedstoneTorch || type == BlockType::Lever) {
            return true;
        }
    }
    return false;
}

}
