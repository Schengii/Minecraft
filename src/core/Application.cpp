#include <memory>
#include <iostream>
#include <glm/glm.hpp>
#include "../vendor/glad/glad.h"
#include "Application.hpp"
#include "Input.hpp"
#include "../world/Raycast.hpp"
#include "../world/RedstoneEngine.hpp"
#include "../world/FluidEngine.hpp"
#include "../world/ExplosionEngine.hpp"
#include "../world/ToolSystem.hpp"
#include "../world/StructureGenerator.hpp"
#include "../physics/PhysicsEngine.hpp"
#include "../audio/AudioManager.hpp"
#include "../inventory/FoodSystem.hpp"
#include <iostream>

namespace Minecraft {

Application::Application() {
    m_Window = std::make_unique<Window>(1280, 720, "Minecraft C++ OpenGL 1:1 Engine");
    Input::init(m_Window->getNativeWindow());
    AudioManager::init();

    m_ThreadPool = std::make_unique<ThreadPool>(4);
    m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 65.0f, 0.0f));
    m_BlockShader = std::make_unique<Shader>("assets/shaders/block.vert", "assets/shaders/block.frag");
    m_ShadowShader = std::make_unique<Shader>("assets/shaders/shadow.vert", "assets/shaders/shadow.frag");
    m_ShadowMap = std::make_unique<ShadowMap>(2048, 2048);
    m_PostProcessing = std::make_unique<PostProcessing>(m_Window->getWidth(), m_Window->getHeight());

    m_DimensionManager = std::make_unique<DimensionManager>();
    m_TimeManager = std::make_unique<TimeManager>();
    m_WeatherManager = std::make_unique<WeatherManager>();
    m_HUD = std::make_unique<HUD>(m_Window->getWidth(), m_Window->getHeight());
    m_InventoryGUI = std::make_unique<InventoryGUI>(m_Window->getWidth(), m_Window->getHeight());
    m_ContainerGUI = std::make_unique<ContainerGUI>(m_Window->getWidth(), m_Window->getHeight());
    m_MenuGUI = std::make_unique<MenuGUI>(m_Window->getWidth(), m_Window->getHeight());
    m_Skybox = std::make_unique<Skybox>();
    m_Inventory = std::make_unique<Inventory>();
    m_PlayerStats = std::make_unique<PlayerStats>();

    m_MobEngine = std::make_unique<MobEngine>();
    m_ItemEntityManager = std::make_unique<ItemEntityManager>();
    m_ChestManager = std::make_unique<ChestManager>();
    m_FurnaceManager = std::make_unique<FurnaceManager>();
    m_ParticleEngine = std::make_unique<ParticleEngine>();
    m_FrustumCuller = std::make_unique<FrustumCuller>();
    m_NetworkManager = std::make_unique<NetworkManager>();

    // Spawn initial interactive test mobs & structures
    m_MobEngine->spawnMob(MobType::Zombie, glm::vec3(5.0f, 65.0f, 5.0f));
    m_MobEngine->spawnMob(MobType::Skeleton, glm::vec3(12.0f, 65.0f, -8.0f));
    m_MobEngine->spawnMob(MobType::Creeper, glm::vec3(-10.0f, 65.0f, 10.0f));
    m_MobEngine->spawnMob(MobType::Pig, glm::vec3(-5.0f, 65.0f, 3.0f));

    World* world = m_DimensionManager->getCurrentWorld();
    if (world) {
        StructureGenerator::generateTree(*world, 8, 65, 8);
        StructureGenerator::generateNetherPortalFrame(*world, -3, 65, -3);
    }
}

Application::~Application() = default;

void Application::run() {
    float lastTime = static_cast<float>(glfwGetTime());

    while (!m_Window->shouldClose() && m_IsRunning) {
        float currentTime = static_cast<float>(glfwGetTime());
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // FPS Calculation
        m_FrameCounter++;
        m_FpsTimer += deltaTime;
        if (m_FpsTimer >= 1.0f) {
            m_FPS = m_FrameCounter / m_FpsTimer;
            m_FrameCounter = 0;
            m_FpsTimer = 0.0f;
        }

        m_Window->pollEvents();
        m_HUD->resize(m_Window->getWidth(), m_Window->getHeight());
        m_InventoryGUI->resize(m_Window->getWidth(), m_Window->getHeight());
        if (m_PostProcessing) {
            m_PostProcessing->resize(m_Window->getWidth(), m_Window->getHeight());
        }

        processInput(deltaTime);
        update(deltaTime);
        render();
        m_Window->swapBuffers();
    }
}

void Application::processInput(float deltaTime) {
    bool escPressedNow = Input::isKeyPressed(GLFW_KEY_ESCAPE);
    if (escPressedNow && !m_EscPressedLast) {
        if (m_State == GameState::Playing) {
            if (m_IsInventoryOpen) {
                m_IsInventoryOpen = false;
                m_Window->setCursorCaptured(true);
            } else {
                m_State = GameState::Paused;
                m_Window->setCursorCaptured(false);
            }
        } else if (m_State == GameState::Paused) {
            m_State = GameState::Playing;
            m_Window->setCursorCaptured(true);
        } else if (m_State == GameState::SettingsMenu) {
            m_State = GameState::MainMenu;
        }
    }
    m_EscPressedLast = escPressedNow;

    if (m_State != GameState::Playing) {
        return;
    }

    // Toggle Inventory with 'E' key
    bool ePressedNow = Input::isKeyPressed(GLFW_KEY_E);
    if (ePressedNow && !m_EPressedLast) {
        m_IsInventoryOpen = !m_IsInventoryOpen;
        m_Window->setCursorCaptured(!m_IsInventoryOpen);
        std::cout << "[GUI] Inventory: " << (m_IsInventoryOpen ? "OPENED" : "CLOSED") << std::endl;
    }
    m_EPressedLast = ePressedNow;

    // Toggle Flying mode with 'F' key
    bool fPressedNow = Input::isKeyPressed(GLFW_KEY_F);
    if (fPressedNow && !m_FPressedLast) {
        m_IsFlying = !m_IsFlying;
        std::cout << "[Player] Flying Mode: " << (m_IsFlying ? "ENABLED" : "DISABLED") << std::endl;
    }
    m_FPressedLast = fPressedNow;

    // Toggle F3 Debug Info
    bool f3PressedNow = Input::isKeyPressed(GLFW_KEY_F3);
    if (f3PressedNow && !m_F3PressedLast) {
        m_ShowDebugInfo = !m_ShowDebugInfo;
        std::cout << "[Debug HUD] F3 Debug Screen: " << (m_ShowDebugInfo ? "SHOW" : "HIDE") << std::endl;
    }
    m_F3PressedLast = f3PressedNow;

    // F4 Key: Cycle Time of Day
    bool f4PressedNow = Input::isKeyPressed(GLFW_KEY_F4);
    if (f4PressedNow && !m_F4PressedLast) {
        float currentTicks = m_TimeManager->getTimeTicks();
        if (currentTicks < 6000.0f) m_TimeManager->setTimeOfDay(6000.0f);
        else if (currentTicks < 12000.0f) m_TimeManager->setTimeOfDay(12000.0f);
        else if (currentTicks < 18000.0f) m_TimeManager->setTimeOfDay(18000.0f);
        else m_TimeManager->setTimeOfDay(0.0f);
    }
    m_F4PressedLast = f4PressedNow;

    // 'T' Key: Fast forward time
    if (Input::isKeyPressed(GLFW_KEY_T)) {
        m_TimeManager->advanceTime(100.0f);
    }

    // Hotbar Block selection (1-9)
    if (Input::isKeyPressed(GLFW_KEY_1)) { m_SelectedSlot = 0; m_SelectedBlock = m_Inventory->getSlot(0).type; }
    if (Input::isKeyPressed(GLFW_KEY_2)) { m_SelectedSlot = 1; m_SelectedBlock = m_Inventory->getSlot(1).type; }
    if (Input::isKeyPressed(GLFW_KEY_3)) { m_SelectedSlot = 2; m_SelectedBlock = m_Inventory->getSlot(2).type; }
    if (Input::isKeyPressed(GLFW_KEY_4)) { m_SelectedSlot = 3; m_SelectedBlock = BlockType::RedstoneWire; }
    if (Input::isKeyPressed(GLFW_KEY_5)) { m_SelectedSlot = 4; m_SelectedBlock = BlockType::RedstoneTorch; }
    if (Input::isKeyPressed(GLFW_KEY_6)) { m_SelectedSlot = 5; m_SelectedBlock = BlockType::Lever; }
    if (Input::isKeyPressed(GLFW_KEY_7)) { m_SelectedSlot = 6; m_SelectedBlock = BlockType::RedstoneLamp; }
    if (Input::isKeyPressed(GLFW_KEY_8)) { m_SelectedSlot = 7; m_SelectedBlock = BlockType::Water; }
    if (Input::isKeyPressed(GLFW_KEY_9)) { m_SelectedSlot = 8; m_SelectedBlock = BlockType::Chest; }

    if (m_IsInventoryOpen) {
        bool leftMouseNow = Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
        if (leftMouseNow && !m_LeftMousePressedLast) {
            m_InventoryGUI->handleMouseClick(*m_Inventory, Input::getMouseX(), Input::getMouseY(), GLFW_MOUSE_BUTTON_LEFT);
        }
        m_LeftMousePressedLast = leftMouseNow;
        return;
    }

    // Movement Controls
    glm::vec3 front = m_Camera->getFront();
    glm::vec3 right = glm::normalize(glm::cross(front, glm::vec3(0, 1, 0)));

    bool isSneaking = !m_IsFlying && Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT);
    bool isSprinting = Input::isKeyPressed(GLFW_KEY_LEFT_CONTROL);

    if (!m_IsFlying && !m_InWater) {
        front.y = 0.0f;
        front = glm::normalize(front);
    }

    float speed = m_IsFlying ? 15.0f : (m_InWater ? 4.0f : (isSprinting ? 9.0f : 6.0f));

    if (Input::isKeyPressed(GLFW_KEY_W)) m_PlayerVelocity += front * speed;
    if (Input::isKeyPressed(GLFW_KEY_S)) m_PlayerVelocity -= front * speed;
    if (Input::isKeyPressed(GLFW_KEY_A)) m_PlayerVelocity -= right * speed;
    if (Input::isKeyPressed(GLFW_KEY_D)) m_PlayerVelocity += right * speed;

    if (m_IsFlying) {
        if (Input::isKeyPressed(GLFW_KEY_SPACE)) m_PlayerVelocity.y += speed;
        if (Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) m_PlayerVelocity.y -= speed;
    } else if (m_InWater) {
        if (Input::isKeyPressed(GLFW_KEY_SPACE)) {
            m_PlayerVelocity.y += 6.0f * deltaTime;
        }
    } else {
        if (Input::isKeyPressed(GLFW_KEY_SPACE) && m_IsGrounded) {
            m_PlayerVelocity.y = 8.5f;
            m_IsGrounded = false;
            if (m_PlayerStats) m_PlayerStats->addExhaustion(0.2f);
            AudioManager::playSound(SoundEffect::Jump);
        }
    }

    // Add movement exhaustion & footstep sounds
    if (!m_IsFlying && m_PlayerStats) {
        float horizSpeed = glm::length(glm::vec2(m_PlayerVelocity.x, m_PlayerVelocity.z));
        m_PlayerStats->addExhaustion(horizSpeed * deltaTime * (isSprinting ? 0.05f : 0.01f));

        if (m_IsGrounded && horizSpeed > 0.8f) {
            m_StepTimer += deltaTime;
            float stepInterval = isSprinting ? 0.28f : 0.42f;
            if (m_StepTimer >= stepInterval) {
                m_StepTimer = 0.0f;
                AudioManager::playSound(SoundEffect::Footstep);
            }
        } else {
            m_StepTimer = 0.0f;
        }
    }

    // Mouse camera look
    if (m_Window->isCursorCaptured()) {
        float dx = static_cast<float>(Input::getMouseDX());
        float dy = static_cast<float>(Input::getMouseDY());
        m_Camera->processMouseMovement(dx, dy);
    }
    Input::updateMouseDelta();

    // Raycast Block & Mob Interaktionen
    World* world = m_DimensionManager->getCurrentWorld();
    bool leftMouseNow = Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);
    bool rightMouseNow = Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT);

    if (world && ((leftMouseNow && !m_LeftMousePressedLast) || (rightMouseNow && !m_RightMousePressedLast))) {
        // Check for right-click food consumption
        if (rightMouseNow && !m_RightMousePressedLast && FoodSystem::isFood(m_SelectedBlock)) {
            if (m_PlayerStats && FoodSystem::eatFood(*m_PlayerStats, m_SelectedBlock)) {
                auto& slot = m_Inventory->getSlot(m_SelectedSlot);
                if (!slot.isEmpty()) {
                    slot.count--;
                    if (slot.count == 0) slot.clear();
                }
                m_SelectedBlock = m_Inventory->getSlot(m_SelectedSlot).type;
            }
        } else {
            int damage = ToolSystem::getDamageDealt(m_SelectedBlock);
            bool mobHit = m_MobEngine->checkPlayerAttack(m_Camera->getPosition(), m_Camera->getFront(), 4.5f, damage, m_ItemEntityManager.get());

            if (!mobHit) {
                RaycastResult hit = Raycast::raycast(*world, m_Camera->getPosition(), m_Camera->getFront(), 6.0f);
                if (hit.hit) {
                    BlockType hitType = world->getBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z);
                    if (rightMouseNow && !m_RightMousePressedLast && hitType == BlockType::Lever) {
                        RedstoneEngine::updateRedstoneNetwork(*world, hit.blockPos);
                        AudioManager::playSound3D(SoundEffect::BlockPlace, glm::vec3(hit.blockPos), m_Camera->getPosition(), m_Camera->getFront());
                    } else if (rightMouseNow && !m_RightMousePressedLast && hitType == BlockType::TNT) {
                        ExplosionEngine::createExplosion(*world, glm::vec3(hit.blockPos) + glm::vec3(0.5f), 4.0f, &m_PlayerVelocity, &m_Camera->getPosition());
                    } else if (leftMouseNow && !m_LeftMousePressedLast) {
                        if (ToolSystem::canHarvest(hitType, m_SelectedBlock)) {
                            m_ItemEntityManager->spawnItemDrop(hitType, 1, glm::vec3(hit.blockPos));
                        }
                        if (m_PlayerStats) m_PlayerStats->addExhaustion(0.05f);
                        m_ParticleEngine->spawnBlockBreak(glm::vec3(hit.blockPos));
                        world->setBlock(hit.blockPos.x, hit.blockPos.y, hit.blockPos.z, BlockType::Air);
                        AudioManager::playSound3D(SoundEffect::BlockBreak, glm::vec3(hit.blockPos), m_Camera->getPosition(), m_Camera->getFront());
                    } else if (rightMouseNow && !m_RightMousePressedLast) {
                        BlockType toPlace = m_SelectedBlock;
                        if (toPlace != BlockType::Air) {
                            world->setBlock(hit.previousPos.x, hit.previousPos.y, hit.previousPos.z, toPlace);
                            RedstoneEngine::updateRedstoneNetwork(*world, hit.previousPos);
                            AudioManager::playSound3D(SoundEffect::BlockPlace, glm::vec3(hit.previousPos), m_Camera->getPosition(), m_Camera->getFront());
                        }
                    }
                }
            }
        }
    }

    m_LeftMousePressedLast = leftMouseNow;
    m_RightMousePressedLast = rightMouseNow;
}

void Application::update(float deltaTime) {
    if (m_TimeManager) {
        m_TimeManager->update(deltaTime);
    }

    if (m_FurnaceManager) {
        m_FurnaceManager->update(deltaTime);
    }

    if (m_PlayerStats) {
        m_PlayerStats->update(deltaTime);
    }

    World* world = m_DimensionManager->getCurrentWorld();

    if (world && m_Camera) {
        glm::vec3 currentPos = m_Camera->getPosition();
        bool isSneaking = !m_IsFlying && Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT);
        PhysicsEngine::updatePlayer(*world, currentPos, m_PlayerVelocity, m_IsGrounded, m_InWater, m_IsFlying, isSneaking, deltaTime);
        
        // Fluid simulation step
        FluidEngine::updateFluids(*world, currentPos);

        // Weather Manager update
        if (m_WeatherManager) {
            m_WeatherManager->update(currentPos, deltaTime);
        }

        // Mob Engine update & AI
        if (m_MobEngine) {
            float playerHp = m_PlayerStats ? m_PlayerStats->getHealth() : 20.0f;
            m_MobEngine->update(*world, currentPos, m_PlayerVelocity, playerHp, deltaTime, m_ItemEntityManager.get());
            if (m_PlayerStats) m_PlayerStats->setHealth(playerHp);
        }

        // Dropped Items update
        if (m_ItemEntityManager) {
            std::vector<std::pair<BlockType, int>> pickedUp;
            m_ItemEntityManager->update(*world, currentPos, pickedUp, deltaTime);
            for (auto& [type, count] : pickedUp) {
                m_Inventory->addItem(type, count);
            }
        }

        // Nether portal teleport check
        glm::vec3 telePos;
        if (m_DimensionManager->checkPortalTeleport(currentPos, telePos)) {
            currentPos = telePos;
        }

        // Particle Engine update
        if (m_ParticleEngine) {
            m_ParticleEngine->update(deltaTime);
        }

        Camera tempCam(currentPos, glm::vec3(0, 1, 0));
        *m_Camera = tempCam;

        m_DimensionManager->update(currentPos, deltaTime);
    }
}

void Application::render() {
    World* world = m_DimensionManager->getCurrentWorld();
    glm::vec3 skyColor = m_TimeManager->getSkyColor();

    if (m_DimensionManager->getCurrentDimension() == DimensionType::Nether) {
        skyColor = glm::vec3(0.18f, 0.04f, 0.04f);
    }

    // 1. Shadow Map Pass
    if (m_ShadowMap && m_ShadowShader && world) {
        m_ShadowMap->bindForWriting();
        m_ShadowShader->use();
        glm::mat4 lightSpaceMatrix = m_ShadowMap->getLightSpaceMatrix(m_TimeManager->getSunDirection(), m_Camera->getPosition());
        m_ShadowShader->setMat4("u_LightSpaceMatrix", lightSpaceMatrix);
        m_ShadowShader->setMat4("u_Model", glm::mat4(1.0f));
        world->render();
        m_ShadowMap->unbind(m_Window->getWidth(), m_Window->getHeight());
    }

    // 2. Offscreen Framebuffer Pass (Post Processing)
    if (m_PostProcessing) {
        m_PostProcessing->bindForWriting();
    }

    glClearColor(skyColor.r, skyColor.g, skyColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 3. Render 3D World
    if (m_BlockShader && m_Camera && world && m_TimeManager) {
        m_BlockShader->use();
        
        glm::mat4 projection = m_Camera->getProjectionMatrix(m_Window->getAspectRatio());
        glm::mat4 view = m_Camera->getViewMatrix();
        glm::mat4 model = glm::mat4(1.0f);

        m_BlockShader->setMat4("u_Projection", projection);
        m_BlockShader->setMat4("u_View", view);
        m_BlockShader->setMat4("u_Model", model);

        m_BlockShader->setVec3("u_SunDirection", m_TimeManager->getSunDirection());
        m_BlockShader->setVec3("u_SunColor", m_TimeManager->getSunColor());
        m_BlockShader->setVec3("u_SkyColor", skyColor);
        m_BlockShader->setFloat("u_AmbientLight", m_TimeManager->getAmbientLight());
        m_BlockShader->setBool("u_IsUnderwater", m_InWater);

        bool holdingTorch = (m_SelectedBlock == BlockType::RedstoneTorch || m_SelectedBlock == BlockType::RedstoneWire);
        m_BlockShader->setVec3("u_PlayerPos", m_Camera->getPosition());
        m_BlockShader->setBool("u_HasHandheldLight", holdingTorch);

        if (m_ShadowMap && glActiveTexture) {
            glActiveTexture(GL_TEXTURE0 + 1);
            glBindTexture(GL_TEXTURE_2D, m_ShadowMap->getDepthMapTexture());
            m_BlockShader->setInt("u_ShadowMap", 1);
            glm::mat4 lightSpaceMatrix = m_ShadowMap->getLightSpaceMatrix(m_TimeManager->getSunDirection(), m_Camera->getPosition());
            m_BlockShader->setMat4("u_LightSpaceMatrix", lightSpaceMatrix);
        }

        world->render();
    }

    // Render screen quad with Post-Processing
    if (m_PostProcessing) {
        m_PostProcessing->unbindAndRender(m_InWater);
    }

    // 4. Render 2D HUD Layer
    if (m_HUD && m_Camera && !m_IsInventoryOpen && m_State == GameState::Playing) {
        float hp = m_PlayerStats ? m_PlayerStats->getHealth() : 20.0f;
        float hunger = m_PlayerStats ? m_PlayerStats->getHunger() : 20.0f;
        m_HUD->render(m_SelectedSlot, m_ShowDebugInfo, m_FPS, m_Camera->getPosition(), m_Camera->getFront(), m_IsFlying, hp, hunger);
    }

    // 5. Render 2D Inventory GUI Layer
    if (m_InventoryGUI && m_Inventory && m_State == GameState::Playing) {
        m_InventoryGUI->render(*m_Inventory, m_IsInventoryOpen);
    }

    // 6. Render Menu GUI Layer
    if (m_MenuGUI) {
        m_MenuGUI->resize(m_Window->getWidth(), m_Window->getHeight());
        double mx = Input::getMouseX();
        double my = Input::getMouseY();
        bool mouseClicked = Input::isMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT);

        if (m_State == GameState::MainMenu) {
            m_Window->setCursorCaptured(false);
            MenuAction action = m_MenuGUI->renderMainMenu(mx, my, mouseClicked, m_WorldSeed);
            if (action == MenuAction::StartNewWorld) {
                m_State = GameState::Playing;
                m_Window->setCursorCaptured(true);
            } else if (action == MenuAction::OpenSettings) {
                m_State = GameState::SettingsMenu;
            } else if (action == MenuAction::QuitGame) {
                m_IsRunning = false;
            }
        } else if (m_State == GameState::Paused) {
            MenuAction action = m_MenuGUI->renderPauseMenu(mx, my, mouseClicked);
            if (action == MenuAction::ResumeGame) {
                m_State = GameState::Playing;
                m_Window->setCursorCaptured(true);
            } else if (action == MenuAction::OpenSettings) {
                m_State = GameState::SettingsMenu;
            } else if (action == MenuAction::QuitGame) {
                m_State = GameState::MainMenu;
            }
        } else if (m_State == GameState::SettingsMenu) {
            MenuAction action = m_MenuGUI->renderSettingsMenu(mx, my, mouseClicked, m_RenderDistance, m_FOV, m_VSync);
            if (action == MenuAction::CloseSettings) {
                m_State = GameState::MainMenu;
            }
        }
    }
}

}
