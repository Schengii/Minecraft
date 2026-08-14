#ifndef MOBENGINE_HPP
#define MOBENGINE_HPP

#include <glm/glm.hpp>
#include <vector>

namespace Minecraft {

class World;

enum class MobType {
    Pig,
    Cow,
    Zombie,
    Skeleton,
    Creeper,
    EnderDragon
};

struct Mob {
    glm::vec3 position{ 0.0f };
    glm::vec3 velocity{ 0.0f };
    float health = 20.0f;
    float maxHealth = 20.0f;
    MobType type = MobType::Zombie;
    bool isGrounded = false;
    float fuseTimer = 0.0f;
    float attackCooldown = 0.0f;
};

struct ArrowEntity {
    glm::vec3 position;
    glm::vec3 velocity;
    bool active = true;
};

class MobEngine {
public:
    MobEngine();

    void spawnMob(MobType type, const glm::vec3& position);
    void update(World& world, glm::vec3& playerPos, glm::vec3& playerVel, float& playerHealth, float deltaTime, class ItemEntityManager* itemMgr = nullptr);
    bool checkPlayerAttack(const glm::vec3& playerPos, const glm::vec3& playerDir, float reach, int damage, class ItemEntityManager* itemMgr = nullptr);

    const std::vector<Mob>& getMobs() const { return m_Mobs; }
    const std::vector<ArrowEntity>& getArrows() const { return m_Arrows; }

private:
    std::vector<Mob> m_Mobs;
    std::vector<ArrowEntity> m_Arrows;
};

}

#endif // MOBENGINE_HPP
