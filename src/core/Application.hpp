#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window.hpp"
#include "../renderer/Shader.hpp"
#include "../renderer/Camera.hpp"
#include "../world/World.hpp"
#include "../world/Block.hpp"
#include <memory>

namespace Minecraft {

class Application {
public:
    Application();
    ~Application();

    void run();

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<Shader> m_BlockShader;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<World> m_World;

    bool m_IsRunning = true;
    bool m_IsFlying = true;
    bool m_IsGrounded = false;
    glm::vec3 m_PlayerVelocity{ 0.0f };
    BlockType m_SelectedBlock = BlockType::Grass;

    bool m_LeftMousePressedLast = false;
    bool m_RightMousePressedLast = false;
    bool m_FPressedLast = false;
};

}

#endif // APPLICATION_HPP
