#include "World.hpp"
#include <iostream>

namespace Minecraft {

World::World(int renderDistance)
    : m_RenderDistance(renderDistance)
{
    m_ThreadPool = std::make_unique<ThreadPool>(4);

    // Initial Chunks
    for (int x = -m_RenderDistance; x <= m_RenderDistance; ++x) {
        for (int z = -m_RenderDistance; z <= m_RenderDistance; ++z) {
            glm::ivec2 pos(x, z);
            m_Chunks[pos] = std::make_unique<Chunk>(x, z);
        }
    }
}

World::~World() = default;

void World::update(const glm::vec3& playerPos) {
    // 1. Collect completed background-generated chunks from workers
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        for (auto& chunk : m_CompletedChunks) {
            if (chunk) {
                glm::ivec2 pos = chunk->getPosition();
                chunk->buildMesh();
                m_Chunks[pos] = std::move(chunk);
                m_LoadingChunks.erase(pos);
            }
        }
        m_CompletedChunks.clear();
    }

    int playerChunkX = static_cast<int>(std::floor(playerPos.x / CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(playerPos.z / CHUNK_SIZE_Z));

    // 2. Queue missing chunks for background generation
    for (int x = playerChunkX - m_RenderDistance; x <= playerChunkX + m_RenderDistance; ++x) {
        for (int z = playerChunkZ - m_RenderDistance; z <= playerChunkZ + m_RenderDistance; ++z) {
            glm::ivec2 pos(x, z);
            if (m_Chunks.find(pos) == m_Chunks.end() && m_LoadingChunks.find(pos) == m_LoadingChunks.end()) {
                m_LoadingChunks.insert(pos);

                m_ThreadPool->enqueue([this, x, z]() {
                    auto newChunk = std::make_unique<Chunk>(x, z);
                    std::lock_guard<std::mutex> lock(m_QueueMutex);
                    m_CompletedChunks.push_back(std::move(newChunk));
                });
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
