#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_ResDeclMacros.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleAnimationOptionData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResF32 mAnimationDuration;
};

class ResParticleAnimationOption : public nw::ut::ResCommon< ResParticleAnimationOptionData >{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleAnimationOption) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('AOPT') };

    NW_RES_CTOR( ResParticleAnimationOption )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, AnimationDuration )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }

    static u32 GetHash(u32 key1, u32 key2, u32 key3, u32 key4);
//    ParticleTime EvaluateAnimationFrame(u32 id, ParticleTime birth, ParticleTime life, ParticleTime time, int updaterIndex) const;
};

struct ResParticleFittingAnimationOptionData : public ResParticleAnimationOptionData{};

class ResParticleFittingAnimationOption : public ResParticleAnimationOption{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFittingAnimationOption) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('FITT') };

    NW_RES_CTOR_INHERIT( ResParticleFittingAnimationOption, ResParticleAnimationOption )
};

struct ResParticleFrameLoopAnimationOptionData : public ResParticleAnimationOptionData{
    nw::ut::ResU32 mLoopDuration;
    nw::ut::ResBool mRandomOffset;
    u8              padding_0[3];
};

class ResParticleFrameLoopAnimationOption : public ResParticleAnimationOption{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFrameLoopAnimationOption) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('FLOO') };

    NW_RES_CTOR_INHERIT( ResParticleFrameLoopAnimationOption, ResParticleAnimationOption )

    NW_RES_FIELD_PRIMITIVE_DECL( u32, LoopDuration )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, RandomOffset )
};

struct ResParticleRandomAnimationOptionData : public ResParticleAnimationOptionData{};

class ResParticleRandomAnimationOption : public ResParticleAnimationOption{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRandomAnimationOption) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('RAND') };

    NW_RES_CTOR_INHERIT( ResParticleRandomAnimationOption, ResParticleAnimationOption )
};

}
}
}