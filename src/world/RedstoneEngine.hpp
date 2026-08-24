#ifndef REDSTONEENGINE_HPP
#define REDSTONEENGINE_HPP

#include "Block.hpp"
#include <glm/glm.hpp>
#include <vector>

namespace Minecraft {

class World;

class RedstoneEngine {
public:
    static void updateRedstoneNetwork(World& world, const glm::ivec3& sourcePos);
    static bool isPowered(World& world, const glm::ivec3& pos);
    static int getSignalStrength(World& world, const glm::ivec3& pos);

    // Piston Mechanics
    static bool tryPushPiston(World& world, const glm::ivec3& pistonPos, const glm::ivec3& pushDir, bool isSticky = false);
    static bool tryRetractStickyPiston(World& world, const glm::ivec3& pistonPos, const glm::ivec3& pullDir);
    static void triggerPistonMechanisms(World& world, const glm::ivec3& sourcePos);

    // Repeater Mechanics
    static int getRepeaterOutput(World& world, const glm::ivec3& pos, int delayTicks = 1);
};

}

#endif // REDSTONEENGINE_HPP
