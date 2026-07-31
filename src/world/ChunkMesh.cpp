#include "ChunkMesh.hpp"
#include "Chunk.hpp"

namespace Minecraft {

ChunkMesh::ChunkMesh() {
    if (glGenVertexArrays) glGenVertexArrays(1, &m_VAO);
    if (glGenBuffers) glGenBuffers(1, &m_VBO);
    if (glGenBuffers) glGenBuffers(1, &m_EBO);
}

ChunkMesh::~ChunkMesh() {
    clear();
    if (m_VAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO && glDeleteBuffers) glDeleteBuffers(1, &m_VBO);
    if (m_EBO && glDeleteBuffers) glDeleteBuffers(1, &m_EBO);
}

void ChunkMesh::clear() {
    m_IndexCount = 0;
}

void ChunkMesh::generate(const Chunk& chunk) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    int worldChunkX = chunk.getChunkX() * CHUNK_SIZE_X;
    int worldChunkZ = chunk.getChunkZ() * CHUNK_SIZE_Z;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                BlockType type = chunk.getBlock(x, y, z);
                if (type == BlockType::Air) continue;

                glm::vec3 blockPos(worldChunkX + x, y, worldChunkZ + z);

                // Top (+Y)
                if (y + 1 >= CHUNK_SIZE_Y || !BlockData::isOpaque(chunk.getBlock(x, y + 1, z)))
                    addFace(blockPos, TOP, type, vertices, indices);

                // Bottom (-Y)
                if (y - 1 < 0 || !BlockData::isOpaque(chunk.getBlock(x, y - 1, z)))
                    addFace(blockPos, BOTTOM, type, vertices, indices);

                // North (+Z)
                if (z + 1 >= CHUNK_SIZE_Z || !BlockData::isOpaque(chunk.getBlock(x, y, z + 1)))
                    addFace(blockPos, NORTH, type, vertices, indices);

                // South (-Z)
                if (z - 1 < 0 || !BlockData::isOpaque(chunk.getBlock(x, y, z - 1)))
                    addFace(blockPos, SOUTH, type, vertices, indices);

                // East (+X)
                if (x + 1 >= CHUNK_SIZE_X || !BlockData::isOpaque(chunk.getBlock(x + 1, y, z)))
                    addFace(blockPos, EAST, type, vertices, indices);

                // West (-X)
                if (x - 1 < 0 || !BlockData::isOpaque(chunk.getBlock(x - 1, y, z)))
                    addFace(blockPos, WEST, type, vertices, indices);
            }
        }
    }

    m_IndexCount = indices.size();
    if (m_IndexCount == 0 || !glBindVertexArray) return;

    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    // TexCoord
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    // Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    // Light
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, light));

    glBindVertexArray(0);
}

void ChunkMesh::render() const {
    if (m_IndexCount == 0 || !glBindVertexArray) return;
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_IndexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ChunkMesh::addFace(const glm::vec3& p, Direction dir, BlockType type, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    unsigned int startIndex = static_cast<unsigned int>(vertices.size());

    float baseLight = 1.0f;
    glm::vec3 normal(0.0f);
    glm::vec2 baseUV = BlockData::getTextureUV(type, dir);
    float tw = 1.0f / 16.0f;

    glm::vec2 uv0 = baseUV;
    glm::vec2 uv1 = baseUV + glm::vec2(tw, 0.0f);
    glm::vec2 uv2 = baseUV + glm::vec2(tw, tw);
    glm::vec2 uv3 = baseUV + glm::vec2(0.0f, tw);

    switch (dir) {
        case TOP:
            normal = glm::vec3(0, 1, 0); baseLight = 1.0f;
            vertices.push_back({ p + glm::vec3(0, 1, 0), uv0, normal, baseLight * 1.00f });
            vertices.push_back({ p + glm::vec3(1, 1, 0), uv1, normal, baseLight * 0.92f });
            vertices.push_back({ p + glm::vec3(1, 1, 1), uv2, normal, baseLight * 0.85f });
            vertices.push_back({ p + glm::vec3(0, 1, 1), uv3, normal, baseLight * 0.92f });
            break;
        case BOTTOM:
            normal = glm::vec3(0, -1, 0); baseLight = 0.5f;
            vertices.push_back({ p + glm::vec3(0, 0, 1), uv0, normal, baseLight });
            vertices.push_back({ p + glm::vec3(1, 0, 1), uv1, normal, baseLight });
            vertices.push_back({ p + glm::vec3(1, 0, 0), uv2, normal, baseLight });
            vertices.push_back({ p + glm::vec3(0, 0, 0), uv3, normal, baseLight });
            break;
        case NORTH:
            normal = glm::vec3(0, 0, 1); baseLight = 0.8f;
            vertices.push_back({ p + glm::vec3(0, 0, 1), uv0, normal, baseLight * 0.95f });
            vertices.push_back({ p + glm::vec3(1, 0, 1), uv1, normal, baseLight * 0.88f });
            vertices.push_back({ p + glm::vec3(1, 1, 1), uv2, normal, baseLight * 1.00f });
            vertices.push_back({ p + glm::vec3(0, 1, 1), uv3, normal, baseLight * 0.95f });
            break;
        case SOUTH:
            normal = glm::vec3(0, 0, -1); baseLight = 0.8f;
            vertices.push_back({ p + glm::vec3(1, 0, 0), uv0, normal, baseLight * 0.88f });
            vertices.push_back({ p + glm::vec3(0, 0, 0), uv1, normal, baseLight * 0.95f });
            vertices.push_back({ p + glm::vec3(0, 1, 0), uv2, normal, baseLight * 1.00f });
            vertices.push_back({ p + glm::vec3(1, 1, 0), uv3, normal, baseLight * 0.95f });
            break;
        case EAST:
            normal = glm::vec3(1, 0, 0); baseLight = 0.7f;
            vertices.push_back({ p + glm::vec3(1, 0, 1), uv0, normal, baseLight * 0.90f });
            vertices.push_back({ p + glm::vec3(1, 0, 0), uv1, normal, baseLight * 0.85f });
            vertices.push_back({ p + glm::vec3(1, 1, 0), uv2, normal, baseLight * 1.00f });
            vertices.push_back({ p + glm::vec3(1, 1, 1), uv3, normal, baseLight * 0.95f });
            break;
        case WEST:
            normal = glm::vec3(-1, 0, 0); baseLight = 0.7f;
            vertices.push_back({ p + glm::vec3(0, 0, 0), uv0, normal, baseLight * 0.85f });
            vertices.push_back({ p + glm::vec3(0, 0, 1), uv1, normal, baseLight * 0.90f });
            vertices.push_back({ p + glm::vec3(0, 1, 1), uv2, normal, baseLight * 1.00f });
            vertices.push_back({ p + glm::vec3(0, 1, 0), uv3, normal, baseLight * 0.95f });
            break;
    }

    indices.push_back(startIndex + 0);
    indices.push_back(startIndex + 1);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 2);
    indices.push_back(startIndex + 3);
    indices.push_back(startIndex + 0);
}

}
