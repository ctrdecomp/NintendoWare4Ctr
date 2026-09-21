#pragma once

#include <nw/types.h>

namespace nw{
namespace gfx{

class ParticleRandom
{
public:
    ParticleRandom()
    {
        m_Seed = 0;
    }

    ParticleRandom(const ParticleRandom& source)
    {
        Set(source);
    }

    void Srand(u32 seed)
    {
        m_Seed = seed;
    }

    u16 Next(u16 maxValue)
    {
        MixRandomSeed();
        return (u16)(m_Seed >> 8) % maxValue;
    }

    f32 NextFloat()
    {
        MixRandomSeed();
        return (f32)((m_Seed >> 16) & 0xffff) / 65536.0f;
    }

    f32 NextFloatSignedOne();

    f32 NextFloatSignedHalf();

    void Set(const ParticleRandom& source)
    {
        m_Seed = source.m_Seed;
    }

private:
    void MixRandomSeed()
    {
        m_Seed = (m_Seed * 214013u) + 2531011u;
    }

    u32 m_Seed;
};

}
}