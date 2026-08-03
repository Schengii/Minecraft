#include "FoodSystem.hpp"
#include "PlayerStats.hpp"
#include "../audio/AudioManager.hpp"

namespace Minecraft {

bool FoodSystem::isFood(BlockType type) {
    switch (type) {
        case BlockType::Apple:
        case BlockType::Bread:
        case BlockType::RawPorkchop:
        case BlockType::CookedPorkchop:
        case BlockType::GoldenApple:
            return true;
        default:
            return false;
    }
}

FoodInfo FoodSystem::getFoodInfo(BlockType type) {
    switch (type) {
        case BlockType::Apple:
            return { 4.0f, 0.0f, 2.4f };
        case BlockType::Bread:
            return { 5.0f, 0.0f, 6.0f };
        case BlockType::RawPorkchop:
            return { 3.0f, 0.0f, 1.8f };
        case BlockType::CookedPorkchop:
            return { 8.0f, 0.0f, 12.8f };
        case BlockType::GoldenApple:
            return { 10.0f, 4.0f, 9.6f };
        default:
            return { 0.0f, 0.0f, 0.0f };
    }
}

bool FoodSystem::eatFood(PlayerStats& stats, BlockType type) {
    if (!isFood(type)) return false;

    // Check if player can eat (hunger < 20 or GoldenApple)
    if (stats.getHunger() >= 20.0f && type != BlockType::GoldenApple) {
        return false;
    }

    FoodInfo info = getFoodInfo(type);
    stats.setHunger(stats.getHunger() + info.hungerRestored);
    if (info.healthRestored > 0.0f) {
        stats.setHealth(stats.getHealth() + info.healthRestored);
    }

    AudioManager::playSound(SoundEffect::BlockBreak);
    return true;
}

}
