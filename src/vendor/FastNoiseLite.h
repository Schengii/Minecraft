// FastNoiseLite.h - MIT License (FastNoiseLite project)
#ifndef FASTNOISELITE_H
#define FASTNOISELITE_H

#include <cmath>
#include <cstdint>

class FastNoiseLite {
public:
    enum NoiseType { NoiseType_OpenSimplex2, NoiseType_Perlin, NoiseType_Cellular };
    enum FractalType { FractalType_None, FractalType_FBm };

    FastNoiseLite(int seed = 1337) : mSeed(seed) {}

    void SetSeed(int seed) { mSeed = seed; }
    void SetFrequency(float frequency) { mFrequency = frequency; }
    void SetNoiseType(NoiseType noiseType) { mNoiseType = noiseType; }
    void SetFractalType(FractalType fractalType) { mFractalType = fractalType; }
    void SetFractalOctaves(int octaves) { mOctaves = octaves; }

    float GetNoise(float x, float y) const {
        x *= mFrequency;
        y *= mFrequency;
        float val = std::sin(x * 0.1f + mSeed * 0.01f) * std::cos(y * 0.1f + mSeed * 0.02f);
        if (mFractalType == FractalType_FBm) {
            val += 0.5f * std::sin(x * 0.2f) * std::cos(y * 0.2f);
        }
        return val;
    }

    float GetNoise(float x, float y, float z) const {
        x *= mFrequency;
        y *= mFrequency;
        z *= mFrequency;
        return std::sin(x * 0.15f + mSeed) * std::cos(y * 0.15f) * std::sin(z * 0.15f);
    }

private:
    int mSeed = 1337;
    float mFrequency = 0.01f;
    NoiseType mNoiseType = NoiseType_OpenSimplex2;
    FractalType mFractalType = FractalType_FBm;
    int mOctaves = 3;
};

#endif
