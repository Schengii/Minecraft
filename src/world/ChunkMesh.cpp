#include "ChunkMesh.hpp"
#include "Chunk.hpp"
#include <algorithm>

namespace Minecraft {

ChunkMesh::ChunkMesh() {
    if (glGenVertexArrays) {
        glGenVertexArrays(1, &m_VAO);
        glGenVertexArrays(1, &m_TransVAO);
    }
    if (glGenBuffers) {
        glGenBuffers(1, &m_VBO);
        glGenBuffers(1, &m_EBO);
        glGenBuffers(1, &m_TransVBO);
        glGenBuffers(1, &m_TransEBO);
    }
}

ChunkMesh::~ChunkMesh() {
    clear();
    if (m_VAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO && glDeleteBuffers) glDeleteBuffers(1, &m_VBO);
    if (m_EBO && glDeleteBuffers) glDeleteBuffers(1, &m_EBO);

    if (m_TransVAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_TransVAO);
    if (m_TransVBO && glDeleteBuffers) glDeleteBuffers(1, &m_TransVBO);
    if (m_TransEBO && glDeleteBuffers) glDeleteBuffers(1, &m_TransEBO);
}

void ChunkMesh::clear() {
    m_IndexCount = 0;
    m_TransIndexCount = 0;
}

void ChunkMesh::render() const {
    if (m_VAO == 0 || m_IndexCount == 0) return;
    glBindVertexArray(m_VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_IndexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void ChunkMesh::renderTransparent() const {
    if (m_TransVAO == 0 || m_TransIndexCount == 0) return;
    glBindVertexArray(m_TransVAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_TransIndexCount), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

float ChunkMesh::calculateVertexAO(bool side1, bool side2, bool corner) {
    if (side1 && side2) {
        return 0.25f; // Deep corner shadow
    }
    int count = (side1 ? 1 : 0) + (side2 ? 1 : 0) + (corner ? 1 : 0);
    return 1.0f - (count * 0.22f);
}

void ChunkMesh::generate(const Chunk& chunk) {
    MeshData meshData = buildMeshData(chunk);
    uploadMeshData(meshData);
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

MeshData ChunkMesh::buildMeshData(const Chunk& chunk) {
    MeshData data;
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

                    // Vertex Ambient Occlusion calculation for corners
                    int ny = (dir == TOP) ? y + 1 : y - 1;
                    bool sLeft = BlockData::isOpaque(chunk.getBlock(x - 1, ny, z));
                    bool sRight = BlockData::isOpaque(chunk.getBlock(x + w, ny, z));
                    bool sBack = BlockData::isOpaque(chunk.getBlock(x, ny, z - 1));
                    bool sFront = BlockData::isOpaque(chunk.getBlock(x, ny, z + h));

                    bool c0 = BlockData::isOpaque(chunk.getBlock(x - 1, ny, z - 1));
                    bool c1 = BlockData::isOpaque(chunk.getBlock(x + w, ny, z - 1));
                    bool c2 = BlockData::isOpaque(chunk.getBlock(x + w, ny, z + h));
                    bool c3 = BlockData::isOpaque(chunk.getBlock(x - 1, ny, z + h));

                    float cornerAO[4] = {
                        calculateVertexAO(sLeft, sBack, c0),
                        calculateVertexAO(sRight, sBack, c1),
                        calculateVertexAO(sRight, sFront, c2),
                        calculateVertexAO(sLeft, sFront, c3)
                    };

                    bool isTrans = (current.type == BlockType::Water || current.type == BlockType::Glass || current.type == BlockType::Leaves || current.type == BlockType::Bamboo);
                    auto& targetVerts = isTrans ? data.transVertices : data.vertices;
                    auto& targetIndices = isTrans ? data.transIndices : data.indices;

                    addMergedFace(pos, static_cast<float>(w), static_cast<float>(h), dir, current.type, baseLight, cornerAO, targetVerts, targetIndices);

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

                    int nz = (dir == NORTH) ? z + 1 : z - 1;
                    bool sLeft = BlockData::isOpaque(chunk.getBlock(x - 1, y, nz));
                    bool sRight = BlockData::isOpaque(chunk.getBlock(x + w, y, nz));
                    bool sDown = (y > 0) && BlockData::isOpaque(chunk.getBlock(x, y - 1, nz));
                    bool sUp = (y + h < CHUNK_SIZE_Y) && BlockData::isOpaque(chunk.getBlock(x, y + h, nz));

                    float cornerAO[4] = {
                        calculateVertexAO(sLeft, sDown, false),
                        calculateVertexAO(sRight, sDown, false),
                        calculateVertexAO(sRight, sUp, false),
                        calculateVertexAO(sLeft, sUp, false)
                    };

                    bool isTrans = (current.type == BlockType::Water || current.type == BlockType::Glass || current.type == BlockType::Leaves || current.type == BlockType::Bamboo);
                    auto& targetVerts = isTrans ? data.transVertices : data.vertices;
                    auto& targetIndices = isTrans ? data.transIndices : data.indices;

                    addMergedFace(pos, static_cast<float>(w), static_cast<float>(h), dir, current.type, baseLight, cornerAO, targetVerts, targetIndices);

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

                    int nx = (dir == EAST) ? x + 1 : x - 1;
                    bool sLeft = BlockData::isOpaque(chunk.getBlock(nx, y, z - 1));
                    bool sRight = BlockData::isOpaque(chunk.getBlock(nx, y, z + w));
                    bool sDown = (y > 0) && BlockData::isOpaque(chunk.getBlock(nx, y - 1, z));
                    bool sUp = (y + h < CHUNK_SIZE_Y) && BlockData::isOpaque(chunk.getBlock(nx, y + h, z));

                    float cornerAO[4] = {
                        calculateVertexAO(sLeft, sDown, false),
                        calculateVertexAO(sRight, sDown, false),
                        calculateVertexAO(sRight, sUp, false),
                        calculateVertexAO(sLeft, sUp, false)
                    };

                    bool isTrans = (current.type == BlockType::Water || current.type == BlockType::Glass || current.type == BlockType::Leaves || current.type == BlockType::Bamboo);
                    auto& targetVerts = isTrans ? data.transVertices : data.vertices;
                    auto& targetIndices = isTrans ? data.transIndices : data.indices;

                    addMergedFace(pos, static_cast<float>(w), static_cast<float>(h), dir, current.type, baseLight, cornerAO, targetVerts, targetIndices);

                    for (int dy = 0; dy < h; ++dy) {
                        for (int dz = 0; dz < w; ++dz) {
                            mask[z + dz][y + dy].active = false;
                        }
                    }
                }
            }
        }
    }

    return data;
}

void ChunkMesh::uploadMeshData(const MeshData& data) {
    uploadMeshData(data.vertices, data.indices);

    m_TransIndexCount = data.transIndices.size();
    if (m_TransIndexCount > 0 && glBindVertexArray) {
        if (!m_TransVAO && glGenVertexArrays) glGenVertexArrays(1, &m_TransVAO);
        if (!m_TransVBO && glGenBuffers) glGenBuffers(1, &m_TransVBO);
        if (!m_TransEBO && glGenBuffers) glGenBuffers(1, &m_TransEBO);

        glBindVertexArray(m_TransVAO);

        glBindBuffer(GL_ARRAY_BUFFER, m_TransVBO);
        glBufferData(GL_ARRAY_BUFFER, data.transVertices.size() * sizeof(Vertex), data.transVertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_TransEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.transIndices.size() * sizeof(unsigned int), data.transIndices.data(), GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, light));

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, ao));

        glBindVertexArray(0);
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

    // AO
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, ao));

    glBindVertexArray(0);
}

void ChunkMesh::addMergedFace(const glm::vec3& p, float w, float h, Direction dir, BlockType type, float baseLight, const float cornerAO[4], std::vector<Vertex>& vertices, std::vector<unsigned int>& indices) {
    glm::vec3 normal(0.0f);
    glm::vec2 baseUV = BlockData::getTextureUV(type, dir);
    float tw = 1.0f / 16.0f;
    glm::vec2 uv0 = baseUV;
    glm::vec2 uv1 = baseUV + glm::vec2(tw, 0.0f);
    glm::vec2 uv2 = baseUV + glm::vec2(tw, tw);
    glm::vec2 uv3 = baseUV + glm::vec2(0.0f, tw);

    int countW = static_cast<int>(w);
    int countH = static_cast<int>(h);

    for (int dw = 0; dw < countW; ++dw) {
        for (int dh = 0; dh < countH; ++dh) {
            unsigned int startIndex = static_cast<unsigned int>(vertices.size());

            // Interpolate corner AO factors across the merged quad
            float uFraction0 = static_cast<float>(dw) / std::max(1.0f, w);
            float uFraction1 = static_cast<float>(dw + 1) / std::max(1.0f, w);
            float vFraction0 = static_cast<float>(dh) / std::max(1.0f, h);
            float vFraction1 = static_cast<float>(dh + 1) / std::max(1.0f, h);

            float ao0 = glm::mix(glm::mix(cornerAO[0], cornerAO[1], uFraction0), glm::mix(cornerAO[3], cornerAO[2], uFraction0), vFraction0);
            float ao1 = glm::mix(glm::mix(cornerAO[0], cornerAO[1], uFraction1), glm::mix(cornerAO[3], cornerAO[2], uFraction1), vFraction0);
            float ao2 = glm::mix(glm::mix(cornerAO[0], cornerAO[1], uFraction1), glm::mix(cornerAO[3], cornerAO[2], uFraction1), vFraction1);
            float ao3 = glm::mix(glm::mix(cornerAO[0], cornerAO[1], uFraction0), glm::mix(cornerAO[3], cornerAO[2], uFraction0), vFraction1);

            switch (dir) {
                case TOP:
                    normal = glm::vec3(0, 1, 0);
                    vertices.push_back({ p + glm::vec3(dw, 1, dh), uv0, normal, baseLight * 1.00f, ao0 });
                    vertices.push_back({ p + glm::vec3(dw + 1, 1, dh), uv1, normal, baseLight * 0.95f, ao1 });
                    vertices.push_back({ p + glm::vec3(dw + 1, 1, dh + 1), uv2, normal, baseLight * 0.90f, ao2 });
                    vertices.push_back({ p + glm::vec3(dw, 1, dh + 1), uv3, normal, baseLight * 0.95f, ao3 });
                    break;
                case BOTTOM:
                    normal = glm::vec3(0, -1, 0);
                    vertices.push_back({ p + glm::vec3(dw, 0, dh + 1), uv0, normal, baseLight * 0.50f, ao0 });
                    vertices.push_back({ p + glm::vec3(dw + 1, 0, dh + 1), uv1, normal, baseLight * 0.50f, ao1 });
                    vertices.push_back({ p + glm::vec3(dw + 1, 0, dh), uv2, normal, baseLight * 0.50f, ao2 });
                    vertices.push_back({ p + glm::vec3(dw, 0, dh), uv3, normal, baseLight * 0.50f, ao3 });
                    break;
                case NORTH:
                    normal = glm::vec3(0, 0, 1);
                    vertices.push_back({ p + glm::vec3(dw, dh, 1), uv0, normal, baseLight * 0.85f, ao0 });
                    vertices.push_back({ p + glm::vec3(dw + 1, dh, 1), uv1, normal, baseLight * 0.80f, ao1 });
                    vertices.push_back({ p + glm::vec3(dw + 1, dh + 1, 1), uv2, normal, baseLight * 0.90f, ao2 });
                    vertices.push_back({ p + glm::vec3(dw, dh + 1, 1), uv3, normal, baseLight * 0.85f, ao3 });
                    break;
                case SOUTH:
                    normal = glm::vec3(0, 0, -1);
                    vertices.push_back({ p + glm::vec3(dw + 1, dh, 0), uv0, normal, baseLight * 0.80f, ao0 });
                    vertices.push_back({ p + glm::vec3(dw, dh, 0), uv1, normal, baseLight * 0.85f, ao1 });
                    vertices.push_back({ p + glm::vec3(dw, dh + 1, 0), uv2, normal, baseLight * 0.90f, ao2 });
                    vertices.push_back({ p + glm::vec3(dw + 1, dh + 1, 0), uv3, normal, baseLight * 0.85f, ao3 });
                    break;
                case EAST:
                    normal = glm::vec3(1, 0, 0);
                    vertices.push_back({ p + glm::vec3(1, dh, dw + 1), uv0, normal, baseLight * 0.75f, ao0 });
                    vertices.push_back({ p + glm::vec3(1, dh, dw), uv1, normal, baseLight * 0.70f, ao1 });
                    vertices.push_back({ p + glm::vec3(1, dh + 1, dw), uv2, normal, baseLight * 0.80f, ao2 });
                    vertices.push_back({ p + glm::vec3(1, dh + 1, dw + 1), uv3, normal, baseLight * 0.75f, ao3 });
                    break;
                case WEST:
                    normal = glm::vec3(-1, 0, 0);
                    vertices.push_back({ p + glm::vec3(0, dh, dw), uv0, normal, baseLight * 0.70f, ao0 });
                    vertices.push_back({ p + glm::vec3(0, dh, dw + 1), uv1, normal, baseLight * 0.75f, ao1 });
                    vertices.push_back({ p + glm::vec3(0, dh + 1, dw + 1), uv2, normal, baseLight * 0.80f, ao2 });
                    vertices.push_back({ p + glm::vec3(0, dh + 1, dw), uv3, normal, baseLight * 0.75f, ao3 });
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
}

}
