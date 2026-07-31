#ifndef AUDIOMANAGER_HPP
#define AUDIOMANAGER_HPP

#include <iostream>
#include <string>

namespace Minecraft {

enum class SoundEffect {
    BlockBreak,
    BlockPlace,
    Footstep,
    Jump
};

class AudioManager {
public:
    static void init();
    static void playSound(SoundEffect effect);
};

}

#endif // AUDIOMANAGER_HPP
