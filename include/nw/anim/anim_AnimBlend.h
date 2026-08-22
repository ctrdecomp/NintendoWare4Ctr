#pragma once

#include <nw/types.h>
#include <nw/ut/ut_ResArray.h>
#include <nw/anim/anim_AnimResult.h>
#include <nw/ut/ut_Flag.h>

namespace nw  {
namespace anim {

class AnimBlendOp{
public:
    enum Flags{
        VALID_SINGLE = AnimResult::FLAG_VALID_COMPONENT0,

        VALID_X = AnimResult::FLAG_VALID_COMPONENT0,
        VALID_Y = AnimResult::FLAG_VALID_COMPONENT1,
        VALID_Z = AnimResult::FLAG_VALID_COMPONENT2,
        VALID_W = AnimResult::FLAG_VALID_COMPONENT3,
        VALID_XY   = VALID_X | VALID_Y,
        VALID_XYZ  = VALID_X | VALID_Y | VALID_Z,
        VALID_XYZW = VALID_X | VALID_Y | VALID_Z | VALID_W,

        VALID_R = AnimResult::FLAG_VALID_COMPONENT0,
        VALID_G = AnimResult::FLAG_VALID_COMPONENT1,
        VALID_B = AnimResult::FLAG_VALID_COMPONENT2,
        VALID_A = AnimResult::FLAG_VALID_COMPONENT3,
        VALID_RGBA = VALID_R | VALID_G | VALID_B | VALID_A,

        CONVERTED = AnimResult::FLAG_CONVERTED
    };

    AnimBlendOp(bool hasBlend, bool hasPostBlend): 
        mHasBlend(hasBlend),
        mHasPostBlend(hasPostBlend) 
    {}
    virtual ~AnimBlendOp() {}

    bool HasBlend() const { return mHasBlend; }
    bool HasPostBlend() const { return mHasPostBlend; }

    virtual bool Blend(AnimResult* dst,float* dstWeights,const AnimResult* src,const float* srcWeights) const{
        (void)dst;
        (void)dstWeights;
        (void)src;
        (void)srcWeights;
        return true;
    }

    virtual bool PostBlend(AnimResult* result, const float* weights) const{
        (void)result;
        (void)weights;
        return true;
    };

    virtual bool Override(AnimResult* dst, const AnimResult* src) const = 0;
    virtual void Apply(void* target, const AnimResult* result) const = 0;
    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const = 0;

protected:
    bool BlendFloatVector(AnimResult* dst,const AnimResult* src,float srcWeight,int compCount) const;

    bool OverrideFloatVector(AnimResult* dst,const AnimResult* src,int compCount,bit32 allCompValidFlag) const;

    void ApplyFloatVector(void* target, const AnimResult* result, int compCount) const;

    void ConvertFloatVectorToAnimResult(AnimResult* result, const void* source, int compCount) const;

private:
    bool mHasBlend;
    bool mHasPostBlend;
};

class AnimBlendOpBool : public AnimBlendOp{
public:

    AnimBlendOpBool(): 
        AnimBlendOp(true, false) 
    {}

    virtual ~AnimBlendOpBool() {}

    virtual bool Blend(AnimResult* dst,float* dstWeights,const AnimResult* src,const float* srcWeights) const;

    virtual bool Override(AnimResult* dst, const AnimResult* src) const{
        *reinterpret_cast<bool*>(dst->GetValueBuffer()) = *reinterpret_cast<const bool*>(src->GetValueBuffer());
        dst->SetFlags(VALID_SINGLE);
        return true;
    }

    virtual void Apply(void* target, const AnimResult* result) const{
        *reinterpret_cast<bool*>(target) = *reinterpret_cast<const bool*>(result->GetValueBuffer());
    }

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const{
        *reinterpret_cast<bool*>(result->GetValueBuffer()) = *reinterpret_cast<const bool*>(source);
        result->SetFlags(VALID_SINGLE);
    }
};

class AnimBlendOpInt : public AnimBlendOp{
public:

    AnimBlendOpInt(): 
        AnimBlendOp(false, false) 
    {}

    virtual ~AnimBlendOpInt() {}

    virtual bool Override(AnimResult* dst, const AnimResult* src) const{
        *reinterpret_cast<int*>(dst->GetValueBuffer()) = *reinterpret_cast<const int*>(src->GetValueBuffer());
        dst->SetFlags(VALID_SINGLE);
        return true;
    }

    virtual void Apply(void* target, const AnimResult* result) const{
        *reinterpret_cast<int*>(target) = *reinterpret_cast<const int*>(result->GetValueBuffer());
    }

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const{
        *reinterpret_cast<int*>(result->GetValueBuffer()) = *reinterpret_cast<const int*>(source);
        result->SetFlags(VALID_SINGLE);
    }
};

class AnimBlendOpFloat : public AnimBlendOp{
public:

    AnimBlendOpFloat(): 
        AnimBlendOp(true, false)
    {}

    virtual ~AnimBlendOpFloat() {}

    virtual bool Blend(AnimResult* dst,float* dstWeights,const AnimResult* src,const float* srcWeights) const;

    virtual bool Override(AnimResult* dst, const AnimResult* src) const{
        *reinterpret_cast<float*>(dst->GetValueBuffer()) = *reinterpret_cast<const float*>(src->GetValueBuffer());
        dst->SetFlags(VALID_SINGLE);
        return true;
    }
    virtual void Apply(void* target, const AnimResult* result) const{
        *reinterpret_cast<float*>(target) = *reinterpret_cast<const float*>(result->GetValueBuffer());
    }

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const{
        *reinterpret_cast<float*>(result->GetValueBuffer()) = *reinterpret_cast<const float*>(source);
        result->SetFlags(VALID_SINGLE);
    }
};

class AnimBlendOpVector2 : public AnimBlendOp{
public:
    enum { COMP_COUNT = 2 };
    enum { VALID_ALL = VALID_XY };

    AnimBlendOpVector2(): 
        AnimBlendOp(true, false)
    {}

    virtual ~AnimBlendOpVector2() {}

    virtual bool Blend(AnimResult* dst,float* dstWeights,const AnimResult* src,const float* srcWeights) const{
        NW_ASSERT(src->GetFlags() & VALID_ALL);
        (void)dstWeights;
        return BlendFloatVector(dst, src, srcWeights[0], COMP_COUNT);
    }

    virtual bool Override(AnimResult* dst, const AnimResult* src) const{
        return OverrideFloatVector(dst, src, COMP_COUNT, VALID_ALL);
    }
    virtual void Apply(void* target, const AnimResult* result) const{
        ApplyFloatVector(target, result, COMP_COUNT);
    }

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const{
        ConvertFloatVectorToAnimResult(result, source, COMP_COUNT);
    }
};

class AnimBlendOpVector3 : public AnimBlendOp{
public:
    enum { COMP_COUNT = 3 };
    enum { VALID_ALL = VALID_XYZ };

    AnimBlendOpVector3(): 
        AnimBlendOp(true, false) 
    {}

    virtual ~AnimBlendOpVector3() {}

    virtual bool Blend(AnimResult* dst, float* dstWeights,const AnimResult* src,const float* srcWeights) const{
        NW_ASSERT(src->GetFlags() & VALID_ALL);
        (void)dstWeights;
        return BlendFloatVector(dst, src, srcWeights[0], COMP_COUNT);
    }

    virtual bool Override(AnimResult* dst, const AnimResult* src) const{
        return OverrideFloatVector(dst, src, COMP_COUNT, VALID_ALL);
    }

    virtual void Apply(void* target, const AnimResult* result) const{
        ApplyFloatVector(target, result, COMP_COUNT);
    }   

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const{
        ConvertFloatVectorToAnimResult(result, source, COMP_COUNT);
    }
};

class AnimBlendOpRgbaColor : public AnimBlendOp{
public:
    enum { COMPONENT_COUNT = 4 };
    AnimBlendOpRgbaColor(): 
        AnimBlendOp(true, false) 
    {}

    virtual ~AnimBlendOpRgbaColor() {}

    virtual bool Blend(AnimResult* dst,float* dstWeights,const AnimResult* src,const float* srcWeights) const;

    virtual bool Override(AnimResult* dst, const AnimResult* src) const;

    virtual void Apply(void* target, const AnimResult* result) const;

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const;
};

class AnimBlendOpTexture : public AnimBlendOp{
public:
    AnimBlendOpTexture(): 
        AnimBlendOp(false, false) 
    {}

    virtual ~AnimBlendOpTexture() {}

    virtual bool Override(AnimResult* dst, const AnimResult* src) const{
        nw::ut::Offset* dstOffset = reinterpret_cast<nw::ut::Offset*>(dst->GetValueBuffer());
        const nw::ut::Offset* srcOffset = reinterpret_cast<const nw::ut::Offset*>(src->GetValueBuffer());

        dstOffset->set_ptr(static_cast<const char*>(srcOffset->to_ptr()));
        dst->SetFlags(VALID_SINGLE);
        return true;
    }

    virtual void Apply(void* target, const AnimResult* result) const{
        nw::ut::Offset* dstOffset = reinterpret_cast<nw::ut::Offset*>(target);
        const nw::ut::Offset* srcOffset = reinterpret_cast<const nw::ut::Offset*>(result->GetValueBuffer());

        dstOffset->set_ptr(static_cast<const char*>(srcOffset->to_ptr()));
    }

    virtual void ConvertToAnimResult(AnimResult* result, const void* source) const{
        nw::ut::Offset* dstOffset = reinterpret_cast<nw::ut::Offset*>(result->GetValueBuffer());
        const nw::ut::Offset* srcOffset = reinterpret_cast<const nw::ut::Offset*>(source);

        dstOffset->set_ptr(static_cast<const char*>(srcOffset->to_ptr()));
        result->SetFlags(VALID_SINGLE);
    }
};

}
}