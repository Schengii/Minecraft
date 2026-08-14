#include "Biome.hpp"

namespace Minecraft {

BiomeType Biome::getBiome(float temperature, float moisture) {
    if (temperature > 0.25f && moisture > 0.25f) {
        return BiomeType::Jungle;
    } else if (temperature > 0.3f && moisture < -0.1f) {
        return BiomeType::Desert;
    } else if (temperature < -0.2f) {
        return BiomeType::Mountains;
    } else if (moisture > 0.2f) {
        return BiomeType::Forest;
    } else {
        return BiomeType::Plains;
    }
}

BlockType Biome::getSurfaceBlock(BiomeType type, int height) {
    switch (type) {
        case BiomeType::Desert:
            return BlockType::Sand;
        case BiomeType::Mountains:
            if (height > 75) return BlockType::Snow;
            return BlockType::Stone;
        case BiomeType::Forest:
        case BiomeType::Plains:
        default:
            return BlockType::Grass;
    }
}

BlockType Biome::getSubSurfaceBlock(BiomeType type) {
    switch (type) {
        case BiomeType::Desert:
            return BlockType::Sand;
        case BiomeType::Mountains:
            return BlockType::Stone;
        case BiomeType::Forest:
        case BiomeType::Plains:
        default:
            return BlockType::Dirt;
    }
}

}
