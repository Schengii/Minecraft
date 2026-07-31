#include "Chunk.hpp"
#include "ChunkMesh.hpp"
#include "../vendor/FastNoiseLite.h"
#include <cmath>

namespace Minecraft {

Chunk::Chunk(int chunkX, int chunkZ)
    : m_ChunkX(chunkX), m_ChunkZ(chunkZ)
{
    m_Mesh = std::make_unique<ChunkMesh>();
    generateTerrain();
}

Chunk::~Chunk() = default;

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

void Chunk::generateTerrain() {
    FastNoiseLite noise(1337);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFrequency(0.015f);

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            float worldX = static_cast<float>(m_ChunkX * CHUNK_SIZE_X + x);
            float worldZ = static_cast<float>(m_ChunkZ * CHUNK_SIZE_Z + z);

            float n = noise.GetNoise(worldX, worldZ);
            int height = static_cast<int>(55 + n * 20.0f);
            if (height >= CHUNK_SIZE_Y - 10) height = CHUNK_SIZE_Y - 10;

            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                if (y == 0) {
                    m_Blocks[x][y][z] = BlockType::Bedrock;
                } else if (y < height - 4) {
                    m_Blocks[x][y][z] = BlockType::Stone;
                } else if (y < height) {
                    m_Blocks[x][y][z] = BlockType::Dirt;
                } else if (y == height) {
                    m_Blocks[x][y][z] = BlockType::Grass;
                } else {
                    m_Blocks[x][y][z] = BlockType::Air;
                }
            }

            // Simple Oak Tree Generation
            if (x > 2 && x < CHUNK_SIZE_X - 2 && z > 2 && z < CHUNK_SIZE_Z - 2) {
                float treeChance = std::abs(noise.GetNoise(worldX * 5.0f, worldZ * 5.0f));
                if (treeChance > 0.65f && height > 45) {
                    int trunkHeight = 5;
                    for (int th = 1; th <= trunkHeight; ++th) {
                        m_Blocks[x][height + th][z] = BlockType::OakLog;
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
