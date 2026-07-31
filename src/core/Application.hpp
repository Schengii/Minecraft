#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include "Window.hpp"
#include "../renderer/Shader.hpp"
#include "../renderer/Camera.hpp"
#include "../world/World.hpp"
#include "../world/Block.hpp"
#include "../world/TimeManager.hpp"
#include "../gui/HUD.hpp"
#include "../gui/InventoryGUI.hpp"
#include "../inventory/Inventory.hpp"
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
    std::unique_ptr<TimeManager> m_TimeManager;
    std::unique_ptr<HUD> m_HUD;
    std::unique_ptr<InventoryGUI> m_InventoryGUI;
    std::unique_ptr<Inventory> m_Inventory;

    bool m_IsRunning = true;
    bool m_IsFlying = true;
    bool m_IsGrounded = false;
    bool m_InWater = false;
    bool m_ShowDebugInfo = false;
    bool m_IsInventoryOpen = false;

    float m_FPS = 0.0f;
    float m_FrameCounter = 0;
    float m_FpsTimer = 0.0f;

    glm::vec3 m_PlayerVelocity{ 0.0f };
    BlockType m_SelectedBlock = BlockType::Grass;
    int m_SelectedSlot = 0;

    bool m_LeftMousePressedLast = false;
    bool m_RightMousePressedLast = false;
    bool m_FPressedLast = false;
    bool m_F3PressedLast = false;
    bool m_F4PressedLast = false;
    bool m_EPressedLast = false;
};

}

#endif // APPLICATION_HPP
