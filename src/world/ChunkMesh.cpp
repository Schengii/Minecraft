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
    buildMeshData(chunk, vertices, indices);
    uploadMeshData(vertices, indices);
}

struct FaceInfo {
    bool active = false;
    BlockType type = BlockType::Air;
    uint8_t light = 0;

    bool operator==(const FaceInfo& o) const {
        return active == o.active && type == o.type && light == o.light;
    }
    bool operator!=(const FaceInfo& o) const {
        return !(*this == o);
    }
};

void ChunkMesh::buildMeshData(const Chunk& chunk, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    vertices.clear();
    indices.clear();

    int worldChunkX = chunk.getChunkX() * CHUNK_SIZE_X;
    int worldChunkZ = chunk.getChunkZ() * CHUNK_SIZE_Z;

    // 1. Greedy meshing for TOP (+Y) and BOTTOM (-Y)
    for (int dirIdx = 0; dirIdx < 2; ++dirIdx) {
        Direction dir = (dirIdx == 0) ? TOP : BOTTOM;
        int dy = (dir == TOP) ? 1 : -1;

        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            FaceInfo mask[CHUNK_SIZE_X][CHUNK_SIZE_Z];

            for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                    BlockType type = chunk.getBlock(x, y, z);
                    if (type == BlockType::Air) continue;

                    int ny = y + dy;
                    bool visible = (ny < 0 || ny >= CHUNK_SIZE_Y || !BlockData::isOpaque(chunk.getBlock(x, ny, z)));
                    if (visible) {
                        if (ny >= 0 && ny < CHUNK_SIZE_Y) {
                            uint8_t s = chunk.getSunlight(x, ny, z);
                            uint8_t b = chunk.getBlocklight(x, ny, z);
                            mask[x][z] = { true, type, static_cast<uint8_t>(std::max(s, b)) };
                        } else {
                            mask[x][z] = { true, type, 15 };
                        }
                    }
                }
            }

            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                    if (!mask[x][z].active) continue;

                    FaceInfo current = mask[x][z];
                    int w = 1, h = 1;

                    while (x + w < CHUNK_SIZE_X && mask[x + w][z] == current) {
                        w++;
                    }

                    bool canExtend = true;
                    while (z + h < CHUNK_SIZE_Z && canExtend) {
                        for (int k = 0; k < w; ++k) {
                            if (mask[x + k][z + h] != current) {
                                canExtend = false;
                                break;
                            }
                        }
                        if (canExtend) h++;
                    }

                    glm::vec3 pos(worldChunkX + x, y, worldChunkZ + z);
                    float baseLight = std::max(0.12f, static_cast<float>(current.light) / 15.0f);
                    addMergedFace(pos, static_cast<float>(w), static_cast<float>(h), dir, current.type, baseLight, vertices, indices);

                    for (int dz = 0; dz < h; ++dz) {
                        for (int dx = 0; dx < w; ++dx) {
                            mask[x + dx][z + dz].active = false;
                        }
                    }
                }
            }
        }
    }

    // 2. Greedy meshing for NORTH (+Z) and SOUTH (-Z)
    for (int dirIdx = 0; dirIdx < 2; ++dirIdx) {
        Direction dir = (dirIdx == 0) ? NORTH : SOUTH;
        int dz = (dir == NORTH) ? 1 : -1;

        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            std::vector<std::vector<FaceInfo>> mask(CHUNK_SIZE_X, std::vector<FaceInfo>(CHUNK_SIZE_Y));

            for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                    BlockType type = chunk.getBlock(x, y, z);
                    if (type == BlockType::Air) continue;

                    int nz = z + dz;
                    bool visible = (nz < 0 || nz >= CHUNK_SIZE_Z || !BlockData::isOpaque(chunk.getBlock(x, y, nz)));
                    if (visible) {
                        if (nz >= 0 && nz < CHUNK_SIZE_Z) {
                            uint8_t s = chunk.getSunlight(x, y, nz);
                            uint8_t b = chunk.getBlocklight(x, y, nz);
                            mask[x][y] = { true, type, static_cast<uint8_t>(std::max(s, b)) };
                        } else {
                            mask[x][y] = { true, type, 15 };
                        }
                    }
                }
            }

            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                    if (!mask[x][y].active) continue;

                    FaceInfo current = mask[x][y];
                    int w = 1, h = 1;

                    while (x + w < CHUNK_SIZE_X && mask[x + w][y] == current) {
                        w++;
                    }

                    bool canExtend = true;
                    while (y + h < CHUNK_SIZE_Y && canExtend) {
                        for (int k = 0; k < w; ++k) {
                            if (mask[x + k][y + h] != current) {
                                canExtend = false;
                                break;
                            }
                        }
                        if (canExtend) h++;
                    }

                    glm::vec3 pos(worldChunkX + x, y, worldChunkZ + z);
                    float baseLight = std::max(0.12f, static_cast<float>(current.light) / 15.0f);
                    addMergedFace(pos, static_cast<float>(w), static_cast<float>(h), dir, current.type, baseLight, vertices, indices);

                    for (int dy = 0; dy < h; ++dy) {
                        for (int dx = 0; dx < w; ++dx) {
                            mask[x + dx][y + dy].active = false;
                        }
                    }
                }
            }
        }
    }

    // 3. Greedy meshing for EAST (+X) and WEST (-X)
    for (int dirIdx = 0; dirIdx < 2; ++dirIdx) {
        Direction dir = (dirIdx == 0) ? EAST : WEST;
        int dx = (dir == EAST) ? 1 : -1;

        for (int x = 0; x < CHUNK_SIZE_X; ++x) {
            std::vector<std::vector<FaceInfo>> mask(CHUNK_SIZE_Z, std::vector<FaceInfo>(CHUNK_SIZE_Y));

            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                    BlockType type = chunk.getBlock(x, y, z);
                    if (type == BlockType::Air) continue;

                    int nx = x + dx;
                    bool visible = (nx < 0 || nx >= CHUNK_SIZE_X || !BlockData::isOpaque(chunk.getBlock(nx, y, z)));
                    if (visible) {
                        if (nx >= 0 && nx < CHUNK_SIZE_X) {
                            uint8_t s = chunk.getSunlight(nx, y, z);
                            uint8_t b = chunk.getBlocklight(nx, y, z);
                            mask[z][y] = { true, type, static_cast<uint8_t>(std::max(s, b)) };
                        } else {
                            mask[z][y] = { true, type, 15 };
                        }
                    }
                }
            }

            for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
                for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                    if (!mask[z][y].active) continue;

                    FaceInfo current = mask[z][y];
                    int w = 1, h = 1;

                    while (z + w < CHUNK_SIZE_Z && mask[z + w][y] == current) {
                        w++;
                    }

                    bool canExtend = true;
                    while (y + h < CHUNK_SIZE_Y && canExtend) {
                        for (int k = 0; k < w; ++k) {
                            if (mask[z + k][y + h] != current) {
                                canExtend = false;
                                break;
                            }
                        }
                        if (canExtend) h++;
                    }

                    glm::vec3 pos(worldChunkX + x, y, worldChunkZ + z);
                    float baseLight = std::max(0.12f, static_cast<float>(current.light) / 15.0f);
                    addMergedFace(pos, static_cast<float>(w), static_cast<float>(h), dir, current.type, baseLight, vertices, indices);

                    for (int dy = 0; dy < h; ++dy) {
                        for (int dz = 0; dz < w; ++dz) {
                            mask[z + dz][y + dy].active = false;
                        }
                    }
                }
            }
        }
    }
}

void ChunkMesh::uploadMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    m_IndexCount = indices.size();
    if (m_IndexCount == 0 || !glBindVertexArray) return;

    if (!m_VAO && glGenVertexArrays) glGenVertexArrays(1, &m_VAO);
    if (!m_VBO && glGenBuffers) glGenBuffers(1, &m_VBO);
    if (!m_EBO && glGenBuffers) glGenBuffers(1, &m_EBO);

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

void ChunkMesh::addFace(const Chunk& chunk, int lx, int ly, int lz, const glm::vec3& p, Direction dir, BlockType type, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    addMergedFace(p, 1.0f, 1.0f, dir, type, 1.0f, vertices, indices);
}

void ChunkMesh::addMergedFace(const glm::vec3& p, float w, float h, Direction dir, BlockType type, float baseLight, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    unsigned int startIndex = static_cast<unsigned int>(vertices.size());
    glm::vec3 normal(0.0f);
    glm::vec2 baseUV = BlockData::getTextureUV(type, dir);
    float tw = 1.0f / 16.0f;

    glm::vec2 uv0 = baseUV;
    glm::vec2 uv1 = baseUV + glm::vec2(tw * w, 0.0f);
    glm::vec2 uv2 = baseUV + glm::vec2(tw * w, tw * h);
    glm::vec2 uv3 = baseUV + glm::vec2(0.0f, tw * h);

    switch (dir) {
        case TOP:
            normal = glm::vec3(0, 1, 0);
            vertices.push_back({ p + glm::vec3(0, 1, 0), uv0, normal, baseLight * 1.00f });
            vertices.push_back({ p + glm::vec3(w, 1, 0), uv1, normal, baseLight * 0.92f });
            vertices.push_back({ p + glm::vec3(w, 1, h), uv2, normal, baseLight * 0.85f });
            vertices.push_back({ p + glm::vec3(0, 1, h), uv3, normal, baseLight * 0.92f });
            break;
        case BOTTOM:
            normal = glm::vec3(0, -1, 0);
            vertices.push_back({ p + glm::vec3(0, 0, h), uv0, normal, baseLight * 0.50f });
            vertices.push_back({ p + glm::vec3(w, 0, h), uv1, normal, baseLight * 0.50f });
            vertices.push_back({ p + glm::vec3(w, 0, 0), uv2, normal, baseLight * 0.50f });
            vertices.push_back({ p + glm::vec3(0, 0, 0), uv3, normal, baseLight * 0.50f });
            break;
        case NORTH:
            normal = glm::vec3(0, 0, 1);
            vertices.push_back({ p + glm::vec3(0, 0, 1), uv0, normal, baseLight * 0.85f });
            vertices.push_back({ p + glm::vec3(w, 0, 1), uv1, normal, baseLight * 0.80f });
            vertices.push_back({ p + glm::vec3(w, h, 1), uv2, normal, baseLight * 0.90f });
            vertices.push_back({ p + glm::vec3(0, h, 1), uv3, normal, baseLight * 0.85f });
            break;
        case SOUTH:
            normal = glm::vec3(0, 0, -1);
            vertices.push_back({ p + glm::vec3(w, 0, 0), uv0, normal, baseLight * 0.80f });
            vertices.push_back({ p + glm::vec3(0, 0, 0), uv1, normal, baseLight * 0.85f });
            vertices.push_back({ p + glm::vec3(0, h, 0), uv2, normal, baseLight * 0.90f });
            vertices.push_back({ p + glm::vec3(w, h, 0), uv3, normal, baseLight * 0.85f });
            break;
        case EAST:
            normal = glm::vec3(1, 0, 0);
            vertices.push_back({ p + glm::vec3(1, 0, w), uv0, normal, baseLight * 0.75f });
            vertices.push_back({ p + glm::vec3(1, 0, 0), uv1, normal, baseLight * 0.70f });
            vertices.push_back({ p + glm::vec3(1, h, 0), uv2, normal, baseLight * 0.80f });
            vertices.push_back({ p + glm::vec3(1, h, w), uv3, normal, baseLight * 0.75f });
            break;
        case WEST:
            normal = glm::vec3(-1, 0, 0);
            vertices.push_back({ p + glm::vec3(0, 0, 0), uv0, normal, baseLight * 0.70f });
            vertices.push_back({ p + glm::vec3(0, 0, w), uv1, normal, baseLight * 0.75f });
            vertices.push_back({ p + glm::vec3(0, h, w), uv2, normal, baseLight * 0.80f });
            vertices.push_back({ p + glm::vec3(0, h, 0), uv3, normal, baseLight * 0.75f });
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
