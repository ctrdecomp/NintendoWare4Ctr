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

struct ResParticleEmitterData : public ResTransformNodeData
{
    nw::ut::ResU32 m_Id;
    nw::ut::BinString toParticleSetPath;
    nw::ut::Offset toParticleForm;
    nw::ut::ResBool m_IsResourceCopyEnabled;
    u8              padding_0[3];
    nw::ut::ResF32 m_EmissionRatio;
    nw::ut::ResF32 m_EmissionRatioRandom;
    nw::ut::ResS32 m_EmissionInterval;
    nw::ut::ResF32 m_EmissionIntervalRandom;
    nw::ut::ResBool m_EmissionSpanInfinity;
    u8              padding_1[3];
    nw::ut::ResS32 m_EmissionSpan;
    nw::ut::ResS32 m_EmissionStart;
};

struct ResParticleEmitterParameterData
{
    nw::ut::ResBool m_IsResourceCopyEnabled;
    u8              padding_0[3];
    nw::ut::ResF32 m_EmissionRatio;
    nw::ut::ResF32 m_EmissionRatioRandom;
    nw::ut::ResS32 m_EmissionInterval;
    nw::ut::ResF32 m_EmissionIntervalRandom;
    nw::ut::ResBool m_EmissionSpanInfinity;
    u8              padding_1[3];
    nw::ut::ResS32 m_EmissionSpan;
    nw::ut::ResS32 m_EmissionStart;
};

class ResParticleEmitter : public ResTransformNode
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleEmitter)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('PEMT')
    };

    NW_RES_CTOR_INHERIT( ResParticleEmitter, ResTransformNode )


    u32 GetId() const { return static_cast<u32>(ref().m_Id); }
    void SetId(u32 value) { ref().m_Id = value; }

    const char* GetParticleSetPath() const { return ref().toParticleSetPath.to_ptr(); }

    ResParticleForm GetParticleForm() { return ResParticleForm(ref().toParticleForm.to_ptr()); }
    const ResParticleForm GetParticleForm() const { return ResParticleForm(ref().toParticleForm.to_ptr()); }

    bool GetIsResourceCopyEnabled() const { return static_cast<bool>(ref().m_IsResourceCopyEnabled); }
    void SetIsResourceCopyEnabled(bool value) { ref().m_IsResourceCopyEnabled = value; }

    f32 GetEmissionRatio() const { return static_cast<f32>(ref().m_EmissionRatio); }
    void SetEmissionRatio(f32 value) { ref().m_EmissionRatio = value; }

    f32 GetEmissionRatioRandom() const { return static_cast<f32>(ref().m_EmissionRatioRandom); }
    void SetEmissionRatioRandom(f32 value) { ref().m_EmissionRatioRandom = value; }

    s32 GetEmissionInterval() const { return static_cast<s32>(ref().m_EmissionInterval); }
    void SetEmissionInterval(s32 value) { ref().m_EmissionInterval = value; }

    f32 GetEmissionIntervalRandom() const { return static_cast<f32>(ref().m_EmissionIntervalRandom); }
    void SetEmissionIntervalRandom(f32 value) { ref().m_EmissionIntervalRandom = value; }

    bool GetEmissionSpanInfinity() const { return static_cast<bool>(ref().m_EmissionSpanInfinity); }
    void SetEmissionSpanInfinity(bool value) { ref().m_EmissionSpanInfinity = value; }

    s32 GetEmissionSpan() const { return static_cast<s32>(ref().m_EmissionSpan); }
    void SetEmissionSpan(s32 value) { ref().m_EmissionSpan = value; }

    s32 GetEmissionStart() const { return static_cast<s32>(ref().m_EmissionStart); }
    void SetEmissionStart(s32 value) { ref().m_EmissionStart = value; }
};

class ResParticleEmitterParameter : public nw::ut::ResCommon<ResParticleEmitterParameterData>
{
public:
    NW_RES_CTOR( ResParticleEmitterParameter )


    bool GetIsResourceCopyEnabled() const { return static_cast<bool>(ref().m_IsResourceCopyEnabled); }
    void SetIsResourceCopyEnabled(bool value) { ref().m_IsResourceCopyEnabled = value; }

    f32 GetEmissionRatio() const { return static_cast<f32>(ref().m_EmissionRatio); }
    void SetEmissionRatio(f32 value) { ref().m_EmissionRatio = value; }

    f32 GetEmissionRatioRandom() const { return static_cast<f32>(ref().m_EmissionRatioRandom); }
    void SetEmissionRatioRandom(f32 value) { ref().m_EmissionRatioRandom = value; }

    s32 GetEmissionInterval() const { return static_cast<s32>(ref().m_EmissionInterval); }
    void SetEmissionInterval(s32 value) { ref().m_EmissionInterval = value; }

    f32 GetEmissionIntervalRandom() const { return static_cast<f32>(ref().m_EmissionIntervalRandom); }
    void SetEmissionIntervalRandom(f32 value) { ref().m_EmissionIntervalRandom = value; }

    bool GetEmissionSpanInfinity() const { return static_cast<bool>(ref().m_EmissionSpanInfinity); }
    void SetEmissionSpanInfinity(bool value) { ref().m_EmissionSpanInfinity = value; }

    s32 GetEmissionSpan() const { return static_cast<s32>(ref().m_EmissionSpan); }
    void SetEmissionSpan(s32 value) { ref().m_EmissionSpan = value; }

    s32 GetEmissionStart() const { return static_cast<s32>(ref().m_EmissionStart); }
    void SetEmissionStart(s32 value) { ref().m_EmissionStart = value; }
};

typedef nw::ut::ResArrayPatricia<ResParticleEmitter>::type  ResEmitterArray;

}
}
}

