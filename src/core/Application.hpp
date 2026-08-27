#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <memory>
#include <glm/glm.hpp>
#include "../vendor/glad/glad.h"
#include "Window.hpp"
#include "ThreadPool.hpp"
#include "../renderer/Shader.hpp"
#include "../renderer/Camera.hpp"
#include "../renderer/ShadowMap.hpp"
#include "../renderer/PostProcessing.hpp"
#include "../world/World.hpp"
#include "../world/Block.hpp"
#include "../world/TimeManager.hpp"
#include "../world/ChestBlock.hpp"
#include "../world/FurnaceBlock.hpp"
#include "../world/WeatherManager.hpp"
#include "../world/DimensionManager.hpp"
#include "../gui/HUD.hpp"
#include "../gui/InventoryGUI.hpp"
#include "../gui/ContainerGUI.hpp"
#include "../gui/MenuGUI.hpp"
#include "../renderer/Skybox.hpp"
#include "../inventory/Inventory.hpp"
#include "../inventory/PlayerStats.hpp"
#include "../ecs/MobEngine.hpp"
#include "../ecs/ItemEntity.hpp"
#include "../renderer/ParticleEngine.hpp"
#include "../renderer/FrustumCuller.hpp"
#include "../net/NetworkManager.hpp"

namespace Minecraft {

enum class GameState {
    MainMenu,
    SettingsMenu,
    Playing,
    Paused
};

class Application {
public:
    Application();
    ~Application();

    void run();
    bool executeCommand(const std::string& cmd);

private:
    void processInput(float deltaTime);
    void update(float deltaTime);
    void render();

    std::unique_ptr<Window> m_Window;
    std::unique_ptr<ThreadPool> m_ThreadPool;
    std::unique_ptr<Shader> m_BlockShader;
    std::unique_ptr<Shader> m_ShadowShader;
    std::unique_ptr<ShadowMap> m_ShadowMap;
    std::unique_ptr<PostProcessing> m_PostProcessing;
    std::unique_ptr<Skybox> m_Skybox;
    std::unique_ptr<Camera> m_Camera;
    std::unique_ptr<DimensionManager> m_DimensionManager;
    std::unique_ptr<TimeManager> m_TimeManager;
    std::unique_ptr<WeatherManager> m_WeatherManager;
    std::unique_ptr<HUD> m_HUD;
    std::unique_ptr<InventoryGUI> m_InventoryGUI;
    std::unique_ptr<ContainerGUI> m_ContainerGUI;
    std::unique_ptr<MenuGUI> m_MenuGUI;
    std::unique_ptr<Inventory> m_Inventory;
    std::unique_ptr<PlayerStats> m_PlayerStats;
    std::unique_ptr<MobEngine> m_MobEngine;
    std::unique_ptr<ItemEntityManager> m_ItemEntityManager;
    std::unique_ptr<ChestManager> m_ChestManager;
    std::unique_ptr<FurnaceManager> m_FurnaceManager;
    std::unique_ptr<ParticleEngine> m_ParticleEngine;
    std::unique_ptr<FrustumCuller> m_FrustumCuller;
    std::unique_ptr<NetworkManager> m_NetworkManager;

    GameState m_State = GameState::MainMenu;
    bool m_IsRunning = true;
    bool m_IsFlying = true;
    bool m_IsGrounded = false;
    bool m_InWater = false;
    bool m_ShowDebugInfo = false;
    bool m_IsInventoryOpen = false;

    int m_RenderDistance = 8;
    float m_FOV = 75.0f;
    bool m_VSync = true;
    std::string m_WorldSeed = "12345";

    float m_FPS = 0.0f;
    float m_FrameCounter = 0;
    float m_FpsTimer = 0.0f;
    float m_StepTimer = 0.0f;

    glm::vec3 m_PlayerVelocity{ 0.0f };
    BlockType m_SelectedBlock = BlockType::Grass;
    int m_SelectedSlot = 0;

    float m_MiningProgress = 0.0f;
    glm::ivec3 m_MiningBlockPos{ 0 };
    float m_ArmSwingProgress = 0.0f;
    float m_WalkBobbing = 0.0f;

    bool m_LeftMousePressedLast = false;
    bool m_RightMousePressedLast = false;
    bool m_FPressedLast = false;
    bool m_F3PressedLast = false;
    bool m_F4PressedLast = false;
    bool m_EPressedLast = false;
    bool m_EscPressedLast = false;
};

}

#endif // APPLICATION_HPP
