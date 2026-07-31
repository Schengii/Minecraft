#include "Application.hpp"
#include "Input.hpp"
#include "../world/Raycast.hpp"
#include "../physics/PhysicsEngine.hpp"
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

    // Toggle Flying mode with 'F' key
    bool fPressedNow = Input::isKeyPressed(GLFW_KEY_F);
    if (fPressedNow && !m_FPressedLast) {
        m_IsFlying = !m_IsFlying;
        std::cout << "[Player] Flying Mode: " << (m_IsFlying ? "ENABLED" : "DISABLED") << std::endl;
    }
    m_FPressedLast = fPressedNow;

    // Hotbar Block selection (1-9)
    if (Input::isKeyPressed(GLFW_KEY_1)) m_SelectedBlock = BlockType::Grass;
    if (Input::isKeyPressed(GLFW_KEY_2)) m_SelectedBlock = BlockType::Dirt;
    if (Input::isKeyPressed(GLFW_KEY_3)) m_SelectedBlock = BlockType::Stone;
    if (Input::isKeyPressed(GLFW_KEY_4)) m_SelectedBlock = BlockType::OakLog;
    if (Input::isKeyPressed(GLFW_KEY_5)) m_SelectedBlock = BlockType::Leaves;
    if (Input::isKeyPressed(GLFW_KEY_6)) m_SelectedBlock = BlockType::Planks;
    if (Input::isKeyPressed(GLFW_KEY_7)) m_SelectedBlock = BlockType::Glass;
    if (Input::isKeyPressed(GLFW_KEY_8)) m_SelectedBlock = BlockType::Sand;
    if (Input::isKeyPressed(GLFW_KEY_9)) m_SelectedBlock = BlockType::Bedrock;

    // Movement Controls
    glm::vec3 front = m_Camera->getFront();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));

    if (!m_IsFlying) {
        front.y = 0.0f;
        front = glm::normalize(front);
    }

    float speed = m_IsFlying ? 15.0f : 6.0f;

    if (Input::isKeyPressed(GLFW_KEY_W)) m_PlayerVelocity += front * speed;
    if (Input::isKeyPressed(GLFW_KEY_S)) m_PlayerVelocity -= front * speed;
    if (Input::isKeyPressed(GLFW_KEY_A)) m_PlayerVelocity -= right * speed;
    if (Input::isKeyPressed(GLFW_KEY_D)) m_PlayerVelocity += right * speed;

    if (m_IsFlying) {
        if (Input::isKeyPressed(GLFW_KEY_SPACE)) m_PlayerVelocity.y += speed;
        if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) m_PlayerVelocity.y -= speed;
    } else {
        if (Input::isKeyPressed(GLFW_KEY_SPACE) && m_IsGrounded) {
            m_PlayerVelocity.y = 8.5f; // Jump impulse
            m_IsGrounded = false;
        }
    }

    // Mouse camera look
    if (m_Window->isCursorCaptured()) {
        float dx = static_cast<float>(Input::getMouseDX());
        float dy = static_cast<float>(Input::getMouseDY());
        m_Camera->processMouseMovement(dx, dy);
    }
    Input::updateMouseDelta();

    // Raycast Block Interaktionen (Abbauen / Platzieren)
    bool leftMouseNow = Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
    bool rightMouseNow = Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

    if ((leftMouseNow && !m_LeftMousePressedLast) || (rightMouseNow && !m_RightMousePressedLast)) {
        RaycastResult hit = Raycast::raycast(*m_World, m_Camera->getPosition(), m_Camera->getFront(), 6.0f);
        if (hit.hit) {
            if (leftMouseNow && !m_LeftMousePressedLast) {
                // Block abbauen
                m_World->setBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, BlockType::Air);
                std::cout << "[World] Destroyed block at (" << hit.blockPos.x << ", " << hit.blockPos.y << ", " << hit.blockPos.z << ")" << std::endl;
            } else if (rightMouseNow && !m_RightMousePressedLast) {
                // Block platzieren
                m_World->setBlock(hit.previousPos.x, hit.previousPos.y, hit.previousPos.z, m_SelectedBlock);
                std::cout << "[World] Placed block at (" << hit.previousPos.x << ", " << hit.previousPos.y << ", " << hit.previousPos.z << ")" << std::endl;
            }
        }
    }

    m_LeftMousePressedLast = leftMouseNow;
    m_RightMousePressedLast = rightMouseNow;
}

void Application::update(float deltaTime) {
    if (m_World && m_Camera) {
        glm::vec3 currentPos = m_Camera->getPosition();
        PhysicsEngine::updatePlayer(*m_World, currentPos, m_PlayerVelocity, m_IsGrounded, m_IsFlying, deltaTime);
        
        // Update camera position
        Camera tempCam(currentPos, glm::vec3(0, 1, 0));
        *m_Camera = tempCam;

        m_World->update(currentPos);
    }
}

void Application::render() {
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f); // Sky blue
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
