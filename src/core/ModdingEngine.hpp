#ifndef MODDINGENGINE_HPP
#define MODDINGENGINE_HPP

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../world/Block.hpp"

namespace Minecraft {

struct CustomBlockDef {
    std::string id;
    std::string name;
    bool isSolid = true;
    bool isOpaque = true;
    int lightEmission = 0;
    float hardness = 1.5f;
    BlockType baseType = BlockType::Stone;
};

struct CustomRecipeDef {
    std::string resultId;
    int resultCount = 1;
    std::vector<std::string> pattern;
};

class ModdingEngine {
public:
    static ModdingEngine& getInstance();

    bool registerBlock(const CustomBlockDef& def);
    bool registerRecipe(const CustomRecipeDef& def);

    const CustomBlockDef* getBlockDef(const std::string& id) const;
    const std::unordered_map<std::string, CustomBlockDef>& getAllBlocks() const { return m_CustomBlocks; }
    const std::vector<CustomRecipeDef>& getAllRecipes() const { return m_CustomRecipes; }

    bool loadModManifest(const std::string& manifestJSON);
    void clear();

private:
    ModdingEngine() = default;
    ~ModdingEngine() = default;

    std::unordered_map<std::string, CustomBlockDef> m_CustomBlocks;
    std::vector<CustomRecipeDef> m_CustomRecipes;
};

}

#endif // MODDINGENGINE_HPP
