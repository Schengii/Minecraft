#include "BrewingEngine.hpp"

namespace Minecraft {

static const std::vector<BrewingRecipe> s_Recipes = {
    { BlockType::Netherrack, PotionType::WaterBottle, PotionType::Awkward },
    { BlockType::Sugar, PotionType::Awkward, PotionType::Speed },
    { BlockType::CarrotCrop, PotionType::Awkward, PotionType::NightVision },
    { BlockType::Apple, PotionType::Awkward, PotionType::InstantHealth },
    { BlockType::Cactus, PotionType::Awkward, PotionType::JumpBoost },
    { BlockType::SoulSand, PotionType::Awkward, PotionType::Poison }
};

PotionType BrewingEngine::getBrewResult(BlockType ingredient, PotionType input) {
    for (const auto& r : s_Recipes) {
        if (r.ingredient == ingredient && r.inputPotion == input) {
            return r.resultPotion;
        }
    }
    return input;
}

bool BrewingEngine::isValidIngredient(BlockType ingredient) {
    for (const auto& r : s_Recipes) {
        if (r.ingredient == ingredient) return true;
    }
    return false;
}

float BrewingEngine::getEffectDuration(PotionType potion) {
    switch (potion) {
        case PotionType::Speed:
        case PotionType::JumpBoost:
        case PotionType::NightVision:
            return 180.0f; // 3 minutes
        case PotionType::Regeneration:
        case PotionType::Poison:
            return 45.0f;  // 45 seconds
        default:
            return 0.0f;
    }
}

int BrewingEngine::getEffectAmplifier(PotionType potion) {
    switch (potion) {
        case PotionType::InstantHealth:
            return 6; // +6 HP instant
        default:
            return 1;
    }
}

std::string BrewingEngine::getPotionName(PotionType potion) {
    switch (potion) {
        case PotionType::WaterBottle: return "Water Bottle";
        case PotionType::Awkward: return "Awkward Potion";
        case PotionType::Speed: return "Potion of Swiftness (Speed I)";
        case PotionType::JumpBoost: return "Potion of Leaping (Jump Boost I)";
        case PotionType::NightVision: return "Potion of Night Vision";
        case PotionType::InstantHealth: return "Potion of Healing";
        case PotionType::Regeneration: return "Potion of Regeneration";
        case PotionType::Poison: return "Potion of Poison";
        default: return "Unknown Potion";
    }
}

bool BrewingEngine::brewBottles(BlockType ingredient, std::array<PotionType, 3>& bottles) {
    bool brewedAny = false;
    for (size_t i = 0; i < 3; ++i) {
        PotionType next = getBrewResult(ingredient, bottles[i]);
        if (next != bottles[i]) {
            bottles[i] = next;
            brewedAny = true;
        }
    }
    return brewedAny;
}

}
