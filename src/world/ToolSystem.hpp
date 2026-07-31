#ifndef TOOLSYSTEM_HPP
#define TOOLSYSTEM_HPP

#include "Block.hpp"

namespace Minecraft {

enum class ToolCategory {
    None,
    Pickaxe,
    Axe,
    Shovel,
    Sword
};

enum class ToolTier {
    None,
    Wood,
    Stone,
    Iron,
    Diamond
};

struct ToolInfo {
    ToolCategory category = ToolCategory::None;
    ToolTier tier = ToolTier::None;
    int maxDurability = 0;
    float speedMultiplier = 1.0f;
};

class ToolSystem {
public:
    static bool isTool(BlockType type);
    static ToolInfo getToolInfo(BlockType type);
    static float getMiningSpeed(BlockType block, BlockType tool);
    static bool canHarvest(BlockType block, BlockType tool);
    static int getDamageDealt(BlockType tool);
};

}

#endif // TOOLSYSTEM_HPP
