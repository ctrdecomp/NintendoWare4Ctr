#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_ParticleTime.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResParticleAnimation.h>
#include <nw/gfx/res/gfx_ResParticleAnimationOption.h>
#include <nw/gfx/res/gfx_ResParticleForm.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {

class ParticleContext;
class ParticleSet;

namespace res{

enum ParticleUsage
{
    PARTICLEUSAGE_INVALID = -1,

    PARTICLEUSAGE_TRANSLATE = 0,
    PARTICLEUSAGE_SCALE,
    PARTICLEUSAGE_ROTATE,
    PARTICLEUSAGE_COLOR,
    PARTICLEUSAGE_ALPHA,
    PARTICLEUSAGE_TEXTURETRANSLATE0,
    PARTICLEUSAGE_TEXTURESCALE0,
    PARTICLEUSAGE_TEXTUREROTATE0,
    PARTICLEUSAGE_SCALE_EXT,

    PARTICLEUSAGE_ID,
    PARTICLEUSAGE_BIRTH,
    PARTICLEUSAGE_LIFE,
    PARTICLEUSAGE_VELOCITY,

    PARTICLEUSAGE_ACTIVEINDEX,
    PARTICLEUSAGE_FREEINDEX,

    PARTICLEUSAGE_NEG_TIMELIMIT,

    PARTICLEUSAGE_COUNT,

    PARTICLEUSAGE_IS_INT = INT_MAX
};

struct ResParticleUpdaterData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResBool m_IsResourceCopyEnabled;
    nw::ut::ResBool m_UpdaterEnabled;
    u8              padding_0[2];
    nw::ut::ResS32 m_TargetStream;
    nw::ut::Offset toParticleAnimation;
};

class ResParticleUpdater : public nw::ut::ResCommon< ResParticleUpdaterData >
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('PUPD') };

    NW_RES_CTOR(ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(bool, IsResourceCopyEnabled)
    NW_RES_FIELD_PRIMITIVE_DECL(bool, UpdaterEnabled)
    NW_RES_FIELD_PRIMITIVE_DECL(ParticleUsage, TargetStream)
    NW_RES_FIELD_CLASS_DECL(ResParticleAnimation, ParticleAnimation)

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

struct ResParticleAccelarationUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResF32 m_Factor;
};

class ResParticleAccelarationUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleAccelarationUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('ACCE') };

    NW_RES_CTOR_INHERIT(ResParticleAccelarationUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(f32, Factor)
};

struct ResParticleFloatUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResF32 m_DefaultValue;
};

class ResParticleFloatUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('FLOA') };

    NW_RES_CTOR_INHERIT(ResParticleFloatUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(f32, DefaultValue)
};

struct ResParticleFloatImmediateUpdaterData : public ResParticleFloatUpdaterData
{
};

class ResParticleFloatImmediateUpdater : public ResParticleFloatUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatImmediateUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('FIMM') };

    NW_RES_CTOR_INHERIT(ResParticleFloatImmediateUpdater, ResParticleFloatUpdater)
};

struct ResParticleFloatImmediate4KeyUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResU32 m_InTime;
    nw::ut::ResU32 m_OutTime;
    nw::ut::ResS32 m_Value0;
    nw::ut::ResS32 m_Value1;
    nw::ut::ResS32 m_Value2;
    nw::ut::ResS32 m_Value3;
    nw::ut::ResS32 m_Value4;
};

class ResParticleFloatImmediate4KeyUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatImmediate4KeyUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('FIM4') };

    NW_RES_CTOR_INHERIT( ResParticleFloatImmediate4KeyUpdater, ResParticleUpdater )

    NW_RES_FIELD_PRIMITIVE_DECL(u32, InTime)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, OutTime)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Value0)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Value1)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Value2)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Value3)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Value4)
};

struct ResParticleVector3Immediate4KeyUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResU32 m_InTime;
    nw::ut::ResU32 m_OutTime;
    nw::ut::ResVec3 m_Value0;
    nw::ut::ResVec3 m_Value1;
    nw::ut::ResVec3 m_Value2;
    nw::ut::ResVec3 m_Value3;
    nw::ut::ResVec3 m_Value4;
};

class ResParticleVector3Immediate4KeyUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Immediate4KeyUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3I4') };

    NW_RES_CTOR_INHERIT(ResParticleVector3Immediate4KeyUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(u32, InTime)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, OutTime)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Value0)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Value1)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Value2)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Value3)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Value4)
};

struct ResParticleGeneralUpdaterData : public ResParticleUpdaterData
{
};

class ResParticleGeneralUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleGeneralUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('GENE') };

    NW_RES_CTOR_INHERIT(ResParticleGeneralUpdater, ResParticleUpdater)
};

struct ResParticleGravityUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResF32 m_Power;
    nw::ut::ResVec3 m_Direction;
};

class ResParticleGravityUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleGravityUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('GRAV') };

    NW_RES_CTOR_INHERIT(ResParticleGravityUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(f32, Power)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Direction)
};

struct ResParticleSpinUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResF32 m_Power;
    nw::ut::ResVec3 m_Axis;
};

class ResParticleSpinUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleSpinUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('SPIN') };

    NW_RES_CTOR_INHERIT(ResParticleSpinUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(f32, Power)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Axis)
};

struct ResParticleRandomUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResVec3 m_Power;
    nw::ut::ResS32 m_Interval;
};

class ResParticleRandomUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRandomUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('RNDU') };

    NW_RES_CTOR_INHERIT( ResParticleRandomUpdater, ResParticleUpdater )

    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Power)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Interval)
};

struct ResParticleVector2ImmediateUpdaterData : public ResParticleUpdaterData
{
    nw::math::VEC2 m_DefaultValue;
};

class ResParticleVector2ImmediateUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector2ImmediateUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V2IM') };

    NW_RES_CTOR_INHERIT(ResParticleVector2ImmediateUpdater, ResParticleUpdater)

    NW_RES_FIELD_VECTOR2_DECL(nw::math::VEC2, DefaultValue)
};

struct ResParticleVector3UpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResVec3 m_DefaultValue;
};

class ResParticleVector3Updater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Updater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('VEC3') };

    NW_RES_CTOR_INHERIT(ResParticleVector3Updater, ResParticleUpdater)

    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, DefaultValue)
};

struct ResParticleVector3AdditiveUpdaterData : public ResParticleVector3UpdaterData
{
};

class ResParticleVector3AdditiveUpdater : public ResParticleVector3Updater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3AdditiveUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3AD') };

    NW_RES_CTOR_INHERIT( ResParticleVector3AdditiveUpdater, ResParticleVector3Updater )
};

struct ResParticleVector3ImmediateUpdaterData : public ResParticleVector3UpdaterData
{
};

class ResParticleVector3ImmediateUpdater : public ResParticleVector3Updater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3ImmediateUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3IM') };

    NW_RES_CTOR_INHERIT(ResParticleVector3ImmediateUpdater, ResParticleVector3Updater)
};

struct ResParticleVector3RandomAdditiveUpdaterData : public ResParticleVector3UpdaterData
{
};

class ResParticleVector3RandomAdditiveUpdater : public ResParticleVector3Updater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3RandomAdditiveUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('V3RA') };

    NW_RES_CTOR_INHERIT( ResParticleVector3RandomAdditiveUpdater, ResParticleVector3Updater )
};

struct ResParticleRotateUpVectorUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResU32 m_Source;
};

class ResParticleRotateUpVectorUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleRotateUpVectorUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('RUVU') };

    enum ParticleRotateUpVectorSource
    {
        SOURCE_VELOCITY = 0,
        SOURCE_DISTANCE
    };

    NW_RES_CTOR_INHERIT(ResParticleRotateUpVectorUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(ParticleRotateUpVectorSource, Source)
};

enum ParticleTexturePatternUpdaterDivisionType
{
    Division1 = 1,
    Division2 = 2,
    Division4 = 4,
    Division8 = 8,
    Division16 = 16
};

struct ResParticleTexturePatternUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResU32 m_DivisionX;
    nw::ut::ResU32 m_DivisionY;
    nw::ut::ResS32 m_TextureIndex;
};

class ResParticleTexturePatternUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleTexturePatternUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('TPAT') };

    NW_RES_CTOR_INHERIT(ResParticleTexturePatternUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(ParticleTexturePatternUpdaterDivisionType, DivisionX)
    NW_RES_FIELD_PRIMITIVE_DECL(ParticleTexturePatternUpdaterDivisionType, DivisionY)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, TextureIndex)
};

struct ResParticleChildUpdaterOptionData
{
    nw::ut::ResTypeInfo typeInfo;
};

class ResParticleChildUpdaterOption : public nw::ut::ResCommon< ResParticleChildUpdaterOptionData >
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleChildUpdaterOption) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('COPT') };

    NW_RES_CTOR( ResParticleChildUpdaterOption )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }

#ifdef NW_GFX_PARTICLE_COMPAT_1_1
    bool CheckTiming(ParticleTime birth, ParticleTime life, ParticleTime limit, ParticleTime prevTime, ParticleTime currentTime);
#else
    bool CheckTiming(ParticleTime birth, ParticleTime life, ParticleTime prevTime, ParticleTime currentTime);
#endif
};

struct ResParticleChildUpdaterFirstUpdateOptionData : public ResParticleChildUpdaterOptionData
{
};

class ResParticleChildUpdaterFirstUpdateOption : public ResParticleChildUpdaterOption
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleChildUpdaterFirstUpdateOption) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('COBU') };

    NW_RES_CTOR_INHERIT(ResParticleChildUpdaterFirstUpdateOption, ResParticleChildUpdaterOption)
};

struct ResParticleChildUpdaterFinalUpdateOptionData : public ResParticleChildUpdaterOptionData
{
};

class ResParticleChildUpdaterFinalUpdateOption : public ResParticleChildUpdaterOption
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleChildUpdaterFinalUpdateOption) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('COFU') };

    NW_RES_CTOR_INHERIT(ResParticleChildUpdaterFinalUpdateOption, ResParticleChildUpdaterOption)
};

struct ResParticleChildUpdaterIntervalOptionData : public ResParticleChildUpdaterOptionData
{
    nw::ut::ResF32 m_Start;
    nw::ut::ResF32 m_End;
    nw::ut::ResS32 m_Interval;
};

class ResParticleChildUpdaterIntervalOption : public ResParticleChildUpdaterOption
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleChildUpdaterIntervalOption) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('COIN') };

    NW_RES_CTOR_INHERIT(ResParticleChildUpdaterIntervalOption, ResParticleChildUpdaterOption)

    NW_RES_FIELD_PRIMITIVE_DECL(f32, Start)
    NW_RES_FIELD_PRIMITIVE_DECL(f32, End)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Interval)
};

struct ResParticleChildUpdaterFrameOptionData : public ResParticleChildUpdaterOptionData
{
    nw::ut::ResS32 m_Start;
    nw::ut::ResS32 m_End;
    nw::ut::ResS32 m_Interval;
};

class ResParticleChildUpdaterFrameOption : public ResParticleChildUpdaterOption
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleChildUpdaterFrameOption) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('COFN') };

    NW_RES_CTOR_INHERIT(ResParticleChildUpdaterFrameOption, ResParticleChildUpdaterOption)

    NW_RES_FIELD_PRIMITIVE_DECL(s32, Start)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, End)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Interval)
};

struct ResParticleChildUpdaterData : public ResParticleUpdaterData
{
    nw::ut::BinString toParticleSetPath;
    nw::ut::Offset toParticleForm;
    nw::ut::Offset toTiming;
    nw::ut::ResS32 m_EmissionRatio;
};

class ResParticleChildUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleChildUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('CHLD') };

    NW_RES_CTOR_INHERIT(ResParticleChildUpdater, ResParticleUpdater)

    NW_RES_FIELD_STRING_DECL(ParticleSetPath)
    NW_RES_FIELD_CLASS_DECL(ResParticleForm, ParticleForm)
    NW_RES_FIELD_CLASS_DECL(ResParticleChildUpdaterOption, Timing)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, EmissionRatio)
};

struct ResParticleUserUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResU32 m_UserParameter;
};

class ResParticleUserUpdater : public ResParticleUpdater
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleUserUpdater) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('USER') };

    typedef void (*UserFunctionType)(ParticleContext*,ParticleSet*,const ResParticleUserUpdater*,f32,f32);

    NW_RES_CTOR_INHERIT(ResParticleUserUpdater, ResParticleUpdater)

    NW_RES_FIELD_PRIMITIVE_DECL(u32, UserParameter)
};

}
}
}