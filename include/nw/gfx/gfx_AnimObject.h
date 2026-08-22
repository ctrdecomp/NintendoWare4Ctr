#pragma once

#include <nw/anim/anim_AnimFrameController.h>
#include <nw/anim/res/anim_ResAnim.h>
#include <nw/anim/res/anim_ResAnimGroup.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_SharedAnimCache.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {
namespace internal{
    void ClearMaterialHash(anim::ResAnimGroupMember member);
}

class SceneNode;
class AnimEvaluator;
class TransformAnimEvaluator;


inline bool AnimWeightNearlyEqual(const float weight, const float value){
    const float Epsilon = 0.001f;
    return math::FAbs(weight - value) <= Epsilon;
}

inline bool AnimWeightNearlyEqualZero(const float weight){
    return AnimWeightNearlyEqual(weight, 0.0f);
}

inline bool AnimWeightNearlyEqualOne(const float weight){
    return AnimWeightNearlyEqual(weight, 1.0f);
}

inline float GetAnimWeightNormalizeScale(float weightSum){
    return AnimWeightNearlyEqualOne(weightSum) || AnimWeightNearlyEqualZero(weightSum) ? 1.0f : 1.0f / weightSum;
}


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
            
            AnimGroup* animGroup = new(buf) AnimGroup(this->mResAnimGroup, this->mSceneNode, allocator);

            Result result = animGroup->Initialize(this->mUseOriginalValue);
            
            if (result.IsSuccess()){
                return animGroup;
            }
            else{
                nw::ut::SafeDestroy(animGroup);
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

    anim::ResAnimGroupMember GetResAnimGroupMember(int memberIdx) const { return this->mResAnimGroup.GetMemberInfoSet(memberIdx); }

    anim::ResAnimGroupMember GetResAnimGroupMember(const char* key) const { return this->mResAnimGroup.GetMemberInfoSet(key); }

    int GetResAnimGroupMemberIndex(const char* key) const { return this->mResAnimGroup.GetMemberInfoSetIndex(key); }

    const char* GetName() const { return this->mResAnimGroup.GetName(); }

    s32 GetMemberCount() const { return this->mResAnimGroup.GetMemberInfoSetCount(); }

    const anim::AnimBlendOp* GetBlendOperationInGroup(int blendOpIdx) const{
        return mBlendOperations[blendOpIdx];
    }

    void SetBlendOperationInGroup(int blendOpIdx, anim::AnimBlendOp* blendOp){
        mBlendOperations[blendOpIdx] = blendOp;
    }

    const anim::AnimBlendOp* GetBlendOperation(int memberIdx) const{
        return this->GetBlendOperationInGroup(this->GetResAnimGroupMember(memberIdx).GetBlendOperationIndex());
    }

    SceneNode* GetSceneNode() const { return mSceneNode; }

    int GetTargetObjectIndex(int memberIdx) const{
        return mTargetObjectIndicies[memberIdx];
    }

    void SetTargetObjectIndex(int memberIdx, const int targetObjIdx){
        mTargetObjectIndicies[memberIdx] = targetObjIdx;
    }

    void* GetTargetObject(int memberIdx) const{
        return mTargetObjects[memberIdx];
    }

    void SetTargetObject(int memberIdx, void* object){
        mTargetObjects[memberIdx] = object;
    }

    void* GetTargetPtr(int memberIdx) const{
        return mTargetPtrs[memberIdx];
    }

    void SetTargetPtr(int memberIdx, void* target){
        mTargetPtrs[memberIdx] = target;
    }

    bool HasOriginalValue() const { return this->mOriginalValues.Size() != 0; }

    const void* GetOriginalValue(int memberIdx) const{
        return mOriginalValues[memberIdx];
    }

    void SetOriginalValue(int memberIdx, const void* value){
        mOriginalValues[memberIdx] = value;
    }

    PreEvaluateCallback GetPreEvaluateCallback() const { return mPreEvaluateCallback; }

    void SetPreEvaluateCallback(PreEvaluateCallback function) { mPreEvaluateCallback = function; }

    bool GetFullBakedAnimEnabled() const { return mFullBakedAnimEnabled; }

    void SetFullBakedAnimEnabled(bool enable) { mFullBakedAnimEnabled = enable; }

    void Reset();
protected:

    AnimGroup(anim::ResAnimGroup resAnimGroup,SceneNode* sceneNode,nw::os::IAllocator* allocator);

    virtual ~AnimGroup() {}
    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, const anim::ResAnimGroup resAnimGroup, bool useOriginalValue);

    Result Initialize(bool useOriginalValue);

private:
    anim::ResAnimGroup mResAnimGroup;
    ut::MoveArray<anim::AnimBlendOp*> mBlendOperations;

    SceneNode* mSceneNode;
    nw::ut::MoveArray<int> mTargetObjectIndicies;
    nw::ut::MoveArray<void*> mTargetObjects;
    nw::ut::MoveArray<void*> mTargetPtrs;
    nw::ut::MoveArray<const void*> mOriginalValues;

    PreEvaluateCallback mPreEvaluateCallback;

    bool mFullBakedAnimEnabled;
};


class AnimObject : public GfxObject{
public:
    NW_UT_RUNTIME_TYPEINFO;

    AnimObject(os::IAllocator* allocator, u32 animType): 
        GfxObject(allocator),
        mAnimGroup(NULL),
        mAnimType(animType) 
    {}
    virtual ~AnimObject() {}

    bool Bind(AnimGroup* animGroup){
        return TryBind(animGroup).IsSuccess();
    }

    virtual Result TryBind(AnimGroup* animGroup) = 0;
    virtual void Release() = 0;
    virtual void UpdateFrame() = 0;

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const = 0;

    const AnimGroup* GetAnimGroup() const { return mAnimGroup; }

    AnimGroup* GetAnimGroup() { return mAnimGroup; }

    void SetAnimGroup(AnimGroup* group) { mAnimGroup = group; }


    virtual bool HasMemberAnim(int memberIdx) const = 0;
    virtual void UpdateCache() = 0;

    enum {
        ANIMTYPE_SIMPLE,
        ANIMTYPE_TRANSFORM_SIMPLE,
        ANIMTYPE_BLENDER
    };

    u32 GetAnimType() const { return mAnimType; }

protected:
    AnimGroup* mAnimGroup;
    u32        mAnimType;
};

class AnimBinding : public GfxObject{
public:

    typedef nw::ut::MoveArray<AnimGroup*> AnimGroupArray;
    typedef nw::ut::MoveArray<AnimObject*> AnimObjectArray;

    class Builder{
    public:
        Builder(): 
            mMaxAnimGroups(1),
            mMaxAnimObjectsPerGroup(1) 
        {}

        Builder& MaxAnimGroups(int maxAnimGroups){
            NW_ASSERT(maxAnimGroups > 0);
            mMaxAnimGroups = maxAnimGroups;
            return *this;
        }

        Builder& MaxAnimObjectsPerGroup(int maxAnimObjects){
            NW_ASSERT(maxAnimObjects > 0);
            mMaxAnimObjectsPerGroup = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const{
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimBinding);
            AnimBinding::GetMemorySizeForInitialize(pSize, mMaxAnimGroups, mMaxAnimObjectsPerGroup);
        }

        AnimBinding* Create(nw::os::IAllocator* allocator){
            void* buf = allocator->Alloc(sizeof(AnimBinding));
            
            if (buf == NULL){
                return NULL;
            }

            AnimBinding* animBinding = new(buf) AnimBinding(allocator, this->mMaxAnimObjectsPerGroup);

            Result result = animBinding->Initialize(this->mMaxAnimGroups, this->mMaxAnimObjectsPerGroup);
            
            if (result.IsSuccess()){
                return animBinding;
            }
            else{
                nw::ut::SafeDestroy(animBinding);
                return NULL;
            }
        }

    private:
        int mMaxAnimGroups;
        int mMaxAnimObjectsPerGroup;
    };

    void Evaluate(anim::ResGraphicsAnimGroup::EvaluationTiming timing);

    void UpdateFrame(){
        for (int animObjIdx = 0; animObjIdx < this->mAnimObjects.Size(); ++animObjIdx){
            if (mAnimObjects[animObjIdx] != NULL){
                this->mAnimObjects[animObjIdx]->UpdateFrame();
            }
        }
    }

    int GetAnimGroupCount() const { return this->mAnimGroups.Size(); }

    const AnimGroup* GetAnimGroup(int groupIdx) const{
        return mAnimGroups[groupIdx];
    }

    AnimGroup* GetAnimGroup(int groupIdx){
        return mAnimGroups[groupIdx];
    }

    int GetAnimGroupIndex(const char* animGroupName) const{
        for (int groupIdx = 0; groupIdx < this->mAnimGroups.Size(); ++groupIdx){
            if (mAnimGroups[groupIdx] != NULL &&
                ::std::strcmp(this->mAnimGroups[groupIdx]->GetName(), animGroupName) == 0){
                return groupIdx;
            }
        }
        return -1;
    }

    void SetAnimGroup(int groupIdx, AnimGroup* animGroup){
        mAnimGroups[groupIdx] = animGroup;
    }

    const AnimObject* GetAnimObject(int groupIdx, int objectIdx = 0) const{
        const int index = groupIdx * mAnimObjectCountPerGroup + objectIdx;
        return mAnimObjects[index];
    }

    AnimObject* GetAnimObject(int groupIdx, int objectIdx = 0){
        const int index = groupIdx * mAnimObjectCountPerGroup + objectIdx;
        return mAnimObjects[index];
    }

    bool SetAnimObject(AnimObject* animObject, int objectIdx = 0){
        const AnimGroup* animGroup = animObject->GetAnimGroup();
        NW_NULL_ASSERT(animGroup);
        return SetAnimObject(animGroup->GetName(), animObject, objectIdx);
    }

    bool SetAnimObject(int groupIdx, AnimObject* animObject, int objectIdx = 0){
        if ((0 <= groupIdx && groupIdx < mAnimGroups.Size()) &&
            (0 <= objectIdx && objectIdx < mAnimObjectCountPerGroup)){
            const int index = groupIdx * mAnimObjectCountPerGroup + objectIdx;
            mAnimObjects[index] = animObject;
            return true;
        }
        return false;
    }

    bool SetAnimObject(const char* animGroupName, AnimObject* animObject, int objectIdx = 0){
        return SetAnimObject(GetAnimGroupIndex(animGroupName), animObject, objectIdx);
    }

protected:
    AnimBinding(nw::os::IAllocator* allocator,int maxAnimObjects): 
        GfxObject(allocator),
        mAnimObjectCountPerGroup(maxAnimObjects)
    {}

    virtual ~AnimBinding(){}

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimGroups, int maxAnimObjects){
        nw::os::MemorySizeCalculator& size = *pSize;

        const int animObjectCount = maxAnimGroups * maxAnimObjects;

        size += sizeof(AnimGroup*) * maxAnimGroups;
        size += sizeof(AnimObject*) * animObjectCount;
    }

    Result Initialize(int maxAnimGroups, int maxAnimObjects){

        const int animObjectCount = maxAnimGroups * maxAnimObjects;

        Result result = INITIALIZE_RESULT_OK;

        {
            void* memory = GetAllocator().Alloc(sizeof(AnimGroup*) * maxAnimGroups);
            if (memory == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            mAnimGroups = AnimGroupArray(memory, maxAnimGroups, &GetAllocator());
        }
        {
            void* memory = GetAllocator().Alloc(sizeof(AnimObject*) * animObjectCount);
            if (memory == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            mAnimObjects = AnimObjectArray(memory, animObjectCount, &GetAllocator());
        }
        
        for (int animGroupIdx = 0; animGroupIdx < maxAnimGroups; ++animGroupIdx){
            mAnimGroups.PushBackFast<AnimGroup*>(NULL);
        }

        for (int animObjectIdx = 0; animObjectIdx < animObjectCount; ++animObjectIdx){
            mAnimObjects.PushBackFast<AnimObject*>(NULL);
        }

        return result;
    }

private:

    NW_FORCE_INLINE void EvaluateSimple(AnimGroup* animGroup, AnimEvaluator* evaluator);
    NW_FORCE_INLINE void EvaluateTransformSimple(AnimGroup* animGroup, TransformAnimEvaluator* evaluator);

    NW_FORCE_INLINE void EvaluateBlender(AnimGroup* animGroup, AnimObject* animObj);

    NW_FORCE_INLINE void EvaluateTransformMember(AnimGroup* animGroup, int memberIdx, AnimObject* animObj);
    NW_FORCE_INLINE void EvaluateTransformMemberFast(AnimGroup* animGroup, int memberIdx, TransformAnimEvaluator* evaluator);
    NW_FORCE_INLINE void EvaluateMember(AnimGroup* animGroup, int memberIdx, AnimObject* animObj, int& lastTargetObjIdx, bool& targetObjSkipFlag);

    AnimGroupArray mAnimGroups;
    AnimObjectArray mAnimObjects;
    const int mAnimObjectCountPerGroup;
};

class BaseAnimEvaluator : public AnimObject{
public:
    NW_UT_RUNTIME_TYPEINFO;

    static const int NotFoundIndex;

    BaseAnimEvaluator(nw::os::IAllocator* allocator, u32 animType): 
        AnimObject(allocator, animType),
        mIsCacheDirty(true),
        mIsCacheExternal(false)
    {}

    virtual ~BaseAnimEvaluator() {}

    virtual Result TryBind(AnimGroup* animGroup);

    virtual void Release(){
        mAnimGroup = NULL;
    }

    virtual void UpdateFrame(){

        if (this->GetStepFrame() != 0.0f){
            mAnimFrameController.UpdateFrame();
            mIsCacheDirty = true;
        }
    }

    virtual void ChangeAnim(const anim::ResAnim animData){
        NW_NULL_ASSERT(mAnimGroup);

        NW_ASSERT(animData.GetMemberAnimSetCount() <= this->mReverseBindIndexTable.Capacity());

        SetResAnim(animData);

        AnimGroup* animGroup = mAnimGroup;
        this->Release();
        this->Bind(animGroup);
    }

    void Reset(){
        for (int i = (this->mAnimGroup->GetMemberCount() - 1); i >= 0; --i){
            this->ResetMember(i);
        }
    }

    void ResetMember(int memberIdx);

    float GetFrame() const { return this->mAnimFrameController.GetFrame(); }

    void SetFrame(float frame){
        this->mAnimFrameController.SetFrame(frame);
        mIsCacheDirty = true;
    }

    void ResetFrame(f32 frame){
        this->mAnimFrameController.GetAnimFrame().ResetFrame(frame);
        mIsCacheDirty = true;
    }

    float GetStepFrame() const { return this->mAnimFrameController.GetStepFrame(); }

    void SetStepFrame(float stepFrame){
        this->mAnimFrameController.SetStepFrame(stepFrame);
        if (stepFrame == 0.0f){
            mIsCacheDirty = true;
            UpdateCache();
        }
    }

    float GetStartFrame() const { return this->mAnimFrameController.GetStartFrame(); }

    void SetStartFrame(float startFrame){
        this->mAnimFrameController.SetStartFrame(startFrame);
        if (startFrame > GetFrame()){
            SetFrame(startFrame);
        }
    }

    float GetEndFrame() const { return this->mAnimFrameController.GetEndFrame(); }

    void SetEndFrame(float endFrame){
        this->mAnimFrameController.SetEndFrame(endFrame);
        if (endFrame < GetFrame()){
            SetFrame(endFrame);
        }
    }

    anim::AnimFrameController::PlayPolicy GetPlayPolicy() const{
        return this->mAnimFrameController.GetPlayPolicy();
    }

    void SetPlayPolicy(anim::AnimFrameController::PlayPolicy playPolicy){
        this->mAnimFrameController.SetPlayPolicy(playPolicy);
        mIsCacheDirty = true;
    }

    const anim::AnimFrameController& AnimFrameController() const { return mAnimFrameController; }

    const void* GetUserData() const { return this->mAnimFrameController.GetUserData(); }

    void* GetUserData() { return this->mAnimFrameController.GetUserData(); }

    void SetUserData(void* userData) { this->mAnimFrameController.SetUserData(userData); }

    const nw::ut::MoveArray<int>& BindIndexTable() const { return mBindIndexTable; }

    nw::ut::MoveArray<int>& BindIndexTable() { return mBindIndexTable; }

    const nw::ut::MoveArray<int>& ReverseBindIndexTable() const { return mReverseBindIndexTable; }

    nw::ut::MoveArray<int>& ReverseBindIndexTable() { return mReverseBindIndexTable; }

    const anim::ResAnim GetAnimData() const { return mAnimData; }

    virtual const void* GetCacheBuffer() const { return NULL; }
    virtual int GetCacheBufferSizeNeeded() const { return 0; }
    virtual void SetCacheBuffer(void* buf, int size){
        (void)buf;  
        (void)size; 
    }

protected:

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,const int maxMembers,const int maxAnimMembers){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(int) * maxMembers;
        size += sizeof(int) * maxAnimMembers;
    }

    virtual Result Initialize(const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers){
        Result result = INITIALIZE_RESULT_OK;

        this->SetResAnim(animData);

        {
            void* memory = GetAllocator().Alloc(sizeof(int) * maxMembers);
            if (memory == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            mBindIndexTable = nw::ut::MoveArray<int>(memory, maxMembers, &GetAllocator());
        }

        {
            void* memory = GetAllocator().Alloc(sizeof(int) * maxAnimMembers);
            if (memory == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);
    
            mReverseBindIndexTable = nw::ut::MoveArray<int>(memory, maxAnimMembers, &GetAllocator());
        }

        return result;
    }

    template <class T>
    Result TryBindTemplate(AnimGroup* animGroup, T indexGetter){
        NW_NULL_ASSERT(animGroup);
        NW_ASSERT(std::strcmp(mAnimData.GetTargetAnimGroupName(), animGroup->GetName()) == 0);
        NW_ASSERT(mAnimGroup == NULL);

        const int memberCount = animGroup->GetMemberCount();
        bool resultResize = this->mBindIndexTable.Resize(memberCount);
        NW_ASSERTMSG(resultResize, 
            "Member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxMembers.");

        const int animMemberCount = this->mAnimData.GetMemberAnimSetCount();
        bool resultReverseResize = this->mReverseBindIndexTable.Resize(animMemberCount);
        NW_ASSERTMSG(resultReverseResize, 
            "Animation member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxAnimMembers.");

        NW_ASSERTMSG(animGroup->GetFullBakedAnimEnabled() == mAnimData.IsFullBakedAnim(),
            "To use full baked animation, call SkeletalModel::SetFullBakedAnimEnabled(true) before Bind.");

        int boundAnimCount = 0;
        for (int memberIdx = 0; memberIdx < memberCount; ++memberIdx){
            mBindIndexTable[memberIdx] = NotFoundIndex;
        }

        for (int animIdx = 0; animIdx < animMemberCount; ++animIdx){
            anim::ResMemberAnim member = mAnimData.GetMemberAnimSet(animIdx);
            const int bindTargetIdx = indexGetter(animGroup, member);

            if (bindTargetIdx == -1){
                mReverseBindIndexTable[animIdx] = NotFoundIndex;
                continue;
            }

            mBindIndexTable[bindTargetIdx] = animIdx;
            mReverseBindIndexTable[animIdx] = bindTargetIdx;
            ++boundAnimCount;

            anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(bindTargetIdx);
            internal::ClearMaterialHash(resAnimGroupMember);
        }

        mAnimGroup = animGroup;

        if (boundAnimCount == this->mAnimData.GetMemberAnimSetCount()){
            return Result(BIND_RESULT_OK);
        }

        if (boundAnimCount == 0){
            return Result(BIND_RESULT_NO_MEMBER_BOUND | Result::MASK_FAIL_BIT);
        }

        return Result(BIND_RESULT_NOT_ALL_ANIM_MEMBER_BOUND);
    }

    nw::ut::MoveArray<int> mBindIndexTable;
    nw::ut::MoveArray<int> mReverseBindIndexTable;
    anim::AnimFrameController mAnimFrameController;
    anim::ResAnim mAnimData;
    bool mIsCacheDirty;
    bool mIsCacheExternal;
    bool mUseSharedCache;

private:
    class BasicIndexGetterFunctor{
    public:
        int operator() (AnimGroup* animGroup, anim::ResMemberAnim member){
            return animGroup->GetResAnimGroupMemberIndex(member.GetPath());
        }
    };
    void SetResAnim(const anim::ResAnim animData){
        mAnimData = animData;
        this->mAnimFrameController.SetStepFrame(1.0f);
        this->mAnimFrameController.SetStartFrame(0.0f);
        this->mAnimFrameController.SetEndFrame(animData.GetFrameSize());
        this->mAnimFrameController.GetAnimFrame().ResetFrame(0.0f);

        switch (animData.GetLoopMode()){
            case anim::ResAnimData::LOOP_MODE_ONETIME:
                this->mAnimFrameController.SetPlayPolicy(anim::PlayPolicy_Onetime);
                break;

            case anim::ResAnimData::LOOP_MODE_LOOP:
                this->mAnimFrameController.SetPlayPolicy(anim::PlayPolicy_Loop);
                break;

            default:
                NW_ASSERT(false);
        }
        mIsCacheDirty = true;
    }
};

class AnimEvaluator : public BaseAnimEvaluator{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder{
    public:

        Builder(): 
            mAnimData(NULL),
            mMaxMembers(64),
            mMaxAnimMembers(64),
            mAllocCache(false) 
        {}

        Builder& AnimData(const anim::ResAnim& animData) { mAnimData = animData; return *this; }

        Builder& MaxMembers(int maxMembers){
            NW_ASSERT(maxMembers > 0);
            mMaxMembers = maxMembers;
            return *this;
        }

        Builder& MaxAnimMembers(int maxAnimMembers){
            NW_ASSERT(maxAnimMembers > 0);
            mMaxAnimMembers = maxAnimMembers;
            return *this;
        }

        Builder& AllocCache(bool allocCache) { mAllocCache = allocCache; return *this; }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const{
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimEvaluator);
            AnimEvaluator::GetMemorySizeForInitialize(pSize, this->mAnimData, this->mMaxMembers, this->mMaxAnimMembers, this->mAllocCache);
        }

        AnimEvaluator* Create(nw::os::IAllocator* allocator){
            void* buf = allocator->Alloc(sizeof(AnimEvaluator));

            if (buf == NULL){
                return NULL;
            }

            AnimEvaluator* animEvaluator = new(buf) AnimEvaluator(allocator);

            Result result = animEvaluator->Initialize(this->mAnimData, this->mMaxMembers, this->mMaxAnimMembers, this->mAllocCache);
            NW_ASSERT(result.IsSuccess());

            return animEvaluator;
        }

    private:
        anim::ResAnim mAnimData;
        int mMaxMembers;
        int mMaxAnimMembers;
        bool mAllocCache;
    };

    virtual void ChangeAnim(const anim::ResAnim animData){
        BaseAnimEvaluator::ChangeAnim(animData);

        this->mCachePtrs.Resize(animData.GetMemberAnimSetCount());

        if (!mIsCacheExternal && mCacheBuf != NULL){
            nw::os::SafeFree(this->mCacheBuf, &GetAllocator());

            if (animData.GetMemberAnimSetCount() != 0){
                mCacheBuf = GetAllocator().Alloc(GetCacheBufferSizeNeeded());
                NW_NULL_ASSERT(this->mCacheBuf);

                SetCacheBufferPointers();
            }
        }
    }

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;
    virtual bool HasMemberAnim(int memberIdx) const{
        return mBindIndexTable[memberIdx] != NotFoundIndex;
    }

    virtual void UpdateCache() { this->UpdateCacheNonVirtual(); }
    
    void UpdateCacheNonVirtual();
    void UpdateCacheImpl();

    virtual int GetCacheBufferSizeNeeded() const;
    virtual const void* GetCacheBuffer() const { return mCacheBuf; }
    virtual void SetCacheBuffer(void* buf, int size){
        mCacheBuf = buf;
        if (buf != NULL){
            NW_ASSERT(size >= GetCacheBufferSizeNeeded());
            (void)size;
            mIsCacheDirty = true;
            mIsCacheExternal = true;
            this->SetCacheBufferPointers();
        }
    }

protected:
    AnimEvaluator( nw::os::IAllocator* allocator);

    virtual ~AnimEvaluator(){
        if (!mIsCacheExternal && mCacheBuf != NULL){
            GetAllocator().Free(this->mCacheBuf);
        }
    }


    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache);

    virtual Result Initialize(const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache);

    static int GetCacheBufferSizeNeeded(const anim::ResAnim& animData);

    void SetCacheBufferPointers();

    void* mCacheBuf;
    SharedAnimCache* mSharedCache;
    nw::ut::MoveArray<anim::AnimResult*> mCachePtrs;
};

class AnimBlender : public AnimObject{
public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::MoveArray<AnimObject*> AnimObjectArray;

    AnimBlender(nw::os::IAllocator* allocator): 
        AnimObject(allocator, ANIMTYPE_BLENDER)
    {}

    virtual ~AnimBlender() {}

    virtual Result TryBind(AnimGroup* animGroup){
        NW_NULL_ASSERT(animGroup);
        NW_ASSERT(this->mAnimGroup == NULL);
        mAnimGroup = animGroup;
        return Result(BIND_RESULT_OK);
    }

    virtual void Release(){
        mAnimGroup = NULL;
    }

    virtual void UpdateFrame(){
        for (int animObjIdx = 0; animObjIdx < this->mAnimObjects.Size(); ++animObjIdx){
            if (mAnimObjects[animObjIdx] != NULL){
                this->mAnimObjects[animObjIdx]->UpdateFrame();
            }
        }
    }

    virtual bool HasMemberAnim(int memberIdx) const{
        for (int animObjIdx = 0; animObjIdx < this->mAnimObjects.Size(); ++animObjIdx){
            if (this->mAnimObjects[animObjIdx] != NULL &&
                this->mAnimObjects[animObjIdx]->HasMemberAnim(memberIdx)){
                return true;
            }
        }
        return false;
    }

    void AddAnimObject(AnimObject* animObj){
        NW_ASSERT(this->mAnimObjects.Size() < this->mAnimObjects.Capacity());
        this->mAnimObjects.PushBack(animObj);
    }

    const AnimObject* GetAnimObject(int animObjIdx) const{
        return mAnimObjects[animObjIdx];
    }

    AnimObject* GetAnimObject(int animObjIdx){
        return mAnimObjects[animObjIdx];
    }

    AnimObject* ReplaceAnimObject(int animObjIdx, AnimObject* animObj){
        AnimObject* oldObj = mAnimObjects[animObjIdx];
        mAnimObjects[animObjIdx] = animObj;
        return oldObj;
    }

    void ClearAnimObjects(){
        this->mAnimObjects.Clear();
    }

    int GetAnimObjectCount() const{
        return this->mAnimObjects.Size();
    }

    int GetMaxAnimObjects() const{
        return this->mAnimObjects.Capacity();
    }

    virtual void UpdateCache(){
        for (int animObjIdx = 0; animObjIdx < this->mAnimObjects.Size(); ++animObjIdx){
            if (mAnimObjects[animObjIdx] != NULL){
                this->mAnimObjects[animObjIdx]->UpdateCache();
            }
        }
    }

protected:

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimObjects){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(AnimObject*) * maxAnimObjects;
    }

    virtual Result Initialize(int maxAnimObjects){
        Result result = INITIALIZE_RESULT_OK;

        void* memory = GetAllocator().Alloc(sizeof(AnimObject*) * maxAnimObjects);

        if (memory == NULL){
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        mAnimObjects = AnimObjectArray(memory, maxAnimObjects, &GetAllocator());

        return result;
    }

    AnimObjectArray mAnimObjects;
};

class AnimInterpolator : public AnimBlender{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder{
    public:
        Builder(): 
            mMaxAnimObjects(2),
            mIgnoreNoAnimMember(false) 
        {}

        Builder& MaxAnimObjects(int maxAnimObjects){
            NW_ASSERT(maxAnimObjects > 0);
            mMaxAnimObjects = maxAnimObjects;
            return *this;
        }

        Builder& IgnoreNoAnimMember(bool ignoreNoAnimMember) { mIgnoreNoAnimMember = ignoreNoAnimMember; return *this; }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const{
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimInterpolator);
            AnimInterpolator::GetMemorySizeForInitialize(pSize, this->mMaxAnimObjects);
        }

        AnimInterpolator* Create(os::IAllocator* allocator){
            void* buf = allocator->Alloc(sizeof(AnimInterpolator));

            if (buf == NULL){
                return NULL;
            }

            AnimInterpolator* animInterpolator = new(buf) AnimInterpolator(allocator);
            
            Result result = animInterpolator->Initialize(this->mMaxAnimObjects, this->mIgnoreNoAnimMember);
            NW_ASSERT(result.IsSuccess());

            return animInterpolator;
        }
    private:
        int mMaxAnimObjects;
        bool mIgnoreNoAnimMember;
    };

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;

    float GetWeight(int animObjIdx) const{
        return mWeights[animObjIdx];
    }

    void SetWeight(int animObjIdx, float weight){
        mWeights[animObjIdx] = weight;
        mNormalizedWeights[animObjIdx] = weight;
        mIsWeightDirty = true;
    }

    void SetNormalizationEnabled(bool enabled){ mIsWeightNormalizationEnabled = enabled; }

    bool GetNormalizationEnabled() const { return mIsWeightNormalizationEnabled; }

protected:
    AnimInterpolator(nw::os::IAllocator* allocator): 
        AnimBlender(allocator),
        mIsOldMethod(false),
        mIsWeightDirty(false),
        mIsWeightNormalizationEnabled(true)
    {}

    virtual ~AnimInterpolator() {}

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimObjects){
        nw::os::MemorySizeCalculator& size = *pSize;

        AnimBlender::GetMemorySizeForInitialize(pSize, maxAnimObjects);
        size += sizeof(float) * maxAnimObjects;
        size += sizeof(float) * maxAnimObjects;
    }

    virtual Result Initialize(int maxAnimObjects, bool ignoreNoAnimMember){
        Result result = AnimBlender::Initialize(maxAnimObjects);
        NW_ENSURE_AND_RETURN(result);

        {
            void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
            if (memory == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            mWeights = nw::ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
            for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx){
                this->mWeights.PushBackFast(animObjIdx == 0 ? 1.0f : 0.0f);
            }
        }

        {
            void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
            if (memory == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            mNormalizedWeights = nw::ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
            for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx){
                this->mNormalizedWeights.PushBackFast(animObjIdx == 0 ? 1.0f : 0.0f);
            }
        }

        mIsOldMethod = ignoreNoAnimMember;
        return result;
    }

    void NormalizeWeight() const{
        float weightSum = 0.0f;
        float normalizeScale;
        for (int i = 0; i < this->mWeights.Size(); ++i){
            weightSum += mWeights[i];
        }
        normalizeScale = GetAnimWeightNormalizeScale(weightSum);
        for (int i = 0; i < this->mWeights.Size(); ++i){
            mNormalizedWeights[i] = mWeights[i] * normalizeScale;
        }
        mIsWeightDirty = false;
    }

    nw::ut::MoveArray<float> mWeights;
    mutable nw::ut::MoveArray<float> mNormalizedWeights;

    bool mIsOldMethod;
    mutable bool mIsWeightDirty;
    bool mIsWeightNormalizationEnabled;
};

class AnimAdder : public AnimBlender{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder{
    public:
        Builder(): 
            mMaxAnimObjects(2) 
        {}

        Builder& MaxAnimObjects(int maxAnimObjects){
            NW_ASSERT(maxAnimObjects > 0);
            mMaxAnimObjects = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const{
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimAdder);
            AnimAdder::GetMemorySizeForInitialize(pSize, this->mMaxAnimObjects);
        }

        AnimAdder* Create(nw::os::IAllocator* allocator){
            void* buf = allocator->Alloc(sizeof(AnimAdder));

            if (buf == NULL){
                return NULL;
            }

            AnimAdder* animAdder = new(buf) AnimAdder(allocator);

            Result result = animAdder->Initialize(this->mMaxAnimObjects);
            NW_ASSERT(result.IsSuccess());

            return animAdder;
        }

    private:
        int mMaxAnimObjects;
    };

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;

    float GetWeight(int animObjIdx) const{
        return mWeights[animObjIdx];
    }

    void SetWeight(int animObjIdx, float weight){
        mWeights[animObjIdx] = weight;
    }

protected:
    AnimAdder(nw::os::IAllocator* allocator):
        AnimBlender(allocator)
    {}

    virtual ~AnimAdder(){ }

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimObjects){
        nw::os::MemorySizeCalculator& size = *pSize;

        AnimBlender::GetMemorySizeForInitialize(pSize, maxAnimObjects);
        size += sizeof(float) * maxAnimObjects;
    }

    virtual Result Initialize(int maxAnimObjects){
        Result result = AnimBlender::Initialize(maxAnimObjects);
        NW_ENSURE_AND_RETURN(result);

        void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
        if (memory == NULL){
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        this->mWeights = nw::ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
        for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx){
            this->mWeights.PushBackFast(1.0f);
        }

        return result;
    }

    nw::ut::MoveArray<float> mWeights;
};

class AnimOverrider : public AnimBlender{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder{
    public:
        Builder(): 
            mMaxAnimObjects(2) 
        {}

        Builder& MaxAnimObjects(int maxAnimObjects){
            NW_ASSERT(maxAnimObjects > 0);
            mMaxAnimObjects = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const{
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimOverrider);
            AnimOverrider::GetMemorySizeForInitialize(pSize, this->mMaxAnimObjects);
        }

        AnimOverrider* Create(os::IAllocator* allocator){
            void* buf = allocator->Alloc(sizeof(AnimOverrider));

            if (buf == NULL){
                return NULL;
            }
            
            AnimOverrider* animOverrider = new(buf) AnimOverrider(allocator);

            Result result = animOverrider->Initialize(this->mMaxAnimObjects);
            NW_ASSERT(result.IsSuccess());
            
            return animOverrider;
        }
    private:
        int mMaxAnimObjects;
    };

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;
protected:

    AnimOverrider(nw::os::IAllocator* allocator): 
        AnimBlender(allocator)
    {}

    virtual ~AnimOverrider() {}
};

}
}