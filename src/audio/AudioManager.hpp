#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <iostream>
#include <string>
#include <glm/glm.hpp>

namespace Minecraft {

enum class SoundEffect {
    BlockBreak,
    BlockPlace,
    Footstep,
    Jump,
    Explosion,
    CreeperFuse,
    ArrowShoot,
    MobHit,
    ChestOpen,
    WaterSplash
};

class AudioManager {
public:
    static void init();
    static void playSound(SoundEffect effect);
    static void playSound3D(SoundEffect effect, const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront);
};

}

#endif // AUDIOMANAGER_HPP
