#ifndef PHYSICSENGINE_HPP
#define PHYSICSENGINE_HPP

#include "AABB.hpp"
#include <glm/glm.hpp>

namespace Minecraft {

class World;

class PhysicsEngine {
public:
    static void updatePlayer(World& world, glm::vec3& position, glm::vec3& velocity, bool& isGrounded, bool& inWater, bool isFlying, float deltaTime);
    static bool isPointInWater(World& world, const glm::vec3& point);

private:
    static bool checkCollision(World& world, const AABB& playerBox);
};

}

#endif // PHYSICSENGINE_HPP
