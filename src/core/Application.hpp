#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window.hpp"
#include "../renderer/Shader.hpp"
#include "../renderer/Camera.hpp"
#include "../world/World.hpp"
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
    float m_LastFrameTime = 0.0f;
};

}

#endif // APPLICATION_HPP
