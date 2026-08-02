#include <glad/glad.h>
#include "ShadowMap.hpp"
#include <iostream>

namespace Minecraft {

ShadowMap::ShadowMap(unsigned int width, unsigned int height) 
    : m_Width(width), m_Height(height) {
    if (!glGenFramebuffers) return;

    glGenFramebuffers(1, &m_FBO);

    glGenTextures(1, &m_DepthMapTex);
    glBindTexture(GL_TEXTURE_2D, m_DepthMapTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthMapTex, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

ShadowMap::~ShadowMap() {
    if (m_DepthMapTex && glDeleteTextures) glDeleteTextures(1, &m_DepthMapTex);
    if (m_FBO && glDeleteFramebuffers) glDeleteFramebuffers(1, &m_FBO);
}

void ShadowMap::bindForWriting() {
    if (!glBindFramebuffer) return;
    glViewport(0, 0, m_Width, m_Height);
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
    glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMap::unbind(unsigned int currentWinWidth, unsigned int currentWinHeight) {
    if (!glBindFramebuffer) return;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, currentWinWidth, currentWinHeight);
}

glm::mat4 ShadowMap::getLightSpaceMatrix(const glm::vec3& sunDir, const glm::vec3& targetPos) const {
    glm::vec3 lightPos = targetPos - glm::normalize(sunDir) * 40.0f;
    glm::mat4 lightProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 1.0f, 90.0f);
    glm::mat4 lightView = glm::lookAt(lightPos, targetPos, glm::vec3(0.0f, 1.0f, 0.0f));
    return lightProjection * lightView;
}

}
