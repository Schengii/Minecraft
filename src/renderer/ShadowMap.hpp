#ifndef SHADOWMAP_HPP
#define SHADOWMAP_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Minecraft {

class ShadowMap {
public:
    ShadowMap(unsigned int width = 2048, unsigned int height = 2048);
    ~ShadowMap();

    void bindForWriting();
    void unbind(unsigned int currentWinWidth, unsigned int currentWinHeight);
    
    GLuint getDepthMapTexture() const { return m_DepthMapTex; }
    glm::mat4 getLightSpaceMatrix(const glm::vec3& sunDir, const glm::vec3& targetPos) const;

private:
    GLuint m_FBO = 0;
    GLuint m_DepthMapTex = 0;
    unsigned int m_Width = 2048;
    unsigned int m_Height = 2048;
};

}

#endif // SHADOWMAP_HPP
