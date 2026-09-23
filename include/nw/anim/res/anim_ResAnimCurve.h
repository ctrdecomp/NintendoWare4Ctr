#pragma once

#include <nw/ut/ut_ResTypes.h>
#include <nw/math/math_Types.h>

namespace nw   {
namespace anim {
namespace res {

namespace internal {
    inline f32 Round(f32 value)
    {
        return nn::math::FFloor(value + 0.5f);
    }

    inline f32 CastS9_10ToF32(s32 value)
    {
        return f32(value) * (1.f / 1024.f);
    }

    inline s32 CastF32ToS9_10(f32 value)
    {
        return s32(Round(value * 1024.f));
    }

    inline f32 CastS7_8ToF32(s32 value)
    {
        return f32(value) * (1.f / 256.f);
    }

    inline s32 CastF32ToS7_8(f32 value)
    {
        return s32(Round(value * 256.f));
    }

    inline f32 CastS10_5ToF32(s32 value)
    {
        return f32(value) * (1.f / 32.f);
    }

    inline s32 CastF32ToS10_5( f32 value )
    {
        return s32(Round(value * 32.f));
    }

    inline f32 CastS6_5ToF32( s32 value )
    {
        return f32(value) * (1.f / 32.f);
    }

    inline s32 CastF32ToS6_5(f32 value)
    {
        return s32(Round(value * 32.f));
    }
}

struct ResFloatKeyFV64Data
{
    nw::ut::ResF32  m_Frame;
    nw::ut::ResF32  m_Value;
    
    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return m_Frame; }
    f32 GetValue()    const { return m_Value; }
};

struct ResFloatKeyFV32Data
{
    nw::ut::ResU32  m_FrameValue;

    u32 GetFrame()    const { return m_FrameValue & 0x00000FFF; }
    f32 GetFrameF32() const { return static_cast<f32>(this->GetFrame()); }
    f32 GetValue()    const { return static_cast<f32>(m_FrameValue >> 12); }
};

struct ResFloatKeyFVSS128Data
{
    nw::ut::ResF32 m_Frame;
    nw::ut::ResF32 m_Value;
    nw::ut::ResF32 m_InSlope;
    nw::ut::ResF32 m_OutSlope;
    
    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return m_Frame; }
    f32 GetValue()    const { return m_Value; }
    f32 GetInSlope()  const { return m_InSlope; }
    f32 GetOutSlope() const { return m_OutSlope; }
};

struct ResFloatKeyFVSS64Data
{
    nw::ut::ResU32 m_FrameValue;
    nw::ut::ResS16 m_InSlope;
    nw::ut::ResS16 m_OutSlope;

    u32 GetFrame()    const { return m_FrameValue & 0x00000FFF; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( m_FrameValue >> 12 ); }
    f32 GetInSlope()  const { return internal::CastS7_8ToF32(m_InSlope); }
    f32 GetOutSlope() const { return internal::CastS7_8ToF32(m_OutSlope); }
};

struct ResFloatKeyFVSS48Data
{
    nw::ut::ResU8  m_FrameValue[3];
    nw::ut::ResU8  m_InOutSlope[3];

    u32 GetFrame()    const { return m_FrameValue[0]; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( u16(m_FrameValue[1]) + (u16(m_FrameValue[2]) << 8) ); }
    f32 GetInSlope()  const { return internal::CastS6_5ToF32( s16(m_InOutSlope[0] + (s8(m_InOutSlope[1] << 4) << 4)) ); }
    f32 GetOutSlope() const { return internal::CastS6_5ToF32( s16((s8(m_InOutSlope[2]) << 4) + (m_InOutSlope[1] >> 4)) ); }
};

struct ResFloatKeyFVS96Data
{
    nw::ut::ResF32 m_Frame;
    nw::ut::ResF32 m_Value;
    nw::ut::ResF32 m_Slope;

    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return m_Value; }
    f32 GetSlope()    const { return m_Slope; }
};

struct ResFloatKeyFVS48Data
{
    nw::ut::ResU16 m_Frame;
    nw::ut::ResU16 m_Value;
    nw::ut::ResS16 m_Slope;

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return internal::CastS10_5ToF32( m_Frame ); }
    f32 GetValue()    const { return static_cast<f32>( m_Value ); }
    f32 GetSlope()    const { return internal::CastS7_8ToF32(m_Slope); }
};

struct ResFloatKeyFVS32Data
{
    nw::ut::ResU8  m_Frame;
    nw::ut::ResU8  m_ValueSlope[3];

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( u16(m_ValueSlope[0]) + (u16(m_ValueSlope[1] << 4) << 4) ); }
    f32 GetSlope()    const { return internal::CastS6_5ToF32((s8(m_ValueSlope[2]) << 4) + (m_ValueSlope[1] >> 4)); }
};

template <typename TKey>
struct ResKeysData
{
    TKey m_KeyValue[1];
};

template <typename TKey>
struct ResQuantizedKeysData
{
    nw::ut::ResF32  m_Scale;
    nw::ut::ResF32  m_Offset;
    nw::ut::ResF32  m_FrameScale;
    
    TKey    m_KeyValue[1];
};

struct ResFloatSegmentCVData
{
    union
    {
        ResKeysData<f32>          cv32;
        ResQuantizedKeysData<u16> cv16;
        ResQuantizedKeysData<u8>  cv8;
    };
};

struct ResFloatSegmentFVData
{
    nw::ut::ResU16 m_NumFrameValues;
    u8     m_Padding_[2];
    
    nw::ut::ResF32 m_InvDuration;
    
    union
    {
        ResKeysData<ResFloatKeyFVSS128Data>         fvss128;
        ResQuantizedKeysData<ResFloatKeyFVSS64Data> fvss64;
        ResQuantizedKeysData<ResFloatKeyFVSS48Data> fvss48;
        ResKeysData<ResFloatKeyFVS96Data>           fvs96;
        ResQuantizedKeysData<ResFloatKeyFVS48Data>  fvs48;
        ResQuantizedKeysData<ResFloatKeyFVS32Data>  fvs32;
        ResKeysData<ResFloatKeyFV64Data>            fv64;
        ResQuantizedKeysData<ResFloatKeyFV32Data>   fv32;
    };
};

struct ResFloatSegmentData
{
    enum Flag
    {
        FLAG_CONSTANT               = (0x1 << 0),
        FLAG_BAKED                  = (0x1 << 1),
        
        FLAG_INTERPORATE_MODE_SHIFT  = 2,
        FLAG_QUANTIZATION_TYPE_SHIFT = 5,
        
        FLAG_INTERPORATE_MODE_MASK  = (0x7 << FLAG_INTERPORATE_MODE_SHIFT),
        FLAG_QUANTIZATION_TYPE_MASK = (0x7 << FLAG_QUANTIZATION_TYPE_SHIFT)
    };
    
    enum InterporateMode
    {
        INTERPORATE_MODE_STEP    = 0,
        INTERPORATE_MODE_LINEAR  = 1,
        INTERPORATE_MODE_UNIFIED_HERMITE = 2,
        INTERPORATE_MODE_HERMITE = 3
    };
    
    enum QuantizeType
    {
        QUANTIZATION_TYPE_FVSS128 = 0,
        QUANTIZATION_TYPE_FVSS64  = 1,
        QUANTIZATION_TYPE_FVSS48  = 2,
        
        QUANTIZATION_TYPE_FVS96   = 3,
        QUANTIZATION_TYPE_FVS48   = 4,
        QUANTIZATION_TYPE_FVS32   = 5,
        
        QUANTIZATION_TYPE_FV64    = 6,
        QUANTIZATION_TYPE_FV32    = 7,
        
        QUANTIZATION_TYPE_CV32   = 0,
        QUANTIZATION_TYPE_CV16   = 1,
        QUANTIZATION_TYPE_CV8    = 2
    };
    
    nw::ut::ResF32  m_StartFrame;
    nw::ut::ResF32  m_EndFrame;
    nw::ut::ResU32  m_Flags;
    
    union
    {
        nw::ut::ResF32            constantValue;
        ResFloatSegmentFVData fv;
        ResFloatSegmentCVData cv;
    };
};

struct ResAnimCurveData
{
    enum RepeatMethod
    {
        METHOD_NONE,
        METHOD_REPEAT,
        METHOD_MIRROR,
        METHOD_NUM
    };
    
    nw::ut::ResF32   m_StartFrame;
    nw::ut::ResF32   m_EndFrame;
    nw::ut::ResU8    m_InRepeatMethod;
    nw::ut::ResU8    m_OutRepeatMethod;
    u8           m_Padding[2];
};

struct ResFloatCurveData : public ResAnimCurveData
{                     
    enum Flag
    {
        FLAG_COMPOSITE_CURVE         = (0x1 << 0),
        FLAG_SHIFT_MAX = 1
    };
    
    nw::ut::ResU32 m_Flags;
};

struct ResSegmentFloatCurveData : public ResFloatCurveData
{
    enum Flag
    {
        FLAG_CONSTANT       = (0x1 << (ResFloatCurveData::FLAG_SHIFT_MAX + 0)),
        FLAG_MONO_SEGMENT   = (0x1 << (ResFloatCurveData::FLAG_SHIFT_MAX + 1))
    };
    union
    {
        ut::ResF32 m_ConstantValue;
        struct 
        {
            ut::ResS32 m_NumSegments;
            ut::Offset toSegments[1];
        } segmentsTable;
    };
};

struct ResCompositeFloatCurveData : public ResFloatCurveData
{
    nw::ut::Offset   toLeftCurve;
    nw::ut::Offset   toRightCurve;
    nw::ut::Offset   toLeftBoolCurve;
    nw::ut::Offset   toRightBoolCurve;
    nw::ut::ResU8    m_CompositeMode;
    u8           padding[3];
};

struct ResIntKeyFV64Data
{
    nw::ut::ResF32 m_Frame;
    nw::ut::ResS32 m_Value;

    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s32 GetValue()    const { return m_Value; }
};

struct ResIntKeyFV32Data
{
    nw::ut::ResU16 m_Frame;
    nw::ut::ResS16 m_Value;

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s16 GetValue()    const { return m_Value; }
};

struct ResIntKeyFV16Data
{
    nw::ut::ResU8 m_Frame;
    nw::ut::ResS8 m_Value;

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s8  GetValue()    const { return m_Value; }
};

struct ResIntCurveFVData
{
    nw::ut::ResU16 m_NumFrameValues;
    u8     m_Padding_[2];
    
    nw::ut::ResF32 m_InvDuration;
    
    union
    {
        ResKeysData<ResIntKeyFV64Data> fv64;
        ResKeysData<ResIntKeyFV32Data> fv32;
        ResKeysData<ResIntKeyFV16Data> fv16;
    };
};

struct ResIntCurveCVData
{
    union
    {
        ResKeysData<nw::ut::ResS32>     cv32;
        ResKeysData<nw::ut::ResS16>     cv16;
        ResKeysData<nw::ut::ResS8>      cv8;
    };
};

struct ResIntCurveData : public ResAnimCurveData
{
    enum Flag
    {
        FLAG_CONSTANT       = (0x1 << 0),
        FLAG_BAKED          = (0x1 << 2),

        FLAG_QUANTIZATION_TYPE_SHIFT = 3,
        FLAG_QUANTIZATION_TYPE_MASK = (0x7 << FLAG_QUANTIZATION_TYPE_SHIFT)
    };
    
    enum QuantizeType
    {
        FLAG_QUANTIZATION_TYPE_FV64   = 0,
        FLAG_QUANTIZATION_TYPE_FV32   = 1,
        FLAG_QUANTIZATION_TYPE_FV16   = 2,
        
        FLAG_QUANTIZATION_TYPE_CV32   = 0,
        FLAG_QUANTIZATION_TYPE_CV16   = 1,
        FLAG_QUANTIZATION_TYPE_CV8    = 2
    };
    
    nw::ut::ResU32 m_Flags;
    
    union
    {
        nw::ut::ResS32                  constantValue;
        ResIntCurveFVData               fv;
        ResIntCurveCVData               cv;
    };
};

struct ResBoolCurveData : public ResAnimCurveData
{
    enum Flag
    {
        FLAG_CONSTANT       = (0x1 << 0),
        FLAG_CONSTANT_VALUE = (0x1 << 1),
        FLAG_BAKED          = (0x1 << 2),

        FLAG_QUANTIZATION_TYPE_SHIFT = 3,
        FLAG_QUANTIZATION_TYPE_MASK = (0x7 << FLAG_QUANTIZATION_TYPE_SHIFT)
    };
    
    enum QuantizeType
    {
        FLAG_QUANTIZATION_TYPE_FV64   = 0,
        FLAG_QUANTIZATION_TYPE_FV32   = 1,
        FLAG_QUANTIZATION_TYPE_FV16   = 2
    };
    
    nw::ut::ResU32 m_Flags;
    
    union
    {
        ResKeysData<nw::ut::ResU8>      cv;
        ResIntCurveFVData               fv;
    };
};

template <typename Type>
struct ResBakedCurveData : public ResAnimCurveData
{
    enum Flag
    {
        FLAG_CONSTANT = (0x1 << 0)
    };

    nw::ut::ResU32 m_Flags;

    struct FrameValue
    {
        Type cv;
        nw::ut::ResU32 flag;
    };
    ResKeysData<FrameValue> frames;
};

struct ResFullBakedCurveData : public ResAnimCurveData
{
    ResKeysData<nw::ut::ResMtx34> frames;
};

typedef ResBakedCurveData<nw::ut::ResVec3> ResVector3CurveData;
typedef ResBakedCurveData<nw::math::VEC4> ResVector4CurveData;

f32  CalcFloatCurve( const ResFloatCurveData* pCurve, f32 frame );
bool CalcBoolCurve( const ResBoolCurveData* pCurve, f32 frame );
s32  CalcIntCurve( const ResIntCurveData* pCurve, f32 frame );

void CalcVector3Curve( nw::math::VEC3* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame );

void CalcTranslateCurve( nw::math::MTX34* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame );
void CalcRotateCurve( nw::math::MTX34* result, bit32* flags, const ResVector4CurveData* pCurve, f32 frame );

void CalcTransformCurve( nw::math::MTX34* result, const ResFullBakedCurveData* pCurve, f32 frame );

}
}
}