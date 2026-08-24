#include "PostProcessing.hpp"
#include <iostream>

namespace Minecraft {

PostProcessing::PostProcessing(unsigned int width, unsigned int height) 
    : m_Width(width), m_Height(height) {
    if (!glGenFramebuffers) return;

    m_Shader = std::make_unique<Shader>("assets/shaders/postprocess.vert", "assets/shaders/postprocess.frag");

    glGenFramebuffers(1, &m_FBO);
    glGenTextures(1, &m_ColorTex);
    glGenRenderbuffers(1, &m_RBO);

    resize(width, height);
    initQuad();
}

PostProcessing::~PostProcessing() {
    if (m_ColorTex && glDeleteTextures) glDeleteTextures(1, &m_ColorTex);
    if (m_RBO && glDeleteRenderbuffers) glDeleteRenderbuffers(1, &m_RBO);
    if (m_FBO && glDeleteFramebuffers) glDeleteFramebuffers(1, &m_FBO);
    if (m_QuadVAO && glDeleteVertexArrays) glDeleteVertexArrays(1, &m_QuadVAO);
    if (m_QuadVBO && glDeleteBuffers) glDeleteBuffers(1, &m_QuadVBO);
}

void PostProcessing::resize(unsigned int width, unsigned int height) {
    if (width == 0 || height == 0 || !glBindFramebuffer) return;
    m_Width = width;
    m_Height = height;

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glBindTexture(GL_TEXTURE_2D, m_ColorTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_Width, m_Height, 0, GL_RGB, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorTex, 0);

    glBindRenderbuffer(GL_RENDERBUFFER, m_RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_Width, m_Height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostProcessing::bindForWriting() {
    if (!glBindFramebuffer) return;
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void PostProcessing::unbindAndRender(bool isUnderwater, bool hasNightVision, bool bloomEnabled) {
    if (!glBindFramebuffer) return;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_Shader) return;
    m_Shader->use();
    m_Shader->setBool("u_IsUnderwater", isUnderwater);
    m_Shader->setBool("u_HasNightVision", hasNightVision);
    m_Shader->setBool("u_BloomEnabled", bloomEnabled);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_ColorTex);
    m_Shader->setInt("u_ScreenTexture", 0);

    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

void PostProcessing::initQuad() {
    if (!glGenVertexArrays) return;
    float quadVertices[] = {
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &m_QuadVBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

    glBindVertexArray(0);
}

}
