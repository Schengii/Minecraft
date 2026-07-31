#ifndef PHYSICSENGINE_HPP
#define PHYSICSENGINE_HPP

#include "AABB.hpp"
#include <glm/glm.hpp>

namespace Minecraft {

class World;

class PhysicsEngine {
public:
    static void updatePlayer(World& world, glm::vec3& position, glm::vec3& velocity, bool& isGrounded, bool isFlying, float deltaTime);

private:
    static bool checkCollision(World& world, const AABB& playerBox);
};

}

#endif // PHYSICSENGINE_HPP
