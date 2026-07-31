#include "Chunk.hpp"
#include "ChunkMesh.hpp"
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
    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            int worldX = m_ChunkX * CHUNK_SIZE_X + x;
            int worldZ = m_ChunkZ * CHUNK_SIZE_Z + z;

            // Procedural Terrain height (sine-based hills + noise representation)
            float n1 = std::sin(worldX * 0.05f) * std::cos(worldZ * 0.05f) * 12.0f;
            float n2 = std::sin(worldX * 0.01f + 1.5f) * std::cos(worldZ * 0.01f) * 25.0f;
            int height = static_cast<int>(50 + n1 + n2);

            if (height >= CHUNK_SIZE_Y) height = CHUNK_SIZE_Y - 1;

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
