#ifndef SKYBOX_HPP
#define SKYBOX_HPP

#include <glm/glm.hpp>
#include "../renderer/Shader.hpp"
#include "../vendor/glad/glad.h"

namespace Minecraft {

class Skybox {
public:
    Skybox();
    ~Skybox();

    void render(const glm::mat4& view, const glm::mat4& projection, float timeOfDay);

private:
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
};

}

#endif // SKYBOX_HPP
