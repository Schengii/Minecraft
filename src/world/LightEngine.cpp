#include "LightEngine.hpp"
#include "Chunk.hpp"
#include "World.hpp"
#include <queue>
#include <algorithm>

namespace Minecraft {

int LightEngine::getLightEmission(BlockType type) {
    switch (type) {
        case BlockType::Lava:
        case BlockType::Glowstone:
        case BlockType::RedstoneLamp:
            return 15;
        case BlockType::RedstoneTorch:
            return 14;
        case BlockType::NetherPortal:
            return 11;
        case BlockType::TNT:
            return 7;
        default:
            return 0;
    }
}

void LightEngine::calculateSunlight(Chunk& chunk, World* world) {
    (void)world;
    std::queue<glm::ivec3> sunQueue;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
            int currentSun = 15;
            for (int y = CHUNK_SIZE_Y - 1; y >= 0; --y) {
                BlockType type = chunk.getBlock(x, y, z);
                if (BlockData::isOpaque(type)) {
                    currentSun = 0;
                    chunk.setSunlight(x, y, z, 0);
                } else {
                    chunk.setSunlight(x, y, z, currentSun);
                    if (currentSun > 0) {
                        sunQueue.push(glm::ivec3(x, y, z));
                    }
                }
            }
        }
    }

    // BFS Horizontal Sunlight Spread
    int dx[6] = { 1, -1, 0, 0, 0, 0 };
    int dy[6] = { 0, 0, 1, -1, 0, 0 };
    int dz[6] = { 0, 0, 0, 0, 1, -1 };

    while (!sunQueue.empty()) {
        glm::ivec3 pos = sunQueue.front();
        sunQueue.pop();

        int currentLight = chunk.getSunlight(pos.x, pos.y, pos.z);
        if (currentLight <= 1) continue;

        for (int i = 0; i < 6; ++i) {
            int nx = pos.x + dx[i];
            int ny = pos.y + dy[i];
            int nz = pos.z + dz[i];

            if (nx >= 0 && nx < CHUNK_SIZE_X && ny >= 0 && ny < CHUNK_SIZE_Y && nz >= 0 && nz < CHUNK_SIZE_Z) {
                BlockType neighborBlock = chunk.getBlock(nx, ny, nz);
                if (!BlockData::isOpaque(neighborBlock)) {
                    int neighborLight = chunk.getSunlight(nx, ny, nz);
                    if (currentLight - 1 > neighborLight) {
                        chunk.setSunlight(nx, ny, nz, currentLight - 1);
                        sunQueue.push(glm::ivec3(nx, ny, nz));
                    }
                }
            }
        }
    }
}

void LightEngine::calculateBlocklight(Chunk& chunk, World* world) {
    (void)world;
    std::queue<glm::ivec3> blockQueue;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                BlockType type = chunk.getBlock(x, y, z);
                int emission = getLightEmission(type);
                if (emission > 0) {
                    chunk.setBlocklight(x, y, z, emission);
                    blockQueue.push(glm::ivec3(x, y, z));
                } else {
                    chunk.setBlocklight(x, y, z, 0);
                }
            }
        }
    }

    int dx[6] = { 1, -1, 0, 0, 0, 0 };
    int dy[6] = { 0, 0, 1, -1, 0, 0 };
    int dz[6] = { 0, 0, 0, 0, 1, -1 };

    while (!blockQueue.empty()) {
        glm::ivec3 pos = blockQueue.front();
        blockQueue.pop();

        int currentLight = chunk.getBlocklight(pos.x, pos.y, pos.z);
        if (currentLight <= 1) continue;

        for (int i = 0; i < 6; ++i) {
            int nx = pos.x + dx[i];
            int ny = pos.y + dy[i];
            int nz = pos.z + dz[i];

            if (nx >= 0 && nx < CHUNK_SIZE_X && ny >= 0 && ny < CHUNK_SIZE_Y && nz >= 0 && nz < CHUNK_SIZE_Z) {
                BlockType neighborBlock = chunk.getBlock(nx, ny, nz);
                if (!BlockData::isOpaque(neighborBlock)) {
                    int neighborLight = chunk.getBlocklight(nx, ny, nz);
                    if (currentLight - 1 > neighborLight) {
                        chunk.setBlocklight(nx, ny, nz, currentLight - 1);
                        blockQueue.push(glm::ivec3(nx, ny, nz));
                    }
                }
            }
        }
    }
}

void LightEngine::calculateChunkLighting(Chunk& chunk, World* world) {
    calculateSunlight(chunk, world);
    calculateBlocklight(chunk, world);
}

void LightEngine::updateBlockLight(World& world, const glm::ivec3& pos) {
    int chunkX = static_cast<int>(std::floor(static_cast<float>(pos.x) / CHUNK_SIZE_X));
    int chunkZ = static_cast<int>(std::floor(static_cast<float>(pos.z) / CHUNK_SIZE_Z));

    Chunk* chunk = world.getChunk(chunkX, chunkZ);
    if (chunk) {
        calculateChunkLighting(*chunk, &world);
        chunk->setDirty(true);
    }
}

}
