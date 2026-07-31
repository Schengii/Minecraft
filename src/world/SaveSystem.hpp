#ifndef SAVESYSTEM_HPP
#define SAVESYSTEM_HPP

#include <string>
#include <vector>

namespace Minecraft {

class Chunk;
class World;

class SaveSystem {
public:
    static bool saveChunk(const Chunk& chunk, const std::string& saveDir = "world_saves");
    static bool loadChunk(Chunk& chunk, const std::string& saveDir = "world_saves");
    static void saveWorld(World& world, const std::string& saveDir = "world_saves");

private:
    static std::string getChunkFilePath(int chunkX, int chunkZ, const std::string& saveDir);
    static void createDirectoryIfNotExists(const std::string& dir);
};

}

#endif // SAVESYSTEM_HPP
