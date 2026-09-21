// Filename: gfx_TransformAnimAdder.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/gfx_TransformAnim.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(TransformAnimAdder, AnimAdder);

const anim::AnimResult* TransformAnimAdder::GetResult(void* target,int memberIdx) const
{
    const anim::AnimBlendOp* blendOp = this->m_AnimGroup->GetBlendOperation(memberIdx);

    CalculatedTransform* transform = reinterpret_cast<CalculatedTransform*>(target);
    const bit32 flagsBak = transform->GetFlags();
    const bool convertedBak = transform->IsEnabledFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);
    transform->EnableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);

    transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

    CalculatedTransform workResult;
    bool written = false;
    bool firstRotateFlag = true;
    math::MTX34 firstRotateMtx;

    for (int animIdx = this->m_AnimObjects.Size() - 1; animIdx >= 0; --animIdx)
    {
        const AnimObject* animObj = m_AnimObjects[animIdx];
        if (animObj == NULL)
        {
            continue;
        }

        const float childWeight = m_Weights[animIdx];
        float srcWeights[3] = { childWeight, childWeight, childWeight };
        TransformAnimEvaluator::DisableSRTWeightsIfNeeded(srcWeights, animObj);

        if (!TransformAnimEvaluator::CheckWeightsNearlyZero(srcWeights))
        {
            workResult.EnableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND, convertedBak);

            const anim::AnimResult* childResult = animObj->GetResult(&workResult, memberIdx);
            if (childResult != NULL)
            {
                written = true;
                const bool evaluatorFlag = ut::DynamicCast<const TransformAnimEvaluator*>(animObj) != NULL;

                if (evaluatorFlag && firstRotateFlag &&
                    !workResult.IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
                    {
                    firstRotateFlag = false;
                    firstRotateMtx = workResult.TransformMatrix();
                }

                if (!blendOp->Blend(reinterpret_cast<anim::AnimResult*>(transform), NULL, childResult, srcWeights))
                {
                    break;
                }
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
    transform->UpdateScaleFlags();
    transform->UpdateRotateFlagsStrictly();
    transform->UpdateTranslateFlags();
    transform->UpdateCompositeFlags();

    return reinterpret_cast<anim::AnimResult*>(target);
}

}
}