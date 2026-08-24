#include "LightEngine.hpp"
#include "Chunk.hpp"
#include "World.hpp"
#include <queue>
#include <algorithm>
#include <cmath>

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

    if (world) {
        propagateCrossChunk(*world, chunk);
    }
}

void LightEngine::calculateBlocklight(Chunk& chunk, World* world) {
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

    if (world) {
        propagateCrossChunk(*world, chunk);
    }
}

void LightEngine::propagateCrossChunk(World& world, Chunk& chunk) {
    int cx = chunk.getChunkX();
    int cz = chunk.getChunkZ();

    int neighborOffsets[4][2] = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };

    for (int i = 0; i < 4; ++i) {
        int nx = cx + neighborOffsets[i][0];
        int nz = cz + neighborOffsets[i][1];
        Chunk* neighbor = world.getChunk(nx, nz);
        if (!neighbor) continue;

        bool updated = false;
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            if (neighborOffsets[i][0] == 1) { // East border
                for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                    int light = chunk.getBlocklight(CHUNK_SIZE_X - 1, y, z);
                    if (light > 1 && !BlockData::isOpaque(neighbor->getBlock(0, y, z))) {
                        if (neighbor->getBlocklight(0, y, z) < light - 1) {
                            neighbor->setBlocklight(0, y, z, light - 1);
                            updated = true;
                        }
                    }
                }
            } else if (neighborOffsets[i][0] == -1) { // West border
                for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                    int light = chunk.getBlocklight(0, y, z);
                    if (light > 1 && !BlockData::isOpaque(neighbor->getBlock(CHUNK_SIZE_X - 1, y, z))) {
                        if (neighbor->getBlocklight(CHUNK_SIZE_X - 1, y, z) < light - 1) {
                            neighbor->setBlocklight(CHUNK_SIZE_X - 1, y, z, light - 1);
                            updated = true;
                        }
                    }
                }
            } else if (neighborOffsets[i][1] == 1) { // North border
                for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                    int light = chunk.getBlocklight(x, y, CHUNK_SIZE_Z - 1);
                    if (light > 1 && !BlockData::isOpaque(neighbor->getBlock(x, y, 0))) {
                        if (neighbor->getBlocklight(x, y, 0) < light - 1) {
                            neighbor->setBlocklight(x, y, 0, light - 1);
                            updated = true;
                        }
                    }
                }
            } else if (neighborOffsets[i][1] == -1) { // South border
                for (int x = 0; x < CHUNK_SIZE_X; ++x) {
                    int light = chunk.getBlocklight(x, y, 0);
                    if (light > 1 && !BlockData::isOpaque(neighbor->getBlock(x, y, CHUNK_SIZE_Z - 1))) {
                        if (neighbor->getBlocklight(x, y, CHUNK_SIZE_Z - 1) < light - 1) {
                            neighbor->setBlocklight(x, y, CHUNK_SIZE_Z - 1, light - 1);
                            updated = true;
                        }
                    }
                }
            }
        }
        if (updated) {
            neighbor->setDirty(true);
        }
    }
}

void LightEngine::addBlockLight(World& world, const glm::ivec3& pos, int intensity) {
    int chunkX = static_cast<int>(std::floor(static_cast<float>(pos.x) / CHUNK_SIZE_X));
    int chunkZ = static_cast<int>(std::floor(static_cast<float>(pos.z) / CHUNK_SIZE_Z));

    Chunk* chunk = world.getChunk(chunkX, chunkZ);
    if (!chunk) return;

    int lx = (pos.x % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X;
    int lz = (pos.z % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;

    chunk->setBlocklight(lx, pos.y, lz, intensity);

    std::queue<glm::ivec3> q;
    q.push(pos);

    int dx[6] = { 1, -1, 0, 0, 0, 0 };
    int dy[6] = { 0, 0, 1, -1, 0, 0 };
    int dz[6] = { 0, 0, 0, 0, 1, -1 };

    while (!q.empty()) {
        glm::ivec3 cur = q.front();
        q.pop();

        int curLight = 0;
        int ccx = static_cast<int>(std::floor(static_cast<float>(cur.x) / CHUNK_SIZE_X));
        int ccz = static_cast<int>(std::floor(static_cast<float>(cur.z) / CHUNK_SIZE_Z));
        Chunk* c = world.getChunk(ccx, ccz);
        if (!c) continue;

        int clx = (cur.x % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X;
        int clz = (cur.z % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;
        curLight = c->getBlocklight(clx, cur.y, clz);

        if (curLight <= 1) continue;

        for (int i = 0; i < 6; ++i) {
            glm::ivec3 next = cur + glm::ivec3(dx[i], dy[i], dz[i]);
            if (next.y < 0 || next.y >= CHUNK_SIZE_Y) continue;

            int ncx = static_cast<int>(std::floor(static_cast<float>(next.x) / CHUNK_SIZE_X));
            int ncz = static_cast<int>(std::floor(static_cast<float>(next.z) / CHUNK_SIZE_Z));
            Chunk* nc = world.getChunk(ncx, ncz);
            if (!nc) continue;

            int nlx = (next.x % CHUNK_SIZE_X + CHUNK_SIZE_X) % CHUNK_SIZE_X;
            int nlz = (next.z % CHUNK_SIZE_Z + CHUNK_SIZE_Z) % CHUNK_SIZE_Z;

            if (!BlockData::isOpaque(nc->getBlock(nlx, next.y, nlz))) {
                int nLight = nc->getBlocklight(nlx, next.y, nlz);
                if (curLight - 1 > nLight) {
                    nc->setBlocklight(nlx, next.y, nlz, curLight - 1);
                    nc->setDirty(true);
                    q.push(next);
                }
            }
        }
    }
}

void LightEngine::removeBlockLight(World& world, const glm::ivec3& pos) {
    updateBlockLight(world, pos);
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
