#include "TradingEngine.hpp"

namespace Minecraft {

std::vector<TradeOffer> TradingEngine::getTradesForProfession(VillagerProfession prof) {
    std::vector<TradeOffer> trades;

    switch (prof) {
        case VillagerProfession::Farmer:
            // 20 Bread -> 1 Emerald
            trades.push_back({ { BlockType::Bread, 20, 64 }, { BlockType::Air, 0, 64 }, { BlockType::Emerald, 1, 64 } });
            // 1 Emerald -> 6 Apples
            trades.push_back({ { BlockType::Emerald, 1, 64 }, { BlockType::Air, 0, 64 }, { BlockType::Apple, 6, 64 } });
            break;

        case VillagerProfession::Blacksmith:
            // 4 Iron -> 1 Emerald
            trades.push_back({ { BlockType::IronOre, 4, 64 }, { BlockType::Air, 0, 64 }, { BlockType::Emerald, 1, 64 } });
            // 3 Emeralds -> 1 Diamond Pickaxe
            trades.push_back({ { BlockType::Emerald, 3, 64 }, { BlockType::Air, 0, 64 }, { BlockType::DiamondPickaxe, 1, 1 } });
            // 5 Emeralds -> 1 Diamond Sword
            trades.push_back({ { BlockType::Emerald, 5, 64 }, { BlockType::Air, 0, 64 }, { BlockType::DiamondSword, 1, 1 } });
            break;

        case VillagerProfession::Librarian:
            // 24 Paper/Planks -> 1 Emerald
            trades.push_back({ { BlockType::Planks, 24, 64 }, { BlockType::Air, 0, 64 }, { BlockType::Emerald, 1, 64 } });
            // 1 Emerald + 1 Book -> 1 Bookshelf
            trades.push_back({ { BlockType::Emerald, 1, 64 }, { BlockType::Stick, 1, 64 }, { BlockType::Bookshelf, 1, 64 } });
            break;

        case VillagerProfession::Cleric:
            // 32 Rotten Meat / RawPork -> 1 Emerald
            trades.push_back({ { BlockType::RawPorkchop, 16, 64 }, { BlockType::Air, 0, 64 }, { BlockType::Emerald, 1, 64 } });
            // 2 Emeralds -> 4 Glowstone
            trades.push_back({ { BlockType::Emerald, 2, 64 }, { BlockType::Air, 0, 64 }, { BlockType::Glowstone, 4, 64 } });
            break;
    }

    return trades;
}

bool TradingEngine::executeTrade(VillagerProfession prof, int tradeIndex, ItemStack& slot1, ItemStack& slot2, ItemStack& resultSlot) {
    auto trades = getTradesForProfession(prof);
    if (tradeIndex < 0 || tradeIndex >= static_cast<int>(trades.size())) return false;

    const TradeOffer& offer = trades[tradeIndex];
    if (!offer.canTrade(slot1, slot2)) return false;

    // Deduct inputs
    slot1.count -= offer.input1.count;
    if (slot1.count <= 0) slot1.clear();

    if (!offer.input2.isEmpty()) {
        slot2.count -= offer.input2.count;
        if (slot2.count <= 0) slot2.clear();
    }

    // Give result
    resultSlot = offer.output;
    return true;
}

}
