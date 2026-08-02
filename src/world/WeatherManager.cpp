#include "WeatherManager.hpp"
#include "../audio/AudioManager.hpp"
#include <cstdlib>
#include <algorithm>

namespace Minecraft {

WeatherManager::WeatherManager() = default;
WeatherManager::~WeatherManager() = default;

void WeatherManager::setWeather(WeatherState state) {
    m_State = state;
    if (state == WeatherState::Clear) m_RainIntensity = 0.0f;
    else if (state == WeatherState::Rain) m_RainIntensity = 0.6f;
    else if (state == WeatherState::Snow) m_RainIntensity = 0.5f;
    else if (state == WeatherState::Thunderstorm) m_RainIntensity = 1.0f;
}

void WeatherManager::update(const glm::vec3& playerPos, float deltaTime) {
    if (m_State == WeatherState::Clear) {
        m_RainIntensity = std::max(0.0f, m_RainIntensity - deltaTime * 0.1f);
        m_Particles.clear();
        return;
    }

    // Weather particle updates
    spawnWeatherParticles(playerPos);

    for (auto it = m_Particles.begin(); it != m_Particles.end(); ) {
        it->position += it->velocity * deltaTime;
        it->lifetime -= deltaTime;
        if (it->lifetime <= 0.0f || it->position.y < playerPos.y - 10.0f) {
            it = m_Particles.erase(it);
        } else {
            ++it;
        }
    }

    // Thunderstorm audio & lightning flashes
    if (m_State == WeatherState::Thunderstorm) {
        m_ThunderTimer += deltaTime;
        if (m_ThunderTimer >= 10.0f) {
            m_ThunderTimer = 0.0f;
            glm::vec3 thunderPos = playerPos + glm::vec3((rand() % 60 - 30), 20.0f, (rand() % 60 - 30));
            AudioManager::playSound3D(SoundEffect::Explosion, thunderPos, playerPos, glm::vec3(0, 0, -1));
        }
    }
}

void WeatherManager::spawnWeatherParticles(const glm::vec3& playerPos) {
    if (m_Particles.size() > 200) return;

    for (int i = 0; i < 5; ++i) {
        RainParticle p;
        p.position = playerPos + glm::vec3((rand() % 30 - 15), 15.0f, (rand() % 30 - 15));
        p.velocity = (m_State == WeatherState::Snow) ? glm::vec3(0, -3.0f, 0) : glm::vec3(0, -16.0f, 0);
        p.lifetime = 1.2f;
        m_Particles.push_back(p);
    }
}

}
