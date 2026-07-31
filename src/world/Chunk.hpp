#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Block.hpp"
#include <vector>
#include <memory>
#include <glm/glm.hpp>

namespace Minecraft {

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

class ChunkMesh;

class Chunk {
public:
    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);

    int getChunkX() const { return m_ChunkX; }
    int getChunkZ() const { return m_ChunkZ; }
    glm::ivec2 getPosition() const { return glm::ivec2(m_ChunkX, m_ChunkZ); }

    void generateTerrain();
    void buildMesh();
    void render();

    bool isDirty() const { return m_IsDirty; }
    void setDirty(bool dirty) { m_IsDirty = dirty; }

private:
    int m_ChunkX;
    int m_ChunkZ;
    BlockType m_Blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
    bool m_IsDirty = true;
    
    std::unique_ptr<ChunkMesh> m_Mesh;
};

}

#endif // CHUNK_HPP
