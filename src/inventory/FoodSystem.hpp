#ifndef FOODSYSTEM_HPP
#define FOODSYSTEM_HPP

#include "../world/Block.hpp"

namespace Minecraft {

class PlayerStats;

struct FoodInfo {
    float hungerRestored = 0.0f;
    float healthRestored = 0.0f;
    float saturation = 0.0f;
};

class FoodSystem {
public:
    static bool isFood(BlockType type);
    static FoodInfo getFoodInfo(BlockType type);
    static bool eatFood(PlayerStats& stats, BlockType type);
};

}

#endif // FOODSYSTEM_HPP
