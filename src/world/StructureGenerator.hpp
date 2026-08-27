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
    static void generateMineshaft(World& world, int x, int y, int z, int length = 16);
    static void generateNetherFortressCorridor(World& world, int x, int y, int z, int length = 20);
    static void generateStronghold(World& world, int x, int y, int z);
    static void generateOceanRuin(World& world, int x, int y, int z);
    static void generateEndCity(World& world, int x, int y, int z);
    static void generateNetherBastion(World& world, int x, int y, int z);
    static void generateOceanMonument(World& world, int x, int y, int z);
    static void generateShipwreck(World& world, int x, int y, int z);
};

}

#endif // STRUCTUREGENERATOR_HPP
