#ifndef MOBENGINE_HPP
#define MOBENGINE_HPP

#include <glm/glm.hpp>
#include <vector>

namespace Minecraft {

class World;

enum class MobType {
    Pig,
    Zombie
};

struct Mob {
    glm::vec3 position{ 0.0f };
    glm::vec3 velocity{ 0.0f };
    float health = 20.0f;
    float maxHealth = 20.0f;
    MobType type = MobType::Zombie;
    bool isGrounded = false;
};

class MobEngine {
public:
    MobEngine();

    void spawnMob(MobType type, const glm::vec3& position);
    void update(World& world, const glm::vec3& playerPos, float deltaTime);
    bool checkPlayerAttack(const glm::vec3& playerPos, const glm::vec3& playerDir, float reach, int damage);

    const std::vector<Mob>& getMobs() const { return m_Mobs; }

private:
    std::vector<Mob> m_Mobs;
};

}

#endif // MOBENGINE_HPP
