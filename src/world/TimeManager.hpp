#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <glm/glm.hpp>

namespace Minecraft {

class TimeManager {
public:
    TimeManager();

    void update(float deltaTime);
    void advanceTime(float ticks);
    void setTimeOfDay(float ticks); // 0 to 24000

    float getTimeTicks() const { return m_TimeTicks; }
    glm::vec3 getSunDirection() const;
    glm::vec3 getSunColor() const;
    glm::vec3 getSkyColor() const;
    float getAmbientLight() const;

private:
    float m_TimeTicks = 6000.0f; // Start at Noon (6000 ticks)
    float m_DayLengthSeconds = 300.0f; // 5 minutes per full day/night cycle
};

}

#endif // TIMEMANAGER_HPP
