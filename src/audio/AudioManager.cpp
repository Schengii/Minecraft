#include "AudioManager.hpp"
#include <thread>
#include <cmath>
#include <algorithm>
#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace Minecraft {

float AudioManager::s_MasterVolume = 1.0f;

void AudioManager::init() {
    std::cout << "[AudioManager] 3D Spatial Sound System Initialized (Master Volume: " << s_MasterVolume << ")." << std::endl;
}

float AudioManager::calculateDistanceGain(const glm::vec3& soundPos, const glm::vec3& listenerPos, float maxDistance) {
    float dist = glm::distance(soundPos, listenerPos);
    if (dist >= maxDistance) return 0.0f;
    float gain = 1.0f - (dist / maxDistance);
    return std::max(0.0f, gain * gain); // Inverse square approximation
}

float AudioManager::calculateStereoPan(const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront) {
    glm::vec3 toSound = soundPos - listenerPos;
    if (glm::length(toSound) < 0.001f) return 0.0f;
    toSound = glm::normalize(toSound);

    glm::vec3 listenerRight = glm::normalize(glm::cross(listenerFront, glm::vec3(0, 1, 0)));
    float pan = glm::dot(toSound, listenerRight);
    return glm::clamp(pan, -1.0f, 1.0f);
}

void AudioManager::playSound(SoundEffect effect, float volume) {
    playSound3D(effect, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), volume);
}

void AudioManager::playSound3D(SoundEffect effect, const glm::vec3& soundPos, const glm::vec3& listenerPos, const glm::vec3& listenerFront, float volume) {
    float gain = calculateDistanceGain(soundPos, listenerPos);
    if (gain <= 0.001f && effect != SoundEffect::Explosion) {
        return;
    }

    float finalVol = volume * gain * s_MasterVolume;
    if (finalVol <= 0.01f) return;

    std::thread([effect, finalVol]() {
#ifdef _WIN32
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

        Beep(freq, duration);
#else
        (void)effect; (void)finalVol;
#endif
    }).detach();
}

}
