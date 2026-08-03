#include "PlayerStats.hpp"
#include "../audio/AudioManager.hpp"
#include <algorithm>

namespace Minecraft {

PlayerStats::PlayerStats() {
    for (auto& slot : m_ArmorSlots) {
        slot.clear();
    }
}

void PlayerStats::update(float deltaTime) {
    // 1. Process Exhaustion
    while (m_Exhaustion >= 4.0f) {
        m_Hunger = std::max(0.0f, m_Hunger - 1.0f);
        m_Exhaustion -= 4.0f;
    }

    // 2. Passive Health Regeneration when well-fed (Hunger >= 18)
    if (m_Hunger >= 18.0f && m_Health < 20.0f) {
        m_RegenTimer += deltaTime;
        if (m_RegenTimer >= 4.0f) {
            m_RegenTimer = 0.0f;
            setHealth(m_Health + 1.0f);
            addExhaustion(1.5f);
        }
    } else {
        m_RegenTimer = 0.0f;
    }

    // 3. Starvation Damage when completely out of food (Hunger == 0)
    if (m_Hunger <= 0.0f) {
        m_StarveTimer += deltaTime;
        if (m_StarveTimer >= 4.0f) {
            m_StarveTimer = 0.0f;
            setHealth(m_Health - 1.0f);
        }
    } else {
        m_StarveTimer = 0.0f;
    }
}

int PlayerStats::getTotalArmorPoints() const {
    int points = 0;
    for (const auto& armor : m_ArmorSlots) {
        if (armor.isEmpty()) continue;
        if (armor.type == BlockType::DiamondPickaxe) points += 5; // Armor tier bonus
        else if (armor.type == BlockType::IronPickaxe) points += 3;
        else if (armor.type == BlockType::WoodPickaxe) points += 1;
        else points += 2;
    }
    return std::min(20, points);
}

float PlayerStats::applyDamageReduction(float incomingDamage) {
    int armorPoints = getTotalArmorPoints();
    float defensePercent = armorPoints * 0.04f; // 4% reduction per armor point, up to 80%
    float finalDamage = incomingDamage * (1.0f - defensePercent);
    applyArmorDurabilityDamage();
    return std::max(0.0f, finalDamage);
}

void PlayerStats::applyArmorDurabilityDamage() {
    for (auto& armor : m_ArmorSlots) {
        if (!armor.isEmpty() && armor.durability > 0) {
            armor.durability--;
            if (armor.durability == 0) {
                armor.clear();
                AudioManager::playSound(SoundEffect::BlockBreak);
            }
        }
    }
}

}
