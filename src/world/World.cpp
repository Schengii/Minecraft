#include "World.hpp"
#include "../renderer/FrustumCuller.hpp"
#include "SaveSystem.hpp"
#include <iostream>
#include <cmath>
#include <vector>

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

World::~World() {
    for (auto& [pos, chunk] : m_Chunks) {
        if (chunk) {
            SaveSystem::saveChunk(*chunk);
        }
    }
}

void World::unloadFarChunks(const glm::vec3& playerPos) {
    int playerChunkX = static_cast<int>(std::floor(playerPos.x / CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(playerPos.z / CHUNK_SIZE_Z));
    int maxDist = m_RenderDistance + 2;

    std::vector<glm::ivec2> chunksToUnload;
    for (const auto& [pos, chunk] : m_Chunks) {
        if (std::abs(pos.x - playerChunkX) > maxDist || std::abs(pos.y - playerChunkZ) > maxDist) {
            chunksToUnload.push_back(pos);
        }
    }

    for (const auto& pos : chunksToUnload) {
        auto it = m_Chunks.find(pos);
        if (it != m_Chunks.end() && it->second) {
            SaveSystem::saveChunk(*it->second);
            m_Chunks.erase(it);
        }
    }
}

void World::update(const glm::vec3& playerPos) {
    // 1. Collect completed background-generated chunks from workers and upload in batches
    {
        std::lock_guard<std::mutex> lock(m_QueueMutex);
        size_t uploadsThisFrame = 0;
        const size_t MAX_UPLOADS_PER_FRAME = 6;

        auto it = m_CompletedChunks.begin();
        while (it != m_CompletedChunks.end() && uploadsThisFrame < MAX_UPLOADS_PER_FRAME) {
            if (*it) {
                glm::ivec2 pos = (*it)->getPosition();
                if ((*it)->hasPendingMesh()) {
                    (*it)->uploadPendingMesh();
                } else {
                    (*it)->buildMesh();
                }
                m_Chunks[pos] = std::move(*it);
                m_LoadingChunks.erase(pos);
                uploadsThisFrame++;
            }
            it = m_CompletedChunks.erase(it);
        }
    }

    int playerChunkX = static_cast<int>(std::floor(playerPos.x / CHUNK_SIZE_X));
    int playerChunkZ = static_cast<int>(std::floor(playerPos.z / CHUNK_SIZE_Z));

    // 2. Queue missing chunks for background generation with asynchronous CPU meshing
    for (int x = playerChunkX - m_RenderDistance; x <= playerChunkX + m_RenderDistance; ++x) {
        for (int z = playerChunkZ - m_RenderDistance; z <= playerChunkZ + m_RenderDistance; ++z) {
            glm::ivec2 pos(x, z);
            if (m_Chunks.find(pos) == m_Chunks.end() && m_LoadingChunks.find(pos) == m_LoadingChunks.end()) {
                m_LoadingChunks.insert(pos);

                m_ThreadPool->enqueue([this, x, z]() {
                    auto newChunk = std::make_unique<Chunk>(x, z);
                    newChunk->buildMeshDataAsync();
                    std::lock_guard<std::mutex> lock(m_QueueMutex);
                    m_CompletedChunks.push_back(std::move(newChunk));
                });
            }
        }
    }

    // 3. Unload distant chunks to preserve memory (LRU Chunk Caching)
    unloadFarChunks(playerPos);
}

void World::render(const FrustumCuller* culler) {
    for (auto& [pos, chunk] : m_Chunks) {
        if (!chunk) continue;
        if (culler) {
            glm::vec3 minAABB(pos.x * CHUNK_SIZE_X, 0.0f, pos.y * CHUNK_SIZE_Z);
            glm::vec3 maxAABB(minAABB.x + CHUNK_SIZE_X, static_cast<float>(CHUNK_SIZE_Y), minAABB.z + CHUNK_SIZE_Z);
            if (!culler->isBoxVisible(minAABB, maxAABB)) {
                continue;
            }
        }
        chunk->render();
    }
}

void World::renderTransparent(const FrustumCuller* culler) {
    for (auto& [pos, chunk] : m_Chunks) {
        if (!chunk) continue;
        if (culler) {
            glm::vec3 minAABB(pos.x * CHUNK_SIZE_X, 0.0f, pos.y * CHUNK_SIZE_Z);
            glm::vec3 maxAABB(minAABB.x + CHUNK_SIZE_X, static_cast<float>(CHUNK_SIZE_Y), minAABB.z + CHUNK_SIZE_Z);
            if (!culler->isBoxVisible(minAABB, maxAABB)) {
                continue;
            }
        }
        chunk->renderTransparent();
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

BlockType World::getTopBlock(int worldX, int worldZ, int& outY) {
    for (int y = 250; y >= 1; --y) {
        BlockType b = getBlock(worldX, y, worldZ);
        if (b != BlockType::Air) {
            outY = y;
            return b;
        }
    }
    outY = 0;
    return BlockType::Air;
}

glm::vec3 World::getMapColor(int worldX, int worldZ) {
    int topY = 0;
    BlockType b = getTopBlock(worldX, worldZ, topY);
    switch (b) {
        case BlockType::Grass: return glm::vec3(0.4f, 0.7f, 0.2f);
        case BlockType::Dirt: return glm::vec3(0.55f, 0.4f, 0.25f);
        case BlockType::Stone:
        case BlockType::Bedrock: return glm::vec3(0.5f, 0.5f, 0.5f);
        case BlockType::Sand: return glm::vec3(0.85f, 0.82f, 0.55f);
        case BlockType::Water: return glm::vec3(0.2f, 0.4f, 0.8f);
        case BlockType::Lava: return glm::vec3(0.9f, 0.35f, 0.1f);
        case BlockType::Leaves: return glm::vec3(0.25f, 0.55f, 0.15f);
        case BlockType::OakLog:
        case BlockType::BirchLog:
        case BlockType::Planks: return glm::vec3(0.6f, 0.45f, 0.25f);
        case BlockType::Snow: return glm::vec3(0.95f, 0.95f, 0.98f);
        case BlockType::Netherrack: return glm::vec3(0.45f, 0.1f, 0.1f);
        case BlockType::EndStone: return glm::vec3(0.85f, 0.85f, 0.65f);
        default: return glm::vec3(0.3f, 0.3f, 0.3f);
    }
}

}
