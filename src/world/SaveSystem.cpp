#include "SaveSystem.hpp"
#include "Chunk.hpp"
#include "World.hpp"
#include "RegionFile.hpp"
#include <fstream>
#include <filesystem>
#include <iostream>

namespace Minecraft {

namespace fs = std::filesystem;

std::string SaveSystem::getChunkFilePath(int chunkX, int chunkZ, const std::string& saveDir) {
    return saveDir + "/chunk_" + std::to_string(chunkX) + "_" + std::to_string(chunkZ) + ".bin";
}

void SaveSystem::createDirectoryIfNotExists(const std::string& dir) {
    if (!fs::exists(dir)) {
        fs::create_directories(dir);
    }
}

bool SaveSystem::saveChunk(const Chunk& chunk, const std::string& saveDir) {
    createDirectoryIfNotExists(saveDir);
    return RegionManager::getInstance().saveChunk(chunk.getChunkX(), chunk.getChunkZ(), chunk.getBlocks(), chunk.getLight(), saveDir);
}

bool SaveSystem::loadChunk(Chunk& chunk, const std::string& saveDir) {
    // 1. Try loading from Anvil .mca Region Files
    BlockType tempBlocks[16][256][16];
    uint8_t tempLight[16][256][16];
    if (RegionManager::getInstance().loadChunk(chunk.getChunkX(), chunk.getChunkZ(), tempBlocks, tempLight, saveDir)) {
        for (int x = 0; x < 16; ++x) {
            for (int y = 0; y < 256; ++y) {
                for (int z = 0; z < 16; ++z) {
                    chunk.setBlock(x, y, z, tempBlocks[x][y][z]);
                    chunk.setRawLight(x, y, z, tempLight[x][y][z]);
                }
            }
        }
        chunk.setDirty(true);
        return true;
    }

    // 2. Legacy fallback: Single .bin file per chunk
    std::string filePath = getChunkFilePath(chunk.getChunkX(), chunk.getChunkZ(), saveDir);

    if (!fs::exists(filePath)) return false;

    std::ifstream inFile(filePath, std::ios::binary);
    if (!inFile.is_open()) return false;

    for (int x = 0; x < CHUNK_SIZE_X; ++x) {
        for (int y = 0; y < CHUNK_SIZE_Y; ++y) {
            for (int z = 0; z < CHUNK_SIZE_Z; ++z) {
                BlockType type;
                inFile.read(reinterpret_cast<char*>(&type), sizeof(BlockType));
                chunk.setBlock(x, y, z, type);
            }
        }
    }

    inFile.close();
    chunk.setDirty(true);
    return true;
}

void SaveSystem::saveWorld(World& world, const std::string& saveDir) {
    (void)world;
    (void)saveDir;
    std::cout << "[SaveSystem] World save complete." << std::endl;
}

}
