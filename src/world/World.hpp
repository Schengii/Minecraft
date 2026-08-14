#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

#include "../core/ThreadPool.hpp"
#include <unordered_set>
#include <mutex>
#include <queue>

namespace std {
    template <>
    struct hash<glm::ivec2> {
        size_t operator()(const glm::ivec2& v) const {
            return hash<int>()(v.x) ^ (hash<int>()(v.y) << 16);
        }
    };
}

namespace Minecraft {

class FrustumCuller;

class World {
public:
    World(int renderDistance = 6);
    ~World();

    void update(const glm::vec3& playerPos);
    void render(const FrustumCuller* culler = nullptr);
    void renderTransparent(const FrustumCuller* culler = nullptr);

    Chunk* getChunk(int chunkX, int chunkZ);
    BlockType getBlock(int worldX, int worldY, int worldZ);
    void setBlock(int worldX, int worldY, int worldZ, BlockType type);

    int getLoadedChunkCount() const { return static_cast<int>(m_Chunks.size()); }

private:
    int m_RenderDistance;
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>> m_Chunks;
    std::unordered_set<glm::ivec2> m_LoadingChunks;
    
    std::mutex m_QueueMutex;
    std::vector<std::unique_ptr<Chunk>> m_CompletedChunks;
    std::unique_ptr<ThreadPool> m_ThreadPool;
};

}

#endif // WORLD_HPP
