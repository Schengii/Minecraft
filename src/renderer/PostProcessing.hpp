#ifndef POSTPROCESSING_HPP
#define POSTPROCESSING_HPP

#include <glad/glad.h>
#include <memory>
#include "Shader.hpp"

namespace Minecraft {

class PostProcessing {
public:
    PostProcessing(unsigned int width, unsigned int height);
    ~PostProcessing();

    void resize(unsigned int width, unsigned int height);
    void bindForWriting();
    void unbindAndRender(bool isUnderwater, bool hasNightVision = false, bool bloomEnabled = true, bool ssaoEnabled = true);

    GLuint getColorTexture() const { return m_ColorTex; }
    bool isBloomEnabled() const { return m_BloomEnabled; }
    void setBloomEnabled(bool enabled) { m_BloomEnabled = enabled; }
    bool isSSAOEnabled() const { return m_SSAOEnabled; }
    void setSSAOEnabled(bool enabled) { m_SSAOEnabled = enabled; }

private:
    GLuint m_FBO = 0;
    GLuint m_ColorTex = 0;
    GLuint m_RBO = 0;
    GLuint m_QuadVAO = 0;
    GLuint m_QuadVBO = 0;

    unsigned int m_Width = 1280;
    unsigned int m_Height = 720;
    bool m_BloomEnabled = true;
    bool m_SSAOEnabled = true;
    std::unique_ptr<Shader> m_Shader;

    void initQuad();
};

}

#endif // POSTPROCESSING_HPP
