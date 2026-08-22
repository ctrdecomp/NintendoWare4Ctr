#pragma once

#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/gfx_StandardSkeleton.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/anim/res/anim_ResAnimGroup.h>

namespace nw{
namespace gfx{

class SkeletalModel  : public Model{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SkeletalModel);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public Model::Description{
        Skeleton* sharedSkeleton;

        Description(): 
            sharedSkeleton(NULL)
        {}
    };

    class Builder{
    public:
        Builder() {}
        ~Builder() {}

        Builder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        Builder& MaxChildren(int maxChildren){
            mDescription.maxChildren = maxChildren;
            return *this;
        }

        Builder& MaxCallbacks(int maxCallbacks){
            mDescription.maxCallbacks = maxCallbacks;
            return *this;
        }

        Builder& BufferOption(bit32 bufferOption){
            mDescription.bufferOption = bufferOption;
            return *this;
        }

        Builder& SharedMaterialModel(Model* model){
            mDescription.sharedMaterialModel = model;
            return *this;
        }

        Builder& SharedSkeleton(Skeleton* sharedSkeleton){
            mDescription.sharedSkeleton = sharedSkeleton;
            return *this;
        }

        Builder& MaxAnimObjectsPerGroup(s32 maxAnimObjectCount){
            mDescription.maxAnimObjectsPerGroup = maxAnimObjectCount;
            return *this;
        }

        Builder& IsAnimationEnabled(bool isAnimationEnabled){
            mDescription.isAnimationEnabled = isAnimationEnabled;
            return *this;
        }

        SkeletalModel* Create(SceneNode* parent,ResSceneObject resource,nw::os::IAllocator* allocator);

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResSkeletalModel resModel) const{
            os::MemorySizeCalculator& size = *pSize;
             
            if (mDescription.sharedSkeleton == NULL){
                ResSkeleton resSkeleton = resModel.GetSkeleton();
                size.Add(sizeof(Skeleton::TransformPose::Transform) * resSkeleton.GetBonesCount(), Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
                StandardSkeleton::GetMemorySizeInternal(pSize,resSkeleton,mDescription.maxCallbacks);
            }
        }

        size_t GetDeviceMemorySize(ResSkeletalModel,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            NW_UNUSED_VARIABLE(alignment);

            return 0;
        }

        void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator*,ResSkeletalModel) const
        {}

    private:
        SkeletalModel::Description mDescription;
    };

    virtual void Accept(ISceneVisitor* visitor);

    ResSkeletalModel GetResSkeletalModel() {
        return ResDynamicCast<ResSkeletalModel>(this->GetResSceneObject());
    }

    const ResSkeletalModel GetResSkeletalModel() const {
        return ResDynamicCast<ResSkeletalModel>(this->GetResSceneObject());
    }

    Skeleton* GetSkeleton() { return this->mSkeleton.Get(); }

    const Skeleton* GetSkeleton() const { return this->mSkeleton.Get(); }

    bool IsSharingSkeleton() const { return this->mSharingSkeleton; }

    void SwapSkeleton( SkeletalModel* skeletalModel ){
        this->mSkeleton.Swap(skeletalModel->mSkeleton);
        bool sharingSkeleton = this->mSharingSkeleton;
        this->mSharingSkeleton = skeletalModel->mSharingSkeleton;
        skeletalModel->mSharingSkeleton = sharingSkeleton;
    }

    AnimGroup* GetSkeletalAnimGroup() { return mSkeletalAnimGroup; }

    const AnimGroup* GetSkeletalAnimGroup() const { return mSkeletalAnimGroup; }

    int GetSkeletalAnimBindingIndex() const { return mSkeletalAnimBindingIndex; }

    const AnimObject* GetSkeletalAnimObject(int objectIndex = 0) const{
        NW_NULL_ASSERT(this->mAnimBinding);
        return this->mAnimBinding->GetAnimObject(this->mSkeletalAnimBindingIndex, objectIndex);
    }

    AnimObject* GetSkeletalAnimObject(int objectIndex = 0){
        NW_NULL_ASSERT(this->mAnimBinding);
        return this->mAnimBinding->GetAnimObject(this->mSkeletalAnimBindingIndex, objectIndex);
    }

    void SetSkeletalAnimObject(AnimObject* animObject, int objectIndex = 0){
        NW_NULL_ASSERT(mAnimBinding);
        this->mAnimBinding->SetAnimObject(this->mSkeletalAnimBindingIndex, animObject, objectIndex);
    }

    bool GetFullBakedAnimEnabled() const{
        return mFullBakedAnimEnabled;
    }

    void SetFullBakedAnimEnabled(bool enable);
    
protected:
    virtual Result Initialize(os::IAllocator* allocator);

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,ResSkeletalModel resModel,Description description){

        os::MemorySizeCalculator& size = *pSize;

        Model::GetMemorySizeForInitialize(pSize, resModel, description);

        if (description.isAnimationEnabled){
            const int animGroupCount = resModel.GetAnimGroupsCount();
            for (int animGroupIdx = 0; animGroupIdx < animGroupCount; ++animGroupIdx){
                anim::ResAnimGroup resAnimGroup = resModel.GetAnimGroups(animGroupIdx);
                const int targetType = resAnimGroup.GetTargetType();
                const bool transformFlag = 
                    (resAnimGroup.GetFlags() & anim::ResAnimGroup::FLAG_IS_CALCULATED_TRANSFORM) != 0;
                if (transformFlag &&
                    targetType == anim::ResGraphicsAnimGroup::TARGET_TYPE_BONE){
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
        mSkeleton(skeleton),
        mSkeletalAnimGroup(NULL),
        mSkeletalAnimBindingIndex(-1),
        mSharingSkeleton(isSharingSkeleton),
        mFullBakedAnimEnabled(false){
        if (!isSharingSkeleton){
            this->mSkeleton->SetOwnerSkeletalModel(this);
        }
    }

    virtual ~SkeletalModel(){
        nw::ut::SafeDestroy(mSkeletalAnimGroup);
    }

    Result CreateSkeletalAnimGroup(nw::os::IAllocator* allocator);

    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

private:
    void SetupAnimGroup(AnimGroup* animGroup, bool fullBakedAnimEnabled) const;

    GfxPtr<Skeleton> mSkeleton;
    AnimGroup* mSkeletalAnimGroup;
    int mSkeletalAnimBindingIndex;
    bool mSharingSkeleton;
    bool mFullBakedAnimEnabled;
};

}
}