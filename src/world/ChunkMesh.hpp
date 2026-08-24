#ifndef CHUNKMESH_HPP
#define CHUNKMESH_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "Block.hpp"

namespace Minecraft {

class Chunk;

struct Vertex {
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    float light;
    float ao;
};

struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Vertex> transVertices;
    std::vector<unsigned int> transIndices;

    void clear() {
        vertices.clear();
        indices.clear();
        transVertices.clear();
        transIndices.clear();
    }
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void generate(const Chunk& chunk);
    static MeshData buildMeshData(const Chunk& chunk);
    void uploadMeshData(const MeshData& data);
    void uploadMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void render() const;
    void renderTransparent() const;
    void clear();

    static float calculateVertexAO(bool side1, bool side2, bool corner);

private:
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
    size_t m_IndexCount = 0;

    GLuint m_TransVAO = 0;
    GLuint m_TransVBO = 0;
    GLuint m_TransEBO = 0;
    size_t m_TransIndexCount = 0;

    static void addFace(const Chunk& chunk, int lx, int ly, int lz, const glm::vec3& blockPos, Direction dir, BlockType type, MeshData& meshData);
    static void addMergedFace(const glm::vec3& startPos, float w, float h, Direction dir, BlockType type, float baseLight, const float cornerAO[4], std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};

}

#endif // CHUNKMESH_HPP
