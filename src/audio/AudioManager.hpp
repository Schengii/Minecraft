#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <iostream>
#include <string>
#include <glm/glm.hpp>
#include "../world/Block.hpp"

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
    WaterSplash,
    WitherShoot,
    LevelUp,
    ToolBreak
};

class AudioManager {
public:
    static void init();
    static void playSound(SoundEffect effect, float volume = 1.0f);
    static void playSound3D(SoundEffect effect, const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront, float volume = 1.0f);
    static void playMaterialFootstep(BlockType block, const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront, float volume = 1.0f);

    static float getMasterVolume() { return s_MasterVolume; }
    static void setMasterVolume(float vol) { s_MasterVolume = glm::clamp(vol, 0.0f, 1.0f); }

    static float calculateDistanceGain(const glm::vec3& soundPos, const glm::vec3& listenerPos, float maxDistance = 40.0f);
    static float calculateStereoPan(const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront);

private:
    static float s_MasterVolume;
};

}

#endif // AUDIOMANAGER_HPP
