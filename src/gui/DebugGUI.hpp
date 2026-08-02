#ifndef DEBUGGUI_HPP
#define DEBUGGUI_HPP

#include <glm/glm.hpp>

namespace Minecraft {

class Application;

class DebugGUI {
public:
    DebugGUI();
    ~DebugGUI();

    void render(float fps, const glm::vec3& playerPos, const glm::vec3& playerFront, float& timeTicks, bool& isFlying);
};

}

#endif // DEBUGGUI_HPP
