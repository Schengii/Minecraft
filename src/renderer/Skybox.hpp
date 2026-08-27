#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glm/glm.hpp>
#include "../renderer/Shader.hpp"
#include "../renderer/Camera.hpp"
#include "../vendor/glad/glad.h"
#include <memory>

namespace Minecraft {

class Skybox {
public:
    Skybox();
    ~Skybox();

    void render(const glm::mat4& view, const glm::mat4& projection, float timeOfDay);
    void renderClouds(const Camera& camera, float timeOfDay, float totalTime);

private:
    void initCloudBuffers();

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;

    GLuint m_CloudVAO = 0;
    GLuint m_CloudVBO = 0;
    std::unique_ptr<Shader> m_CloudShader;
    bool m_CloudsInitialized = false;
};

}

#endif // SKYBOX_HPP
