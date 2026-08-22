#pragma once

#include <nw/types.h>

namespace nw{
namespace gfx{

class ParticleRandom{
public:
    ParticleRandom(){
        mSeed = 0;
    }

    ParticleRandom(const ParticleRandom& source){
        Set(source);
    }

    void Srand(u32 seed){
        mSeed = seed;
    }

    u16 Next(u16 maxValue){
        MixRandomSeed();
        return (u16)(mSeed >> 8) % maxValue;
    }

    f32 NextFloat(){
        MixRandomSeed();
        return (f32)((mSeed >> 16) & 0xffff) / 65536.0f;
    }

    f32 NextFloatSignedOne();

    f32 NextFloatSignedHalf();

    void Set(const ParticleRandom& source){
        mSeed = source.mSeed;
    }

private:
    void MixRandomSeed(){
        mSeed = (mSeed * 214013u) + 2531011u;
    }

    u32 mSeed;
};

}
}