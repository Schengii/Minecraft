#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP

#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include "../world/Block.hpp"
#include "../world/TimeManager.hpp"
#include "../world/WeatherManager.hpp"
#include "../inventory/PlayerStats.hpp"
#include "../inventory/Inventory.hpp"
#include "../renderer/Camera.hpp"

namespace Minecraft {

class CommandParser {
public:
    static bool execute(const std::string& cmd,
                        TimeManager* timeMgr,
                        WeatherManager* weatherMgr,
                        PlayerStats* playerStats,
                        Inventory* inventory,
                        Camera* camera,
                        bool* isFlying)
    {
        if (cmd.empty()) return false;
        std::stringstream ss(cmd);
        std::string prefix;
        ss >> prefix;

        if (prefix == "/gamemode") {
            std::string mode;
            ss >> mode;
            if (mode == "creative" || mode == "1" || mode == "c") {
                if (isFlying) *isFlying = true;
                return true;
            } else if (mode == "survival" || mode == "0" || mode == "s") {
                if (isFlying) *isFlying = false;
                return true;
            }
        } else if (prefix == "/time") {
            std::string sub;
            ss >> sub;
            if (sub == "set") {
                std::string timeVal;
                ss >> timeVal;
                if (!timeMgr) return true;
                if (timeVal == "day") timeMgr->setTimeOfDay(1000.0f);
                else if (timeVal == "noon") timeMgr->setTimeOfDay(6000.0f);
                else if (timeVal == "sunset") timeMgr->setTimeOfDay(12000.0f);
                else if (timeVal == "night") timeMgr->setTimeOfDay(14000.0f);
                else if (timeVal == "midnight") timeMgr->setTimeOfDay(18000.0f);
                else if (timeVal == "sunrise") timeMgr->setTimeOfDay(23000.0f);
                else {
                    try {
                        float val = std::stof(timeVal);
                        timeMgr->setTimeOfDay(val);
                    } catch (...) { return false; }
                }
                return true;
            }
        } else if (prefix == "/tp") {
            float x, y, z;
            if (ss >> x >> y >> z) {
                if (camera) {
                    Camera tempCam(glm::vec3(x, y, z), glm::vec3(0, 1, 0));
                    *camera = tempCam;
                }
                return true;
            }
        } else if (prefix == "/give") {
            std::string itemName;
            int count = 1;
            if (ss >> itemName) {
                if (!(ss >> count)) count = 1;
                BlockType type = BlockType::DiamondOre;
                if (itemName == "diamond" || itemName == "diamond_ore") type = BlockType::DiamondOre;
                else if (itemName == "iron" || itemName == "iron_ore") type = BlockType::IronOre;
                else if (itemName == "gold" || itemName == "gold_ore") type = BlockType::GoldOre;
                else if (itemName == "wood" || itemName == "oak_log") type = BlockType::OakLog;
                else if (itemName == "stone") type = BlockType::Stone;
                else if (itemName == "obsidian") type = BlockType::Obsidian;
                else if (itemName == "tnt") type = BlockType::TNT;
                else if (itemName == "sword" || itemName == "diamond_sword") type = BlockType::DiamondSword;
                else if (itemName == "pickaxe" || itemName == "diamond_pickaxe") type = BlockType::DiamondPickaxe;
                else if (itemName == "apple" || itemName == "golden_apple") type = BlockType::GoldenApple;

                if (inventory) {
                    inventory->addItem(type, count);
                }
                return true;
            }
        } else if (prefix == "/weather") {
            std::string weatherType;
            ss >> weatherType;
            if (weatherType == "clear") {
                if (weatherMgr) weatherMgr->setWeather(WeatherState::Clear);
                return true;
            } else if (weatherType == "rain") {
                if (weatherMgr) weatherMgr->setWeather(WeatherState::Rain);
                return true;
            } else if (weatherType == "thunder") {
                if (weatherMgr) weatherMgr->setWeather(WeatherState::Thunderstorm);
                return true;
            }
        } else if (prefix == "/kill") {
            if (playerStats) playerStats->setHealth(0.0f);
            return true;
        } else if (prefix == "/heal") {
            if (playerStats) {
                playerStats->setHealth(20.0f);
                playerStats->setHunger(20.0f);
                playerStats->setOxygen(300.0f);
                playerStats->setFireTicks(0.0f);
            }
            return true;
        }
        return false;
    }
};

}

#endif // COMMANDPARSER_HPP
