

#include <nw/anim/res/anim_ResAnimGroup.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw{
namespace anim{
    class AnimBlendOp;
}

namespace gfx {

class SceneNode;

class AnimGroup : public GfxObject{
public:
    typedef bool (*PreEvaluateCallback)(AnimGroup* animGroup, int targetObjIdx);

    class Builder{
    public:
        Builder(): 
            mSceneNode(NULL),
            mUseOriginalValue(false) 
        {}

        Builder& ResAnimGroup(anim::ResAnimGroup resAnimGroup){
            mResAnimGroup = resAnimGroup;
            return *this;
        }

        Builder& SetSceneNode(SceneNode* sceneNode) { mSceneNode = sceneNode; return *this; }

        Builder& UseOriginalValue(bool use) { mUseOriginalValue = use; return *this; }

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const{
            os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const{
            os::MemorySizeCalculator& size = *pSize;

            size += sizeof(AnimGroup);
            AnimGroup::GetMemorySizeForInitialize(pSize, this->mResAnimGroup, this->mUseOriginalValue);
        }

        AnimGroup* Create(os::IAllocator* allocator){
            void* buf = allocator->Alloc(sizeof(AnimGroup));
            
            if (buf == NULL){
                return NULL;
            }
            
            AnimGroup* animGroup = new(buf) AnimGroup(mResAnimGroup, mSceneNode, allocator);

            Result result = animGroup->Initialize(mUseOriginalValue);
            
            if (result.IsSuccess()){
                return animGroup;
            }
            else{
                SafeDestroy(animGroup);
                return NULL;
            }
        }

    private:
        SceneNode* mSceneNode;
        anim::ResAnimGroup mResAnimGroup;
        bool mUseOriginalValue;
    };

    anim::ResAnimGroup GetResAnimGroup() const { return mResAnimGroup; }

    anim::ResGraphicsAnimGroup GetResGraphicsAnimGroup() const{
        return *reinterpret_cast<const anim::ResGraphicsAnimGroup*>(&this->mResAnimGroup);
    }

    anim::ResAnimGroupMember GetResAnimGroupMember(int memberIdx) const { return mResAnimGroup.GetMemberInfoSet(memberIdx); }

    anim::ResAnimGroupMember GetResAnimGroupMember(const char* key) const { return mResAnimGroup.GetMemberInfoSet(key); }

    int GetResAnimGroupMemberIndex(const char* key) const { return mResAnimGroup.GetMemberInfoSetIndex(key); }

    const char* GetName() const { return mResAnimGroup.GetName(); }

    s32 GetMemberCount() const { return mResAnimGroup.GetMemberInfoSetCount(); }

    const anim::AnimBlendOp* GetBlendOperationInGroup(int blendOpIdx) const{
        NW_MINMAXLT_ASSERT(blendOpIdx, 0, mBlendOperations.Size());
        return mBlendOperations[blendOpIdx];
    }

    void SetBlendOperationInGroup(int blendOpIdx, anim::AnimBlendOp* blendOp){
        NW_MINMAXLT_ASSERT(blendOpIdx, 0, mBlendOperations.Size());
        mBlendOperations[blendOpIdx] = blendOp;
    }

    const anim::AnimBlendOp* GetBlendOperation(int memberIdx) const{
        return GetBlendOperationInGroup(GetResAnimGroupMember(memberIdx).GetBlendOperationIndex());
    }

    SceneNode* GetSceneNode() const { return mSceneNode; }

    int GetTargetObjectIndex(int memberIdx) const{
        NW_MINMAXLT_ASSERT(memberIdx, 0, mTargetObjectIndicies.Size());
        return mTargetObjectIndicies[memberIdx];
    }

    void SetTargetObjectIndex(int memberIdx, const int targetObjIdx){
        NW_MINMAXLT_ASSERT(memberIdx, 0, mTargetObjectIndicies.Size());
        mTargetObjectIndicies[memberIdx] = targetObjIdx;
    }

    void* GetTargetObject(int memberIdx) const{
        NW_MINMAXLT_ASSERT(memberIdx, 0, mTargetObjects.Size());
        return mTargetObjects[memberIdx];
    }

    void SetTargetObject(int memberIdx, void* object){
        NW_MINMAXLT_ASSERT(memberIdx, 0, mTargetObjects.Size());
        mTargetObjects[memberIdx] = object;
    }

    void* GetTargetPtr(int memberIdx) const{
        NW_MINMAXLT_ASSERT(memberIdx, 0, mTargetPtrs.Size());
        return mTargetPtrs[memberIdx];
    }

    void SetTargetPtr(int memberIdx, void* target){
        NW_MINMAXLT_ASSERT(memberIdx, 0, mTargetPtrs.Size());
        mTargetPtrs[memberIdx] = target;
    }

    bool HasOriginalValue() const { return mOriginalValues.Size() != 0; }

    const void* GetOriginalValue(int memberIdx) const{
        NW_MINMAXLT_ASSERT(memberIdx, 0, mOriginalValues.Size());
        return mOriginalValues[memberIdx];
    }

    void SetOriginalValue(int memberIdx, const void* value){
        NW_MINMAXLT_ASSERT(memberIdx, 0, mOriginalValues.Size());
        mOriginalValues[memberIdx] = value;
    }

    PreEvaluateCallback GetPreEvaluateCallback() const { return mPreEvaluateCallback; }
    void SetPreEvaluateCallback(PreEvaluateCallback function) { mPreEvaluateCallback = function; }

    bool GetFullBakedAnimEnabled() const { return mFullBakedAnimEnabled; }
    void SetFullBakedAnimEnabled(bool enable) { mFullBakedAnimEnabled = enable; }

    void Reset();

protected:
    AnimGroup(anim::ResAnimGroup resAnimGroup,SceneNode* sceneNode,os::IAllocator* allocator);
    virtual ~AnimGroup() {}

    static void GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize, const anim::ResAnimGroup resAnimGroup, bool useOriginalValue);
    Result Initialize(bool useOriginalValue);
private:
    anim::ResAnimGroup mResAnimGroup;
    ut::MoveArray<anim::AnimBlendOp*> mBlendOperations;
    SceneNode* mSceneNode;
    ut::MoveArray<int> mTargetObjectIndicies;
    ut::MoveArray<void*> mTargetObjects;
    ut::MoveArray<void*> mTargetPtrs;
    ut::MoveArray<const void*> mOriginalValues;
    PreEvaluateCallback mPreEvaluateCallback;
    bool mFullBakedAnimEnabled;
};

}
}