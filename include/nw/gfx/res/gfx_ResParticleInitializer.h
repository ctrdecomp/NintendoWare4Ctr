#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Color.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw{
namespace gfx {
namespace res {

struct ResParticleInitializerData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResBool m_IsResourceCopyEnabled;
    nw::ut::ResBool m_InitializerEnabled;
    u8              padding_0[2];
    nw::ut::ResS32 m_TargetStream;
};

class ResParticleInitializer : public nw::ut::ResCommon< ResParticleInitializerData >
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('INIT') };

    NW_RES_CTOR( ResParticleInitializer )


    bool GetIsResourceCopyEnabled() const { return static_cast<bool>(ref().m_IsResourceCopyEnabled); }
    void SetIsResourceCopyEnabled(bool value) { ref().m_IsResourceCopyEnabled = value; }

    bool GetInitializerEnabled() const { return static_cast<bool>(ref().m_InitializerEnabled); }
    void SetInitializerEnabled(bool value) { ref().m_InitializerEnabled = value; }

    ParticleUsage GetTargetStream() const { return static_cast<ParticleUsage>(ref().m_TargetStream); }
    void SetTargetStream(ParticleUsage value) { ref().m_TargetStream = value; }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

struct ResParticleDirectionalVelocityInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_Power;
    nw::ut::ResVec3 m_Direction;
};

class ResParticleDirectionalVelocityInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleDirectionalVelocityInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('DIRV') };

    NW_RES_CTOR_INHERIT( ResParticleDirectionalVelocityInitializer, ResParticleInitializer )


    f32 GetPower() const { return static_cast<f32>(ref().m_Power); }
    void SetPower(f32 value) { ref().m_Power = value; }

    void SetDirection(f32 x, f32 y, f32 z) { ref().m_Direction.Set(x, y, z); }
    const nw::math::VEC3& GetDirection() const { return ref().m_Direction; }
    void SetDirection(const nw::math::VEC3& value) { ref().m_Direction = value; }
};

struct ResParticleRandomDirectionalVelocityInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_Power;
    nw::ut::ResVec3 m_Direction;
    nw::ut::ResF32 m_Angle;
};

class ResParticleRandomDirectionalVelocityInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRandomDirectionalVelocityInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('RDRV') };

    NW_RES_CTOR_INHERIT( ResParticleRandomDirectionalVelocityInitializer, ResParticleInitializer )


    f32 GetPower() const { return static_cast<f32>(ref().m_Power); }
    void SetPower(f32 value) { ref().m_Power = value; }

    void SetDirection(f32 x, f32 y, f32 z) { ref().m_Direction.Set(x, y, z); }
    const nw::math::VEC3& GetDirection() const { return ref().m_Direction; }
    void SetDirection(const nw::math::VEC3& value) { ref().m_Direction = value; }

    f32 GetAngle() const { return static_cast<f32>(ref().m_Angle); }
    void SetAngle(f32 value) { ref().m_Angle = value; }
};

struct ResParticleOriginVelocityInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_Power;
};

class ResParticleOriginVelocityInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleOriginVelocityInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('ORIV') };

    NW_RES_CTOR_INHERIT( ResParticleOriginVelocityInitializer, ResParticleInitializer )


    f32 GetPower() const { return static_cast<f32>(ref().m_Power); }
    void SetPower(f32 value) { ref().m_Power = value; }
};

struct ResParticleRandomVelocityInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_Power;
};

class ResParticleRandomVelocityInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRandomVelocityInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('RNDV') };

    NW_RES_CTOR_INHERIT( ResParticleRandomVelocityInitializer, ResParticleInitializer )


    f32 GetPower() const { return static_cast<f32>(ref().m_Power); }
    void SetPower(f32 value) { ref().m_Power = value; }
};

struct ResParticleYAxisVelocityInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_Power;
};

class ResParticleYAxisVelocityInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleYAxisVelocityInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('YAXV') };

    NW_RES_CTOR_INHERIT( ResParticleYAxisVelocityInitializer, ResParticleInitializer )


        f32 GetPower() const { return static_cast<f32>(ref().m_Power); }
        void SetPower(f32 value) { ref().m_Power = value; }
};

struct ResParticleFloatRandomInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_BaseValue;
    nw::ut::ResF32 m_Random;
};

class ResParticleFloatRandomInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatRandomInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('FRND') };

    NW_RES_CTOR_INHERIT( ResParticleFloatRandomInitializer, ResParticleInitializer )


    f32 GetBaseValue() const { return static_cast<f32>(ref().m_BaseValue); }
    void SetBaseValue(f32 value) { ref().m_BaseValue = value; }

    f32 GetRandom() const { return static_cast<f32>(ref().m_Random); }
    void SetRandom(f32 value) { ref().m_Random = value; }
};

struct ResParticleFloatRangeRandomInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_MaxValue;
    nw::ut::ResF32 m_MinValue;
};

class ResParticleFloatRangeRandomInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatRangeRandomInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('URRA') };

    NW_RES_CTOR_INHERIT( ResParticleFloatRangeRandomInitializer, ResParticleInitializer )


    f32 GetMaxValue() const { return static_cast<f32>(ref().m_MaxValue); }
    void SetMaxValue(f32 value) { ref().m_MaxValue = value; }

    f32 GetMinValue() const { return static_cast<f32>(ref().m_MinValue); }
    void SetMinValue(f32 value) { ref().m_MinValue = value; }
};

struct ResParticleFloatImmediateInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_ImmediateValue;
};

class ResParticleFloatImmediateInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatImmediateInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('SFIM') };

    NW_RES_CTOR_INHERIT( ResParticleFloatImmediateInitializer, ResParticleInitializer )


    f32 GetImmediateValue() const { return static_cast<f32>(ref().m_ImmediateValue); }
    void SetImmediateValue(f32 value) { ref().m_ImmediateValue = value; }
};

struct ResParticleVector2ImmediateInitializerData : public ResParticleInitializerData
{
    nw::math::VEC2 m_ImmediateValue;
};

class ResParticleVector2ImmediateInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector2ImmediateInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V2IM') };

    NW_RES_CTOR_INHERIT( ResParticleVector2ImmediateInitializer, ResParticleInitializer )


    void SetImmediateValue(f32 x, f32 y) { ref().m_ImmediateValue.Set(x, y); }
    const nw::math::VEC2& GetImmediateValue() const { return ref().m_ImmediateValue; }
    void SetImmediateValue(const nw::math::VEC2& value) { ref().m_ImmediateValue = value; }
};

struct ResParticleVector3ImmediateInitializerData : public ResParticleInitializerData
{
    nw::ut::ResVec3 m_ImmediateValue;
};

class ResParticleVector3ImmediateInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3ImmediateInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3IM') };

    NW_RES_CTOR_INHERIT( ResParticleVector3ImmediateInitializer, ResParticleInitializer )


    void SetImmediateValue(f32 x, f32 y, f32 z) { ref().m_ImmediateValue.Set(x, y, z); }
    const nw::math::VEC3& GetImmediateValue() const { return ref().m_ImmediateValue; }
    void SetImmediateValue(const nw::math::VEC3& value) { ref().m_ImmediateValue = value; }
};

struct ResParticleVector3Random1InitializerData : public ResParticleInitializerData
{
    nw::ut::ResVec3 m_BaseValue;
    nw::ut::ResF32 m_Random;
};

class ResParticleVector3Random1Initializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Random1Initializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3R1') };

    NW_RES_CTOR_INHERIT( ResParticleVector3Random1Initializer, ResParticleInitializer )


    void SetBaseValue(f32 x, f32 y, f32 z) { ref().m_BaseValue.Set(x, y, z); }
    const nw::math::VEC3& GetBaseValue() const { return ref().m_BaseValue; }
    void SetBaseValue(const nw::math::VEC3& value) { ref().m_BaseValue = value; }

    f32 GetRandom() const { return static_cast<f32>(ref().m_Random); }
    void SetRandom(f32 value) { ref().m_Random = value; }
};

struct ResParticleVector3Random3InitializerData : public ResParticleInitializerData
{
    nw::ut::ResVec3 m_BaseValue;
    nw::ut::ResVec3 m_Random;
};

class ResParticleVector3Random3Initializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Random3Initializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3R3') };

    NW_RES_CTOR_INHERIT( ResParticleVector3Random3Initializer, ResParticleInitializer )


    void SetBaseValue(f32 x, f32 y, f32 z) { ref().m_BaseValue.Set(x, y, z); }
    const nw::math::VEC3& GetBaseValue() const { return ref().m_BaseValue; }
    void SetBaseValue(const nw::math::VEC3& value) { ref().m_BaseValue = value; }

    void SetRandom(f32 x, f32 y, f32 z) { ref().m_Random.Set(x, y, z); }
    const nw::math::VEC3& GetRandom() const { return ref().m_Random; }
    void SetRandom(const nw::math::VEC3& value) { ref().m_Random = value; }
};

struct ResParticleVector3MultRandomInitializerData : public ResParticleInitializerData
{
    nw::ut::ResF32 m_Random;
};

class ResParticleVector3MultRandomInitializer : public ResParticleInitializer
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3MultRandomInitializer) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3MR') };

    NW_RES_CTOR_INHERIT(ResParticleVector3MultRandomInitializer, ResParticleInitializer)


    f32 GetRandom() const { return static_cast<f32>(ref().m_Random); }
    void SetRandom(f32 value) { ref().m_Random = value; }
};

}
}
}

