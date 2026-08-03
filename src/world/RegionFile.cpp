#include "RegionFile.hpp"
#include <filesystem>
#include <iostream>
#include <cstring>
#include <cmath>

namespace Minecraft {

namespace fs = std::filesystem;

std::string RegionFile::getRegionFilePath(int regionX, int regionZ, const std::string& directory) {
    return directory + "/r." + std::to_string(regionX) + "." + std::to_string(regionZ) + ".mca";
}

RegionFile::RegionFile(int regionX, int regionZ, const std::string& directory)
    : m_RegionX(regionX), m_RegionZ(regionZ)
{
    m_FilePath = getRegionFilePath(regionX, regionZ, directory);
    std::memset(m_Offsets, 0, sizeof(m_Offsets));
    std::memset(m_Timestamps, 0, sizeof(m_Timestamps));
    m_SectorFree.resize(2, false); // First 2 sectors reserved for header tables

    if (!fs::exists(directory)) {
        fs::create_directories(directory);
    }

    if (fs::exists(m_FilePath)) {
        loadHeader();
    } else {
        initHeader();
    }
}

RegionFile::~RegionFile() = default;

void RegionFile::initHeader() {
    std::ofstream outFile(m_FilePath, std::ios::binary);
    if (!outFile.is_open()) return;

    outFile.write(reinterpret_cast<const char*>(m_Offsets), sizeof(m_Offsets));
    outFile.write(reinterpret_cast<const char*>(m_Timestamps), sizeof(m_Timestamps));
    outFile.close();
}

void RegionFile::loadHeader() {
    std::ifstream inFile(m_FilePath, std::ios::binary);
    if (!inFile.is_open()) return;

    inFile.read(reinterpret_cast<char*>(m_Offsets), sizeof(m_Offsets));
    inFile.read(reinterpret_cast<char*>(m_Timestamps), sizeof(m_Timestamps));
    inFile.close();

    // Determine allocated sectors
    size_t fileSize = fs::file_size(m_FilePath);
    size_t totalSectors = (fileSize + SECTOR_SIZE - 1) / SECTOR_SIZE;
    if (totalSectors < 2) totalSectors = 2;
    m_SectorFree.assign(totalSectors, true);
    m_SectorFree[0] = false; // Header
    m_SectorFree[1] = false; // Header

    for (int i = 0; i < 1024; ++i) {
        uint32_t val = m_Offsets[i];
        uint32_t sectorOffset = val >> 8;
        uint32_t sectorCount = val & 0xFF;
        if (sectorOffset != 0 && sectorOffset + sectorCount <= m_SectorFree.size()) {
            for (uint32_t s = 0; s < sectorCount; ++s) {
                m_SectorFree[sectorOffset + s] = false;
            }
        }
    }
}

bool RegionFile::writeChunk(int localChunkX, int localChunkZ, const BlockType blocks[16][256][16], const uint8_t light[16][256][16]) {
    if (localChunkX < 0 || localChunkX >= 32 || localChunkZ < 0 || localChunkZ >= 32) return false;

    int chunkIndex = localChunkX + localChunkZ * 32;

    // Package payload: blocks + light
    std::vector<char> payload;
    size_t dataSize = sizeof(BlockType) * 16 * 256 * 16 + sizeof(uint8_t) * 16 * 256 * 16;
    payload.resize(dataSize);

    std::memcpy(payload.data(), blocks, sizeof(BlockType) * 16 * 256 * 16);
    std::memcpy(payload.data() + sizeof(BlockType) * 16 * 256 * 16, light, sizeof(uint8_t) * 16 * 256 * 16);

    uint32_t totalBytes = static_cast<uint32_t>(4 + payload.size());
    uint32_t sectorsNeeded = (totalBytes + SECTOR_SIZE - 1) / SECTOR_SIZE;

    // Find continuous free sectors
    uint32_t sectorOffset = 0;
    uint32_t freeRun = 0;

    for (size_t s = 2; s < m_SectorFree.size(); ++s) {
        if (m_SectorFree[s]) {
            if (freeRun == 0) sectorOffset = static_cast<uint32_t>(s);
            freeRun++;
            if (freeRun >= sectorsNeeded) break;
        } else {
            freeRun = 0;
        }
    }

    if (freeRun < sectorsNeeded) {
        sectorOffset = static_cast<uint32_t>(m_SectorFree.size());
        m_SectorFree.resize(sectorOffset + sectorsNeeded, true);
    }

    for (uint32_t s = 0; s < sectorsNeeded; ++s) {
        m_SectorFree[sectorOffset + s] = false;
    }

    m_Offsets[chunkIndex] = (sectorOffset << 8) | (sectorsNeeded & 0xFF);

    std::fstream file(m_FilePath, std::ios::in | std::ios::out | std::ios::binary);
    if (!file.is_open()) return false;

    // Update Header offset entry
    file.seekp(chunkIndex * sizeof(uint32_t));
    uint32_t offsetEntry = m_Offsets[chunkIndex];
    file.write(reinterpret_cast<const char*>(&offsetEntry), sizeof(uint32_t));

    // Write chunk payload at sector offset
    file.seekp(sectorOffset * SECTOR_SIZE);
    uint32_t len = static_cast<uint32_t>(payload.size());
    file.write(reinterpret_cast<const char*>(&len), sizeof(uint32_t));
    file.write(payload.data(), payload.size());

    file.close();
    return true;
}

bool RegionFile::readChunk(int localChunkX, int localChunkZ, BlockType blocks[16][256][16], uint8_t light[16][256][16]) {
    if (localChunkX < 0 || localChunkX >= 32 || localChunkZ < 0 || localChunkZ >= 32) return false;

    int chunkIndex = localChunkX + localChunkZ * 32;
    uint32_t val = m_Offsets[chunkIndex];
    uint32_t sectorOffset = val >> 8;

    if (sectorOffset == 0) return false;

    std::ifstream inFile(m_FilePath, std::ios::binary);
    if (!inFile.is_open()) return false;

    inFile.seekg(sectorOffset * SECTOR_SIZE);
    uint32_t length = 0;
    inFile.read(reinterpret_cast<char*>(&length), sizeof(uint32_t));

    size_t expectedSize = sizeof(BlockType) * 16 * 256 * 16 + sizeof(uint8_t) * 16 * 256 * 16;
    if (length != expectedSize) {
        inFile.close();
        return false;
    }

    inFile.read(reinterpret_cast<char*>(blocks), sizeof(BlockType) * 16 * 256 * 16);
    inFile.read(reinterpret_cast<char*>(light), sizeof(uint8_t) * 16 * 256 * 16);

    inFile.close();
    return true;
}

RegionManager& RegionManager::getInstance() {
    static RegionManager instance;
    return instance;
}

RegionFile* RegionManager::getRegion(int chunkX, int chunkZ, const std::string& directory) {
    int regionX = static_cast<int>(std::floor(static_cast<float>(chunkX) / 32.0f));
    int regionZ = static_cast<int>(std::floor(static_cast<float>(chunkZ) / 32.0f));

    glm::ivec2 key(regionX, regionZ);
    auto it = m_Regions.find(key);
    if (it != m_Regions.end()) {
        return it->second.get();
    }

    m_Regions[key] = std::make_unique<RegionFile>(regionX, regionZ, directory);
    return m_Regions[key].get();
}

bool RegionManager::saveChunk(int chunkX, int chunkZ, const BlockType blocks[16][256][16], const uint8_t light[16][256][16], const std::string& directory) {
    RegionFile* reg = getRegion(chunkX, chunkZ, directory);
    if (!reg) return false;

    int localX = (chunkX % 32 + 32) % 32;
    int localZ = (chunkZ % 32 + 32) % 32;
    return reg->writeChunk(localX, localZ, blocks, light);
}

bool RegionManager::loadChunk(int chunkX, int chunkZ, BlockType blocks[16][256][16], uint8_t light[16][256][16], const std::string& directory) {
    RegionFile* reg = getRegion(chunkX, chunkZ, directory);
    if (!reg) return false;

    int localX = (chunkX % 32 + 32) % 32;
    int localZ = (chunkZ % 32 + 32) % 32;
    return reg->readChunk(localX, localZ, blocks, light);
}

void RegionManager::clearCache() {
    m_Regions.clear();
}

}
