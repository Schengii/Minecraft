#ifndef LIGHTENGINE_HPP
#define LIGHTENGINE_HPP

#include "Block.hpp"
#include <glm/glm.hpp>
#include <queue>

namespace Minecraft {

class Chunk;
class World;

struct LightNode {
    int x, y, z;
    int lightLevel;
};

class LightEngine {
public:
    static int getLightEmission(BlockType type);
    static void calculateSunlight(Chunk& chunk, World* world = nullptr);
    static void calculateBlocklight(Chunk& chunk, World* world = nullptr);
    static void calculateChunkLighting(Chunk& chunk, World* world = nullptr);
    static void updateBlockLight(World& world, const glm::ivec3& pos);
};

}

#endif // LIGHTENGINE_HPP
