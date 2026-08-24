#ifndef TEXTUREATLAS_HPP
#define TEXTUREATLAS_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include <cstdint>
#include "../world/Block.hpp"

namespace Minecraft {

class TextureAtlas {
public:
    static TextureAtlas& getInstance();

    void init();
    void bind(unsigned int unit = 0) const;
    void unbind() const;

    GLuint getTextureID() const { return m_TextureID; }
    static glm::vec2 getTileUV(int tileX, int tileY);
    static glm::vec2 getBlockUV(BlockType type, Direction face);

    // Procedural generation helpers
    static void generateDefaultAtlas(std::vector<uint8_t>& outPixels, int atlasWidth = 256, int atlasHeight = 256);

private:
    TextureAtlas() = default;
    ~TextureAtlas();

    GLuint m_TextureID = 0;
    bool m_IsInitialized = false;

    static void fillTile(std::vector<uint8_t>& pixels, int atlasW, int tileX, int tileY, const uint8_t color[4], float noiseIntensity = 0.1f);
    static void fillTilePattern(std::vector<uint8_t>& pixels, int atlasW, int tileX, int tileY, int patternType);
};

}

#endif // TEXTUREATLAS_HPP
