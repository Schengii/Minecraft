#ifndef PLAYERSTATS_HPP
#define PLAYERSTATS_HPP

#include "ItemStack.hpp"
#include <array>
#include <vector>
#include <algorithm>

namespace Minecraft {

enum class StatusEffect {
    None,
    Speed,
    JumpBoost,
    NightVision,
    Regeneration,
    Poison
};

struct ActiveEffect {
    StatusEffect effect = StatusEffect::None;
    float duration = 0.0f;
    int amplifier = 1;
};

class PlayerStats {
public:
    PlayerStats();

    float getHealth() const { return m_Health; }
    void setHealth(float hp) { m_Health = std::clamp(hp, 0.0f, 20.0f); }

    float getHunger() const { return m_Hunger; }
    void setHunger(float hunger) { m_Hunger = std::clamp(hunger, 0.0f, 20.0f); }

    float getExhaustion() const { return m_Exhaustion; }
    void addExhaustion(float amount) { m_Exhaustion += amount; }

    float getOxygen() const { return m_Oxygen; }
    void setOxygen(float ox) { m_Oxygen = std::clamp(ox, 0.0f, 300.0f); }

    float getFireTicks() const { return m_FireTicks; }
    void setFireTicks(float ticks) { m_FireTicks = std::max(0.0f, ticks); }
    bool isOnFire() const { return m_FireTicks > 0.0f; }

    float getFallDistance() const { return m_FallDistance; }
    void setFallDistance(float dist) { m_FallDistance = std::max(0.0f, dist); }
    void addFallDistance(float dist) { m_FallDistance += dist; }
    void resetFallDistance() { m_FallDistance = 0.0f; }
    float applyFallDamage(float distance);

    void updateEnvironmentalEffects(bool isHeadUnderwater, bool inLava, bool inWater, float deltaTime);

    void update(float deltaTime);

    // Armor Slots: 0=Helmet, 1=Chestplate, 2=Leggings, 3=Boots
    ItemStack& getArmorSlot(int slotIndex) { return m_ArmorSlots[slotIndex]; }
    const ItemStack& getArmorSlot(int slotIndex) const { return m_ArmorSlots[slotIndex]; }

    int getTotalArmorPoints() const;
    float applyDamageReduction(float incomingDamage);
    void applyArmorDurabilityDamage();

    // Potion Status Effects
    void addEffect(StatusEffect effect, float duration, int amplifier = 1);
    bool hasEffect(StatusEffect effect) const;
    float getSpeedMultiplier() const;
    float getJumpMultiplier() const;
    bool hasNightVision() const { return hasEffect(StatusEffect::NightVision); }

    // Elytra & Gliding Mechanics
    bool isGliding() const { return m_IsGliding; }
    void setGliding(bool gliding) { m_IsGliding = gliding; }

    // Animal Mount Riding Mechanics
    bool isRiding() const { return m_IsRiding; }
    void setRiding(bool riding, int mobIdx = -1) { m_IsRiding = riding; m_RiddenMobIndex = mobIdx; }
    int getRiddenMobIndex() const { return m_RiddenMobIndex; }

    const std::vector<ActiveEffect>& getActiveEffects() const { return m_ActiveEffects; }

private:
    float m_Health = 20.0f;
    float m_Hunger = 20.0f;
    float m_Exhaustion = 0.0f;
    float m_RegenTimer = 0.0f;
    float m_StarveTimer = 0.0f;
    float m_Oxygen = 300.0f;
    float m_DrownTimer = 0.0f;
    float m_FireTicks = 0.0f;
    float m_FireDamageTimer = 0.0f;
    float m_FallDistance = 0.0f;
    bool m_IsGliding = false;
    bool m_IsRiding = false;
    int m_RiddenMobIndex = -1;
    std::array<ItemStack, 4> m_ArmorSlots;
    std::vector<ActiveEffect> m_ActiveEffects;
};

}

#endif // PLAYERSTATS_HPP
