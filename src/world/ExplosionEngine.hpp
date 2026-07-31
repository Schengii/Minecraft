#ifndef EXPLOSIONENGINE_HPP
#define EXPLOSIONENGINE_HPP

#include <glm/glm.hpp>

namespace Minecraft {

class World;

class ExplosionEngine {
public:
    static void createExplosion(World& world, const glm::vec3& center, float radius = 4.0f, glm::vec3* playerVel = nullptr, const glm::vec3* playerPos = nullptr);
};

}

#endif // EXPLOSIONENGINE_HPP
