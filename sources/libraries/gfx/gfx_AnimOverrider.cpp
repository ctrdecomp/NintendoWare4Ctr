#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AnimOverrider, AnimBlender);

const anim::AnimResult* AnimOverrider::GetResult(void* target,int memberIdx) const{

    const anim::AnimBlendOp* blendOp = this->mAnimGroup->GetBlendOperation(memberIdx);
    if (blendOp == NULL || !blendOp->HasBlend()){
        for (int animIdx = this->mAnimObjects.Size() - 1; animIdx >= 0; --animIdx){
            if (mAnimObjects[animIdx] == NULL){
                continue;
            }
            const anim::AnimResult* childResult = this->mAnimObjects[animIdx]->GetResult(target, memberIdx);
            if (childResult != NULL){
                return childResult;
            }
        }
        return NULL;
    }

    anim::AnimResult* result = reinterpret_cast<anim::AnimResult*>(target);
    anim::AnimResult workResult;
    bool written = false;
    for (int animIdx = this->mAnimObjects.Size() - 1; animIdx >= 0; --animIdx){
        if (mAnimObjects[animIdx] == NULL){
            continue;
        }
        const anim::AnimResult* childResult = this->mAnimObjects[animIdx]->GetResult(&workResult, memberIdx);
        if (childResult != NULL){
            written = true;
            if (blendOp->Override(result, childResult)){
                return result;
            }
        }
    }

    if (!written){
        return NULL;
    }

    return result;
}

}
}