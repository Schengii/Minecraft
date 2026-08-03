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
};

class ChunkMesh {
public:
    ChunkMesh();
    ~ChunkMesh();

    void generate(const Chunk& chunk);
    void buildMeshData(const Chunk& chunk, std::vector<Vertex>& outVertices, std::vector<unsigned int>& outIndices);
    void uploadMeshData(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
    void render() const;
    void clear();

private:
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;
    size_t m_IndexCount = 0;

    void addFace(const Chunk& chunk, int lx, int ly, int lz, const glm::vec3& blockPos, Direction dir, BlockType type, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};

}

#endif // CHUNKMESH_HPP
