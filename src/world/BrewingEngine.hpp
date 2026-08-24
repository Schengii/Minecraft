#ifndef BREWINGENGINE_HPP
#define BREWINGENGINE_HPP

#include "../inventory/ItemStack.hpp"
#include <string>
#include <vector>
#include <array>

namespace Minecraft {

enum class PotionType {
    WaterBottle,
    Awkward,
    Speed,
    JumpBoost,
    NightVision,
    InstantHealth,
    Regeneration,
    Poison
};

struct BrewingRecipe {
    BlockType ingredient;
    PotionType inputPotion;
    PotionType resultPotion;
};

class BrewingEngine {
public:
    static PotionType getBrewResult(BlockType ingredient, PotionType input);
    static bool isValidIngredient(BlockType ingredient);
    static float getEffectDuration(PotionType potion);
    static int getEffectAmplifier(PotionType potion);

    static std::string getPotionName(PotionType potion);

    // Stand simulation
    static bool brewBottles(BlockType ingredient, std::array<PotionType, 3>& bottles);
};

}

#endif // BREWINGENGINE_HPP
