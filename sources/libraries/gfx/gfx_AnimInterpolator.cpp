// Filename: gfx_AnimInterpolator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AnimInterpolator , AnimBlender);

const anim::AnimResult* AnimInterpolator::GetResult(void* target,int memberIdx) const
{
    const anim::AnimBlendOp* blendOp = this->m_AnimGroup->GetBlendOperation(memberIdx);
    if (blendOp == NULL || !blendOp->HasBlend())
    {
        for (int animIdx = this->m_AnimObjects.Size() - 1; animIdx >= 0; --animIdx)
        {
            if (m_AnimObjects[animIdx] == NULL)
            {
                continue;
            }
            const anim::AnimResult* childResult = this->m_AnimObjects[animIdx]->GetResult(target, memberIdx);
            if (childResult != NULL)
            {
                return childResult;
            }
        }
        return NULL;
    }

    if (m_IsOldMethod)
    {
        float weightSum = 0.0f;
        const AnimObject* lastAnimObj = NULL;
        int validAnimCount = 0;
        for (int animIdx = 0; animIdx < this->m_AnimObjects.Size(); ++animIdx)
        {
            if (m_AnimObjects[animIdx] == NULL)
            {
                continue;
            }
            const float weight = m_Weights[animIdx];
            if (!AnimWeightNearlyEqualZero(weight))
            {
                if (this->m_AnimObjects[animIdx]->HasMemberAnim(memberIdx))
                {
                    weightSum += weight;
                    lastAnimObj = m_AnimObjects[animIdx];
                    ++validAnimCount;
                }
            }
        }

        if (validAnimCount == 0)
        {
            return NULL;
        }
        else if (validAnimCount == 1)
        {
            return lastAnimObj->GetResult(target, memberIdx);
        }

        const float weightNormalizeScale = GetAnimWeightNormalizeScale(weightSum);

        anim::AnimResult* result = reinterpret_cast<anim::AnimResult*>(target);
        const bool convertedBak = result->IsEnabledFlags(anim::AnimResult::FLAG_CONVERTED);
        result->EnableFlags(anim::AnimResult::FLAG_CONVERTED, true);

        anim::AnimResult workResult;
        bool written = false;
        float compWeights[anim::AnimResult::MAX_COMPONENTS];
        for (int animIdx = this->m_AnimObjects.Size() - 1; animIdx >= 0; --animIdx)
        {
            if (m_AnimObjects[animIdx] == NULL)
            {
                continue;
            }
            const float childWeight = m_Weights[animIdx] * weightNormalizeScale;
            if (!AnimWeightNearlyEqualZero(childWeight))
            {
                workResult.EnableFlags(anim::AnimResult::FLAG_CONVERTED, convertedBak);

                const anim::AnimResult* childResult = this->m_AnimObjects[animIdx]->GetResult(&workResult, memberIdx);
                if (childResult != NULL)
                {
                    written = true;
                    if (!blendOp->Blend(result, compWeights, childResult, &childWeight))
                    {
                        break;
                    }
                }
            }
        }

        if (!convertedBak)
        {
            result->DisableFlags(anim::AnimResult::FLAG_CONVERTED);   
        }

        if (!written)
        {
            return NULL;
        }

        if (!convertedBak && blendOp->HasPostBlend())
        {
            blendOp->PostBlend(result, compWeights);    
        }
        return result;
    }
    else{
        bool isValidAnim = false;
        for (int animIdx = 0; animIdx < this->m_AnimObjects.Size(); ++animIdx)
        {
            if (m_AnimObjects[animIdx] && this->m_AnimObjects[animIdx]->HasMemberAnim(memberIdx))
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

        anim::AnimResult* result = reinterpret_cast<anim::AnimResult*>(target);
        const bool convertedBak = result->IsEnabledFlags(anim::AnimResult::FLAG_CONVERTED);
        result->EnableFlags(anim::AnimResult::FLAG_CONVERTED, true);

        anim::AnimResult workResult;
        float compWeights[anim::AnimResult::MAX_COMPONENTS];
        for (int animIdx = this->m_AnimObjects.Size() - 1; animIdx >= 0; --animIdx)
        {
            const float childWeight = m_NormalizedWeights[animIdx];
            if (!AnimWeightNearlyEqualZero(childWeight))
            {

                workResult.EnableFlags(anim::AnimResult::FLAG_CONVERTED, convertedBak);

                const anim::AnimResult* childResult = NULL;
                if (m_AnimObjects[animIdx] != NULL)
                {
                    childResult = this->m_AnimObjects[animIdx]->GetResult(&workResult, memberIdx);
                }

                if (childResult == NULL)
                {
                    NW_ASSERT(GetAnimGroup()->HasOriginalValue());
                    blendOp->ConvertToAnimResult(&workResult, GetAnimGroup()->GetOriginalValue(memberIdx));
                    childResult = &workResult;

                    NW_NULL_ASSERT(childResult);
                }

                if (!blendOp->Blend(result, compWeights, childResult, &childWeight))
                {
                    break;
                }
            
            }
        }

        if (!convertedBak)
        {
            result->DisableFlags(anim::AnimResult::FLAG_CONVERTED);   
        }

        if (!convertedBak && blendOp->HasPostBlend())
        {
            blendOp->PostBlend(result, compWeights);    
        }
        return result;
    }
}

}
}