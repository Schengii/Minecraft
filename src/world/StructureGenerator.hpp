#ifndef STRUCTUREGENERATOR_HPP
#define STRUCTUREGENERATOR_HPP

#include <glm/glm.hpp>
#include "Block.hpp"

namespace Minecraft {

class World;

class StructureGenerator {
public:
    static void generateTree(World& world, int x, int y, int z, bool isBirch = false);
    static void generateDungeon(World& world, int x, int y, int z);
    static void generateDesertTemple(World& world, int x, int y, int z);
    static void generateNetherPortalFrame(World& world, int x, int y, int z);
};

}

#endif // STRUCTUREGENERATOR_HPP
