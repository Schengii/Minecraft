#ifndef FLUIDENGINE_HPP
#define FLUIDENGINE_HPP

namespace Minecraft {

class World;

class FluidEngine {
public:
    static void updateFluids(World& world);
};

}

#endif // FLUIDENGINE_HPP
