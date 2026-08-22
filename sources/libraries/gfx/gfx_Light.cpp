

#include <nw/gfx/gfx_AnimEvaluator.h>
#include <nw/gfx/gfx_Light.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_AmbientLight.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(Light, TransformNode);

void Light::Accept(ISceneVisitor* visitor){
    visitor->VisitLight(this);
    AcceptChildren(visitor);
}


Result Light::CreateAnimGroup(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    NW_ASSERT(this->mOriginalValue.IsValid());

    if (!mAnimBinding){
        return result;
    }

    ResSceneNode resSceneNode = GetResSceneNode();
    NW_ASSERT(resSceneNode.IsValid());

    NW_ASSERT(resSceneNode.GetAnimGroupsCount() == 1);
    anim::ResAnimGroup resAnimGroup = resSceneNode.GetAnimGroups(0);

    NW_ASSERT(resAnimGroup.GetTargetType() == anim::ResGraphicsAnimGroup::TARGET_TYPE_LIGHT);

    AnimGroup* animGroup = AnimGroup::Builder().ResAnimGroup(resAnimGroup).SetSceneNode(this).UseOriginalValue(true).Create(allocator);

    if (animGroup == NULL){
        result |= Result::MASK_FAIL_BIT;
    }

    NW_ENSURE_AND_RETURN(result);

    this->mAnimGroup = animGroup;

    const int animMemberCount = this->mAnimGroup->GetMemberCount();
    for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx){
        anim::ResAnimGroupMember member = this->mAnimGroup->GetResAnimGroupMember(memberIdx);

        void* object = GetAnimTargetObject(member);
        m_AnimGroup->SetTargetObject(memberIdx, object);

        if (member.GetObjectType() == anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM && member.GetMemberType() == anim::ResTransformMember::MEMBER_TYPE_TRANSFORM){
            this->mAnimGroup->SetTargetPtr(memberIdx, &this->Transform());
        }
        else{
            u8* target = static_cast<u8*>(object);
            target += member.GetMemberOffset();
            this->mAnimGroup->SetTargetPtr(memberIdx, target);
        }

        this->mAnimGroup->SetTargetObjectIndex(memberIdx, 0);

        if (member.GetObjectType() == nw::anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM){
            this->mAnimGroup->SetOriginalValue(memberIdx, &this->mOriginalTransform);
        }
        else{
            u8* originalValue = reinterpret_cast<u8*>(this->mOriginalValue.ptr());
            originalValue += member.GetMemberOffset();
            this->mAnimGroup->SetOriginalValue(memberIdx, originalValue);
        }
    }

    this->mAnimBinding->SetAnimGroup(0, this->mAnimGroup);

    return result;
}

void Light::DestroyOriginalValue(){
    NW_ASSERT(mOriginalValue.IsValid());

    GetAllocator().Free(this->mOriginalValue.ptr());
    mOriginalValue = ResLight(NULL);
}

void* Light::GetAnimTargetObject(const anim::ResAnimGroupMember& anim){
    switch (anim.GetObjectType()){
    case anim::ResAnimGroupMember::OBJECT_TYPE_TRANSFORM:{
            return static_cast<gfx::TransformNode*>(this);
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_LIGHT:{
            ResLight resLight = ResDynamicCast<ResLight>(this->GetResSceneObject());
            NW_ASSERT(resLight.IsValid());

            ResLightData* ptr = resLight.ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_FRAGMENT_LIGHT:{
            NW_ASSERT(ut::IsTypeOf<gfx::FragmentLight>(this));
            ResFragmentLightData* ptr = static_cast<gfx::FragmentLight&>(*this).GetResFragmentLight().ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_AMBIENT_LIGHT:{
            NW_ASSERT(ut::IsTypeOf<gfx::AmbientLight>(this));
            ResAmbientLightData* ptr = static_cast<gfx::AmbientLight&>(*this).GetResAmbientLight().ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_VERTEX_LIGHT:{
            NW_ASSERT(ut::IsTypeOf<gfx::VertexLight>(this));
            ResVertexLightData* ptr = static_cast<gfx::VertexLight&>(*this).GetResVertexLight().ptr();
            return ptr;
        }

    case anim::ResAnimGroupMember::OBJECT_TYPE_HEMISPHERE_LIGHT:{
            NW_ASSERT(ut::IsTypeOf<gfx::HemiSphereLight>(this));
            ResHemiSphereLightData* ptr = static_cast<gfx::HemiSphereLight&>(*this).GetResHemiSphereLight().ptr();
            return ptr;
        }

    default:
        NW_ASSERT(false);
        return NULL;
    }
}

bool Light::ValidateLightAnimType(AnimObject* animObject){
    if (animObject == NULL){
        return true;
    }

    AnimBlender* blender = ut::DynamicCast<AnimBlender*>(animObject);

    if (blender != NULL){
        const int animObjCount = blender->GetAnimObjectCount();
        for (int i = 0; i < animObjCount; ++i){
            AnimObject* animObj = blender->GetAnimObject(i);

            if (!ValidateLightAnimType(animObj)){
                return false;
            }
        }

        return true;
    }
    else{
        AnimEvaluator* evaluator = ut::DynamicCast<AnimEvaluator*>(animObject);
        NW_NULL_ASSERT(evaluator);

        anim::ResLightAnim lightAnim(evaluator->GetAnimData().ptr());

        return ((lightAnim.GetLightType() == GetLightType()) && (lightAnim.GetLightKind() == GetLightKind()));
    }
}

}
}