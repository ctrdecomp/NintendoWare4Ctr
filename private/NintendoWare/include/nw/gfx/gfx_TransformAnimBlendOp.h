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

#ifndef NW_GFX_TRANSFORMANIMBLENDOP_H_
#define NW_GFX_TRANSFORMANIMBLENDOP_H_

#include <nw/anim/anim_AnimBlend.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class TransformAnimBlendOp : public anim::AnimBlendOp
{
public:
    //
    //
    //
    static const float WeightDiscard;

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    TransformAnimBlendOp(bool hasBlend, bool hasPostBlend)
    : AnimBlendOp(hasBlend, hasPostBlend) {}

    //
    virtual ~TransformAnimBlendOp() {}

    //


    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    virtual void Apply(void* target, const anim::AnimResult* result) const;

    //
    //
    //
    //
    //
    //
    virtual void ConvertToAnimResult(anim::AnimResult* result, const void* source) const;

    //

protected:
    //----------------------------------------
    //
    //

    //
    enum BasicBlendFlags
    {
        FLAG_ACCURATE_SCALE_SHIFT = 0,
        FLAG_QUATERNION_ROTATE_SHIFT = 1,

        // Whether to perform an accurate scale blend.
        FLAG_ACCURATE_SCALE = 0x1 << FLAG_ACCURATE_SCALE_SHIFT,

        // Whether to blend a rotation with a quaternion. When off, blend the matrix's rotation component.
        FLAG_QUATERNION_ROTATE = 0x1 << FLAG_QUATERNION_ROTATE_SHIFT
    };

    //
    //
    //
    //
    //
    //
    //
    void BlendScaleStandard(
        CalculatedTransform* dst,
        const CalculatedTransform* src,
        const float weight) const;

    //
    //
    //
    //
    //
    //
    //
    void BlendScaleAccurate(
        CalculatedTransform* dst,
        const CalculatedTransform* src,
        const float weight) const;

    //
    //
    //
    //
    //
    //
    //
    bool PostBlendAccurateScale(CalculatedTransform* transform) const;

    //
    //
    //
    //
    //
    //
    //
    void BlendRotateMatrix(
        CalculatedTransform* dst,
        const CalculatedTransform* src,
        const float weight) const;

    //
    //
    //
    //
    //
    //
    //
    void BlendRotateQuaternion(
        CalculatedTransform* dst,
        const CalculatedTransform* src,
        const float weight) const;

    //
    //
    //
    //
    //
    //
    //
    void BlendTranslate(
        CalculatedTransform* dst,
        const CalculatedTransform* src,
        const float weight) const;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    bool OverrideTransform(
        CalculatedTransform* dst,
        const CalculatedTransform* src,
        const bit32 blendFlags
    ) const;

    //
};

//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class TransformAnimBlendOpStandard : public TransformAnimBlendOp
{
public:
    //----------------------------------------
    //
    //

    //
    TransformAnimBlendOpStandard()
    : TransformAnimBlendOp(true, true)
    {}

    //
    virtual ~TransformAnimBlendOpStandard() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool Blend(
        anim::AnimResult* dst,
        float* dstWeights,
        const anim::AnimResult* src,
        const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleStandard(dstX, srcX, srcWeights[0]);
        BlendRotateMatrix (dstX, srcX, srcWeights[1]);
        BlendTranslate    (dstX, srcX, srcWeights[2]);
        return true;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        return dstX->NormalizeRotateMatrix();
    }

    //
    //
    //
    //
    //
    //
    //
    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, 0);
    }

    //
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class TransformAnimBlendOpAccScale : public TransformAnimBlendOp
{
public:
    //----------------------------------------
    //
    //

    //
    TransformAnimBlendOpAccScale()
    : TransformAnimBlendOp(true, true)
    {}

    //
    virtual ~TransformAnimBlendOpAccScale() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool Blend(
        anim::AnimResult* dst,
        float* dstWeights,
        const anim::AnimResult* src,
        const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleAccurate(dstX, srcX, srcWeights[0]);
        BlendRotateMatrix (dstX, srcX, srcWeights[1]);
        BlendTranslate    (dstX, srcX, srcWeights[2]);
        return true;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        const bool scaleRet = PostBlendAccurateScale(dstX);
        return dstX->NormalizeRotateMatrix() && scaleRet;
    }

    //
    //
    //
    //
    //
    //
    //
    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, FLAG_ACCURATE_SCALE);
    }

    //
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class TransformAnimBlendOpQuat : public TransformAnimBlendOp
{
public:
    //----------------------------------------
    //
    //

    //
    TransformAnimBlendOpQuat()
    : TransformAnimBlendOp(true, true)
    {}

    //
    virtual ~TransformAnimBlendOpQuat() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool Blend(
        anim::AnimResult* dst,
        float* dstWeights,
        const anim::AnimResult* src,
        const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleStandard   (dstX, srcX, srcWeights[0]);
        BlendRotateQuaternion(dstX, srcX, srcWeights[1]);
        BlendTranslate       (dstX, srcX, srcWeights[2]);
        return true;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        return dstX->QuaternionToRotateMatrix();
    }

    //
    //
    //
    //
    //
    //
    //
    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, FLAG_QUATERNION_ROTATE);
    }

    //
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class TransformAnimBlendOpAccScaleQuat : public TransformAnimBlendOp
{
public:
    //----------------------------------------
    //
    //

    //
    TransformAnimBlendOpAccScaleQuat()
    : TransformAnimBlendOp(true, true)
    {}

    //
    virtual ~TransformAnimBlendOpAccScaleQuat() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool Blend(
        anim::AnimResult* dst,
        float* dstWeights,
        const anim::AnimResult* src,
        const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleAccurate   (dstX, srcX, srcWeights[0]);
        BlendRotateQuaternion(dstX, srcX, srcWeights[1]);
        BlendTranslate       (dstX, srcX, srcWeights[2]);
        return true;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        const bool scaleRet = PostBlendAccurateScale(dstX);
        return dstX->QuaternionToRotateMatrix() && scaleRet;
    }

    //
    //
    //
    //
    //
    //
    //
    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, FLAG_ACCURATE_SCALE | FLAG_QUATERNION_ROTATE);
    }

    //
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class AnimBlendOpTransform : public TransformAnimBlendOp
{
public:
    //----------------------------------------
    //
    //

    //
    AnimBlendOpTransform()
        : TransformAnimBlendOp(true, true)
    {}

    //
    virtual ~AnimBlendOpTransform() {}

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool Blend(
        anim::AnimResult* dst,
        float* dstWeights,
        const anim::AnimResult* src,
        const float* srcWeight) const
    {
        NW_NULL_ASSERT(dst);
        NW_NULL_ASSERT(src);
        NW_NULL_ASSERT(srcWeight);

        // The same as when TransformAnimIntepolator raises FLAG_IS_IGNORE_ALL only occurs here for the first blend.
        // 
        if (!dst->IsEnabledFlags(VALID_SINGLE))
        {
            CalculatedTransform* transform =
                reinterpret_cast<CalculatedTransform*>(dst->GetValueBuffer());
            transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

            // To use VALID_SINGLE, it's matched to the AnimBlendOpFloat::Blend function, etc.
            dst->EnableFlags(VALID_SINGLE);
        }

        // Make the weight an array.
        float srcWeights[3] = { *srcWeight, *srcWeight, *srcWeight };

        return blendOp.Blend(
            reinterpret_cast<anim::AnimResult*>(dst->GetValueBuffer()),
            dstWeights,
            reinterpret_cast<const anim::AnimResult*>(src->GetValueBuffer()),
            srcWeights);
    }

    //
    //
    //
    //
    //
    //
    //
    //
    virtual bool PostBlend(anim::AnimResult* result, const float* weight) const
    {
        NW_NULL_ASSERT(result);

        CalculatedTransform* transform =
            reinterpret_cast<CalculatedTransform*>(result->GetValueBuffer());

        bool resultBlend;
        // Since weights are not used in the TransformAnimBlendOpStandard::PostBlend function, pass NULL regardless of whether weight is specified.
        // 
#if 0
        if (weight)
        {
            float weights[3] = { *weight, *weight, *weight };
            resultBlend= blendOp.PostBlend(
                reinterpret_cast<anim::AnimResult*>(transform),
                weights);
        }
        else
#endif
        {
            NW_UNUSED_VARIABLE(weight);
            resultBlend= blendOp.PostBlend(
                reinterpret_cast<anim::AnimResult*>(transform),
                NULL);
        }

        // Perform flag updating
        // TODO: Change to the same flag update method as TransformAnimInterpolator.
        transform->UpdateScaleFlags();
        transform->UpdateRotateFlags();
        transform->UpdateTranslateFlags();
        transform->UpdateCompositeFlags();

        transform->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);

        // If this flag is not set, calculations are not performed in the TransformNode::UpdateTransform function.
        transform->EnableFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED);

        return resultBlend;
    }

    //
    //
    //
    //
    //
    //
    //
    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        // Get the transform.
        CalculatedTransform* transform =
            reinterpret_cast<CalculatedTransform*>(dst->GetValueBuffer());

        // The same as when TransformAnimIntepolator raises FLAG_IS_IGNORE_ALL only occurs here for the first blend.
        // 
        if (!dst->IsEnabledFlags(VALID_SINGLE))
        {
            transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

            // To use VALID_SINGLE, it's matched to the AnimBlendOpFloat::Blend function, etc.
            dst->EnableFlags(VALID_SINGLE);
        }

        bool result = blendOp.Override(
            reinterpret_cast<anim::AnimResult*>(dst->GetValueBuffer()),
            reinterpret_cast<const anim::AnimResult*>(src->GetValueBuffer()));

        // Considering that there are times when not all components will be overwritten, since we do not understand the timing of when all the blends will end from within BlendOp, set the flag every time.
        // 
        transform->UpdateScaleFlags();
        transform->UpdateRotateFlags();
        transform->UpdateTranslateFlags();
        transform->UpdateCompositeFlags();

        // As with blend, raise the flag when TransformMatrix is changed
        transform->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        // If this flag is not set, calculations are not performed in the TransformNode::UpdateTransform function.
        transform->EnableFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED);

        return result;
    }

private:

    // The BlendOperation instance to which processing is entrusted
    TransformAnimBlendOpStandard blendOp;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_TRANSFORMANIMBLENDOP_H_
