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

glm::vec3 Biome::getGrassColor(BiomeType type) {
    switch (type) {
        case BiomeType::Desert:
            return glm::vec3(0.75f, 0.71f, 0.38f);
        case BiomeType::Jungle:
            return glm::vec3(0.35f, 0.80f, 0.22f);
        case BiomeType::Taiga:
            return glm::vec3(0.40f, 0.65f, 0.50f);
        case BiomeType::Swamp:
            return glm::vec3(0.42f, 0.52f, 0.27f);
        case BiomeType::Forest:
            return glm::vec3(0.47f, 0.74f, 0.35f);
        case BiomeType::Mountains:
            return glm::vec3(0.50f, 0.70f, 0.45f);
        case BiomeType::Plains:
        default:
            return glm::vec3(0.57f, 0.74f, 0.35f);
    }
}

glm::vec3 Biome::getFoliageColor(BiomeType type) {
    switch (type) {
        case BiomeType::Desert:
            return glm::vec3(0.68f, 0.70f, 0.32f);
        case BiomeType::Jungle:
            return glm::vec3(0.28f, 0.78f, 0.18f);
        case BiomeType::Taiga:
            return glm::vec3(0.35f, 0.60f, 0.45f);
        case BiomeType::Swamp:
            return glm::vec3(0.38f, 0.48f, 0.24f);
        case BiomeType::Forest:
        case BiomeType::Mountains:
        case BiomeType::Plains:
        default:
            return glm::vec3(0.44f, 0.72f, 0.32f);
    }
}

}
