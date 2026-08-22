#pragma once

#include <nw/ut/ut_ResTypes.h>
#include <nw/math/math_Types.h>

namespace nw   {
namespace anim {
namespace res {

namespace internal {
    inline f32 Round( f32 value ){
        return nn::math::FFloor(value + 0.5f);
    }

    inline f32 CastS9_10ToF32( s32 value ){
        return f32(value) * (1.f / 1024.f);
    }

    inline s32 CastF32ToS9_10( f32 value ){
        return s32(Round(value * 1024.f));
    }

    inline f32 CastS7_8ToF32( s32 value ){
        return f32(value) * (1.f / 256.f);
    }

    inline s32 CastF32ToS7_8( f32 value ){
        return s32(Round(value * 256.f));
    }

    inline f32 CastS10_5ToF32( s32 value ){
        return f32(value) * (1.f / 32.f);
    }

    inline s32 CastF32ToS10_5( f32 value ){
        return s32(Round(value * 32.f));
    }

    inline f32 CastS6_5ToF32( s32 value ){
        return f32(value) * (1.f / 32.f);
    }

    inline s32 CastF32ToS6_5(f32 value){
        return s32(Round(value * 32.f));
    }
}

struct ResFloatKeyFV64Data{
    nw::ut::ResF32  mFrame;
    nw::ut::ResF32  mValue;
    
    f32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return mFrame; }
    f32 GetValue()    const { return mValue; }
};

struct ResFloatKeyFV32Data{
    nw::ut::ResU32  mFrameValue;

    u32 GetFrame()    const { return mFrameValue & 0x00000FFF; }
    f32 GetFrameF32() const { return static_cast<f32>(this->GetFrame()); }
    f32 GetValue()    const { return static_cast<f32>(mFrameValue >> 12); }
};

struct ResFloatKeyFVSS128Data{
    nw::ut::ResF32 mFrame;
    nw::ut::ResF32 mValue;
    nw::ut::ResF32 mInSlope;
    nw::ut::ResF32 mOutSlope;
    
    f32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return mFrame; }
    f32 GetValue()    const { return mValue; }
    f32 GetInSlope()  const { return mInSlope; }
    f32 GetOutSlope() const { return mOutSlope; }
};

struct ResFloatKeyFVSS64Data{
    nw::ut::ResU32 mFrameValue;
    nw::ut::ResS16 mInSlope;
    nw::ut::ResS16 mOutSlope;

    u32 GetFrame()    const { return mFrameValue & 0x00000FFF; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( mFrameValue >> 12 ); }
    f32 GetInSlope()  const { return internal::CastS7_8ToF32(mInSlope); }
    f32 GetOutSlope() const { return internal::CastS7_8ToF32(mOutSlope); }
};

struct ResFloatKeyFVSS48Data{
    nw::ut::ResU8  mFrameValue[3];
    nw::ut::ResU8  mInOutSlope[3];

    u32 GetFrame()    const { return mFrameValue[0]; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( u16(mFrameValue[1]) + (u16(mFrameValue[2]) << 8) ); }
    f32 GetInSlope()  const { return internal::CastS6_5ToF32( s16(mInOutSlope[0] + (s8(mInOutSlope[1] << 4) << 4)) ); }
    f32 GetOutSlope() const { return internal::CastS6_5ToF32( s16((s8(mInOutSlope[2]) << 4) + (mInOutSlope[1] >> 4)) ); }
};

struct ResFloatKeyFVS96Data{
    nw::ut::ResF32 mFrame;
    nw::ut::ResF32 mValue;
    nw::ut::ResF32 mSlope;

    f32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return mValue; }
    f32 GetSlope()    const { return mSlope; }
};

struct ResFloatKeyFVS48Data{
    nw::ut::ResU16 mFrame;
    nw::ut::ResU16 mValue;
    nw::ut::ResS16 mSlope;

    u32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return internal::CastS10_5ToF32( mFrame ); }
    f32 GetValue()    const { return static_cast<f32>( mValue ); }
    f32 GetSlope()    const { return internal::CastS7_8ToF32(mSlope); }
};

struct ResFloatKeyFVS32Data{
    nw::ut::ResU8  mFrame;
    nw::ut::ResU8  mValueSlope[3];

    u32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    f32 GetValue()    const { return static_cast<f32>( u16(mValueSlope[0]) + (u16(mValueSlope[1] << 4) << 4) ); }
    f32 GetSlope()    const { return internal::CastS6_5ToF32((s8(mValueSlope[2]) << 4) + (mValueSlope[1] >> 4)); }
};

template <typename TKey>
struct ResKeysData{
    TKey mKeyValue[1];
};

template <typename TKey>
struct ResQuantizedKeysData{
    nw::ut::ResF32  mScale;
    nw::ut::ResF32  mOffset;
    nw::ut::ResF32  mFrameScale;
    
    TKey    mKeyValue[1];
};

struct ResFloatSegmentCVData{
    union{
        ResKeysData<f32>          cv32;
        ResQuantizedKeysData<u16> cv16;
        ResQuantizedKeysData<u8>  cv8;
    };
};

struct ResFloatSegmentFVData{
    nw::ut::ResU16 m_NumFrameValues;
    u8     mPadding_[2];
    
    nw::ut::ResF32 mInvDuration;
    
    union{
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

struct ResFloatSegmentData{
    enum Flag{
        FLAG_CONSTANT               = (0x1 << 0),
        FLAG_BAKED                  = (0x1 << 1),
        
        FLAG_INTERPORATE_MODE_SHIFT  = 2,
        FLAG_QUANTIZATION_TYPE_SHIFT = 5,
        
        FLAG_INTERPORATE_MODE_MASK  = (0x7 << FLAG_INTERPORATE_MODE_SHIFT),
        FLAG_QUANTIZATION_TYPE_MASK = (0x7 << FLAG_QUANTIZATION_TYPE_SHIFT)
    };
    
    enum InterporateMode{
        INTERPORATE_MODE_STEP    = 0,
        INTERPORATE_MODE_LINEAR  = 1,
        INTERPORATE_MODE_UNIFIED_HERMITE = 2,
        INTERPORATE_MODE_HERMITE = 3
    };
    
    enum QuantizeType{
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
    
    nw::ut::ResF32  mStartFrame;
    nw::ut::ResF32  mEndFrame;
    nw::ut::ResU32  mFlags;
    
    union{
        nw::ut::ResF32            constantValue;
        ResFloatSegmentFVData fv;
        ResFloatSegmentCVData cv;
    };
};

struct ResAnimCurveData{
    enum RepeatMethod{
        METHOD_NONE,
        METHOD_REPEAT,
        METHOD_MIRROR,
        METHOD_NUM
    };
    
    nw::ut::ResF32   mStartFrame;
    nw::ut::ResF32   mEndFrame;
    nw::ut::ResU8    mInRepeatMethod;
    nw::ut::ResU8    mOutRepeatMethod;
    u8           mPadding[2];
};

struct ResFloatCurveData : public ResAnimCurveData{                     
    enum Flag{
        FLAG_COMPOSITE_CURVE         = (0x1 << 0),
        FLAG_SHIFT_MAX = 1
    };
    
    nw::ut::ResU32 mFlags;
};

struct ResSegmentFloatCurveData : public ResFloatCurveData{
    enum Flag{
        FLAG_CONSTANT       = (0x1 << (ResFloatCurveData::FLAG_SHIFT_MAX + 0)),
        FLAG_MONO_SEGMENT   = (0x1 << (ResFloatCurveData::FLAG_SHIFT_MAX + 1))
    };

    union{
        nw::ut::ResF32 mConstantValue;
        struct{
            s32 mNumSegments;
            nw::ut::Offset toSegments[1];
        } segmentsTable;
    };
};

struct ResCompositeFloatCurveData : public ResFloatCurveData{
    nw::ut::Offset   toLeftCurve;
    nw::ut::Offset   toRightCurve;
    nw::ut::Offset   toLeftBoolCurve;
    nw::ut::Offset   toRightBoolCurve;
    nw::ut::ResU8    mCompositeMode;
    u8           padding[3];
};

struct ResIntKeyFV64Data{
    nw::ut::ResF32 mFrame;
    nw::ut::ResS32 mValue;

    f32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s32 GetValue()    const { return mValue; }
};

struct ResIntKeyFV32Data{
    nw::ut::ResU16 mFrame;
    nw::ut::ResS16 mValue;

    u32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s16 GetValue()    const { return mValue; }
};

struct ResIntKeyFV16Data{
    nw::ut::ResU8 mFrame;
    nw::ut::ResS8 mValue;

    u32 GetFrame()    const { return mFrame; }
    f32 GetFrameF32() const { return static_cast<f32>( this->GetFrame() ); }
    s8  GetValue()    const { return mValue; }
};

struct ResIntCurveFVData{
    nw::ut::ResU16 mNumFrameValues;
    u8     mPadding_[2];
    
    nw::ut::ResF32 mInvDuration;
    
    union{
        ResKeysData<ResIntKeyFV64Data> fv64;
        ResKeysData<ResIntKeyFV32Data> fv32;
        ResKeysData<ResIntKeyFV16Data> fv16;
    };
};

struct ResIntCurveCVData{
    union{
        ResKeysData<nw::ut::ResS32>     cv32;
        ResKeysData<nw::ut::ResS16>     cv16;
        ResKeysData<nw::ut::ResS8>      cv8;
    };
};

struct ResIntCurveData : public ResAnimCurveData{
    enum Flag{
        FLAG_CONSTANT       = (0x1 << 0),
        FLAG_BAKED          = (0x1 << 2),

        FLAG_QUANTIZATION_TYPE_SHIFT = 3,
        FLAG_QUANTIZATION_TYPE_MASK = (0x7 << FLAG_QUANTIZATION_TYPE_SHIFT)
    };
    
    enum QuantizeType{
        FLAG_QUANTIZATION_TYPE_FV64   = 0,
        FLAG_QUANTIZATION_TYPE_FV32   = 1,
        FLAG_QUANTIZATION_TYPE_FV16   = 2,
        
        FLAG_QUANTIZATION_TYPE_CV32   = 0,
        FLAG_QUANTIZATION_TYPE_CV16   = 1,
        FLAG_QUANTIZATION_TYPE_CV8    = 2
    };
    
    nw::ut::ResU32 mFlags;
    
    union{
        nw::ut::ResS32                  constantValue;
        ResIntCurveFVData               fv;
        ResIntCurveCVData               cv;
    };
};

struct ResBoolCurveData : public ResAnimCurveData{
    enum Flag{
        FLAG_CONSTANT       = (0x1 << 0),
        FLAG_CONSTANT_VALUE = (0x1 << 1),
        FLAG_BAKED          = (0x1 << 2),

        FLAG_QUANTIZATION_TYPE_SHIFT = 3,
        FLAG_QUANTIZATION_TYPE_MASK = (0x7 << FLAG_QUANTIZATION_TYPE_SHIFT)
    };
    
    enum QuantizeType{
        FLAG_QUANTIZATION_TYPE_FV64   = 0,
        FLAG_QUANTIZATION_TYPE_FV32   = 1,
        FLAG_QUANTIZATION_TYPE_FV16   = 2
    };
    
    nw::ut::ResU32 mFlags;
    
    union{
        ResKeysData<nw::ut::ResU8>      cv;
        ResIntCurveFVData               fv;
    };
};

template <typename Type>
struct ResBakedCurveData : public ResAnimCurveData{
    enum Flag{
        FLAG_CONSTANT = (0x1 << 0)
    };

    nw::ut::ResU32 mFlags;

    struct FrameValue{
        Type cv;
        nw::ut::ResU32 flag;
    };
    ResKeysData<FrameValue> frames;
};

struct ResFullBakedCurveData : public ResAnimCurveData{
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