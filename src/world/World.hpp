#ifndef WORLD_HPP
#define WORLD_HPP

#include "Chunk.hpp"
#include <unordered_map>
#include <memory>
#include <glm/glm.hpp>

namespace std {
    template <>
    struct hash<glm::ivec2> {
        size_t operator()(const glm::ivec2& v) const {
            return hash<int>()(v.x) ^ (hash<int>()(v.y) << 16);
        }
    };
}

namespace Minecraft {

class World {
public:
    World(int renderDistance = 6);
    ~World();

    void update(const glm::vec3& playerPos);
    void render();

    Chunk* getChunk(int chunkX, int chunkZ);
    BlockType getBlock(int worldX, int worldY, int worldZ);
    void setBlock(int worldX, int worldY, int worldZ, BlockType type);

private:
    int m_RenderDistance;
    std::unordered_map<glm::ivec2, std::unique_ptr<Chunk>> m_Chunks;
};

}

#endif // WORLD_HPP
