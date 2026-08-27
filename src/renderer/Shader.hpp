#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>

namespace Minecraft {

class Shader {
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    void use() const;

    void setBool(const std::string& name, bool value) const;
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setVec2(const std::string& name, const glm::vec2& value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec4(const std::string& name, const glm::vec4& value) const;
    void setMat4(const std::string& name, const glm::mat4& value) const;

    GLuint getID() const { return m_RendererID; }

private:
    GLuint m_RendererID = 0;
    mutable std::unordered_map<std::string, GLint> m_UniformLocationCache;

    GLint getUniformLocation(const std::string& name) const;
    std::string readFile(const std::string& filePath);
    GLuint compileShader(GLenum type, const std::string& source);
};

}

#endif // SHADER_HPP
