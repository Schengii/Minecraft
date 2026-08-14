#ifndef VILLAGEGENERATOR_HPP
#define VILLAGEGENERATOR_HPP

#include "Block.hpp"
#include <glm/glm.hpp>

namespace Minecraft {

class World;
class MobEngine;

class VillageGenerator {
public:
    static void generateVillage(World& world, int originX, int originY, int originZ, MobEngine* mobEngine = nullptr);
    static void generateHouse(World& world, int x, int y, int z);
    static void generateBlacksmith(World& world, int x, int y, int z);
    static void generateFarmPlot(World& world, int x, int y, int z);
    static void generateWell(World& world, int x, int y, int z);
};

}

#endif // VILLAGEGENERATOR_HPP
