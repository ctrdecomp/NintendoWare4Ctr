// Filename: gfx_SkeletonModel.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_AnimObject.h>

#include <nw/anim/res/anim_ResAnimGroup.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Foreach.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION( SkeletalModel, Model );

SkeletalModel* SkeletalModel::Builder::Create(SceneNode* parent,ResSceneObject resource,os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    ResSkeletalModel resModel = ResStaticCast<ResSkeletalModel>(resource);

    GfxPtr<Skeleton> skeleton;
    bool isSharedSkeleton = true;
    if (m_Description.sharedSkeleton != NULL)
    {
        skeleton.Reset(this->m_Description.sharedSkeleton, false);
        isSharedSkeleton = true;
    }
    else
    {
        ResSkeleton resSkeleton = resModel.GetSkeleton();
        NW_ASSERT(resSkeleton.IsValid());
        Skeleton::TransformPose::TransformArray poseTransforms(resSkeleton.GetBonesCount(), allocator);
        ResBoneArray bones = resSkeleton.GetBones();
        ResBoneArray::iterator bonesEnd = bones.end();
        for (ResBoneArray::iterator bone = bones.begin(); bone != bonesEnd; ++bone)
        {
            poseTransforms.PushBackFast(*bone);
        }
        skeleton.Reset(StandardSkeleton::Create(resSkeleton,m_Description.maxCallbacks,
            m_Description.isFixedSizeMemory,
            poseTransforms,
            allocator));

        isSharedSkeleton = false;
    }

    void* memory = allocator->Alloc(sizeof(SkeletalModel));
    NW_NULL_ASSERT(memory);
    
    SkeletalModel* model = new(memory) SkeletalModel(allocator,resModel,skeleton,
        isSharedSkeleton,
        this->m_Description);

    Result result = model->Initialize(allocator);
    NW_ASSERT(result.IsSuccess());

    if (parent)
    {
        bool isAttached = parent->AttachChild(model);
        NW_ASSERT(isAttached);
    }

    return model;
}

void SkeletalModel::Accept(ISceneVisitor* visitor)
{
    visitor->VisitSkeletalModel(this);
    AcceptChildren(visitor);
}

void SkeletalModel::SetFullBakedAnimEnabled(bool enable)
{
    m_FullBakedAnimEnabled = enable;

    if (enable)
    {
        this->m_Skeleton->GetResSkeleton().EnableFlags(ResSkeletonData::FLAG_MODEL_COORDINATE);
    }
    else{
        this->m_Skeleton->GetResSkeleton().DisableFlags(ResSkeletonData::FLAG_MODEL_COORDINATE);
    }

    NW_NULL_ASSERT(this->m_SkeletalAnimGroup);
    this->SetupAnimGroup(this->m_SkeletalAnimGroup, enable);
}

Result SkeletalModel::CreateSkeletalAnimGroup(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    AnimBinding* animBinding = GetAnimBinding();
    if (animBinding == NULL)
    {
        return result;
    }

    ResSceneObject resSceneObject = GetResSceneObject();
    ResSkeletalModel resModel = *reinterpret_cast<ResSkeletalModel*>(&resSceneObject);
    NW_ASSERT(resModel.IsValid());

    ResSkeleton resSkeleton = resModel.GetSkeleton();
    Skeleton::TransformPose& pose = this->m_Skeleton->LocalTransformPose();
    Skeleton::OriginalPose& originalPose = this->m_Skeleton->LocalOriginalPose();

    const int animGroupCount = resModel.GetAnimGroupsCount();
    for (int animGroupIdx = 0; animGroupIdx < animGroupCount; ++animGroupIdx)
    {
        anim::ResAnimGroup resAnimGroup = resModel.GetAnimGroups(animGroupIdx);
        const int targetType = resAnimGroup.GetTargetType();
        const bool transformFlag = 
            (resAnimGroup.GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM) != 0;
        if (transformFlag &&
            targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_BONE)
            {
            AnimGroup* animGroup = AnimGroup::Builder()
                .ResAnimGroup(resAnimGroup)
                .SetSceneNode(this)
                .UseOriginalValue(true)
                .Create(allocator);

            if (animGroup == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }

            NW_ENSURE_AND_RETURN(result);

            const int animMemberCount = animGroup->GetMemberCount();
            for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx)
            {
                anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(memberIdx);

                ResBone bone = resSkeleton.GetBones(resAnimGroupMember.GetPath());
                NW_ASSERT(bone.IsValid());
                const int boneIdx = bone.GetIndex();
                animGroup->SetTargetObjectIndex(memberIdx, boneIdx);
                animGroup->SetOriginalValue(memberIdx, originalPose.GetTransform(boneIdx));

                void* object = GetAnimTargetObject(resAnimGroupMember);
                animGroup->SetTargetObject(memberIdx, object);
            }
            animBinding->SetAnimGroup(animGroupIdx, animGroup);
            m_SkeletalAnimGroup = animGroup;
            m_SkeletalAnimBindingIndex = animGroupIdx;

            SetupAnimGroup(animGroup, this->m_FullBakedAnimEnabled);

            break;
        }
    }

    return result;
}

void* SkeletalModel::GetAnimTargetObject(const anim::ResAnimGroupMember& anim)
{
    switch (anim.GetObjectType())
    {
    case anim::ResAnimGroupMember::OBJECT_TYPE_BONE:{
            anim::ResBoneMember member = ResStaticCast<anim::ResBoneMember>(anim);
            const char* boneName = member.GetBoneName();

            const int boneIndex =
                GetResSkeletalModel().GetSkeleton().GetBones(boneName).GetIndex();

            CalculatedTransform* ptr = GetSkeleton()->LocalTransformPose().GetTransform(boneIndex);
            return ptr;
        }

    default:
        NW_ASSERT(false);
        return NULL;
    }
}

Result SkeletalModel::Initialize(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= Model::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    if (!m_SharingSkeleton)
    {
        result |= CreateSkeletalAnimGroup(allocator);
        NW_ENSURE_AND_RETURN(result);
    }

    return result;
}

void SkeletalModel::SetupAnimGroup(AnimGroup* animGroup, bool fullBakedAnimEnabled) const
{
    const int animMemberCount = animGroup->GetMemberCount();
    for (int memberIdx = 0; memberIdx < animMemberCount; ++memberIdx)
    {
        anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(memberIdx);

        ResBone bone = this->m_Skeleton->GetResSkeleton().GetBones(resAnimGroupMember.GetPath());
        NW_ASSERT(bone.IsValid());
        const int boneIdx = bone.GetIndex();

        if (fullBakedAnimEnabled)
        {
            void* target = this->m_Skeleton->WorldMatrixPose().GetMatrix(boneIdx);
            animGroup->SetTargetPtr(memberIdx, target);
        }
        else{
            void* target = this->m_Skeleton->LocalTransformPose().GetTransform(boneIdx);
            animGroup->SetTargetPtr(memberIdx, target);
        }
    }

    this->m_SkeletalAnimGroup->SetFullBakedAnimEnabled(fullBakedAnimEnabled);
}

}
}