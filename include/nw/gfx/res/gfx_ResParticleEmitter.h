#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResParticleForm.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleEmitterData : public ResTransformNodeData{
    nw::ut::ResU32 mId;
    nw::ut::BinString toParticleSetPath;
    nw::ut::Offset toParticleForm;
    nw::ut::ResBool mIsResourceCopyEnabled;
    u8              padding_0[3];
    nw::ut::ResF32 mEmissionRatio;
    nw::ut::ResF32 mEmissionRatioRandom;
    nw::ut::ResS32 mEmissionInterval;
    nw::ut::ResF32 mEmissionIntervalRandom;
    nw::ut::ResBool mEmissionSpanInfinity;
    u8              padding_1[3];
    nw::ut::ResS32 mEmissionSpan;
    nw::ut::ResS32 mEmissionStart;
};

struct ResParticleEmitterParameterData{
    nw::ut::ResBool mIsResourceCopyEnabled;
    u8              padding_0[3];
    nw::ut::ResF32 mEmissionRatio;
    nw::ut::ResF32 mEmissionRatioRandom;
    nw::ut::ResS32 mEmissionInterval;
    nw::ut::ResF32 mEmissionIntervalRandom;
    nw::ut::ResBool mEmissionSpanInfinity;
    u8              padding_1[3];
    nw::ut::ResS32 mEmissionSpan;
    nw::ut::ResS32 mEmissionStart;
};

class ResParticleEmitter : public ResTransformNode{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleEmitter) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PEMT') };

    NW_RES_CTOR_INHERIT( ResParticleEmitter, ResTransformNode )

    NW_RES_FIELD_PRIMITIVE_DECL( u32, Id )
    NW_RES_FIELD_STRING_DECL( ParticleSetPath )
    NW_RES_FIELD_CLASS_DECL( ResParticleForm, ParticleForm)
    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsResourceCopyEnabled)
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatio )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatioRandom )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionInterval )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionIntervalRandom )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EmissionSpanInfinity)
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionSpan)
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionStart)
};

class ResParticleEmitterParameter : public nw::ut::ResCommon<ResParticleEmitterParameterData>{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleEmitterParameter) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PEMT') };

    NW_RES_CTOR( ResParticleEmitterParameter )

    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsResourceCopyEnabled)
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatio )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionRatioRandom )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionInterval )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, EmissionIntervalRandom )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, EmissionSpanInfinity)
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionSpan)
    NW_RES_FIELD_PRIMITIVE_DECL( s32, EmissionStart)
};

typedef nw::ut::ResArrayPatricia<ResParticleEmitter>::type  ResEmitterArray;

}
}
}