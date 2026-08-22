#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_TransformAnim.h>

namespace nw{
namespace gfx{

void AnimBinding::Evaluate(anim::ResGraphicsAnimGroup::EvaluationTiming timing){
    for (int animGroupIdx = 0; animGroupIdx < this->mAnimGroups.Size(); ++animGroupIdx){
        AnimGroup* animGroup = mAnimGroups[animGroupIdx];

        if (animGroup == NULL || animGroup->GetResGraphicsAnimGroup().GetEvaluationTiming() != timing){
            continue;
        }

        for (int animObjectIdx = 0; animObjectIdx < mAnimObjectCountPerGroup; ++animObjectIdx){
            const int index = animGroupIdx * mAnimObjectCountPerGroup + animObjectIdx;
            AnimObject* animObj = mAnimObjects[index];
            if (animObj == NULL){
                continue;
            }

            switch (animObj->GetAnimType()){
            case AnimObject::ANIMTYPE_SIMPLE:
                static_cast<AnimEvaluator*>(animObj)->UpdateCacheNonVirtual();
                EvaluateSimple(animGroup, static_cast<AnimEvaluator*>(animObj));
                break;
            case AnimObject::ANIMTYPE_TRANSFORM_SIMPLE:
                static_cast<TransformAnimEvaluator*>(animObj)->UpdateCacheNonVirtual();
                EvaluateTransformSimple(animGroup, static_cast<TransformAnimEvaluator*>(animObj));
                break;
            default:
                animObj->UpdateCache();
                EvaluateBlender(animGroup, animObj);
            }
        }
    }
}

void AnimBinding::EvaluateSimple(AnimGroup* animGroup, AnimEvaluator* evaluator){
    NW_ASSERT(!(animGroup->GetResAnimGroup().GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM));

    int lastTargetObjIdx = -1;
    bool targetObjSkipFlag = false;
    int animCount = evaluator->GetAnimData().GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < animCount; ++animIdx){
        int memberIdx = evaluator->ReverseBindIndexTable()[animIdx];
        if (memberIdx == BaseAnimEvaluator::NotFoundIndex){
            continue;
        }
        EvaluateMember(animGroup, memberIdx, evaluator, lastTargetObjIdx, targetObjSkipFlag);
    }
}

void AnimBinding::EvaluateTransformSimple(AnimGroup* animGroup, TransformAnimEvaluator* evaluator){
    NW_ASSERT(animGroup->GetResAnimGroup().GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM);

    int animCount = evaluator->GetAnimData().GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < animCount; ++animIdx){
        int memberIdx = evaluator->ReverseBindIndexTable()[animIdx];
        if (memberIdx == BaseAnimEvaluator::NotFoundIndex){
            continue;
        }
        EvaluateTransformMemberFast(animGroup, memberIdx, evaluator);
    }
}

void AnimBinding::EvaluateBlender(AnimGroup* animGroup, AnimObject* animObj){
    if (animGroup->GetResAnimGroup().GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM){

        int memberCount = animGroup->GetMemberCount();
        for (int memberIdx = 0; memberIdx < memberCount; ++memberIdx){
            EvaluateTransformMember(animGroup, memberIdx, animObj);
        }
    }
    else{
        int lastTargetObjIdx = -1;
        bool targetObjSkipFlag = false;
        int memberCount = animGroup->GetMemberCount();
        for (int memberIdx = 0; memberIdx < memberCount; ++memberIdx){
            EvaluateMember(animGroup, memberIdx, animObj, lastTargetObjIdx, targetObjSkipFlag);
        }
    }
}

void AnimBinding::EvaluateTransformMember(AnimGroup* animGroup, int memberIdx, AnimObject* animObj){
    AnimGroup::PreEvaluateCallback preEvaluateCallback = animGroup->GetPreEvaluateCallback();

    if (preEvaluateCallback != NULL){
        const int targetObjIdx = animGroup->GetTargetObjectIndex(memberIdx);
        if (!preEvaluateCallback(animGroup, targetObjIdx)){
            return;
        }
    }

    CalculatedTransform* target = static_cast<CalculatedTransform*>(animGroup->GetTargetPtr(memberIdx));
    animObj->GetResult(target, memberIdx);
}

void AnimBinding::EvaluateTransformMemberFast(AnimGroup* animGroup, int memberIdx, TransformAnimEvaluator* evaluator){
    AnimGroup::PreEvaluateCallback preEvaluateCallback = animGroup->GetPreEvaluateCallback();

    if (preEvaluateCallback != NULL){
        const int targetObjIdx = animGroup->GetTargetObjectIndex(memberIdx);
        if (!preEvaluateCallback(animGroup, targetObjIdx)){
            return;
        }
    }

    CalculatedTransform* target = static_cast<CalculatedTransform*>(animGroup->GetTargetPtr(memberIdx));
    evaluator->GetResultFast(target, memberIdx);
}

void AnimBinding::EvaluateMember(AnimGroup* animGroup, int memberIdx, AnimObject* animObj, int& lastTargetObjIdx, bool& targetObjSkipFlag){
    AnimGroup::PreEvaluateCallback preEvaluateCallback = animGroup->GetPreEvaluateCallback();

    if (preEvaluateCallback != NULL){
        const int targetObjIdx = animGroup->GetTargetObjectIndex(memberIdx);
        if (targetObjIdx != lastTargetObjIdx){
            targetObjSkipFlag = !preEvaluateCallback(animGroup, targetObjIdx);
            lastTargetObjIdx = targetObjIdx;
        }
        if (targetObjSkipFlag){
            return;
        }
    }

    anim::ResAnimGroupMember member = animGroup->GetResAnimGroupMember(memberIdx);
    void* target = NULL;
    ut::Offset texturePatternTarget;

    if (member.GetObjectType() == anim::ResAnimGroupMember::OBJECT_TYPE_TEXTURE_MAPPER &&
        member.GetMemberType() == anim::ResTextureMapperMember::MEMBER_TYPE_TEXTURE){
        target = &texturePatternTarget;
    }
    else{
        target = animGroup->GetTargetPtr(memberIdx);
    }

    const anim::AnimBlendOp* blendOp = animGroup->GetBlendOperation(memberIdx);
    const anim::AnimResult* resultPtr = NULL;
    if (blendOp != NULL){
        anim::AnimResult result;
        resultPtr = animObj->GetResult(&result, memberIdx);
        if (resultPtr != NULL){
            blendOp->Apply(target, resultPtr);
        }
    }
    else{
        resultPtr = animObj->GetResult(target, memberIdx);
    }

    bool isTransformMember = (
        (member.GetObjectType() == anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM) &&
        (member.GetMemberType() == anim::ResTransformMember::MEMBER_TYPE_TRANSFORM)
        );

    if (!isTransformMember && resultPtr){
        void* object = animGroup->GetTargetObject(memberIdx);
        member.SetValueForType(object, target);
    }
}

}
}