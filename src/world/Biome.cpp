#include "Biome.hpp"

namespace Minecraft {

BiomeType Biome::getBiome(float temp, float moisture) {
    if (temp > 0.6f && moisture < 0.2f) {
        return BiomeType::Desert;
    } else if (temp > 0.3f && moisture > 0.4f) {
        return BiomeType::Jungle;
    } else if (temp < -0.3f && moisture > 0.1f) {
        return BiomeType::Taiga;
    } else if (temp > 0.1f && moisture > 0.6f) {
        return BiomeType::Swamp;
    } else if (temp < -0.2f) {
        return BiomeType::Mountains;
    } else if (moisture > 0.3f) {
        return BiomeType::Forest;
    } else {
        return BiomeType::Plains;
    }
}

BiomeType Biome::getNetherBiome(float temp, float moisture) {
    if (temp > 0.4f && moisture > 0.2f) {
        return BiomeType::CrimsonForest;
    } else if (temp > 0.2f && moisture < -0.2f) {
        return BiomeType::WarpedForest;
    } else if (temp < -0.3f) {
        return BiomeType::SoulSandValley;
    } else if (moisture < -0.4f) {
        return BiomeType::BasaltDeltas;
    }
    return BiomeType::CrimsonForest;
}

BlockType Biome::getSurfaceBlock(BiomeType type, int height) {
    switch (type) {
        case BiomeType::Desert:
            return BlockType::Sand;
        case BiomeType::Mountains:
            return (height > 90) ? BlockType::Snow : BlockType::Stone;
        case BiomeType::Taiga:
            return BlockType::Grass;
        case BiomeType::Swamp:
            return BlockType::Grass;
        case BiomeType::CrimsonForest:
            return BlockType::Netherrack;
        case BiomeType::WarpedForest:
            return BlockType::Netherrack;
        case BiomeType::SoulSandValley:
            return BlockType::SoulSand;
        case BiomeType::BasaltDeltas:
            return BlockType::Obsidian;
        default:
            return BlockType::Grass;
    }
}

BlockType Biome::getSubSurfaceBlock(BiomeType type) {
    switch (type) {
        case BiomeType::Desert:
            return BlockType::Sand;
        case BiomeType::SoulSandValley:
            return BlockType::SoulSand;
        case BiomeType::CrimsonForest:
        case BiomeType::WarpedForest:
            return BlockType::Netherrack;
        default:
            return BlockType::Dirt;
    }
}

}
