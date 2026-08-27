#include "PlayerStats.hpp"

namespace Minecraft {

PlayerStats::PlayerStats() {
    for (auto& slot : m_ArmorSlots) {
        slot = { BlockType::Air, 0, 64 };
    }
}

int PlayerStats::getTotalArmorPoints() const {
    int points = 0;
    for (const auto& slot : m_ArmorSlots) {
        if (slot.type != BlockType::Air) {
            points += 4;
        }
    }
    return points;
}

float PlayerStats::applyDamageReduction(float incomingDamage) {
    int points = getTotalArmorPoints();
    float reduction = static_cast<float>(points) * 0.04f;
    reduction = std::clamp(reduction, 0.0f, 0.80f);
    return incomingDamage * (1.0f - reduction);
}

void PlayerStats::applyArmorDurabilityDamage() {
    for (auto& slot : m_ArmorSlots) {
        if (slot.type != BlockType::Air && slot.durability > 0) {
            slot.durability--;
            if (slot.durability <= 0) {
                slot = { BlockType::Air, 0, 64 };
            }
        }
    }
}

void PlayerStats::addEffect(StatusEffect effect, float duration, int amplifier) {
    for (auto& active : m_ActiveEffects) {
        if (active.effect == effect) {
            active.duration = std::max(active.duration, duration);
            active.amplifier = std::max(active.amplifier, amplifier);
            return;
        }
    }
    m_ActiveEffects.push_back({ effect, duration, amplifier });
}

bool PlayerStats::hasEffect(StatusEffect effect) const {
    for (const auto& active : m_ActiveEffects) {
        if (active.effect == effect && active.duration > 0.0f) return true;
    }
    return false;
}

float PlayerStats::getSpeedMultiplier() const {
    float mult = 1.0f;
    for (const auto& active : m_ActiveEffects) {
        if (active.effect == StatusEffect::Speed && active.duration > 0.0f) {
            mult += 0.20f * active.amplifier;
        }
    }
    return mult;
}

float PlayerStats::getJumpMultiplier() const {
    float mult = 1.0f;
    for (const auto& active : m_ActiveEffects) {
        if (active.effect == StatusEffect::JumpBoost && active.duration > 0.0f) {
            mult += 0.35f * active.amplifier;
        }
    }
    return mult;
}

void PlayerStats::update(float deltaTime) {
    // 1. Update Status Effects
    for (auto it = m_ActiveEffects.begin(); it != m_ActiveEffects.end(); ) {
        it->duration -= deltaTime;
        if (it->effect == StatusEffect::Regeneration && it->duration > 0.0f) {
            setHealth(m_Health + 1.0f * deltaTime);
        } else if (it->effect == StatusEffect::Poison && it->duration > 0.0f) {
            if (m_Health > 1.0f) {
                setHealth(m_Health - 0.5f * deltaTime);
            }
        }

        if (it->duration <= 0.0f) {
            it = m_ActiveEffects.erase(it);
        } else {
            ++it;
        }
    }

    // 2. Natural Health Regeneration when hunger >= 18
    if (m_Hunger >= 18.0f && m_Health < 20.0f) {
        m_RegenTimer += deltaTime;
        if (m_RegenTimer >= 4.0f) {
            m_Health = std::min(20.0f, m_Health + 1.0f);
            m_Exhaustion += 6.0f;
            m_RegenTimer = 0.0f;
        }
    } else {
        m_RegenTimer = 0.0f;
    }

    // 3. Starvation Damage when hunger == 0
    if (m_Hunger <= 0.0f) {
        m_StarveTimer += deltaTime;
        if (m_StarveTimer >= 4.0f) {
            if (m_Health > 1.0f) {
                m_Health -= 1.0f;
            }
            m_StarveTimer = 0.0f;
        }
    } else {
        m_StarveTimer = 0.0f;
    }

    // 4. Exhaustion to Hunger depletion
    if (m_Exhaustion >= 4.0f) {
        m_Hunger = std::max(0.0f, m_Hunger - 1.0f);
        m_Exhaustion -= 4.0f;
    }
}

float PlayerStats::applyFallDamage(float distance) {
    if (distance <= 3.0f) return 0.0f;
    float rawDamage = distance - 3.0f;
    float actualDamage = applyDamageReduction(rawDamage);
    setHealth(m_Health - actualDamage);
    applyArmorDurabilityDamage();
    return actualDamage;
}

void PlayerStats::updateEnvironmentalEffects(bool isHeadUnderwater, bool inLava, bool inWater, float deltaTime) {
    // Oxygen & Drowning
    if (isHeadUnderwater) {
        m_Oxygen = std::max(0.0f, m_Oxygen - 30.0f * deltaTime);
        if (m_Oxygen <= 0.0f) {
            m_DrownTimer += deltaTime;
            if (m_DrownTimer >= 1.0f) {
                setHealth(m_Health - 2.0f); // 1 heart of drowning damage
                m_DrownTimer = 0.0f;
            }
        } else {
            m_DrownTimer = 0.0f;
        }
    } else {
        m_Oxygen = std::min(300.0f, m_Oxygen + 150.0f * deltaTime);
        m_DrownTimer = 0.0f;
    }

    // Lava & Fire Burning
    if (inLava) {
        m_FireTicks = 300.0f; // 15 seconds of fire
        setHealth(m_Health - 4.0f * deltaTime); // direct contact damage
    } else if (inWater) {
        m_FireTicks = 0.0f; // extinguished in water
        m_FireDamageTimer = 0.0f;
    } else if (m_FireTicks > 0.0f) {
        m_FireTicks -= 20.0f * deltaTime;
        m_FireDamageTimer += deltaTime;
        if (m_FireDamageTimer >= 1.0f) {
            setHealth(m_Health - 1.0f); // fire burn damage
            m_FireDamageTimer = 0.0f;
        }
    }
}

}
