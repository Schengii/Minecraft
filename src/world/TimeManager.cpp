#include "TimeManager.hpp"
#include <cmath>
#include <algorithm>

namespace Minecraft {

TimeManager::TimeManager() = default;

void TimeManager::update(float deltaTime) {
    float ticksPerSecond = 24000.0f / m_DayLengthSeconds;
    m_TimeTicks += ticksPerSecond * deltaTime;

    if (m_TimeTicks >= 24000.0f) {
        m_TimeTicks -= 24000.0f;
    }
}

void TimeManager::advanceTime(float ticks) {
    m_TimeTicks += ticks;
    if (m_TimeTicks >= 24000.0f) {
        m_TimeTicks -= 24000.0f;
    }
}

void TimeManager::setTimeOfDay(float ticks) {
    m_TimeTicks = ticks;
}

glm::vec3 TimeManager::getSunDirection() const {
    float angle = (m_TimeTicks / 24000.0f) * 2.0f * 3.14159265f;
    // Rotate sun across the sky
    float x = std::cos(angle);
    float y = std::sin(angle);
    return glm::normalize(glm::vec3(x, y, 0.4f));
}

glm::vec3 TimeManager::getSunColor() const {
    float sunY = getSunDirection().y;
    if (sunY > 0.2f) {
        // Bright day sun
        return glm::vec3(1.0f, 0.98f, 0.85f);
    } else if (sunY > -0.1f) {
        // Sunset / Sunrise warm orange
        float t = (sunY + 0.1f) / 0.3f;
        return glm::mix(glm::vec3(0.95f, 0.45f, 0.15f), glm::vec3(1.0f, 0.98f, 0.85f), t);
    } else {
        // Moon light (pale blue)
        return glm::vec3(0.3f, 0.4f, 0.6f);
    }
}

glm::vec3 TimeManager::getSkyColor() const {
    float sunY = getSunDirection().y;
    glm::vec3 daySky(0.53f, 0.81f, 0.98f);
    glm::vec3 sunsetSky(0.95f, 0.55f, 0.25f);
    glm::vec3 nightSky(0.02f, 0.04f, 0.12f);

    if (sunY > 0.2f) {
        return daySky;
    } else if (sunY > -0.1f) {
        float t = (sunY + 0.1f) / 0.3f;
        return glm::mix(sunsetSky, daySky, t);
    } else if (sunY > -0.3f) {
        float t = (sunY + 0.3f) / 0.2f;
        return glm::mix(nightSky, sunsetSky, t);
    } else {
        return nightSky;
    }
}

float TimeManager::getAmbientLight() const {
    float sunY = getSunDirection().y;
    if (sunY > 0.2f) {
        return 0.5f; // Daytime ambient
    } else if (sunY > -0.2f) {
        float t = (sunY + 0.2f) / 0.4f;
        return glm::mix(0.12f, 0.5f, t);
    } else {
        return 0.10f; // Night ambient
    }
}

}
