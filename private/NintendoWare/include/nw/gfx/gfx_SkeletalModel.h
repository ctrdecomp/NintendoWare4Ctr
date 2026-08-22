/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_SKELETALMODEL_H_
#define NW_GFX_SKELETALMODEL_H_

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/gfx_StandardSkeleton.h>
#include <nw/gfx/gfx_AnimObject.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace anim
{
namespace res
{
class ResAnimGroupMember;
}
}
}

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SkeletalModel  : public Model
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SkeletalModel);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public Model::Description
    {
        Skeleton* sharedSkeleton; //

        //
        Description()
            : sharedSkeleton(NULL)
        {}
    };

    //
    //
    //
    class Builder
    {
    public:
        //
        Builder() {}

        //
        ~Builder() {}

        //
        //
        //
        //
        //
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        Builder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        //
        Builder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        //
        //
        //
        Builder& BufferOption(bit32 bufferOption)
        {
            m_Description.bufferOption = bufferOption;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        //
        //
        Builder& SharedMaterialModel(Model* model)
        {
            m_Description.sharedMaterialModel = model;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        Builder& SharedSkeleton(Skeleton* sharedSkeleton)
        {
            m_Description.sharedSkeleton = sharedSkeleton;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        Builder& SharedMeshNodeVisibilityModel(Model* model)
        {
            m_Description.sharedMeshNodeVisibilityModel = model;
            return *this;
        }

        //
        //
        //
        Builder& MaxAnimObjectsPerGroup(s32 maxAnimObjectCount)
        {
            m_Description.maxAnimObjectsPerGroup = maxAnimObjectCount;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        Builder& IsAnimationEnabled(bool isAnimationEnabled)
        {
            m_Description.isAnimationEnabled = isAnimationEnabled;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        //
        //
        //
        //
        SkeletalModel* Create(
            SceneNode* parent,
            ResSceneObject resource,
            os::IAllocator* allocator);

        //
        //
        //
        //
        size_t GetMemorySize(
            ResSkeletalModel resModel,
            size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size, resModel);

            return size.GetSizeWithPadding(alignment);
        }

        //
        //
        //
        size_t GetDeviceMemorySize(
            ResSkeletalModel,
            size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            NW_UNUSED_VARIABLE(alignment);

            return 0;
        }

        //
        void GetMemorySizeInternal(
            os::MemorySizeCalculator* pSize,
            ResSkeletalModel resModel) const
        {
            os::MemorySizeCalculator& size = *pSize;

            if (m_Description.sharedSkeleton == NULL)
            {
                ResSkeleton resSkeleton = resModel.GetSkeleton();

#ifdef NW_MOVE_ARRAY_CACHE_LINE_ALIGNMENT_ENABLED
                size.Add(
                    sizeof(Skeleton::TransformPose::Transform) * resSkeleton.GetBonesCount(),
                    Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
#else
                size += sizeof(Skeleton::TransformPose::Transform) * resSkeleton.GetBonesCount();
#endif
                StandardSkeleton::GetMemorySizeInternal(
                    pSize,
                    resSkeleton,
                    m_Description.maxCallbacks);
            }

            // Create model
            size += sizeof(SkeletalModel);
            SkeletalModel::GetMemorySizeForInitialize(pSize, resModel, m_Description);
        }

        //
        void GetDeviceMemorySizeInternal(
            os::MemorySizeCalculator*,
            ResSkeletalModel) const
        {
        }

    private:
        SkeletalModel::Description m_Description;
    };

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //

    //----------------------------------------
    //
    //

    //
    ResSkeletalModel GetResSkeletalModel()
    {
        return ResDynamicCast<ResSkeletalModel>(this->GetResSceneObject());
    }

    //
    const ResSkeletalModel GetResSkeletalModel() const
    {
        return ResDynamicCast<ResSkeletalModel>(this->GetResSceneObject());
    }

    //

    //----------------------------------------
    //
    //

    //
    Skeleton* GetSkeleton() { return m_Skeleton.Get(); }

    //
    const Skeleton* GetSkeleton() const { return m_Skeleton.Get(); }

    //
    bool IsSharingSkeleton() const { return this->m_SharingSkeleton; }

    //
    //
    //
    //
    //
    //
    //
    void SwapSkeleton( SkeletalModel* skeletalModel )
    {
        this->m_Skeleton.Swap(skeletalModel->m_Skeleton);
        bool sharingSkeleton = this->m_SharingSkeleton;
        this->m_SharingSkeleton = skeletalModel->m_SharingSkeleton;
        skeletalModel->m_SharingSkeleton = sharingSkeleton;
    }

    //

    //----------------------------------------
    //
    //

    //
    AnimGroup* GetSkeletalAnimGroup() { return m_SkeletalAnimGroup; }

    //
    const AnimGroup* GetSkeletalAnimGroup() const { return m_SkeletalAnimGroup; }

    //
    int GetSkeletalAnimBindingIndex() const { return m_SkeletalAnimBindingIndex; }

    //
    const AnimObject* GetSkeletalAnimObject(int objectIndex = 0) const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        NW_FAILSAFE_IF(m_SkeletalAnimGroup == NULL)
        {
            // The target animation group does not exist.
            return NULL;
        }
        return m_AnimBinding->GetAnimObject(m_SkeletalAnimBindingIndex, objectIndex);
    }

    //
    AnimObject* GetSkeletalAnimObject(int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        NW_FAILSAFE_IF(m_SkeletalAnimGroup == NULL)
        {
            // The target animation group does not exist.
            return NULL;
        }
        return m_AnimBinding->GetAnimObject(m_SkeletalAnimBindingIndex, objectIndex);
    }

    //
    //
    //
    void SetSkeletalAnimObject(AnimObject* animObject, int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        NW_FAILSAFE_IF(m_SkeletalAnimGroup == NULL)
        {
            // The target animation group does not exist.
            return;
        }
        m_AnimBinding->SetAnimObject(m_SkeletalAnimBindingIndex, animObject, objectIndex);
    }

    //
    bool GetFullBakedAnimEnabled() const
    {
        return m_FullBakedAnimEnabled;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    void SetFullBakedAnimEnabled(bool enable);

    //

protected:
    virtual Result Initialize(os::IAllocator* allocator);

    //
    //
    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        ResSkeletalModel resModel,
        Description description)
    {
        // SkeletalModel::Initialize
        os::MemorySizeCalculator& size = *pSize;

        Model::GetMemorySizeForInitialize(pSize, resModel, description);

        // SkeletalModel::CreateSkeletalAnimGroup
        if (description.isAnimationEnabled)
        {
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
                    AnimGroup::Builder()
                        .ResAnimGroup(resAnimGroup)
                        .UseOriginalValue(true)
                        .GetMemorySizeInternal(pSize);
                    break; // Because there is no other dedicated skeletal model animation group.
                }
            }
        }
    }

    //----------------------------------------
    //
    //

    //
    SkeletalModel(
        os::IAllocator* allocator,
        ResSkeletalModel resource,
        GfxPtr<Skeleton>& skeleton,
        bool isSharingSkeleton,
        const SkeletalModel::Description& description)
    : Model(
        allocator,
        resource,
        description),
      m_Skeleton(skeleton),
      m_SkeletalAnimGroup(NULL),
      m_SkeletalAnimBindingIndex(-1),
      m_SharingSkeleton(isSharingSkeleton),
      m_FullBakedAnimEnabled(false)
    {
        if (!isSharingSkeleton)
        {
            m_Skeleton->SetOwnerSkeletalModel(this);
        }
    }

    //
    virtual ~SkeletalModel()
    {
        SafeDestroy(m_SkeletalAnimGroup);
    }

    //

    //
    Result CreateSkeletalAnimGroup(os::IAllocator* allocator);

    //
    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

private:
    void SetupAnimGroup(AnimGroup* animGroup, bool fullBakedAnimEnabled) const;

    GfxPtr<Skeleton> m_Skeleton;
    AnimGroup* m_SkeletalAnimGroup;
    int m_SkeletalAnimBindingIndex;
    bool m_SharingSkeleton;
    bool m_FullBakedAnimEnabled;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_SKELETALMODEL_H_
