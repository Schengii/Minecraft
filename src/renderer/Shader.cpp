#include "Shader.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>

namespace Minecraft {

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath) {
    std::string vertexCode = readFile(vertexPath);
    std::string fragmentCode = readFile(fragmentPath);

    GLuint vertShader = compileShader(GL_VERTEX_SHADER, vertexCode);
    GLuint fragShader = compileShader(GL_FRAGMENT_SHADER, fragmentCode);

    m_RendererID = glCreateProgram();
    glAttachShader(m_RendererID, vertShader);
    glAttachShader(m_RendererID, fragShader);
    glLinkProgram(m_RendererID);

    GLint success;
    glGetProgramiv(m_RendererID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetProgramInfoLog(m_RendererID, 512, nullptr, infoLog);
        std::cerr << "[Shader Error] Program Linking Failed:\n" << infoLog << std::endl;
    }

    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
}

Shader::~Shader() {
    if (m_RendererID) {
        glDeleteProgram(m_RendererID);
    }
}

void Shader::use() const {
    glUseProgram(m_RendererID);
}

void Shader::setBool(const std::string& name, bool value) const {
    glUniform1i(getUniformLocation(name), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
    glUniform1i(getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, float value) const {
    glUniform1f(getUniformLocation(name), value);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
    glUniform3f(getUniformLocation(name), value.x, value.y, value.z);
}

void Shader::setMat4(const std::string& name, const glm::mat4& value) const {
    glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

GLint Shader::getUniformLocation(const std::string& name) const {
    if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
        return m_UniformLocationCache[name];

    GLint location = glGetUniformLocation(m_RendererID, name.c_str());
    m_UniformLocationCache[name] = location;
    return location;
}

std::string Shader::readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Shader Error] Could not open shader file: " << filePath << std::endl;
        return "";
    }
    std::stringstream stream;
    stream << file.rdbuf();
    return stream.str();
}

GLuint Shader::compileShader(GLenum type, const std::string& source) {
    GLuint shader = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);
        std::cerr << "[Shader Error] Compilation Failed:\n" << infoLog << std::endl;
    }
    return shader;
}

}
