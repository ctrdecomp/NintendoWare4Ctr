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
            dstMtx.matrix[0][0] = srcMtx.matrix[0][0] * weight;
            dstMtx.matrix[0][1] = srcMtx.matrix[0][1] * weight;
            dstMtx.matrix[0][2] = srcMtx.matrix[0][2] * weight;

            dstMtx.matrix[1][0] = srcMtx.matrix[1][0] * weight;
            dstMtx.matrix[1][1] = srcMtx.matrix[1][1] * weight;
            dstMtx.matrix[1][2] = srcMtx.matrix[1][2] * weight;
        }
        else{
            dstMtx.matrix[0][0] += srcMtx.matrix[0][0] * weight;
            dstMtx.matrix[0][1] += srcMtx.matrix[0][1] * weight;
            dstMtx.matrix[0][2] += srcMtx.matrix[0][2] * weight;

            dstMtx.matrix[1][0] += srcMtx.matrix[1][0] * weight;
            dstMtx.matrix[1][1] += srcMtx.matrix[1][1] * weight;
            dstMtx.matrix[1][2] += srcMtx.matrix[1][2] * weight;
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

        float& addedWeight = dstMtx.matrix[1][1];
        math::QUAT srcQ;
        if (src->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND))
        {
            srcQ = math::QUAT(srcMtx.matrix[0][0], srcMtx.matrix[0][1], srcMtx.matrix[0][2], srcMtx.matrix[1][0]);
        }
        else{
            math::MTX34ToQUAT(&srcQ, &srcMtx);
        }
        math::QUAT dstQ;
        if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
            addedWeight = weight;
            dstQ = srcQ;
        }
        else{
            dstQ = math::QUAT(dstMtx.matrix[0][0], dstMtx.matrix[0][1], dstMtx.matrix[0][2], dstMtx.matrix[1][0]);
            addedWeight += weight;
            const float t = (addedWeight != 0.0f) ? weight / addedWeight : 0.0f;
            math::QUATSlerp(&dstQ, &dstQ, &srcQ, t);
        }
        dstMtx.matrix[0][0] = dstQ.x;
        dstMtx.matrix[0][1] = dstQ.y;
        dstMtx.matrix[0][2] = dstQ.z;
        dstMtx.matrix[1][0] = dstQ.w;

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

        math::VEC3 srcT(srcMtx.matrix[0][3], srcMtx.matrix[1][3], srcMtx.matrix[2][3]);
        if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE))
        {
            VEC3Scale(&srcT, &srcT, weight);
            dst->SetTranslate(srcT);
            dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE);
        }
        else{
            math::VEC3 dstT(dstMtx.matrix[0][3], dstMtx.matrix[1][3], dstMtx.matrix[2][3]);
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
        dstMtx.matrix[0][0] = srcMtx.matrix[0][0];
        dstMtx.matrix[0][1] = srcMtx.matrix[0][1];
        dstMtx.matrix[0][2] = srcMtx.matrix[0][2];

        dstMtx.matrix[1][0] = srcMtx.matrix[1][0];
        dstMtx.matrix[1][1] = srcMtx.matrix[1][1];
        dstMtx.matrix[1][2] = srcMtx.matrix[1][2];

        dstMtx.matrix[2][0] = srcMtx.matrix[2][0];
        dstMtx.matrix[2][1] = srcMtx.matrix[2][1];
        dstMtx.matrix[2][2] = srcMtx.matrix[2][2];

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE);

        if (needToConverted && (blendFlags & FLAG_QUATERNION_ROTATE))
        {
            dst->RotateMatrixToQuaternion();
        }
    }

    if (dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE) && !src->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE))
    {
        dstMtx.matrix[0][3] = srcMtx.matrix[0][3];
        dstMtx.matrix[1][3] = srcMtx.matrix[1][3];
        dstMtx.matrix[2][3] = srcMtx.matrix[2][3];

        dst->DisableFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE);
    }

    dst->SetTransformMatrix(dstMtx);

    return !dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE) && !dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE) && !dst->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE);
}

}
}