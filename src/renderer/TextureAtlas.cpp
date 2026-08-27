#include <glad/glad.h>
#include "TextureAtlas.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

namespace Minecraft {

TextureAtlas& TextureAtlas::getInstance() {
    static TextureAtlas instance;
    return instance;
}

TextureAtlas::~TextureAtlas() {
    if (m_TextureID && glDeleteTextures) {
        glDeleteTextures(1, &m_TextureID);
    }
}

glm::vec2 TextureAtlas::getTileUV(int tileX, int tileY) {
    float u = static_cast<float>(tileX) / 16.0f;
    float v = static_cast<float>(tileY) / 16.0f;
    return glm::vec2(u, v);
}

glm::vec2 TextureAtlas::getBlockUV(BlockType type, Direction face) {
    return BlockData::getTextureUV(type, face);
}

void TextureAtlas::fillTile(std::vector<uint8_t>& pixels, int atlasW, int tileX, int tileY, const uint8_t color[4], float noiseIntensity) {
    int startX = tileX * 16;
    int startY = tileY * 16;

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int px = startX + x;
            int py = startY + y;
            int index = (py * atlasW + px) * 4;

            float n = ((std::sin(x * 12.9898f + y * 78.233f) * 43758.5453f) - std::floor((std::sin(x * 12.9898f + y * 78.233f) * 43758.5453f))) * 2.0f - 1.0f;
            float factor = 1.0f + n * noiseIntensity;

            pixels[index + 0] = static_cast<uint8_t>(std::clamp(color[0] * factor, 0.0f, 255.0f));
            pixels[index + 1] = static_cast<uint8_t>(std::clamp(color[1] * factor, 0.0f, 255.0f));
            pixels[index + 2] = static_cast<uint8_t>(std::clamp(color[2] * factor, 0.0f, 255.0f));
            pixels[index + 3] = color[3];
        }
    }
}

void TextureAtlas::fillTilePattern(std::vector<uint8_t>& pixels, int atlasW, int tileX, int tileY, int patternType) {
    int startX = tileX * 16;
    int startY = tileY * 16;

    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            int px = startX + x;
            int py = startY + y;
            int index = (py * atlasW + px) * 4;

            uint8_t r = 200, g = 200, b = 200, a = 255;

            // Pattern Types
            if (patternType == 1) { // Grass Top (Green lush with noise)
                r = 85 + (x * 3) % 20; g = 150 + (y * 5) % 35; b = 45;
            } else if (patternType == 2) { // Grass Side (Green top border, Dirt bottom)
                if (y < 4) {
                    r = 85 + (x * 3) % 20; g = 150 + (y * 5) % 35; b = 45;
                } else {
                    r = 130 + (x * 7) % 25; g = 85 + (y * 3) % 15; b = 50;
                }
            } else if (patternType == 3) { // Dirt (Rich brown)
                r = 130 + (x * 7) % 25; g = 85 + (y * 3) % 15; b = 50;
            } else if (patternType == 4) { // Stone (Granular grey)
                int val = 125 + ((x * 13 + y * 7) % 30);
                r = g = b = val;
            } else if (patternType == 5) { // Oak Planks (Wood grain strips)
                int strip = (y / 4);
                int val = 170 + (strip * 15) + (x % 10);
                r = val; g = val - 40; b = val - 80;
            } else if (patternType == 6) { // Diamond Ore (Stone with cyan flecks)
                int val = 125 + ((x * 13 + y * 7) % 30);
                if ((x % 5 == 2 && y % 4 == 1) || (x % 6 == 4 && y % 5 == 3)) {
                    r = 40; g = 230; b = 230;
                } else {
                    r = g = b = val;
                }
            } else if (patternType == 7) { // Water (Deep translucent blue)
                r = 30; g = 90 + (x % 20); b = 220; a = 180;
            } else if (patternType == 8) { // Lava (Warm incandescent orange/red)
                r = 240; g = 110 + (y * 8) % 50; b = 20;
            } else if (patternType == 9) { // Glass (Clear with white border reflection)
                if (x == 0 || x == 15 || y == 0 || y == 15 || (x == y && x < 6)) {
                    r = 230; g = 245; b = 255; a = 200;
                } else {
                    r = 200; g = 230; b = 255; a = 40;
                }
            } else if (patternType == 10) { // Glowstone (Luminescent yellow/gold)
                r = 255; g = 210 + (x * 5) % 40; b = 100;
            } else if (patternType == 11) { // Netherrack (Deep bloodstone crimson)
                r = 110 + (x * 9) % 35; g = 25; b = 25;
            }

            pixels[index + 0] = r;
            pixels[index + 1] = g;
            pixels[index + 2] = b;
            pixels[index + 3] = a;
        }
    }
}

void TextureAtlas::generateDefaultAtlas(std::vector<uint8_t>& outPixels, int atlasWidth, int atlasHeight) {
    outPixels.assign(atlasWidth * atlasHeight * 4, 255);

    // Tile 0: Grass Top
    fillTilePattern(outPixels, atlasWidth, 0, 0, 1);
    // Tile 1: Stone
    fillTilePattern(outPixels, atlasWidth, 1, 0, 4);
    // Tile 2: Dirt
    fillTilePattern(outPixels, atlasWidth, 2, 0, 3);
    // Tile 3: Grass Side
    fillTilePattern(outPixels, atlasWidth, 3, 0, 2);
    // Tile 4: Oak Planks
    fillTilePattern(outPixels, atlasWidth, 4, 0, 5);
    // Tile 5: Diamond Ore
    fillTilePattern(outPixels, atlasWidth, 2, 3, 6);
    // Tile 6: Water
    fillTilePattern(outPixels, atlasWidth, 13, 12, 7);
    // Tile 7: Lava
    fillTilePattern(outPixels, atlasWidth, 13, 14, 8);
    // Tile 8: Glass
    fillTilePattern(outPixels, atlasWidth, 1, 3, 9);
    // Tile 9: Glowstone
    fillTilePattern(outPixels, atlasWidth, 9, 6, 10);
    // Tile 10: Netherrack
    fillTilePattern(outPixels, atlasWidth, 7, 6, 11);
}

void TextureAtlas::init() {
    if (m_IsInitialized || !glGenTextures) return;

    std::vector<uint8_t> pixels;
    generateDefaultAtlas(pixels, 256, 256);

    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 256, 256, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
    m_IsInitialized = true;
    std::cout << "[TextureAtlas] 256x256 Pixel-Art Texture Atlas generated & uploaded." << std::endl;
}

void TextureAtlas::bind(unsigned int unit) const {
    if (!m_TextureID || !glBindTexture) return;
    if (glActiveTexture) {
        glActiveTexture(GL_TEXTURE0 + unit);
    }
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void TextureAtlas::unbind() const {
    if (glBindTexture) {
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

}
