#ifndef PHYSICSENGINE_HPP
#define PHYSICSENGINE_HPP

#include "AABB.hpp"
#include <glm/glm.hpp>

namespace Minecraft {

class World;

class PhysicsEngine {
public:
    static void updatePlayer(World& world, glm::vec3& position, glm::vec3& velocity, bool& isGrounded, bool& inWater, bool isFlying, bool isSneaking, float deltaTime, bool isGliding = false, const glm::vec3& lookDir = glm::vec3(0.0f, 0.0f, -1.0f));
    static void updateMinecart(World& world, glm::vec3& position, glm::vec3& velocity, float deltaTime);
    static void updateBoat(World& world, glm::vec3& position, glm::vec3& velocity, float deltaTime);
    static bool isPointInWater(World& world, const glm::vec3& point);
    static bool isPointInLava(World& world, const glm::vec3& point);
    static bool isHeadUnderwater(World& world, const glm::vec3& position);

    static bool checkCollision(World& world, const AABB& playerBox);
};

}

#endif // PHYSICSENGINE_HPP
