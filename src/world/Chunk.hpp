#ifndef CHUNK_HPP
#define CHUNK_HPP

#include "Block.hpp"
#include "ChunkMesh.hpp"
#include "ChunkSection.hpp"
#include <vector>
#include <memory>
#include <array>
#include <glm/glm.hpp>

namespace Minecraft {

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

class Chunk {
public:
    Chunk(int chunkX, int chunkZ);
    ~Chunk();

    BlockType getBlock(int x, int y, int z) const;
    void setBlock(int x, int y, int z, BlockType type);

    int getSunlight(int x, int y, int z) const;
    void setSunlight(int x, int y, int z, int val);

    int getBlocklight(int x, int y, int z) const;
    void setBlocklight(int x, int y, int z, int val);

    uint8_t getRawLight(int x, int y, int z) const;
    void setRawLight(int x, int y, int z, uint8_t val);

    int getChunkX() const { return m_ChunkX; }
    int getChunkZ() const { return m_ChunkZ; }
    glm::ivec2 getPosition() const { return glm::ivec2(m_ChunkX, m_ChunkZ); }

    const BlockType (&getBlocks() const)[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z] { return m_Blocks; }
    const uint8_t (&getLight() const)[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z] { return m_Light; }

    void generateTerrain();
    void buildMesh();
    void buildMeshDataAsync();
    void uploadPendingMesh();
    bool hasPendingMesh() const { return m_PendingMeshData != nullptr; }

    void render();
    void renderTransparent();

    bool isDirty() const { return m_IsDirty; }
    void setDirty(bool dirty) { m_IsDirty = dirty; }

    ChunkMesh* getMesh() { return m_Mesh.get(); }
    const ChunkMesh* getMesh() const { return m_Mesh.get(); }

    ChunkSection* getSection(int sectionY) {
        if (sectionY >= 0 && sectionY < 16) return m_Sections[sectionY].get();
        return nullptr;
    }

private:
    int m_ChunkX;
    int m_ChunkZ;
    BlockType m_Blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
    uint8_t m_Light[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];
    bool m_IsDirty = true;
    
    std::unique_ptr<ChunkMesh> m_Mesh;
    std::unique_ptr<MeshData> m_PendingMeshData;
    std::array<std::unique_ptr<ChunkSection>, 16> m_Sections;
};

}

#endif // CHUNK_HPP
