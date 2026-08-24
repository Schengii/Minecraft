#ifndef BIOME_HPP
#define BIOME_HPP

#include "Block.hpp"

namespace Minecraft {

enum class BiomeType {
    Plains,
    Desert,
    Forest,
    Mountains,
    Jungle,
    Taiga,
    Swamp,
    CrimsonForest,
    WarpedForest,
    SoulSandValley,
    BasaltDeltas
};

class Biome {
public:
    static BiomeType getBiome(float temperature, float moisture);
    static BiomeType getNetherBiome(float temperature, float moisture);
    static BlockType getSurfaceBlock(BiomeType type, int height);
    static BlockType getSubSurfaceBlock(BiomeType type);
};

}

#endif // BIOME_HPP
