#include "World.hpp"
#include <iostream>

namespace Minecraft {

World::World(int renderDistance)
    : m_RenderDistance(renderDistance)
{
    // Generate initial chunks around (0,0)
    for (int x = -m_RenderDistance; x <= m_RenderDistance; ++x) {
        for (int z = -m_RenderDistance; z <= m_RenderDistance; ++z) {
            glm::ivec2 pos(x, z);
            m_Chunks[pos] = std::make_unique<Chunk>(x, z);
        }
    }
}

World::~World() = default;

void World::update(const glm::vec3& playerPos) {
    int playerChunkX = static_cast<int>(std::floor(playerPos.x / CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(playerPos.z / CHUNK_SIZE_Z));

    // Dynamic Chunk loading around player position
    for (int x = playerChunkX - m_RenderDistance; x <= playerChunkX + m_RenderDistance; ++x) {
        for (int z = playerChunkZ - m_RenderDistance; z <= playerChunkZ + m_RenderDistance; ++z) {
            glm::ivec2 pos(x, z);
            if (m_Chunks.find(pos) == m_Chunks.end()) {
                m_Chunks[pos] = std::make_unique<Chunk>(x, z);
            }
        }
    }
}

void World::render() {
    for (auto& [pos, chunk] : m_Chunks) {
        if (chunk) {
            chunk->render();
        }
    }
}

Chunk* World::getChunk(int chunkX, int chunkZ) {
    glm::ivec2 pos(chunkX, chunkZ);
    auto it = m_Chunks.find(pos);
    if (it != m_Chunks.end()) {
        return it->second.get();
    }
    return nullptr;
}

BlockType World::getBlock(int worldX, int worldY, int worldZ) {
    int chunkX = static_cast<int>(std::floor(static_cast<float>(worldX) / CHUNK_SIZE_X));
    int chunkZ = static_cast<int>(std::floor(static_cast<float>(worldZ) / CHUNK_SIZE_Z));

    Chunk* chunk = getChunk(chunkX, chunkZ);
    if (!chunk) return BlockType::Air;

    int localX = (worldX % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X;
    int localZ = (worldZ % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;

    return chunk->getBlock(localX, worldY, localZ);
}

void World::setBlock(int worldX, int worldY, int worldZ, BlockType type) {
    int chunkX = static_cast<int>(std::floor(static_cast<float>(worldX) / CHUNK_SIZE_X));
    int chunkZ = static_cast<int>(std::floor(static_cast<float>(worldZ) / CHUNK_SIZE_Z));

    Chunk* chunk = getChunk(chunkX, chunkZ);
    if (chunk) {
        int localX = (worldX % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X;
        int localZ = (worldZ % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
        chunk->setBlock(localX, worldY, localZ, type);
    }
}

}
