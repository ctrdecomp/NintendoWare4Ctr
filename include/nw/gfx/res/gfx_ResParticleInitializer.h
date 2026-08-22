#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Color.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw{
namespace gfx {
namespace res {

struct ResParticleInitializerData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResBool mIsResourceCopyEnabled;
    nw::ut::ResBool mInitializerEnabled;
    u8              padding_0[2];
    nw::ut::ResS32 mTargetStream;
};

class ResParticleInitializer : public nw::ut::ResCommon< ResParticleInitializerData >{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('INIT') };

    NW_RES_CTOR( ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsResourceCopyEnabled )
    NW_RES_FIELD_PRIMITIVE_DECL( bool, InitializerEnabled )
    NW_RES_FIELD_PRIMITIVE_DECL( ParticleUsage, TargetStream )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

struct ResParticleDirectionalVelocityInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mPower;
    nw::ut::ResVec3 mDirection;
};

class ResParticleDirectionalVelocityInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleDirectionalVelocityInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('DIRV') };

    NW_RES_CTOR_INHERIT( ResParticleDirectionalVelocityInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Power )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Direction )
};

struct ResParticleRandomDirectionalVelocityInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mPower;
    nw::ut::ResVec3 mDirection;
    nw::ut::ResF32 mAngle;
};

class ResParticleRandomDirectionalVelocityInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRandomDirectionalVelocityInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('RDRV') };

    NW_RES_CTOR_INHERIT( ResParticleRandomDirectionalVelocityInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Power )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Direction )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Angle )
};

struct ResParticleOriginVelocityInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mPower;
};

class ResParticleOriginVelocityInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleOriginVelocityInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('ORIV') };

    NW_RES_CTOR_INHERIT( ResParticleOriginVelocityInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Power )
};

struct ResParticleRandomVelocityInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mPower;
};

class ResParticleRandomVelocityInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRandomVelocityInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('RNDV') };

    NW_RES_CTOR_INHERIT( ResParticleRandomVelocityInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Power )
};

struct ResParticleYAxisVelocityInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mPower;
};

class ResParticleYAxisVelocityInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleYAxisVelocityInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('YAXV') };

    NW_RES_CTOR_INHERIT( ResParticleYAxisVelocityInitializer, ResParticleInitializer )

        NW_RES_FIELD_PRIMITIVE_DECL( f32, Power )
};

struct ResParticleFloatRandomInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mBaseValue;
    nw::ut::ResF32 mRandom;
};

class ResParticleFloatRandomInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatRandomInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('FRND') };

    NW_RES_CTOR_INHERIT( ResParticleFloatRandomInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, BaseValue )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Random )
};

struct ResParticleFloatRangeRandomInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mMaxValue;
    nw::ut::ResF32 mMinValue;
};

class ResParticleFloatRangeRandomInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatRangeRandomInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('URRA') };

    NW_RES_CTOR_INHERIT( ResParticleFloatRangeRandomInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, MaxValue )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, MinValue )
};

struct ResParticleFloatImmediateInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mImmediateValue;
};

class ResParticleFloatImmediateInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatImmediateInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SFIM') };

    NW_RES_CTOR_INHERIT( ResParticleFloatImmediateInitializer, ResParticleInitializer )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, ImmediateValue )
};

struct ResParticleVector2ImmediateInitializerData : public ResParticleInitializerData{
    nw::ut::VEC2 mImmediateValue;
};

class ResParticleVector2ImmediateInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector2ImmediateInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('V2IM') };

    NW_RES_CTOR_INHERIT( ResParticleVector2ImmediateInitializer, ResParticleInitializer )

    NW_RES_FIELD_VECTOR2_DECL( nw::math::VEC2, ImmediateValue )
};

struct ResParticleVector3ImmediateInitializerData : public ResParticleInitializerData{
    nw::ut::ResVec3 mImmediateValue;
};

class ResParticleVector3ImmediateInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3ImmediateInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('V3IM') };

    NW_RES_CTOR_INHERIT( ResParticleVector3ImmediateInitializer, ResParticleInitializer )

    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, ImmediateValue )
};

struct ResParticleVector3Random1InitializerData : public ResParticleInitializerData{
    nw::ut::ResVec3 mBaseValue;
    nw::ut::ResF32 mRandom;
};

class ResParticleVector3Random1Initializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Random1Initializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('V3R1') };

    NW_RES_CTOR_INHERIT( ResParticleVector3Random1Initializer, ResParticleInitializer )

    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, BaseValue )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Random )
};

struct ResParticleVector3Random3InitializerData : public ResParticleInitializerData{
    nw::ut::ResVec3 mBaseValue;
    nw::ut::ResVec3 mRandom;
};

class ResParticleVector3Random3Initializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Random3Initializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('V3R3') };

    NW_RES_CTOR_INHERIT( ResParticleVector3Random3Initializer, ResParticleInitializer )

    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, BaseValue )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Random )
};

struct ResParticleVector3MultRandomInitializerData : public ResParticleInitializerData{
    nw::ut::ResF32 mRandom;
};

class ResParticleVector3MultRandomInitializer : public ResParticleInitializer{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3MultRandomInitializer) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('V3MR') };

    NW_RES_CTOR_INHERIT(ResParticleVector3MultRandomInitializer, ResParticleInitializer)

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Random )
};

}
}
}