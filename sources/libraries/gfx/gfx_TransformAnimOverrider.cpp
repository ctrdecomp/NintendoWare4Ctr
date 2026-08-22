#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/gfx_TransformAnim.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(TransformAnimOverrider, AnimOverrider);

const anim::AnimResult* TransformAnimOverrider::GetResult(void* target,int memberIdx) const{
    const anim::AnimBlendOp* blendOp = this->mAnimGroup->GetBlendOperation(memberIdx);

    CalculatedTransform* transform = reinterpret_cast<CalculatedTransform*>(target);
    const bit32 flagsBak = transform->GetFlags();
    transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

    CalculatedTransform workResult;
    bool written = false;
    for (int animIdx = this->mAnimObjects.Size() - 1; animIdx >= 0; --animIdx){
        const AnimObject* animObj = mAnimObjects[animIdx];
        if (animObj == NULL){
            continue;
        }

        float srcWeights[3] = {1.0f, 1.0f, 1.0f};
        TransformAnimEvaluator::DisableSRTWeightsIfNeeded(srcWeights, animObj);

        if (!TransformAnimEvaluator::CheckWeightsNearlyZero(srcWeights)){
            const anim::AnimResult* childResult =
                animObj->GetResult(&workResult, memberIdx);
            if (childResult != NULL)
            {
                written = true;
                if (blendOp->Override(reinterpret_cast<anim::AnimResult*>(transform),
                    childResult)){
                    break;
                }
            }
        }
    }

    if (!written){
        transform->RestoreFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL, flagsBak);
        return NULL;
    }

    transform->UpdateScaleFlags();
    transform->UpdateRotateFlagsStrictly();
    transform->UpdateTranslateFlags();
    transform->UpdateCompositeFlags();

    return reinterpret_cast<anim::AnimResult*>(target);
}

}
}