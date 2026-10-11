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
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleUpdater) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('PUPD') 
    };

    NW_RES_CTOR(ResParticleUpdater)


    bool GetIsResourceCopyEnabled() const { return static_cast<bool>(ref().m_IsResourceCopyEnabled); }           
    void SetIsResourceCopyEnabled(bool value) { ref().m_IsResourceCopyEnabled = value; }

    bool GetUpdaterEnabled() const { return static_cast<bool>(ref().m_UpdaterEnabled); }           
    void SetUpdaterEnabled(bool value) { ref().m_UpdaterEnabled = value; }

    ParticleUsage GetTargetStream() const { return static_cast<ParticleUsage>(ref().m_TargetStream); }           
    void SetTargetStream(ParticleUsage value) { ref().m_TargetStream = value; }

    ResParticleAnimation GetParticleAnimation() { return ResParticleAnimation(ref().toParticleAnimation.to_ptr()); }                     
    const ResParticleAnimation GetParticleAnimation() const { return ResParticleAnimation(ref().toParticleAnimation.to_ptr()); }

    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }
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

    f32 GetFactor() const { return static_cast<f32>(ref().m_Factor); }           
    void SetFactor(f32 value) { ref().m_Factor = value; }
};

struct ResParticleFloatUpdaterData : public ResParticleUpdaterData
{
    nw::ut::ResF32 m_DefaultValue;
};

class ResParticleFloatUpdater : public ResParticleUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatUpdater) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('FLOA') 
    };

    NW_RES_CTOR_INHERIT(ResParticleFloatUpdater, ResParticleUpdater)


    f32 GetDefaultValue() const { return static_cast<f32>(ref().m_DefaultValue); }           
    void SetDefaultValue(f32 value) { ref().m_DefaultValue = value; }
};

struct ResParticleFloatImmediateUpdaterData : public ResParticleFloatUpdaterData
{
};

class ResParticleFloatImmediateUpdater : public ResParticleFloatUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatImmediateUpdater) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('FIMM') 
    };

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
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleFloatImmediate4KeyUpdater) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('FIM4') 
    };

    NW_RES_CTOR_INHERIT(ResParticleFloatImmediate4KeyUpdater, ResParticleUpdater)


    u32 GetInTime() const { return static_cast<u32>(ref().m_InTime); }           
    void SetInTime(u32 value) { ref().m_InTime = value; }

    u32 GetOutTime() const { return static_cast<u32>(ref().m_OutTime); }           
    void SetOutTime(u32 value) { ref().m_OutTime = value; }

    s32 GetValue0() const { return static_cast<s32>(ref().m_Value0); }           
    void SetValue0(s32 value) { ref().m_Value0 = value; }

    s32 GetValue1() const { return static_cast<s32>(ref().m_Value1); }           
    void SetValue1(s32 value) { ref().m_Value1 = value; }

    s32 GetValue2() const { return static_cast<s32>(ref().m_Value2); }           
    void SetValue2(s32 value) { ref().m_Value2 = value; }

    s32 GetValue3() const { return static_cast<s32>(ref().m_Value3); }           
    void SetValue3(s32 value) { ref().m_Value3 = value; }

    s32 GetValue4() const { return static_cast<s32>(ref().m_Value4); }           
    void SetValue4(s32 value) { ref().m_Value4 = value; }
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
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleVector3Immediate4KeyUpdater) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('V3I4') 
    };

    NW_RES_CTOR_INHERIT(ResParticleVector3Immediate4KeyUpdater, ResParticleUpdater)


    u32 GetInTime() const { return static_cast<u32>(ref().m_InTime); }           
    void SetInTime(u32 value) { ref().m_InTime = value; }

    u32 GetOutTime() const { return static_cast<u32>(ref().m_OutTime); }           
    void SetOutTime(u32 value) { ref().m_OutTime = value; }

    void SetValue0(f32 x, f32 y, f32 z) { ref().m_Value0.Set(x, y, z); }            
    const nw::math::VEC3& GetValue0() const { return ref().m_Value0; }                       
    void SetValue0(const nw::math::VEC3& value) { ref().m_Value0 = value; }

    void SetValue1(f32 x, f32 y, f32 z) { ref().m_Value1.Set(x, y, z); }            
    const nw::math::VEC3& GetValue1() const { return ref().m_Value1; }                       
    void SetValue1(const nw::math::VEC3& value) { ref().m_Value1 = value; }

    void SetValue2(f32 x, f32 y, f32 z) { ref().m_Value2.Set(x, y, z); }            
    const nw::math::VEC3& GetValue2() const { return ref().m_Value2; }                       
    void SetValue2(const nw::math::VEC3& value) { ref().m_Value2 = value; }

    void SetValue3(f32 x, f32 y, f32 z) { ref().m_Value3.Set(x, y, z); }            
    const nw::math::VEC3& GetValue3() const { return ref().m_Value3; }                       
    void SetValue3(const nw::math::VEC3& value) { ref().m_Value3 = value; }

    void SetValue4(f32 x, f32 y, f32 z) { ref().m_Value4.Set(x, y, z); }            
    const nw::math::VEC3& GetValue4() const { return ref().m_Value4; }                       
    void SetValue4(const nw::math::VEC3& value) { ref().m_Value4 = value; }
};

struct ResParticleGeneralUpdaterData : public ResParticleUpdaterData
{
};

class ResParticleGeneralUpdater : public ResParticleUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleGeneralUpdater) 
    };
    enum
    { 
        SIGNATURE = NW_RES_SIGNATURE32('GENE') 
    };

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


    f32 GetPower() const { return static_cast<f32>(ref().m_Power); }           
    void SetPower(f32 value) { ref().m_Power = value; }

    void SetDirection(f32 x, f32 y, f32 z) { ref().m_Direction.Set(x, y, z); }            
    const nw::math::VEC3& GetDirection() const { return ref().m_Direction; }                       
    void SetDirection(const nw::math::VEC3& value) { ref().m_Direction = value; }
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


    f32 GetPower() const { return static_cast<f32>(ref().m_Power); }           
    void SetPower(f32 value) { ref().m_Power = value; }

    void SetAxis(f32 x, f32 y, f32 z) { ref().m_Axis.Set(x, y, z); }            
    const nw::math::VEC3& GetAxis() const { return ref().m_Axis; }                       
    void SetAxis(const nw::math::VEC3& value) { ref().m_Axis = value; }
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


    void SetPower(f32 x, f32 y, f32 z) { ref().m_Power.Set(x, y, z); }            
    const nw::math::VEC3& GetPower() const { return ref().m_Power; }                       
    void SetPower(const nw::math::VEC3& value) { ref().m_Power = value; }

    s32 GetInterval() const { return static_cast<s32>(ref().m_Interval); }           
    void SetInterval(s32 value) { ref().m_Interval = value; }
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


    void SetDefaultValue(f32 x, f32 y) { ref().m_DefaultValue.Set(x, y); }                      
    const nw::math::VEC2& GetDefaultValue() const { return ref().m_DefaultValue; }                       
    void SetDefaultValue(const nw::math::VEC2& value) { ref().m_DefaultValue = value; }
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


    void SetDefaultValue(f32 x, f32 y, f32 z) { ref().m_DefaultValue.Set(x, y, z); }            
    const nw::math::VEC3& GetDefaultValue() const { return ref().m_DefaultValue; }                       
    void SetDefaultValue(const nw::math::VEC3& value) { ref().m_DefaultValue = value; }
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


    ParticleRotateUpVectorSource GetSource() const { return static_cast<ParticleRotateUpVectorSource>(ref().m_Source); }           
    void SetSource(ParticleRotateUpVectorSource value) { ref().m_Source = value; }
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


    ParticleTexturePatternUpdaterDivisionType GetDivisionX() const { return static_cast<ParticleTexturePatternUpdaterDivisionType>(ref().m_DivisionX); }           
    void SetDivisionX(ParticleTexturePatternUpdaterDivisionType value) { ref().m_DivisionX = value; }

    ParticleTexturePatternUpdaterDivisionType GetDivisionY() const { return static_cast<ParticleTexturePatternUpdaterDivisionType>(ref().m_DivisionY); }           
    void SetDivisionY(ParticleTexturePatternUpdaterDivisionType value) { ref().m_DivisionY = value; }

    u32 GetTextureIndex() const { return static_cast<u32>(ref().m_TextureIndex); }           
    void SetTextureIndex(u32 value) { ref().m_TextureIndex = value; }
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


    f32 GetStart() const { return static_cast<f32>(ref().m_Start); }           
    void SetStart(f32 value) { ref().m_Start = value; }

    f32 GetEnd() const { return static_cast<f32>(ref().m_End); }           
    void SetEnd(f32 value) { ref().m_End = value; }

    s32 GetInterval() const { return static_cast<s32>(ref().m_Interval); }           
    void SetInterval(s32 value) { ref().m_Interval = value; }
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


    s32 GetStart() const { return static_cast<s32>(ref().m_Start); }           
    void SetStart(s32 value) { ref().m_Start = value; }

    s32 GetEnd() const { return static_cast<s32>(ref().m_End); }           
    void SetEnd(s32 value) { ref().m_End = value; }

    s32 GetInterval() const { return static_cast<s32>(ref().m_Interval); }           
    void SetInterval(s32 value) { ref().m_Interval = value; }
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


    const char* GetParticleSetPath() const { return ref().toParticleSetPath.to_ptr(); }

    ResParticleForm GetParticleForm() { return ResParticleForm(ref().toParticleForm.to_ptr()); }                     
    const ResParticleForm GetParticleForm() const { return ResParticleForm(ref().toParticleForm.to_ptr()); }

    ResParticleChildUpdaterOption GetTiming() { return ResParticleChildUpdaterOption(ref().toTiming.to_ptr()); }                     
    const ResParticleChildUpdaterOption GetTiming() const { return ResParticleChildUpdaterOption(ref().toTiming.to_ptr()); }

    s32 GetEmissionRatio() const { return static_cast<s32>(ref().m_EmissionRatio); }           
    void SetEmissionRatio(s32 value) { ref().m_EmissionRatio = value; }
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


    u32 GetUserParameter() const { return static_cast<u32>(ref().m_UserParameter); }           
    void SetUserParameter(u32 value) { ref().m_UserParameter = value; }
};

}
}
}

