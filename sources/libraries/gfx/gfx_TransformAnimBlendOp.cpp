// Filename: gfx_TransformAnimBlendOp.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_TransformAnim.h>
#include <nn/math/ARMv6/inline/math_Types.ipp>

namespace nw{
namespace gfx{

const float TransformAnimBlendOp::WeightDiscard = -1.0f;

namespace {

void FlogVector3(math::VEC3* dst)
{
    dst->x = math::FLog(dst->x);
    dst->y = math::FLog(dst->y);
    dst->z = math::FLog(dst->z);
}

void BlendVector3(math::VEC3* dst,const math::VEC3* src,const float weight,const bool overrideFlag)
{
    if (overrideFlag)
    {
        VEC3Scale(dst, src, weight);
    }   
    else{
        math::VEC3 tmp;
        VEC3Scale(&tmp, src, weight);
        VEC3Add(dst, dst, &tmp);
    }
}

}

void TransformAnimBlendOp::Apply(void* target, const anim::AnimResult* result) const
{
    *reinterpret_cast<CalculatedTransform*>(target) = *reinterpret_cast<const CalculatedTransform*>(result->GetValueBuffer());
}

void TransformAnimBlendOp::ConvertToAnimResult(anim::AnimResult* result,const void* source) const
{
    *reinterpret_cast<CalculatedTransform*>(result->GetValueBuffer()) = *reinterpret_cast<const CalculatedTransform*>(source);
}

void TransformAnimBlendOp::BlendScaleStandard(CalculatedTransform* dst,const CalculatedTransform* src,const float weight) const
{
    if (weight != TransformAnimBlendOp::WeightDiscard)
    {
        BlendVector3(&dst->m_Scale, &src->Scale(),
            weight, dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE));

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE);
        dst->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }
}

void TransformAnimBlendOp::BlendScaleAccurate(CalculatedTransform* dst,const CalculatedTransform* src,const float weight) const
{
    if (weight != TransformAnimBlendOp::WeightDiscard)
    {

        math::VEC3 logScale = src->Scale();
        FlogVector3(&logScale);
        BlendVector3(&dst->m_Scale, &logScale, weight, dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE));

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE);
        dst->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }
}

bool TransformAnimBlendOp::PostBlendAccurateScale(CalculatedTransform* transform) const
{
    math::VEC3& scale = transform->m_Scale;
    scale.x = math::FExp(scale.x);
    scale.y = math::FExp(scale.y);
    scale.z = math::FExp(scale.z);
    transform->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    return true;
}

void TransformAnimBlendOp::BlendRotateMatrix(CalculatedTransform* dst,const CalculatedTransform* src,const float weight) const
{
    if (weight != TransformAnimBlendOp::WeightDiscard)
    {
        math::MTX34& dstMtx = dst->m_TransformMatrix;
        const math::MTX34& srcMtx = src->TransformMatrix();

        if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
            dstMtx.f._00 = srcMtx.f._00 * weight;
            dstMtx.f._01 = srcMtx.f._01 * weight;
            dstMtx.f._02 = srcMtx.f._02 * weight;

            dstMtx.f._10 = srcMtx.f._10 * weight;
            dstMtx.f._11 = srcMtx.f._11 * weight;
            dstMtx.f._12 = srcMtx.f._12 * weight;
        }
        else
        {
            dstMtx.f._00 += srcMtx.f._00 * weight;
            dstMtx.f._01 += srcMtx.f._01 * weight;
            dstMtx.f._02 += srcMtx.f._02 * weight;

            dstMtx.f._10 += srcMtx.f._10 * weight;
            dstMtx.f._11 += srcMtx.f._11 * weight;
            dstMtx.f._12 += srcMtx.f._12 * weight;
        }

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE);
        dst->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }
}

void TransformAnimBlendOp::BlendRotateQuaternion(CalculatedTransform* dst,const CalculatedTransform* src,const float weight) const
{
    if (weight != TransformAnimBlendOp::WeightDiscard)
    {
        math::MTX34& dstMtx = dst->m_TransformMatrix;
        const math::MTX34& srcMtx = src->TransformMatrix();

        float& addedWeight = dstMtx.f._11;
        math::QUAT srcQ;
        if (src->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND))
        {
            srcQ = math::QUAT(srcMtx.f._00, srcMtx.f._01, srcMtx.f._02, srcMtx.f._10);
        }
        else
        {
            math::MTX34ToQUAT(&srcQ, &srcMtx);
        }
        math::QUAT dstQ;
        if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
            addedWeight = weight;
            dstQ = srcQ;
        }
        else
        {
            dstQ = math::QUAT(dstMtx.f._00, dstMtx.f._01, dstMtx.f._02, dstMtx.f._10);
            addedWeight += weight;
            const float t = (addedWeight != 0.0f) ? weight / addedWeight : 0.0f;
            math::QUATSlerp(&dstQ, &dstQ, &srcQ, t);
        }
        dstMtx.f._00 = dstQ.x;
        dstMtx.f._01 = dstQ.y;
        dstMtx.f._02 = dstQ.z;
        dstMtx.f._10 = dstQ.w;

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE);
        dst->SetTransformMatrix(dstMtx);
    }
}

void TransformAnimBlendOp::BlendTranslate(CalculatedTransform* dst,const CalculatedTransform* src,const float weight) const
{
    if (weight != TransformAnimBlendOp::WeightDiscard)
    {
        const math::MTX34& dstMtx = dst->TransformMatrix();
        const math::MTX34& srcMtx = src->TransformMatrix();

        math::VEC3 srcT(srcMtx.f._03, srcMtx.f._13, srcMtx.f._23);
        if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE))
        {
            VEC3Scale(&srcT, &srcT, weight);
            dst->SetTranslate(srcT);
            dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE);
        }
        else
        {
            math::VEC3 dstT(dstMtx.f._03, dstMtx.f._13, dstMtx.f._23);
            BlendVector3(&dstT, &srcT, weight, false);
            dst->SetTranslate(dstT);
        }
    }
}

bool TransformAnimBlendOp::OverrideTransform(CalculatedTransform* dst,const CalculatedTransform* src,const bit32 blendFlags) const
{
    const bool needToConverted = dst->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND) && !src->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);

    math::MTX34 dstMtx = dst->TransformMatrix();
    const math::MTX34& srcMtx = src->TransformMatrix();

    if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE) &&
        !src->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE))
        {
        dst->SetScale(src->Scale());

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE);

        if (needToConverted && (blendFlags & FLAG_ACCURATE_SCALE))
        {
            math::VEC3& dstScale = dst->m_Scale;
            FlogVector3(&dstScale);
            dst->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        }
    }

    if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE) &&
        !src->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
        dstMtx.f._00 = srcMtx.f._00;
        dstMtx.f._01 = srcMtx.f._01;
        dstMtx.f._02 = srcMtx.f._02;

        dstMtx.f._10 = srcMtx.f._10;
        dstMtx.f._11 = srcMtx.f._11;
        dstMtx.f._12 = srcMtx.f._12;

        dstMtx.f._20 = srcMtx.f._20;
        dstMtx.f._21 = srcMtx.f._21;
        dstMtx.f._22 = srcMtx.f._22;

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE);

        if (needToConverted && (blendFlags & FLAG_QUATERNION_ROTATE))
        {
            dst->RotateMatrixToQuaternion();
        }
    }

    if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE) && !src->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE))
    {
        dstMtx.f._03 = srcMtx.f._03;
        dstMtx.f._13 = srcMtx.f._13;
        dstMtx.f._23 = srcMtx.f._23;

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE);
    }

    dst->SetTransformMatrix(dstMtx);

    return !dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE) && !dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE) && !dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE);
}

}
}