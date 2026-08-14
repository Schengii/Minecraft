#ifndef TRADINGENGINE_HPP
#define TRADINGENGINE_HPP

#include "../inventory/ItemStack.hpp"
#include <vector>
#include <string>

namespace Minecraft {

enum class VillagerProfession {
    Farmer,
    Blacksmith,
    Librarian,
    Cleric
};

struct TradeOffer {
    ItemStack input1;
    ItemStack input2;
    ItemStack output;
    int maxUses = 16;
    int uses = 0;

    bool canTrade(const ItemStack& offer1, const ItemStack& offer2) const {
        if (offer1.type != input1.type || offer1.count < input1.count) return false;
        if (!input2.isEmpty()) {
            if (offer2.type != input2.type || offer2.count < input2.count) return false;
        }
        return uses < maxUses;
    }
};

class TradingEngine {
public:
    static std::vector<TradeOffer> getTradesForProfession(VillagerProfession prof);
    static bool executeTrade(VillagerProfession prof, int tradeIndex, ItemStack& slot1, ItemStack& slot2, ItemStack& resultSlot);
};

}

#endif // TRADINGENGINE_HPP
