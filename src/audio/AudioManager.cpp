#include "AudioManager.hpp"
#include <thread>
#ifdef _WIN32
#include <windows.h>
#endif

namespace Minecraft {

void AudioManager::init() {
    std::cout << "[AudioManager] Win32 Sound System Initialized." << std::endl;
}

void AudioManager::playSound(SoundEffect effect) {
    std::thread([effect]() {
#ifdef _WIN32
        switch (effect) {
            case SoundEffect::BlockBreak:
                Beep(160, 35);
                break;
            case SoundEffect::BlockPlace:
                Beep(420, 30);
                break;
            case SoundEffect::Footstep:
                Beep(240, 20);
                break;
            case SoundEffect::Jump:
                Beep(580, 45);
                break;
        }
#else
        (void)effect;
#endif
    }).detach();
}

}

