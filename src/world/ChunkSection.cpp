#include "ChunkSection.hpp"
#include "Chunk.hpp"

namespace Minecraft {

ChunkSection::ChunkSection(int sectionY)
    : m_SectionY(sectionY), m_Mesh(std::make_unique<ChunkMesh>())
{
}

ChunkSection::~ChunkSection() = default;

void ChunkSection::notifyBlockChange(BlockType oldType, BlockType newType) {
    if (oldType == BlockType::Air && newType != BlockType::Air) {
        m_NonAirCount++;
    } else if (oldType != BlockType::Air && newType == BlockType::Air) {
        if (m_NonAirCount > 0) m_NonAirCount--;
    }
    m_IsDirty = true;
}

void ChunkSection::buildMeshDataAsync(const Chunk& parentChunk) {
    m_PendingMeshData = std::make_unique<MeshData>(ChunkMesh::buildMeshData(parentChunk));
}

void ChunkSection::uploadPendingMesh() {
    if (m_PendingMeshData && m_Mesh) {
        m_Mesh->uploadMeshData(*m_PendingMeshData);
        m_PendingMeshData.reset();
        m_IsDirty = false;
    }
}

void ChunkSection::buildMesh(const Chunk& parentChunk) {
    if (m_IsDirty && m_Mesh) {
        m_Mesh->generate(parentChunk);
        m_IsDirty = false;
    }
}

void ChunkSection::render() const {
    if (m_Mesh && m_NonAirCount > 0) {
        m_Mesh->render();
    }
}

void ChunkSection::renderTransparent() const {
    if (m_Mesh && m_NonAirCount > 0) {
        m_Mesh->renderTransparent();
    }
}

}
