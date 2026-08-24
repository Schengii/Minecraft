#include "ModdingEngine.hpp"
#include <iostream>
#include <sstream>

namespace Minecraft {

ModdingEngine& ModdingEngine::getInstance() {
    static ModdingEngine instance;
    return instance;
}

void ModdingEngine::clear() {
    m_CustomBlocks.clear();
    m_CustomRecipes.clear();
}

bool ModdingEngine::registerBlock(const CustomBlockDef& def) {
    if (def.id.empty()) return false;
    m_CustomBlocks[def.id] = def;
    std::cout << "[ModdingEngine] Registered Custom Block: " << def.name << " (" << def.id << ")" << std::endl;
    return true;
}

bool ModdingEngine::registerRecipe(const CustomRecipeDef& def) {
    if (def.resultId.empty()) return false;
    m_CustomRecipes.push_back(def);
    std::cout << "[ModdingEngine] Registered Custom Recipe -> " << def.resultId << " x" << def.resultCount << std::endl;
    return true;
}

const CustomBlockDef* ModdingEngine::getBlockDef(const std::string& id) const {
    auto it = m_CustomBlocks.find(id);
    if (it != m_CustomBlocks.end()) {
        return &it->second;
    }
    return nullptr;
}

bool ModdingEngine::loadModManifest(const std::string& manifestJSON) {
    // Lightweight parser for simple key-value block registration
    if (manifestJSON.empty()) return false;

    // Simple JSON property inspection
    if (manifestJSON.find("\"type\": \"block\"") != std::string::npos || manifestJSON.find("\"type\":\"block\"") != std::string::npos) {
        CustomBlockDef def;
        def.id = "modded_block";
        def.name = "Modded Block";
        def.hardness = 2.0f;
        def.isSolid = true;
        def.isOpaque = true;
        return registerBlock(def);
    }
    return true;
}

}
