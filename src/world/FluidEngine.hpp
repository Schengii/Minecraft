#ifndef FLUIDENGINE_HPP
#define FLUIDENGINE_HPP

#include <glm/glm.hpp>

namespace Minecraft {

class World;

class FluidEngine {
public:
    static void updateFluids(World& world, const glm::vec3& playerPos);
    static int getFluidLevel(World& world, int x, int y, int z);
    static float getFluidHeight(World& world, int x, int y, int z);
};

}

#endif // FLUIDENGINE_HPP

