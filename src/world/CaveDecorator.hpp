#ifndef CAVEDECORATOR_HPP
#define CAVEDECORATOR_HPP

#include <glm/glm.hpp>
#include "Block.hpp"

namespace Minecraft {

class World;

class CaveDecorator {
public:
    static void decorateCaveColumn(World& world, int x, int z, int minY, int maxY);
    static void generateStalagmite(World& world, int x, int y, int z, int height);
    static void generateStalactite(World& world, int x, int y, int z, int height);
};

}

#endif // CAVEDECORATOR_HPP
