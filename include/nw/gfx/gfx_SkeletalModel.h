#pragma once

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/gfx_StandardSkeleton.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/anim/res/anim_ResAnimGroup.h>

namespace nw{
namespace gfx{

class SkeletalModel  : public Model
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SkeletalModel);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public Model::Description
    {
        Skeleton* sharedSkeleton;

        Description(): 
            sharedSkeleton(NULL) {}
    };

    class Builder
    {
    public:
        Builder() {}
        ~Builder() {}

        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        Builder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        Builder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        Builder& BufferOption(bit32 bufferOption)
        {
            m_Description.bufferOption = bufferOption;
            return *this;
        }

        Builder& SharedMaterialModel(Model* model)
        {
            m_Description.sharedMaterialModel = model;
            return *this;
        }

        Builder& SharedSkeleton(Skeleton* sharedSkeleton)
        {
            m_Description.sharedSkeleton = sharedSkeleton;
            return *this;
        }

        Builder& MaxAnimObjectsPerGroup(s32 maxAnimObjectCount)
        {
            m_Description.maxAnimObjectsPerGroup = maxAnimObjectCount;
            return *this;
        }

        Builder& IsAnimationEnabled(bool isAnimationEnabled)
        {
            m_Description.isAnimationEnabled = isAnimationEnabled;
            return *this;
        }

        SkeletalModel* Create(SceneNode* parent,ResSceneObject resource,nw::os::IAllocator* allocator);

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResSkeletalModel resModel) const
        {
            os::MemorySizeCalculator& size = *pSize;
             
            if (m_Description.sharedSkeleton == NULL)
            {
                ResSkeleton resSkeleton = resModel.GetSkeleton();
                size.Add(sizeof(Skeleton::TransformPose::Transform) * resSkeleton.GetBonesCount(), Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
                StandardSkeleton::GetMemorySizeInternal(pSize,resSkeleton,m_Description.maxCallbacks);
            }
        }

        size_t GetDeviceMemorySize(ResSkeletalModel,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            NW_UNUSED_VARIABLE(alignment);

            return 0;
        }

        void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator*,ResSkeletalModel) const {}

    private:
        SkeletalModel::Description m_Description;
    };

    virtual void Accept(ISceneVisitor* visitor);

    ResSkeletalModel GetResSkeletalModel() 
    {
        return ResDynamicCast<ResSkeletalModel>(this->GetResSceneObject());
    }

    const ResSkeletalModel GetResSkeletalModel() const 
    {
        return ResDynamicCast<ResSkeletalModel>(this->GetResSceneObject());
    }

    Skeleton* GetSkeleton() { return this->m_Skeleton.Get(); }

    const Skeleton* GetSkeleton() const { return this->m_Skeleton.Get(); }

    bool IsSharingSkeleton() const { return this->m_SharingSkeleton; }

    void SwapSkeleton( SkeletalModel* skeletalModel )
    {
        this->m_Skeleton.Swap(skeletalModel->m_Skeleton);
        bool sharingSkeleton = this->m_SharingSkeleton;
        this->m_SharingSkeleton = skeletalModel->m_SharingSkeleton;
        skeletalModel->m_SharingSkeleton = sharingSkeleton;
    }

    AnimGroup* GetSkeletalAnimGroup() { return m_SkeletalAnimGroup; }

    const AnimGroup* GetSkeletalAnimGroup() const { return m_SkeletalAnimGroup; }

    int GetSkeletalAnimBindingIndex() const { return m_SkeletalAnimBindingIndex; }

    const AnimObject* GetSkeletalAnimObject(int objectIndex = 0) const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(this->m_SkeletalAnimBindingIndex, objectIndex);
    }

    AnimObject* GetSkeletalAnimObject(int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(this->m_SkeletalAnimBindingIndex, objectIndex);
    }

    void SetSkeletalAnimObject(AnimObject* animObject, int objectIndex = 0)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        this->m_AnimBinding->SetAnimObject(this->m_SkeletalAnimBindingIndex, animObject, objectIndex);
    }

    bool GetFullBakedAnimEnabled() const
    {
        return m_FullBakedAnimEnabled;
    }

    void SetFullBakedAnimEnabled(bool enable);
    
protected:
    virtual Result Initialize(os::IAllocator* allocator);

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,ResSkeletalModel resModel,Description description)
    {

        os::MemorySizeCalculator& size = *pSize;

        Model::GetMemorySizeForInitialize(pSize, resModel, description);

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
                    break;
                }
            }
        }
    }

    SkeletalModel(nw::os::IAllocator* allocator,ResSkeletalModel resource,GfxPtr<Skeleton>& skeleton,bool isSharingSkeleton,const SkeletalModel::Description& description): 
        Model(allocator,resource,description),
        m_Skeleton(skeleton),
        m_SkeletalAnimGroup(NULL),
        m_SkeletalAnimBindingIndex(-1),
        m_SharingSkeleton(isSharingSkeleton),
        m_FullBakedAnimEnabled(false)
        {
        if (!isSharingSkeleton)
        {
            this->m_Skeleton->SetOwnerSkeletalModel(this);
        }
    }

    virtual ~SkeletalModel()
    {
        nw::ut::SafeDestroy(m_SkeletalAnimGroup);
    }

    Result CreateSkeletalAnimGroup(nw::os::IAllocator* allocator);

    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

private:
    void SetupAnimGroup(AnimGroup* animGroup, bool fullBakedAnimEnabled) const;

    GfxPtr<Skeleton> m_Skeleton;
    AnimGroup* m_SkeletalAnimGroup;
    int m_SkeletalAnimBindingIndex;
    bool m_SharingSkeleton;
    bool m_FullBakedAnimEnabled;
};

}
}