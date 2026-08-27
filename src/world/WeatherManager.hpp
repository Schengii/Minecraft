#ifndef WEATHERMANAGER_HPP
#define WEATHERMANAGER_HPP

#include <glm/glm.hpp>
#include <vector>

namespace Minecraft {

enum class WeatherState {
    Clear,
    Rain,
    Snow,
    Thunderstorm
};

struct RainParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float lifetime = 1.0f;
};

class WeatherManager {
public:
    WeatherManager();
    ~WeatherManager();

    void update(const glm::vec3& playerPos, float deltaTime);
    void setWeather(WeatherState state);

    WeatherState getWeatherState() const { return m_State; }
    float getRainIntensity() const { return m_RainIntensity; }
    bool isRaining() const { return m_State == WeatherState::Rain || m_State == WeatherState::Thunderstorm || m_State == WeatherState::Snow; }
    bool isThundering() const { return m_State == WeatherState::Thunderstorm; }

    const std::vector<RainParticle>& getParticles() const { return m_Particles; }

private:
    WeatherState m_State = WeatherState::Clear;
    float m_RainIntensity = 0.0f; // 0.0f to 1.0f
    float m_ThunderTimer = 0.0f;
    std::vector<RainParticle> m_Particles;

    void spawnWeatherParticles(const glm::vec3& playerPos);
};

}

#endif // WEATHERMANAGER_HPP
