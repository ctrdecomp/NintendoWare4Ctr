/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_ANIM_RESANIMATIONCURVE_H_
#define NW_ANIM_RESANIMATIONCURVE_H_

#include <nw/types.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
namespace nw {
namespace anim {
namespace res {

namespace internal {
    NW_INLINE f32
    Round( f32 value )
    {
        return nn::math::FFloor(value + 0.5f);
    }

    NW_INLINE f32
    CastS9_10ToF32( s32 value )
    {
        return f32(value) * (1.f / 1024.f);
    }

    NW_INLINE s32
    CastF32ToS9_10( f32 value )
    {
        return s32(Round(value * 1024.f));
    }

    NW_INLINE f32
    CastS7_8ToF32( s32 value )
    {
        return f32(value) * (1.f / 256.f);
    }

    NW_INLINE s32
    CastF32ToS7_8( f32 value )
    {
        return s32(Round(value * 256.f));
    }

    NW_INLINE f32
    CastS10_5ToF32( s32 value )
    {
        return f32(value) * (1.f / 32.f);
    }

    NW_INLINE s32
    CastF32ToS10_5( f32 value )
    {
        return s32(Round(value * 32.f));
    }

    NW_INLINE f32
    CastS6_5ToF32( s32 value )
    {
        return f32(value) * (1.f / 32.f);
    }

    NW_INLINE s32
    CastF32ToS6_5( f32 value )
    {
        return s32(Round(value * 32.f));
    }
} /* namespace internal */

// TODO: The quantized number of bits are provisional.

//
struct ResFloatKeyFV64Data
{
    ut::ResF32  m_Frame;
    ut::ResF32  m_Value;

    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return m_Frame; }
    f32 GetValue()    const { return m_Value; }
};

//
struct ResFloatKeyFV32Data
{
    ut::ResU32  m_FrameValue; // u12 m_Frame; u20 m_Value * scale + offset;

    u32 GetFrame()    const { return m_FrameValue & 0x00000FFF; }
    f32 GetFrameF32() const { return static_cast<f32>(this->GetFrame()); }
    f32 GetValue()    const { return static_cast<f32>(m_FrameValue >> 12); }
};

//
struct ResFloatKeyFVSS128Data
{
    ut::ResF32 m_Frame;
    ut::ResF32 m_Value;
    ut::ResF32 m_InSlope;
    ut::ResF32 m_OutSlope;

    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return m_Frame; }
    f32 GetValue()    const { return m_Value; }
    f32 GetInSlope()  const { return m_InSlope; }
    f32 GetOutSlope() const { return m_OutSlope; }
};

//
struct ResFloatKeyFVSS64Data
{
    ut::ResU32 m_FrameValue; // u12 m_Frame; u20 m_Value * scale + offset;
    ut::ResS16 m_InSlope;    // fx7.8
    ut::ResS16 m_OutSlope;   // fx7.8

    u32 GetFrame()    const { return m_FrameValue & 0x00000FFF; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( m_FrameValue >> 12 ); }
    f32 GetInSlope()  const { return internal::CastS7_8ToF32(m_InSlope); }
    f32 GetOutSlope() const { return internal::CastS7_8ToF32(m_OutSlope); }
};

//
struct ResFloatKeyFVSS48Data
{
    ut::ResU8  m_FrameValue[3]; // u8 m_Frame; u16 m_Value * scale + offset;
    ut::ResU8  m_InOutSlope[3]; // fx6.5 m_InSlope; fx6.5 m_OutSlope;

    u32 GetFrame()    const { return m_FrameValue[0]; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( u16(m_FrameValue[1]) + (u16(m_FrameValue[2]) << 8) ); }
    f32 GetInSlope()  const { return internal::CastS6_5ToF32( s16(m_InOutSlope[0] + (s8(m_InOutSlope[1] << 4) << 4)) ); }
    f32 GetOutSlope() const { return internal::CastS6_5ToF32( s16((s8(m_InOutSlope[2]) << 4) + (m_InOutSlope[1] >> 4)) ); }
};

//
struct ResFloatKeyFVS96Data
{
    ut::ResF32 m_Frame;
    ut::ResF32 m_Value;
    ut::ResF32 m_Slope;

    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return m_Value; }
    f32 GetSlope()    const { return m_Slope; }
};

//
struct ResFloatKeyFVS48Data
{
    ut::ResU16 m_Frame; // fx10.5
    ut::ResU16 m_Value; // m_Value * scale + offset
    ut::ResS16 m_Slope; // fx7.8

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return internal::CastS10_5ToF32( m_Frame ); }
    f32 GetValue()    const { return static_cast<f32>( m_Value ); }
    f32 GetSlope()    const { return internal::CastS7_8ToF32(m_Slope); }
};

//
struct ResFloatKeyFVS32Data
{
    ut::ResU8  m_Frame;         // u8
    ut::ResU8  m_ValueSlope[3]; // u12 m_Value * scale + offset; fx6.5 m_Slope;

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( u16(m_ValueSlope[0]) + (u16(m_ValueSlope[1] << 4) << 4) ); }
    f32 GetSlope()    const { return internal::CastS6_5ToF32((s8(m_ValueSlope[2]) << 4) + (m_ValueSlope[1] >> 4)); }
};

//
template <typename TKey>
struct ResKeysData
{
    TKey m_KeyValue[1];  // When made a variable-length array, the compiler fails, so the size is made 1.
};

//
template <typename TKey>
struct ResQuantizedKeysData
{
    ut::ResF32  m_Scale; // Apply to Value
    ut::ResF32  m_Offset;
    ut::ResF32  m_FrameScale; // Apply to Frame

    TKey    m_KeyValue[1];
};

//
struct ResFloatSegmentCVData
{
    union
    {
        ResKeysData<f32>          cv32;
        ResQuantizedKeysData<u16> cv16;
        ResQuantizedKeysData<u8>  cv8;
    };
};

//
struct ResFloatSegmentFVData
{
    ut::ResU16 m_NumFrameValues;
    u8     m_Padding_[2];

    ut::ResF32 m_InvDuration;

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

//
struct ResFloatSegmentData
{
    // Store the characteristics of the segment curve as a flag.
    // FLAG_CONSTANT : When this bit is 1, this segment's animation result is handled as a constant.
    // 
    //                 At this time, the FLAG_BAKED, FLAG_QUANTIZED, FLAG_INTERPORATE_MODE_MASK, and FLAG_QUANTIZED_TYPE_MASK settings become invalid.
    // 
    //
    // FLAG_BAKED    : Animation data is handled in frame format.
    //                 When in frame format, the FLAG_INTERPORATE_MODE_MASK value is ignored.
    //
    // FLAG_INTERPORATE_MODE_MASK : Sets the interpolation method between key frames.
    //
    // FLAG_QUANTIZATION_TYPE_MASK    : Sets the quantization method.
    //
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

    ut::ResF32  m_StartFrame;
    ut::ResF32  m_EndFrame;
    ut::ResU32  m_Flags;

    union
    {
        ut::ResF32            constantValue;
        ResFloatSegmentFVData fv;
        ResFloatSegmentCVData cv;
    };
};

//
struct ResAnimCurveData
{
    enum RepeatMethod
    {
        METHOD_NONE,
        METHOD_REPEAT,
        METHOD_MIRROR,
        METHOD_NUM
    };

    ut::ResF32   m_StartFrame;
    ut::ResF32   m_EndFrame;
    ut::ResU8    m_InRepeatMethod;
    ut::ResU8    m_OutRepeatMethod;
    u8           m_Padding[2];
};

//
struct ResFloatCurveData : public ResAnimCurveData
{
    // Store the characteristics of the segment float curve as a flag.
    // FLAG_COMPOSITE_CURVE : Indicates that this curve is a composite curve.
    //
    enum Flag
    {
        FLAG_COMPOSITE_CURVE         = (0x1 << 0),
        FLAG_SHIFT_MAX = 1
    };

    ut::ResU32 m_Flags;
};

//
struct ResSegmentFloatCurveData : public ResFloatCurveData
{
    // Store the characteristics of the segment float curve as a flag.
    // FLAG_CONSTANT : When this bit is 1, this curve's animation result is handled as a constant.
    // 
    // FLAG_MONO_SEGMENT : When this bit is 1, handles the curve as composed of a single segment.
    // 
    //
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


//
struct ResCompositeFloatCurveData : public ResFloatCurveData
{
    ut::Offset   toLeftCurve;
    ut::Offset   toRightCurve;
    ut::Offset   toLeftBoolCurve;
    ut::Offset   toRightBoolCurve;
    ut::ResU8    m_CompositeMode;
    u8           padding[3];
};



//
struct ResIntKeyFV64Data
{
    ut::ResF32 m_Frame;
    ut::ResS32 m_Value;

    f32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s32 GetValue()    const { return m_Value; }
};

//
struct ResIntKeyFV32Data
{
    ut::ResU16 m_Frame;
    ut::ResS16 m_Value;

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s16 GetValue()    const { return m_Value; }
};

//
struct ResIntKeyFV16Data
{
    ut::ResU8 m_Frame;
    ut::ResS8 m_Value;

    u32 GetFrame()    const { return m_Frame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s8  GetValue()    const { return m_Value; }
};

//
struct ResIntCurveFVData
{
    ut::ResU16 m_NumFrameValues;
    u8     m_Padding_[2];

    ut::ResF32 m_InvDuration;

    union
    {
        ResKeysData<ResIntKeyFV64Data> fv64;
        ResKeysData<ResIntKeyFV32Data> fv32;
        ResKeysData<ResIntKeyFV16Data> fv16;
    };
};

//
struct ResIntCurveCVData
{
    union
    {
        ResKeysData<ut::ResS32>     cv32;
        ResKeysData<ut::ResS16>     cv16;
        ResKeysData<ut::ResS8>      cv8;
    };
};


//
struct ResIntCurveData : public ResAnimCurveData
{
    // Store the characteristics of the Int curve as a flag.
    // FLAG_CONSTANT : When this bit is 1, the animation result is handled as a constant.
    //                 At this time, FLAG_BAKED is ignored.
    //
    // FLAG_BAKED    : Animation data is handled in frame format.
    //
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

    ut::ResU32 m_Flags;

    union
    {
        ut::ResS32                  constantValue;
        ResIntCurveFVData           fv;
        ResIntCurveCVData           cv;
    };
};


//
struct ResBoolCurveData : public ResAnimCurveData
{
    // Store the characteristics of the Bool curve as a flag.
    // FLAG_CONSTANT : When this bit is 1, FLAG_CONSTANT_VALUE is handled as a constant value result.
    // 
    //                 At this time, FLAG_BAKED is ignored.
    //
    // FLAG_BAKED    : Animation data is handled in frame format.
    //                 For frame format, Boolean values for 8 frames are stored in 1 byte.
    // 
    //
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

    ut::ResU32 m_Flags;

    union
    {
        ResKeysData<ut::ResU8>      cv;
        ResIntCurveFVData           fv;
    };
};

//
//
template <typename Type>
struct ResBakedCurveData : public ResAnimCurveData
{
    // Store the characteristics of the curve as a flag.
    // FLAG_CONSTANT : When this bit is 1, the animation result is handled as a constant.
    enum Flag
    {
        FLAG_CONSTANT = (0x1 << 0)
    };

    // TODO: Support for quantization
    ut::ResU32 m_Flags;

    struct FrameValue
    {
        Type cv;
        ut::ResU32 flag;
    };
    ResKeysData<FrameValue> frames;
};

//
//
struct ResFullBakedCurveData : public ResAnimCurveData
{
    ResKeysData<ut::ResMtx34_> frames;
};

// Define type names for baked animations.
typedef ResBakedCurveData<ut::ResVec3_> ResVector3CurveData; //
typedef ResBakedCurveData<ut::ResVec4> ResVector4CurveData; //

f32  CalcFloatCurve( const ResFloatCurveData* pCurve, f32 frame ); //
bool CalcBoolCurve( const ResBoolCurveData* pCurve, f32 frame ); //
s32  CalcIntCurve( const ResIntCurveData* pCurve, f32 frame ); //

void CalcVector3Curve( math::VEC3* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame ); //

void CalcTranslateCurve( math::MTX34* result, bit32* flags, const ResVector3CurveData* pCurve, f32 frame ); //
void CalcRotateCurve( math::MTX34* result, bit32* flags, const ResVector4CurveData* pCurve, f32 frame ); //

void CalcTransformCurve( math::MTX34* result, const ResFullBakedCurveData* pCurve, f32 frame ); //

} /* namespace res */
} /* namespace anim */
} /* namespace nw */
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif
#endif

#endif /* NW_ANIM_RESANIMATIONCURVE_H_ */
