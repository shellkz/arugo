#pragma once
#include <random>
#include <chrono>

namespace Random
{

    inline std::mt19937 &engine()
    {
        static std::mt19937 rng([]()
                                {
        std::size_t seed = std::random_device{}();
        seed ^= static_cast<std::size_t>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        );
        return seed; }());
        return rng;
    }

    inline int randomInt(int min, int max)
    {
        std::uniform_int_distribution<int> dist(min, max);
        return dist(engine());
    }

    inline float randomFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(engine());
    }

}