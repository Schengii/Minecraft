#include "Chunk.hpp"
#include "ChunkMesh.hpp"
#include "SaveSystem.hpp"
#include "LightEngine.hpp"
#include "Biome.hpp"
#include "../vendor/FastNoiseLite.h"
#include <cmath>

namespace Minecraft {

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX), m_ChunkZ(chunkZ)
{
    m_Mesh = std::make_unique<ChunkMesh>();

    if (!SaveSystem::loadChunk(*this)) {
        generateTerrain();
        SaveSystem::saveChunk(*this);
    }
}

Chunk::~Chunk() {
    SaveSystem::saveChunk(*this);
}

BlockType Chunk::getBlock(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) {
        return BlockType::Air;
    }
    return m_Blocks[x][y][z];
}

void Chunk::setBlock(int x, int y, int z, BlockType type) {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return;
    m_Blocks[x][y][z] = type;
    m_IsDirty = true;
}

int Chunk::getSunlight(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return 15;
    return (m_Light[x][y][z] >> 4) & 0x0F;
}

void Chunk::setSunlight(int x, int y, int z, int val) {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return;
    uint8_t cur = m_Light[x][y][z] & 0x0F;
    m_Light[x][y][z] = (static_cast<uint8_t>(val & 0x0F) << 4) | cur;
}

int Chunk::getBlocklight(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return 0;
    return m_Light[x][y][z] & 0x0F;
}

void Chunk::setBlocklight(int x, int y, int z, int val) {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return;
    uint8_t sun = m_Light[x][y][z] & 0xF0;
    m_Light[x][y][z] = sun | static_cast<uint8_t>(val & 0x0F);
}

uint8_t Chunk::getRawLight(int x, int y, int z) const {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return 0xFF;
    return m_Light[x][y][z];
}

void Chunk::setRawLight(int x, int y, int z, uint8_t val) {
    if (x < 0 || x >= CHUNK_SIZE_X || y < 0 || y >= CHUNK_SIZE_Y || z < 0 || z >= CHUNK_SIZE_Z) return;
    m_Light[x][y][z] = val;
}

void Chunk::generateTerrain() {
    FastNoiseLite heightNoise(1337);
    heightNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    heightNoise.SetFrequency(0.015f);

    FastNoiseLite tempNoise(5555);
    tempNoise.SetFrequency(0.005f);

    FastNoiseLite moistureNoise(7777);
    moistureNoise.SetFrequency(0.005f);

    FastNoiseLite caveNoise(4242);
    caveNoise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    caveNoise.SetFrequency(0.04f);

    FastNoiseLite oreNoise(9999);
    oreNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    oreNoise.SetFrequency(0.1f);

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            float worldX = static_cast<float>(m_ChunkX * CHUNK_SIZE_X + x);
            float worldZ = static_cast<float>(m_ChunkZ * CHUNK_SIZE_Z + z);

            float temp = tempNoise.GetNoise(worldX, worldZ);
            float moisture = moistureNoise.GetNoise(worldX, worldZ);
            BiomeType biome = Biome::getBiome(temp, moisture);

            float n = heightNoise.GetNoise(worldX, worldZ);
            float heightMultiplier = (biome == BiomeType::Mountains) ? 35.0f : 20.0f;
            int height = static_cast<int>(55 + n * heightMultiplier);
            if (height >= CHUNK_SIZE_Y - 10) height = CHUNK_SIZE_Y - 10;

            BlockType topBlock = Biome::getSurfaceBlock(biome, height);
            BlockType subBlock = Biome::getSubSurfaceBlock(biome);

            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                if (y == 0) {
                    m_Blocks[x][y][z] = BlockType::Bedrock;
                } else if (y < height - 4) {
                    float caveVal = caveNoise.GetNoise(worldX, static_cast<float>(y), worldZ);
                    if (caveVal > 0.42f) {
                        if (y < 10) {
                            m_Blocks[x][y][z] = BlockType::Lava;
                        } else {
                            m_Blocks[x][y][z] = BlockType::Air;
                        }
                    } else {
                        float oreVal = oreNoise.GetNoise(worldX + y, static_cast<float>(y), worldZ);
                        if (y <= 16 && oreVal > 0.75f) {
                            m_Blocks[x][y][z] = BlockType::DiamondOre;
                        } else if (y <= 30 && oreVal > 0.68f) {
                            m_Blocks[x][y][z] = BlockType::GoldOre;
                        } else if (y <= 45 && oreVal > 0.60f) {
                            m_Blocks[x][y][z] = BlockType::IronOre;
                        } else if (y <= 60 && oreVal > 0.52f) {
                            m_Blocks[x][y][z] = BlockType::CoalOre;
                        } else {
                            m_Blocks[x][y][z] = BlockType::Stone;
                        }
                    }
                } else if (y < height) {
                    m_Blocks[x][y][z] = subBlock;
                } else if (y == height) {
                    m_Blocks[x][y][z] = topBlock;
                } else {
                    m_Blocks[x][y][z] = BlockType::Air;
                }
            }

            // Biome Structure Decorators
            if (x > 2 && x < CHUNK_SIZE_X - 2 && z > 2 && z < CHUNK_SIZE_Z - 2) {
                float treeChance = std::abs(heightNoise.GetNoise(worldX * 5.0f, worldZ * 5.0f));

                if (biome == BiomeType::Desert && treeChance > 0.75f) {
                    // Cactus Structure
                    for (int ch = 1; ch <= 3; ++ch) {
                        m_Blocks[x][height + ch][z] = BlockType::Cactus;
                    }
                } else if ((biome == BiomeType::Forest || biome == BiomeType::Plains) && treeChance > 0.60f) {
                    // Oak or Birch Trees
                    BlockType logType = (biome == BiomeType::Forest && treeChance > 0.8f) ? BlockType::BirchLog : BlockType::OakLog;
                    int trunkHeight = 5;
                    for (int th = 1; th <= trunkHeight; ++th) {
                        m_Blocks[x][height + th][z] = logType;
                    }
                    for (int lx = -2; lx <= 2; ++lx) {
                        for (int lz = -2; lz <= 2; ++lz) {
                            for (int ly = trunkHeight - 1; ly <= trunkHeight + 1; ++ly) {
                                if (m_Blocks[x + lx][height + ly][z + lz] == BlockType::Air) {
                                    m_Blocks[x + lx][height + ly][z + lz] = BlockType::Leaves;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    LightEngine::calculateChunkLighting(*this);
    m_IsDirty = true;
}

void Chunk::buildMesh() {
    if (m_IsDirty && m_Mesh) {
        m_Mesh->generate(*this);
        m_IsDirty = false;
    }
}

void Chunk::render() {
    if (m_IsDirty) {
        buildMesh();
    }
    if (m_Mesh) {
        m_Mesh->render();
    }
}

}
