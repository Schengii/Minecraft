#include "Application.hpp"
#include "Input.hpp"
#include <iostream>

namespace Minecraft {

Application::Application() {
    m_Window = std::make_unique<Window>(1280, 720, "Minecraft C++ OpenGL 1:1 Engine");
    Input::init(m_Window->getNativeWindow());

    m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 65.0f, 0.0f));
    m_BlockShader = std::make_unique<Shader>("assets/shaders/block.vert", "assets/shaders/block.frag");
    m_World = std::make_unique<World>(4); // Render distance = 4 chunks
}

Application::~Application() = default;

void Application::run() {
    float lastTime = static_cast<float>(glfwGetTime());

    while (!m_Window->shouldClose() && m_IsRunning) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        m_Window->pollEvents();
        processInput(deltaTime);
        update(deltaTime);
        render();
        m_Window->swapBuffers();
    }
}

void Application::processInput(float deltaTime) {
    if (Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
        m_IsRunning = false;
    }

    if (Input::isKeyPressed(GLFW_KEY_W))
        m_Camera->processKeyboard(FORWARD, deltaTime);
    if (Input::isKeyPressed(GLFW_KEY_S))
        m_Camera->processKeyboard(BACKWARD, deltaTime);
    if (Input::isKeyPressed(GLFW_KEY_A))
        m_Camera->processKeyboard(LEFT, deltaTime);
    if (Input::isKeyPressed(GLFW_KEY_D))
        m_Camera->processKeyboard(RIGHT, deltaTime);
    if (Input::isKeyPressed(GLFW_KEY_SPACE))
        m_Camera->processKeyboard(UP, deltaTime);
    if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT))
        m_Camera->processKeyboard(DOWN, deltaTime);

    if (m_Window->isCursorCaptured()) {
        float dx = static_cast<float>(Input::getMouseDX());
        float dy = static_cast<float>(Input::getMouseDY());
        m_Camera->processMouseMovement(dx, dy);
    }

    Input::updateMouseDelta();
}

void Application::update(float deltaTime) {
    (void)deltaTime;
    if (m_World && m_Camera) {
        m_World->update(m_Camera->getPosition());
    }
}

void Application::render() {
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Minecraft Sky Blue Color
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (m_BlockShader && m_Camera && m_World) {
        m_BlockShader->use();
        
        glm::mat4 projection = m_Camera->getProjectionMatrix(m_Window->getAspectRatio());
        glm::mat4 view = m_Camera->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        m_BlockShader->setMat4("u_Projection", projection);
        m_BlockShader->setMat4("u_View", view);
        m_BlockShader->setMat4("u_Model", model);

        m_BlockShader->setVec3("u_SunDirection", glm::vec3(0.5f, 1.0f, 0.3f));
        m_BlockShader->setVec3("u_SunColor", glm::vec3(1.0f, 0.95f, 0.8f));
        m_BlockShader->setVec3("u_SkyColor", glm::vec3(0.53f, 0.81f, 0.98f));

        m_World->render();
    }
}

}
