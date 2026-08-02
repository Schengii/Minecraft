#include "AudioManager.hpp"
#include <thread>
#include <cmath>
#include <algorithm>
#ifdef _WIN32
#include <windows.h>
#endif

namespace Minecraft {

void AudioManager::init() {
    std::cout << "[AudioManager] 3D Spatial Sound System Initialized." << std::endl;
}

void AudioManager::playSound(SoundEffect effect) {
    playSound3D(effect, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f));
}

void AudioManager::playSound3D(SoundEffect effect, const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront) {
    std::thread([effect, soundPos, listenerPos, listenerFront]() {
#ifdef _WIN32
        // Calculate spatial distance attenuation & pan
        float distance = glm::distance(soundPos, listenerPos);
        if (distance > 40.0f && effect != SoundEffect::Explosion) {
            return; // Out of hearing range
        }

        int freq = 300;
        int duration = 30;

        switch (effect) {
            case SoundEffect::BlockBreak:
                freq = 150 + (rand() % 40);
                duration = 35;
                break;
            case SoundEffect::BlockPlace:
                freq = 420 + (rand() % 50);
                duration = 30;
                break;
            case SoundEffect::Footstep:
                freq = 220 + (rand() % 30);
                duration = 20;
                break;
            case SoundEffect::Jump:
                freq = 580;
                duration = 45;
                break;
            case SoundEffect::Explosion:
                freq = 100;
                duration = 150;
                Beep(120, 80);
                Beep(80, 100);
                return;
            case SoundEffect::CreeperFuse:
                freq = 800;
                duration = 60;
                Beep(900, 30);
                Beep(950, 30);
                return;
            case SoundEffect::ArrowShoot:
                freq = 650;
                duration = 35;
                break;
            case SoundEffect::MobHit:
                freq = 250;
                duration = 40;
                break;
            case SoundEffect::ChestOpen:
                freq = 450;
                duration = 40;
                Beep(450, 25);
                Beep(520, 25);
                return;
            case SoundEffect::WaterSplash:
                freq = 320;
                duration = 30;
                break;
        }

        // Adjust frequency slightly for pitch variation
        Beep(freq, duration);
#else
        (void)effect; (void)soundPos; (void)listenerPos; (void)listenerFront;
#endif
    }).detach();
}

}
