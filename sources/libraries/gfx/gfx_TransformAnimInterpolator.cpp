// Filename: gfx_TransformAnimInterpolator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/gfx_TransformAnim.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(TransformAnimInterpolator, AnimInterpolator);

const anim::AnimResult* TransformAnimInterpolator::GetResult(void* target,int memberIdx) const
{
    const anim::AnimBlendOp* blendOp = this->m_AnimGroup->GetBlendOperation(memberIdx);

    const bit32 TRANSFORM_FLAG_MASK = 
        CalculatedTransform::FLAG_IS_IDENTITY |
        CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO |
        CalculatedTransform::FLAG_IS_ROTATE_ZERO |
        CalculatedTransform::FLAG_IS_TRANSLATE_ZERO |
        CalculatedTransform::FLAG_IS_SCALE_ONE |
        CalculatedTransform::FLAG_IS_UNIFORM_SCALE;

    if (m_IsOldMethod)
    {
        float weightSums[3] = { 0.0f, 0.0f, 0.0f };
        for (int animIdx = 0; animIdx < this->m_AnimObjects.Size(); ++animIdx)
        {
            if (m_AnimObjects[animIdx] == NULL)
            {
                continue;
            }
            const float weight = m_Weights[animIdx];
            if (!AnimWeightNearlyEqualZero(weight))
            {
                const AnimObject* animObj = m_AnimObjects[animIdx];
                if (animObj->HasMemberAnim(memberIdx))
                {
                    const TransformAnimEvaluator* evaluator = nw::ut::DynamicCast<const TransformAnimEvaluator*>(animObj);
                    if (evaluator != NULL)
                    {

                        if (!evaluator->GetIsScaleDisabled())
                        {
                            weightSums[0] += weight;
                        }
                        if (!evaluator->GetIsRotateDisabled())
                        {
                            weightSums[1] += weight;
                        }
                        if (!evaluator->GetIsTranslateDisabled())
                        {
                            weightSums[2] += weight;
                        }
                    }
                    else{
                        weightSums[0] += weight;
                        weightSums[1] += weight;
                        weightSums[2] += weight;
                    }
                }
            }
        }

        if (TransformAnimEvaluator::CheckWeightsNearlyZero(weightSums))
        {
            return NULL;
        }

        float weightNormalizeScale[3] = {
            GetAnimWeightNormalizeScale(weightSums[0]), 
            GetAnimWeightNormalizeScale(weightSums[1]), 
            GetAnimWeightNormalizeScale(weightSums[2])
        };

        CalculatedTransform* transform = reinterpret_cast<CalculatedTransform*>(target);
        const bit32 flagsBak = transform->GetFlags();
        const bool convertedBak = transform->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);
        transform->EnableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);

        transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

        CalculatedTransform workResult;
        bool written = false;
        bool firstRotateFlag = true;
        math::MTX34 firstRotateMtx;
        bit32 transformFlag = TRANSFORM_FLAG_MASK;

        for (int animIdx = this->m_AnimObjects.Size() - 1; animIdx >= 0; --animIdx)
        {

            const AnimObject* animObj = m_AnimObjects[animIdx];
            if (animObj == NULL)
            {
                continue;
            }

            const float childWeight = m_Weights[animIdx];
            NW_ASSERT(childWeight >= 0.0f);
            float srcWeights[3] ={
                childWeight * weightNormalizeScale[0],
                childWeight * weightNormalizeScale[1],
                childWeight * weightNormalizeScale[2]
            };
            TransformAnimEvaluator::DisableSRTWeightsIfNeeded(srcWeights, animObj);

            if (!TransformAnimEvaluator::CheckWeightsNearlyZero(srcWeights))
            {

                workResult.EnableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND, convertedBak);

                const anim::AnimResult* childResult = animObj->GetResult(&workResult, memberIdx);

                if (childResult != NULL)
                {
                    written = true;
                    const bool evaluatorFlag = nw::ut::DynamicCast<const TransformAnimEvaluator*>(animObj) != NULL;

                    if (evaluatorFlag && firstRotateFlag &&
                        !workResult.IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
                        {
                        firstRotateFlag = false;
                        firstRotateMtx = workResult.TransformMatrix();
                    }

                    if (!blendOp->Blend(reinterpret_cast<anim::AnimResult*>(transform),
                        NULL, childResult, srcWeights))
                    {
                        break;
                    }

                    transformFlag &= workResult.GetFlags();
                }
            }
        }


        if (!convertedBak)
        {
            transform->DisableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);   
        }

        if (!written)
        {
            transform->RestoreFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL, flagsBak);
            return NULL;
        }

        if (!convertedBak && blendOp->HasPostBlend())
        {
            if (!blendOp->PostBlend(reinterpret_cast<anim::AnimResult*>(transform), NULL))
        {
                transform->AdjustZeroRotateMatrix(!firstRotateFlag, firstRotateMtx);
            }
        }

        transform->RestoreFlags(TRANSFORM_FLAG_MASK, transformFlag);
    }
    else{
        bool isValidAnim = false;
        for (int animIdx = 0; animIdx < this->m_AnimObjects.Size(); ++animIdx)
        {
            if (m_AnimObjects[animIdx] && m_AnimObjects[animIdx]->HasMemberAnim(memberIdx))
            {
                isValidAnim = true;
                break;
            }
        }

        if (!isValidAnim)
        {
            return NULL;
        }

        if (m_IsWeightDirty && m_IsWeightNormalizationEnabled)
        {
            NormalizeWeight();
        }

        CalculatedTransform* transform = reinterpret_cast<CalculatedTransform*>(target);
        const bit32 flagsBak = transform->GetFlags();
        const bool convertedBak = transform->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);
        transform->EnableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);

        transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

        CalculatedTransform workResult;
        bool firstRotateFlag = true;
        math::MTX34 firstRotateMtx;
        bit32 transformFlag = TRANSFORM_FLAG_MASK;
        for (int animIdx = this->m_AnimObjects.Size() - 1; animIdx >= 0; --animIdx)
        {
            const AnimObject* animObj = m_AnimObjects[animIdx];

            const float childWeight = m_NormalizedWeights[animIdx];
            NW_ASSERT(childWeight >= 0.0f);
            const float srcWeights[3] ={
                childWeight,
                childWeight,
                childWeight
            };

            if (!TransformAnimEvaluator::CheckWeightsNearlyZero(srcWeights))
            {

                workResult.EnableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND, convertedBak);

                const anim::AnimResult* childResult = NULL;
                if (animObj != NULL)
                {
                    childResult = animObj->GetResult(&workResult, memberIdx);
                }

                if (childResult == NULL)
                {
                    const math::Transform3* originalValue = static_cast<const math::Transform3*>(GetAnimGroup()->GetOriginalValue(memberIdx));
                    workResult.SetTransform(*originalValue);
                    workResult.UpdateScaleFlags();
                    workResult.UpdateRotateFlagsStrictly();
                    workResult.UpdateTranslateFlags();
                    workResult.UpdateCompositeFlags();
                    childResult = reinterpret_cast<anim::AnimResult*>(&workResult);
                }

                const bool evaluatorFlag = nw::ut::DynamicCast<const TransformAnimEvaluator*>(animObj) != NULL;

                if (evaluatorFlag && firstRotateFlag &&
                    !workResult.IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
                    {
                    firstRotateFlag = false;
                    firstRotateMtx = workResult.TransformMatrix();
                }

                if (!blendOp->Blend(reinterpret_cast<anim::AnimResult*>(transform),
                    NULL, childResult, srcWeights))
                {
                    break;
                }

                transformFlag &= workResult.GetFlags();
            }
        }

        if (!convertedBak)
        {
            transform->DisableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);   
        }

        if (!convertedBak && blendOp->HasPostBlend())
        {
            if (!blendOp->PostBlend(reinterpret_cast<anim::AnimResult*>(transform), NULL))
        {
                transform->AdjustZeroRotateMatrix(!firstRotateFlag, firstRotateMtx);
            }
        }

        transform->RestoreFlags(TRANSFORM_FLAG_MASK, transformFlag);
    }

    return reinterpret_cast<anim::AnimResult*>(target);
}

}
}