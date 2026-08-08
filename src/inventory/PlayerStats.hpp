#ifndef PLAYERSTATS_HPP
#define PLAYERSTATS_HPP

#include "ItemStack.hpp"
#include <array>
#include <algorithm>

namespace Minecraft {

class PlayerStats {
public:
    PlayerStats();

    float getHealth() const { return m_Health; }
    void setHealth(float hp) { m_Health = std::clamp(hp, 0.0f, 20.0f); }

    float getHunger() const { return m_Hunger; }
    void setHunger(float hunger) { m_Hunger = std::clamp(hunger, 0.0f, 20.0f); }

    float getExhaustion() const { return m_Exhaustion; }
    void addExhaustion(float amount) { m_Exhaustion += amount; }

    void update(float deltaTime);

    // Armor Slots: 0=Helmet, 1=Chestplate, 2=Leggings, 3=Boots
    ItemStack& getArmorSlot(int slotIndex) { return m_ArmorSlots[slotIndex]; }
    const ItemStack& getArmorSlot(int slotIndex) const { return m_ArmorSlots[slotIndex]; }

    int getTotalArmorPoints() const;
    float applyDamageReduction(float incomingDamage);
    void applyArmorDurabilityDamage();

private:
    float m_Health = 20.0f;
    float m_Hunger = 20.0f;
    float m_Exhaustion = 0.0f;
    float m_RegenTimer = 0.0f;
    float m_StarveTimer = 0.0f;
    std::array<ItemStack, 4> m_ArmorSlots;
};

}

#endif // PLAYERSTATS_HPP
