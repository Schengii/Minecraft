#include "DimensionManager.hpp"
#include "StructureGenerator.hpp"
#include "../audio/AudioManager.hpp"
#include <iostream>

namespace Minecraft {

DimensionManager::DimensionManager() {
    m_Overworld = std::make_unique<World>(4);
    m_NetherWorld = std::make_unique<World>(2);
    generateNetherTerrain(*m_NetherWorld);

    m_CurrentWorld = std::move(m_Overworld);
}

DimensionManager::~DimensionManager() = default;

void DimensionManager::update(const glm::vec3& playerPos, float deltaTime) {
    if (m_CurrentWorld) {
        m_CurrentWorld->update(playerPos);
    }
}

void DimensionManager::switchDimension(DimensionType newDim) {
    if (m_CurrentDimension == newDim) return;

    if (newDim == DimensionType::Nether) {
        m_Overworld = std::move(m_CurrentWorld);
        m_CurrentWorld = std::move(m_NetherWorld);
        m_CurrentDimension = DimensionType::Nether;
        std::cout << "[DimensionManager] Teleported to THE NETHER!" << std::endl;
    } else {
        m_NetherWorld = std::move(m_CurrentWorld);
        m_CurrentWorld = std::move(m_Overworld);
        m_CurrentDimension = DimensionType::Overworld;
        std::cout << "[DimensionManager] Teleported to OVERWORLD!" << std::endl;
    }
    AudioManager::playSound(SoundEffect::Explosion);
}

bool DimensionManager::checkPortalTeleport(const glm::vec3& playerPos, glm::vec3& outTeleportPos) {
    if (!m_CurrentWorld) return false;
    int px = static_cast<int>(std::floor(playerPos.x));
    int py = static_cast<int>(std::floor(playerPos.y));
    int pz = static_cast<int>(std::floor(playerPos.z));

    if (m_CurrentWorld->getBlock(px, py, pz) == BlockType::NetherPortal) {
        if (m_CurrentDimension == DimensionType::Overworld) {
            outTeleportPos = glm::vec3(px / 8.0f, 40.0f, pz / 8.0f);
            switchDimension(DimensionType::Nether);
        } else {
            outTeleportPos = glm::vec3(px * 8.0f, 65.0f, pz * 8.0f);
            switchDimension(DimensionType::Overworld);
        }
        return true;
    }
    return false;
}

void DimensionManager::generateNetherTerrain(World& nether) {
    // Fill Nether with Netherrack, Lava, SoulSand, and Glowstone
    for (int x = -16; x < 16; ++x) {
        for (int z = -16; z < 16; ++z) {
            nether.setBlock(x, 0, z, BlockType::Bedrock);
            nether.setBlock(x, 127, z, BlockType::Bedrock);

            for (int y = 1; y < 15; ++y) {
                nether.setBlock(x, y, z, BlockType::Lava);
            }
            for (int y = 15; y < 45; ++y) {
                if ((x + z) % 3 == 0) nether.setBlock(x, y, z, BlockType::SoulSand);
                else nether.setBlock(x, y, z, BlockType::Netherrack);
            }
            for (int y = 45; y < 90; ++y) {
                nether.setBlock(x, y, z, BlockType::Air); // Nether cave space
            }
            for (int y = 90; y < 127; ++y) {
                if (y == 100 && (x * z) % 5 == 0) nether.setBlock(x, y, z, BlockType::Glowstone);
                else nether.setBlock(x, y, z, BlockType::Netherrack);
            }
        }
    }
    // Generate Nether Portal at spawn
    StructureGenerator::generateNetherPortalFrame(nether, 0, 40, 0);
}

}
