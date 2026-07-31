#ifndef FLUIDENGINE_HPP
#define FLUIDENGINE_HPP

#include <glm/glm.hpp>

namespace Minecraft {

class World;

class FluidEngine {
public:
    static void updateFluids(World& world, const glm::vec3& playerPos);
};

}

#endif // FLUIDENGINE_HPP

