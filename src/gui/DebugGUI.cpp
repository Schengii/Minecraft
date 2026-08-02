#include "DebugGUI.hpp"
#include <iostream>

namespace Minecraft {

DebugGUI::DebugGUI() = default;
DebugGUI::~DebugGUI() = default;

void DebugGUI::render(float fps, const glm::vec3& playerPos, const glm::vec3& playerFront, float& timeTicks, bool& isFlying) {
    // Light-weight fallback console log overlay if ImGui binary bindings are optional
}

}
