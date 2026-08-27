#ifndef ENTITYRENDERER_HPP
#define ENTITYRENDERER_HPP

#include <vector>
#include <memory>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "../ecs/MobEngine.hpp"
#include "../ecs/ItemEntity.hpp"
#include "../renderer/Shader.hpp"
#include "../renderer/Camera.hpp"

namespace Minecraft {

class EntityRenderer {
public:
    static EntityRenderer& getInstance();
    ~EntityRenderer();

    void init();
    void render(const Camera& camera, const MobEngine& mobEngine, 
                const ItemEntityManager& itemMgr, float totalTime);

    // Renders the held block/tool in first-person view
    void renderFirstPersonHand(const Camera& camera, BlockType heldItem, 
                               float swingProgress, float walkBobbing, float totalTime);

private:
    EntityRenderer();
    void initCubeBuffers();
    void renderBox(const glm::mat4& viewProj, const glm::vec3& position, 
                   const glm::vec3& rotation, const glm::vec3& scale, 
                   const glm::vec3& pivot, const glm::vec4& color, 
                   BlockType texType = BlockType::Air);

    void renderHumanoid(const glm::mat4& viewProj, const Mob& mob, const glm::vec4& skinColor, const glm::vec4& clothesColor);
    void renderCreeper(const glm::mat4& viewProj, const Mob& mob);
    void renderQuadruped(const glm::mat4& viewProj, const Mob& mob, const glm::vec4& bodyColor);
    void renderDragon(const glm::mat4& viewProj, const Mob& mob, float totalTime);

    GLuint m_CubeVAO = 0;
    GLuint m_CubeVBO = 0;
    std::unique_ptr<Shader> m_EntityShader;
    bool m_IsInitialized = false;
};

}

#endif // ENTITYRENDERER_HPP
