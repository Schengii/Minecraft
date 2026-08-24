#ifndef CHUNKSECTION_HPP
#define CHUNKSECTION_HPP

#include "Block.hpp"
#include "ChunkMesh.hpp"
#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace Minecraft {

class Chunk;

constexpr int SECTION_SIZE_X = 16;
constexpr int SECTION_SIZE_Y = 16;
constexpr int SECTION_SIZE_Z = 16;
constexpr int SECTIONS_PER_CHUNK = 16;

class ChunkSection {
public:
    ChunkSection(int sectionY = 0);
    ~ChunkSection();

    int getSectionY() const { return m_SectionY; }
    bool isDirty() const { return m_IsDirty; }
    void setDirty(bool dirty) { m_IsDirty = dirty; }

    bool isEmpty() const { return m_NonAirCount == 0; }
    void notifyBlockChange(BlockType oldType, BlockType newType);

    void buildMeshDataAsync(const Chunk& parentChunk);
    void uploadPendingMesh();
    void buildMesh(const Chunk& parentChunk);

    void render() const;
    void renderTransparent() const;

    ChunkMesh* getMesh() { return m_Mesh.get(); }
    bool hasPendingMesh() const { return m_PendingMeshData != nullptr; }

private:
    int m_SectionY;
    int m_NonAirCount = 0;
    bool m_IsDirty = true;

    std::unique_ptr<ChunkMesh> m_Mesh;
    std::unique_ptr<MeshData> m_PendingMeshData;
};

}

#endif // CHUNKSECTION_HPP
