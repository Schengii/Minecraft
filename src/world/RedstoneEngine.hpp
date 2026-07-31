#ifndef REDSTONEENGINE_HPP
#define REDSTONEENGINE_HPP

#include "Block.hpp"
#include <glm/glm.hpp>

namespace Minecraft {

class World;

class RedstoneEngine {
public:
    static void updateRedstoneNetwork(World& world, const glm::ivec3& sourcePos);
    static bool isPowered(World& world, const glm::ivec3& pos);
    static int getSignalStrength(World& world, const glm::ivec3& pos);
};

}

#endif // REDSTONEENGINE_HPP

