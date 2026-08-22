#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AnimAdder, AnimBlender);

const anim::AnimResult* AnimAdder::GetResult(void* target,int memberIdx) const{
    const anim::AnimBlendOp* blendOp = this->mAnimGroup->GetBlendOperation(memberIdx);
    if (blendOp == NULL || !blendOp->HasBlend()){
        for (int animIdx = this->mAnimObjects.Size() - 1; animIdx >= 0; --animIdx){
            if (mAnimObjects[animIdx] == NULL){
                continue;
            }
            const anim::AnimResult* childResult = mAnimObjects[animIdx]->GetResult(target, memberIdx);
            if (childResult != NULL){
                return childResult;
            }
        }
        return NULL;
    }

    anim::AnimResult* result = reinterpret_cast<anim::AnimResult*>(target);
    const bool convertedBak = result->IsEnabledFlags(anim::AnimResult::FLAG_CONVERTED);
    result->EnableFlags(anim::AnimResult::FLAG_CONVERTED, true);

    anim::AnimResult workResult;
    bool written = false;
    for (int animIdx = this->mAnimObjects.Size() - 1; animIdx >= 0; --animIdx){
        if (mAnimObjects[animIdx] == NULL){
            continue;
        }
        const float childWeight = mWeights[animIdx];
        if (!AnimWeightNearlyEqualZero(childWeight)){

            workResult.EnableFlags(anim::AnimResult::FLAG_CONVERTED, convertedBak);

            const anim::AnimResult* childResult = this->mAnimObjects[animIdx]->GetResult(&workResult, memberIdx);
            if (childResult != NULL){
                written = true;
                if (!blendOp->Blend(result, NULL, childResult, &childWeight)){
                    break;
                }
            }
        }
    }

    if (!convertedBak){
        result->DisableFlags(anim::AnimResult::FLAG_CONVERTED);   
    }

    if (!written){
        return NULL;
    }

    if (!convertedBak && blendOp->HasPostBlend()){
        blendOp->PostBlend(result, NULL);    
    }
    return result;
}

}
}