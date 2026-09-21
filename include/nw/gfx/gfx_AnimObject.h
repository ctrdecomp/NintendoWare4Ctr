#pragma once

#include <nw/anim/anim_AnimFrameController.h>
#include <nw/anim/res/anim_ResAnim.h>
#include <nw/gfx/gfx_AnimGroup.h>
#include <nw/gfx/gfx_SharedAnimCache.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {
namespace internal{
    void ClearMaterialHash(anim::ResAnimGroupMember member);
}

class SceneNode;
class AnimEvaluator;
class TransformAnimEvaluator;


inline bool AnimWeightNearlyEqual(const float weight, const float value)
{
    const float Epsilon = 0.001f;
    return math::FAbs(weight - value) <= Epsilon;
}

inline bool AnimWeightNearlyEqualZero(const float weight)
{
    return AnimWeightNearlyEqual(weight, 0.0f);
}

inline bool AnimWeightNearlyEqualOne(const float weight)
{
    return AnimWeightNearlyEqual(weight, 1.0f);
}

inline float GetAnimWeightNormalizeScale(float weightSum)
{
    return AnimWeightNearlyEqualOne(weightSum) || AnimWeightNearlyEqualZero(weightSum) ? 1.0f : 1.0f / weightSum;
}

class AnimObject : public GfxObject
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    AnimObject(os::IAllocator* allocator, u32 animType): 
        GfxObject(allocator),
        m_AnimGroup(NULL),
        m_AnimType(animType) {}
    virtual ~AnimObject() {}

    bool Bind(AnimGroup* animGroup)
    {
        return TryBind(animGroup).IsSuccess();
    }

    virtual Result TryBind(AnimGroup* animGroup) = 0;
    virtual void Release() = 0;
    virtual void UpdateFrame() = 0;

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const = 0;

    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    void SetAnimGroup(AnimGroup* group) { m_AnimGroup = group; }


    virtual bool HasMemberAnim(int memberIdx) const = 0;
    virtual void UpdateCache() = 0;

    enum
    {
        ANIMTYPE_SIMPLE,
        ANIMTYPE_TRANSFORM_SIMPLE,
        ANIMTYPE_BLENDER
    };

    u32 GetAnimType() const { return m_AnimType; }

protected:
    AnimGroup* m_AnimGroup;
    u32        m_AnimType;
};

class AnimBinding : public GfxObject
{
public:

    typedef nw::ut::MoveArray<AnimGroup*> AnimGroupArray;
    typedef nw::ut::MoveArray<AnimObject*> AnimObjectArray;

    class Builder
    {
    public:
        Builder(): 
            m_MaxAnimGroups(1),
            m_MaxAnimObjectsPerGroup(1) {}

        Builder& MaxAnimGroups(int maxAnimGroups)
        {
            NW_ASSERT(maxAnimGroups > 0);
            m_MaxAnimGroups = maxAnimGroups;
            return *this;
        }

        Builder& MaxAnimObjectsPerGroup(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjectsPerGroup = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const
        {
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimBinding);
            AnimBinding::GetMemorySizeForInitialize(pSize, m_MaxAnimGroups, m_MaxAnimObjectsPerGroup);
        }

        AnimBinding* Create(nw::os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimBinding));
            
            if (buf == NULL)
            {
                return NULL;
            }

            AnimBinding* animBinding = new(buf) AnimBinding(allocator, this->m_MaxAnimObjectsPerGroup);

            Result result = animBinding->Initialize(this->m_MaxAnimGroups, this->m_MaxAnimObjectsPerGroup);
            
            if (result.IsSuccess())
            {
                return animBinding;
            }
            else
            {
                nw::ut::SafeDestroy(animBinding);
                return NULL;
            }
        }

    private:
        int m_MaxAnimGroups;
        int m_MaxAnimObjectsPerGroup;
    };

    void Evaluate(anim::ResGraphicsAnimGroup::EvaluationTiming timing);

    void UpdateFrame()
    {
        for (int animObjIdx = 0; animObjIdx < this->m_AnimObjects.Size(); ++animObjIdx)
        {
            if (m_AnimObjects[animObjIdx] != NULL)
            {
                this->m_AnimObjects[animObjIdx]->UpdateFrame();
            }
        }
    }

    int GetAnimGroupCount() const { return this->m_AnimGroups.Size(); }

    const AnimGroup* GetAnimGroup(int groupIdx) const
    {
        return m_AnimGroups[groupIdx];
    }

    AnimGroup* GetAnimGroup(int groupIdx)
    {
        return m_AnimGroups[groupIdx];
    }

    int GetAnimGroupIndex(const char* animGroupName) const
    {
        for (int groupIdx = 0; groupIdx < this->m_AnimGroups.Size(); ++groupIdx)
        {
            if (m_AnimGroups[groupIdx] != NULL &&
                ::std::strcmp(this->m_AnimGroups[groupIdx]->GetName(), animGroupName) == 0)
            {
                return groupIdx;
            }
        }
        return -1;
    }

    void SetAnimGroup(int groupIdx, AnimGroup* animGroup)
    {
        m_AnimGroups[groupIdx] = animGroup;
    }

    const AnimObject* GetAnimObject(int groupIdx, int objectIdx = 0) const
    {
        const int index = groupIdx * m_AnimObjectCountPerGroup + objectIdx;
        return m_AnimObjects[index];
    }

    AnimObject* GetAnimObject(int groupIdx, int objectIdx = 0)
    {
        const int index = groupIdx * m_AnimObjectCountPerGroup + objectIdx;
        return m_AnimObjects[index];
    }

    bool SetAnimObject(AnimObject* animObject, int objectIdx = 0)
    {
        const AnimGroup* animGroup = animObject->GetAnimGroup();
        NW_NULL_ASSERT(animGroup);
        return SetAnimObject(animGroup->GetName(), animObject, objectIdx);
    }

    bool SetAnimObject(int groupIdx, AnimObject* animObject, int objectIdx = 0)
    {
        if ((0 <= groupIdx && groupIdx < m_AnimGroups.Size()) &&
            (0 <= objectIdx && objectIdx < m_AnimObjectCountPerGroup))
            {
            const int index = groupIdx * m_AnimObjectCountPerGroup + objectIdx;
            m_AnimObjects[index] = animObject;
            return true;
        }
        return false;
    }

    bool SetAnimObject(const char* animGroupName, AnimObject* animObject, int objectIdx = 0)
    {
        return SetAnimObject(GetAnimGroupIndex(animGroupName), animObject, objectIdx);
    }

protected:
    AnimBinding(nw::os::IAllocator* allocator,int maxAnimObjects): 
        GfxObject(allocator),
        m_AnimObjectCountPerGroup(maxAnimObjects) {}

    virtual ~AnimBinding() {}

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimGroups, int maxAnimObjects)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        const int animObjectCount = maxAnimGroups * maxAnimObjects;

        size += sizeof(AnimGroup*) * maxAnimGroups;
        size += sizeof(AnimObject*) * animObjectCount;
    }

    Result Initialize(int maxAnimGroups, int maxAnimObjects)
    {

        const int animObjectCount = maxAnimGroups * maxAnimObjects;

        Result result = INITIALIZE_RESULT_OK;

        {
            void* memory = GetAllocator().Alloc(sizeof(AnimGroup*) * maxAnimGroups);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }

            NW_ENSURE_AND_RETURN(result);

            m_AnimGroups = AnimGroupArray(memory, maxAnimGroups, &GetAllocator());
        }
        {
            void* memory = GetAllocator().Alloc(sizeof(AnimObject*) * animObjectCount);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_AnimObjects = AnimObjectArray(memory, animObjectCount, &GetAllocator());
        }
        
        for (int animGroupIdx = 0; animGroupIdx < maxAnimGroups; ++animGroupIdx)
        {
            m_AnimGroups.PushBackFast<AnimGroup*>(NULL);
        }

        for (int animObjectIdx = 0; animObjectIdx < animObjectCount; ++animObjectIdx)
        {
            m_AnimObjects.PushBackFast<AnimObject*>(NULL);
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

    AnimGroupArray m_AnimGroups;
    AnimObjectArray m_AnimObjects;
    const int m_AnimObjectCountPerGroup;
};

class BaseAnimEvaluator : public AnimObject
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    static const int NotFoundIndex;

    BaseAnimEvaluator(nw::os::IAllocator* allocator, u32 animType): 
        AnimObject(allocator, animType),
        m_IsCacheDirty(true),
        m_IsCacheExternal(false) {}

    virtual ~BaseAnimEvaluator() {}

    virtual Result TryBind(AnimGroup* animGroup);

    virtual void Release()
    {
        m_AnimGroup = NULL;
    }

    virtual void UpdateFrame()
    {
        if (this->GetStepFrame() != 0.0f)
        {
            m_AnimFrameController.UpdateFrame();
            m_IsCacheDirty = true;
        }
    }

    virtual void ChangeAnim(const anim::ResAnim animData)
    {
        NW_NULL_ASSERT(m_AnimGroup);

        NW_ASSERT(animData.GetMemberAnimSetCount() <= this->m_ReverseBindIndexTable.Capacity());

        SetResAnim(animData);

        AnimGroup* animGroup = m_AnimGroup;
        this->Release();
        this->Bind(animGroup);
    }

    void Reset()
    {
        for (int i = (this->m_AnimGroup->GetMemberCount() - 1); i >= 0; --i)
        {
            this->ResetMember(i);
        }
    }

    void ResetMember(int memberIdx);

    float GetFrame() const { return this->m_AnimFrameController.GetFrame(); }

    void SetFrame(float frame)
    {
        this->m_AnimFrameController.SetFrame(frame);
        m_IsCacheDirty = true;
    }

    void ResetFrame(f32 frame)
    {
        this->m_AnimFrameController.GetAnimFrame().ResetFrame(frame);
        m_IsCacheDirty = true;
    }

    float GetStepFrame() const { return this->m_AnimFrameController.GetStepFrame(); }

    void SetStepFrame(float stepFrame)
    {
        this->m_AnimFrameController.SetStepFrame(stepFrame);
        if (stepFrame == 0.0f)
        {
            m_IsCacheDirty = true;
            UpdateCache();
        }
    }

    float GetStartFrame() const { return this->m_AnimFrameController.GetStartFrame(); }

    void SetStartFrame(float startFrame)
    {
        this->m_AnimFrameController.SetStartFrame(startFrame);
        if (startFrame > GetFrame())
        {
            SetFrame(startFrame);
        }
    }

    float GetEndFrame() const { return this->m_AnimFrameController.GetEndFrame(); }

    void SetEndFrame(float endFrame)
    {
        this->m_AnimFrameController.SetEndFrame(endFrame);
        if (endFrame < GetFrame())
        {
            SetFrame(endFrame);
        }
    }

    anim::AnimFrameController::PlayPolicy GetPlayPolicy() const
    {
        return this->m_AnimFrameController.GetPlayPolicy();
    }

    void SetPlayPolicy(anim::AnimFrameController::PlayPolicy playPolicy)
    {
        this->m_AnimFrameController.SetPlayPolicy(playPolicy);
        m_IsCacheDirty = true;
    }

    const anim::AnimFrameController& AnimFrameController() const { return m_AnimFrameController; }

    const void* GetUserData() const { return this->m_AnimFrameController.GetUserData(); }

    void* GetUserData() { return this->m_AnimFrameController.GetUserData(); }

    void SetUserData(void* userData) { this->m_AnimFrameController.SetUserData(userData); }

    const nw::ut::MoveArray<int>& BindIndexTable() const { return m_BindIndexTable; }

    nw::ut::MoveArray<int>& BindIndexTable() { return m_BindIndexTable; }

    const nw::ut::MoveArray<int>& ReverseBindIndexTable() const { return m_ReverseBindIndexTable; }

    nw::ut::MoveArray<int>& ReverseBindIndexTable() { return m_ReverseBindIndexTable; }

    const anim::ResAnim GetAnimData() const { return m_AnimData; }

    virtual const void* GetCacheBuffer() const { return NULL; }
    virtual int GetCacheBufferSizeNeeded() const { return 0; }
    virtual void SetCacheBuffer(void* buf, int size)
    {
        (void)buf;  
        (void)size; 
    }

protected:

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,const int maxMembers,const int maxAnimMembers)
{
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(int) * maxMembers;
        size += sizeof(int) * maxAnimMembers;
    }

    virtual Result Initialize(const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers)
    {
        Result result = INITIALIZE_RESULT_OK;

        this->SetResAnim(animData);

        {
            void* memory = GetAllocator().Alloc(sizeof(int) * maxMembers);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_BindIndexTable = nw::ut::MoveArray<int>(memory, maxMembers, &GetAllocator());
        }

        {
            void* memory = GetAllocator().Alloc(sizeof(int) * maxAnimMembers);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);
    
            m_ReverseBindIndexTable = nw::ut::MoveArray<int>(memory, maxAnimMembers, &GetAllocator());
        }

        return result;
    }

    template <class T>
    Result TryBindTemplate(AnimGroup* animGroup, T indexGetter)
    {
        NW_NULL_ASSERT(animGroup);
        NW_ASSERT(std::strcmp(m_AnimData.GetTargetAnimGroupName(), animGroup->GetName()) == 0);
        NW_ASSERT(m_AnimGroup == NULL);

        const int memberCount = animGroup->GetMemberCount();
        bool resultResize = this->m_BindIndexTable.Resize(memberCount);
        NW_ASSERTMSG(resultResize, "Member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxMembers.");

        const int animMemberCount = this->m_AnimData.GetMemberAnimSetCount();
        bool resultReverseResize = this->m_ReverseBindIndexTable.Resize(animMemberCount);
        NW_ASSERTMSG(resultReverseResize,
            "Animation member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxAnimMembers.");

        NW_ASSERTMSG(animGroup->GetFullBakedAnimEnabled() == m_AnimData.IsFullBakedAnim(),
            "To use full baked animation, call SkeletalModel::SetFullBakedAnimEnabled(true) before Bind.");

        int boundAnimCount = 0;
        for (int memberIdx = 0; memberIdx < memberCount; ++memberIdx)
        {
            m_BindIndexTable[memberIdx] = NotFoundIndex;
        }

        for (int animIdx = 0; animIdx < animMemberCount; ++animIdx)
        {
            anim::ResMemberAnim member = m_AnimData.GetMemberAnimSet(animIdx);
            const int bindTargetIdx = indexGetter(animGroup, member);

            if (bindTargetIdx == -1)
            {
                m_ReverseBindIndexTable[animIdx] = NotFoundIndex;
                continue;
            }

            m_BindIndexTable[bindTargetIdx] = animIdx;
            m_ReverseBindIndexTable[animIdx] = bindTargetIdx;
            ++boundAnimCount;

            anim::ResAnimGroupMember resAnimGroupMember = animGroup->GetResAnimGroupMember(bindTargetIdx);
            internal::ClearMaterialHash(resAnimGroupMember);
        }

        m_AnimGroup = animGroup;

        if (boundAnimCount == this->m_AnimData.GetMemberAnimSetCount())
        {
            return Result(BIND_RESULT_OK);
        }

        if (boundAnimCount == 0)
        {
            return Result(BIND_RESULT_NO_MEMBER_BOUND | Result::MASK_FAIL_BIT);
        }

        return Result(BIND_RESULT_NOT_ALL_ANIM_MEMBER_BOUND);
    }

    nw::ut::MoveArray<int> m_BindIndexTable;
    nw::ut::MoveArray<int> m_ReverseBindIndexTable;
    anim::AnimFrameController m_AnimFrameController;
    anim::ResAnim m_AnimData;
    bool m_IsCacheDirty;
    bool m_IsCacheExternal;
    bool m_UseSharedCache;

private:
    class BasicIndexGetterFunctor
    {
    public:
        int operator() (AnimGroup* animGroup, anim::ResMemberAnim member)
    {
            return animGroup->GetResAnimGroupMemberIndex(member.GetPath());
        }
    };

    void SetResAnim(const anim::ResAnim animData)
    {
        m_AnimData = animData;
        this->m_AnimFrameController.SetStepFrame(1.0f);
        this->m_AnimFrameController.SetStartFrame(0.0f);
        this->m_AnimFrameController.SetEndFrame(animData.GetFrameSize());
        this->m_AnimFrameController.GetAnimFrame().ResetFrame(0.0f);

        switch (animData.GetLoopMode())
        {
            case anim::ResAnimData::LOOP_MODE_ONETIME:
                this->m_AnimFrameController.SetPlayPolicy(anim::PlayPolicy_Onetime);
                break;

            case anim::ResAnimData::LOOP_MODE_LOOP:
                this->m_AnimFrameController.SetPlayPolicy(anim::PlayPolicy_Loop);
                break;

            default:
                NW_ASSERT(false);
        }
        m_IsCacheDirty = true;
    }
};

class AnimEvaluator : public BaseAnimEvaluator
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): 
            m_AnimData(NULL),
            m_MaxMembers(64),
            m_MaxAnimMembers(64),
            m_AllocCache(false) {}

        Builder& AnimData(const anim::ResAnim& animData) { m_AnimData = animData; return *this; }

        Builder& MaxMembers(int maxMembers)
        {
            NW_ASSERT(maxMembers > 0);
            m_MaxMembers = maxMembers;
            return *this;
        }

        Builder& MaxAnimMembers(int maxAnimMembers)
        {
            NW_ASSERT(maxAnimMembers > 0);
            m_MaxAnimMembers = maxAnimMembers;
            return *this;
        }

        Builder& AllocCache(bool allocCache) { m_AllocCache = allocCache; return *this; }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const
        {
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimEvaluator);
            AnimEvaluator::GetMemorySizeForInitialize(pSize, this->m_AnimData, this->m_MaxMembers, this->m_MaxAnimMembers, this->m_AllocCache);
        }

        AnimEvaluator* Create(nw::os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimEvaluator));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimEvaluator* animEvaluator = new(buf) AnimEvaluator(allocator);

            Result result = animEvaluator->Initialize(this->m_AnimData, this->m_MaxMembers, this->m_MaxAnimMembers, this->m_AllocCache);
            NW_ASSERT(result.IsSuccess());

            return animEvaluator;
        }

    private:
        anim::ResAnim m_AnimData;
        int m_MaxMembers;
        int m_MaxAnimMembers;
        bool m_AllocCache;
    };

    virtual void ChangeAnim(const anim::ResAnim animData)
    {
        BaseAnimEvaluator::ChangeAnim(animData);

        this->m_CachePtrs.Resize(animData.GetMemberAnimSetCount());

        if (!m_IsCacheExternal && m_CacheBuf != NULL)
        {
            nw::os::SafeFree(this->m_CacheBuf, &GetAllocator());

            if (animData.GetMemberAnimSetCount() != 0)
            {
                m_CacheBuf = GetAllocator().Alloc(GetCacheBufferSizeNeeded());
                NW_NULL_ASSERT(this->m_CacheBuf);

                SetCacheBufferPointers();
            }
        }
    }

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;

    virtual bool HasMemberAnim(int memberIdx) const
    {
        return m_BindIndexTable[memberIdx] != NotFoundIndex;
    }

    virtual void UpdateCache() { this->UpdateCacheNonVirtual(); }
    
    void UpdateCacheNonVirtual();
    void UpdateCacheImpl();

    virtual int GetCacheBufferSizeNeeded() const;
    virtual const void* GetCacheBuffer() const { return m_CacheBuf; }
    virtual void SetCacheBuffer(void* buf, int size)
    {
        m_CacheBuf = buf;
        if (buf != NULL)
        {
            NW_ASSERT(size >= GetCacheBufferSizeNeeded());
            (void)size;
            m_IsCacheDirty = true;
            m_IsCacheExternal = true;
            this->SetCacheBufferPointers();
        }
    }

protected:
    AnimEvaluator( nw::os::IAllocator* allocator);

    virtual ~AnimEvaluator()
    {
        if (!m_IsCacheExternal && m_CacheBuf != NULL)
        {
            GetAllocator().Free(this->m_CacheBuf);
        }
    }


    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize,const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache);

    virtual Result Initialize(const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache);

    static int GetCacheBufferSizeNeeded(const anim::ResAnim& animData);

    void SetCacheBufferPointers();

    void* m_CacheBuf;
    SharedAnimCache* m_SharedCache;
    nw::ut::MoveArray<anim::AnimResult*> m_CachePtrs;
};

class AnimBlender : public AnimObject
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::MoveArray<AnimObject*> AnimObjectArray;

    AnimBlender(nw::os::IAllocator* allocator): 
        AnimObject(allocator, ANIMTYPE_BLENDER) {}

    virtual ~AnimBlender() {}

    virtual Result TryBind(AnimGroup* animGroup)
    {
        NW_NULL_ASSERT(animGroup);
        NW_ASSERT(this->m_AnimGroup == NULL);
        m_AnimGroup = animGroup;
        return Result(BIND_RESULT_OK);
    }

    virtual void Release()
    {
        m_AnimGroup = NULL;
    }

    virtual void UpdateFrame()
    {
        for (int animObjIdx = 0; animObjIdx < this->m_AnimObjects.Size(); ++animObjIdx)
        {
            if (m_AnimObjects[animObjIdx] != NULL)
            {
                this->m_AnimObjects[animObjIdx]->UpdateFrame();
            }
        }
    }

    virtual bool HasMemberAnim(int memberIdx) const
    {
        for (int animObjIdx = 0; animObjIdx < this->m_AnimObjects.Size(); ++animObjIdx)
        {
            if (this->m_AnimObjects[animObjIdx] != NULL &&
                this->m_AnimObjects[animObjIdx]->HasMemberAnim(memberIdx))
                {
                return true;
            }
        }
        return false;
    }

    void AddAnimObject(AnimObject* animObj)
    {
        NW_ASSERT(this->m_AnimObjects.Size() < this->m_AnimObjects.Capacity());
        this->m_AnimObjects.PushBack(animObj);
    }

    const AnimObject* GetAnimObject(int animObjIdx) const
    {
        return m_AnimObjects[animObjIdx];
    }

    AnimObject* GetAnimObject(int animObjIdx)
    {
        return m_AnimObjects[animObjIdx];
    }

    AnimObject* ReplaceAnimObject(int animObjIdx, AnimObject* animObj)
    {
        AnimObject* oldObj = m_AnimObjects[animObjIdx];
        m_AnimObjects[animObjIdx] = animObj;
        return oldObj;
    }

    void ClearAnimObjects()
    {
        this->m_AnimObjects.Clear();
    }

    int GetAnimObjectCount() const
    {
        return this->m_AnimObjects.Size();
    }

    int GetMaxAnimObjects() const
    {
        return this->m_AnimObjects.Capacity();
    }

    virtual void UpdateCache()
    {
        for (int animObjIdx = 0; animObjIdx < this->m_AnimObjects.Size(); ++animObjIdx)
        {
            if (m_AnimObjects[animObjIdx] != NULL)
            {
                this->m_AnimObjects[animObjIdx]->UpdateCache();
            }
        }
    }

protected:

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimObjects)
{
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(AnimObject*) * maxAnimObjects;
    }

    virtual Result Initialize(int maxAnimObjects)
    {
        Result result = INITIALIZE_RESULT_OK;

        void* memory = GetAllocator().Alloc(sizeof(AnimObject*) * maxAnimObjects);

        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }

        NW_ENSURE_AND_RETURN(result);

        m_AnimObjects = AnimObjectArray(memory, maxAnimObjects, &GetAllocator());

        return result;
    }

    AnimObjectArray m_AnimObjects;
};

class AnimInterpolator : public AnimBlender
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): 
            m_MaxAnimObjects(2),
            m_IgnoreNoAnimMember(false) {}

        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        Builder& IgnoreNoAnimMember(bool ignoreNoAnimMember) { m_IgnoreNoAnimMember = ignoreNoAnimMember; return *this; }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimInterpolator);
            AnimInterpolator::GetMemorySizeForInitialize(pSize, this->m_MaxAnimObjects);
        }

        AnimInterpolator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimInterpolator));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimInterpolator* animInterpolator = new(buf) AnimInterpolator(allocator);
            
            Result result = animInterpolator->Initialize(this->m_MaxAnimObjects, this->m_IgnoreNoAnimMember);
            NW_ASSERT(result.IsSuccess());

            return animInterpolator;
        }
    private:
        int m_MaxAnimObjects;
        bool m_IgnoreNoAnimMember;
    };

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;

    float GetWeight(int animObjIdx) const
    {
        return m_Weights[animObjIdx];
    }

    void SetWeight(int animObjIdx, float weight)
    {
        m_Weights[animObjIdx] = weight;
        m_NormalizedWeights[animObjIdx] = weight;
        m_IsWeightDirty = true;
    }

    void SetNormalizationEnabled(bool enabled) { m_IsWeightNormalizationEnabled = enabled; }

    bool GetNormalizationEnabled() const { return m_IsWeightNormalizationEnabled; }

protected:
    AnimInterpolator(nw::os::IAllocator* allocator): 
        AnimBlender(allocator),
        m_IsOldMethod(false),
        m_IsWeightDirty(false),
        m_IsWeightNormalizationEnabled(true) {}

    virtual ~AnimInterpolator() {}

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimObjects)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        AnimBlender::GetMemorySizeForInitialize(pSize, maxAnimObjects);
        size += sizeof(float) * maxAnimObjects;
        size += sizeof(float) * maxAnimObjects;
    }

    virtual Result Initialize(int maxAnimObjects, bool ignoreNoAnimMember)
    {
        Result result = AnimBlender::Initialize(maxAnimObjects);
        NW_ENSURE_AND_RETURN(result);

        {
            void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_Weights = nw::ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
            for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx)
            {
                this->m_Weights.PushBackFast(animObjIdx == 0 ? 1.0f : 0.0f);
            }
        }

        {
            void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_NormalizedWeights = nw::ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
            for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx)
            {
                this->m_NormalizedWeights.PushBackFast(animObjIdx == 0 ? 1.0f : 0.0f);
            }
        }

        m_IsOldMethod = ignoreNoAnimMember;
        return result;
    }

    void NormalizeWeight() const
    {
        float weightSum = 0.0f;
        float normalizeScale;
        for (int i = 0; i < this->m_Weights.Size(); ++i)
        {
            weightSum += m_Weights[i];
        }

        normalizeScale = GetAnimWeightNormalizeScale(weightSum);
        for (int i = 0; i < this->m_Weights.Size(); ++i)
        {
            m_NormalizedWeights[i] = m_Weights[i] * normalizeScale;
        }
        m_IsWeightDirty = false;
    }

    nw::ut::MoveArray<float> m_Weights;
    mutable nw::ut::MoveArray<float> m_NormalizedWeights;

    bool m_IsOldMethod;
    mutable bool m_IsWeightDirty;
    bool m_IsWeightNormalizationEnabled;
};

class AnimAdder : public AnimBlender
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): 
            m_MaxAnimObjects(2) {}

        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const
        {
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimAdder);
            AnimAdder::GetMemorySizeForInitialize(pSize, this->m_MaxAnimObjects);
        }

        AnimAdder* Create(nw::os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimAdder));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimAdder* animAdder = new(buf) AnimAdder(allocator);

            Result result = animAdder->Initialize(this->m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());

            return animAdder;
        }

    private:
        int m_MaxAnimObjects;
    };

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;

    float GetWeight(int animObjIdx) const
    {
        return m_Weights[animObjIdx];
    }

    void SetWeight(int animObjIdx, float weight)
    {
        m_Weights[animObjIdx] = weight;
    }

protected:
    AnimAdder(nw::os::IAllocator* allocator):
        AnimBlender(allocator) {}

    virtual ~AnimAdder() { }

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, int maxAnimObjects)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        AnimBlender::GetMemorySizeForInitialize(pSize, maxAnimObjects);
        size += sizeof(float) * maxAnimObjects;
    }

    virtual Result Initialize(int maxAnimObjects)
    {
        Result result = AnimBlender::Initialize(maxAnimObjects);
        NW_ENSURE_AND_RETURN(result);

        void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        this->m_Weights = nw::ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
        for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx)
        {
            this->m_Weights.PushBackFast(1.0f);
        }

        return result;
    }

    nw::ut::MoveArray<float> m_Weights;
};

class AnimOverrider : public AnimBlender
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): 
            m_MaxAnimObjects(2) {}

        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const
        {
            nw::os::MemorySizeCalculator& size = *pSize;
            
            size += sizeof(AnimOverrider);
            AnimOverrider::GetMemorySizeForInitialize(pSize, this->m_MaxAnimObjects);
        }

        AnimOverrider* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimOverrider));

            if (buf == NULL)
            {
                return NULL;
            }
            
            AnimOverrider* animOverrider = new(buf) AnimOverrider(allocator);

            Result result = animOverrider->Initialize(this->m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());
            
            return animOverrider;
        }
    private:
        int m_MaxAnimObjects;
    };

    virtual const anim::AnimResult* GetResult(void* target,int memberIdx) const;
protected:

    AnimOverrider(nw::os::IAllocator* allocator): 
        AnimBlender(allocator) {}

    virtual ~AnimOverrider() {}
};

}
}