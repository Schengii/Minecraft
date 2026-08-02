#ifndef DIMENSIONMANAGER_HPP
#define DIMENSIONMANAGER_HPP

#include <memory>
#include <glm/glm.hpp>
#include "World.hpp"

namespace Minecraft {

enum class DimensionType {
    Overworld,
    Nether
};

class DimensionManager {
public:
    DimensionManager();
    ~DimensionManager();

    void update(const glm::vec3& playerPos, float deltaTime);
    void switchDimension(DimensionType newDim);

    World* getCurrentWorld() { return m_CurrentWorld.get(); }
    DimensionType getCurrentDimension() const { return m_CurrentDimension; }
    bool checkPortalTeleport(const glm::vec3& playerPos, glm::vec3& outTeleportPos);

private:
    DimensionType m_CurrentDimension = DimensionType::Overworld;
    std::unique_ptr<World> m_Overworld;
    std::unique_ptr<World> m_NetherWorld;
    std::unique_ptr<World> m_CurrentWorld;

    void generateNetherTerrain(World& nether);
};

}

#endif // DIMENSIONMANAGER_HPP
