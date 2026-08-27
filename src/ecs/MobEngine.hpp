#ifndef MOBENGINE_HPP
#define MOBENGINE_HPP

#include <glm/glm.hpp>
#include <vector>
#include <deque>

namespace Minecraft {

class World;

enum class MobType {
    Pig,
    Cow,
    Zombie,
    Skeleton,
    Creeper,
    EnderDragon,
    Villager,
    IronGolem,
    Wither
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
    float hurtTime = 0.0f;       // Visual hit feedback
    float limbSwing = 0.0f;      // Walking animation
    float yaw = 0.0f;
    float pitch = 0.0f;
    std::deque<glm::ivec3> path; // 3D A* Waypoints
};

struct ArrowEntity {
    glm::vec3 position;
    glm::vec3 velocity;
    bool active = true;
};

struct WitherSkullEntity {
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
    void checkNaturalSpawning(World& world, const glm::vec3& playerPos, float deltaTime);

    static std::deque<glm::ivec3> findPath3D(World& world, const glm::ivec3& start, const glm::ivec3& target, int maxSteps = 30);

    const std::vector<Mob>& getMobs() const { return m_Mobs; }
    const std::vector<ArrowEntity>& getArrows() const { return m_Arrows; }
    const std::vector<WitherSkullEntity>& getWitherSkulls() const { return m_WitherSkulls; }

private:
    std::vector<Mob> m_Mobs;
    std::vector<ArrowEntity> m_Arrows;
    std::vector<WitherSkullEntity> m_WitherSkulls;
    float m_SpawnTimer = 0.0f;
};

}

#endif // MOBENGINE_HPP
