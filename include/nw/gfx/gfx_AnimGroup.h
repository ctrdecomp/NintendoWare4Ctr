#pragma once

#include <nw/anim/res/anim_ResAnimGroup.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw {

namespace anim {

class AnimBlendOp;

}

namespace gfx {

class SceneNode;

class AnimGroup : public GfxObject
{
public:
    typedef bool (*PreEvaluateCallback)(AnimGroup* animGroup, int targetObjIdx);

    class Builder
    {
    public:
        Builder(): 
            m_SceneNode(NULL),
            m_UseOriginalValue(false) {}

        Builder& ResAnimGroup(anim::ResAnimGroup resAnimGroup)
        {
            m_ResAnimGroup = resAnimGroup;
            return *this;
        }

        Builder& SetSceneNode(SceneNode* sceneNode) { m_SceneNode = sceneNode; return *this; }

        Builder& UseOriginalValue(bool use) { m_UseOriginalValue = use; return *this; }

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            os::MemorySizeCalculator& size = *pSize;

            size += sizeof(AnimGroup);
            AnimGroup::GetMemorySizeForInitialize(pSize, this->m_ResAnimGroup, this->m_UseOriginalValue);
        }

        AnimGroup* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimGroup));
            
            if (buf == NULL)
            {
                return NULL;
            }
            
            AnimGroup* animGroup = new(buf) AnimGroup(this->m_ResAnimGroup, this->m_SceneNode, allocator);

            Result result = animGroup->Initialize(this->m_UseOriginalValue);
            
            if (result.IsSuccess())
            {
                return animGroup;
            }
            else
            {
                nw::ut::SafeDestroy(animGroup);
                return NULL;
            }
        }

    private:
        SceneNode* m_SceneNode;
        anim::ResAnimGroup m_ResAnimGroup;
        bool m_UseOriginalValue;
    };

    anim::ResAnimGroup GetResAnimGroup() const { return m_ResAnimGroup; }

    anim::ResGraphicsAnimGroup GetResGraphicsAnimGroup() const
    {
        return *reinterpret_cast<const anim::ResGraphicsAnimGroup*>(&this->m_ResAnimGroup);
    }

    anim::ResAnimGroupMember GetResAnimGroupMember(int memberIdx) const { return this->m_ResAnimGroup.GetMemberInfoSet(memberIdx); }

    anim::ResAnimGroupMember GetResAnimGroupMember(const char* key) const { return this->m_ResAnimGroup.GetMemberInfoSet(key); }

    int GetResAnimGroupMemberIndex(const char* key) const { return this->m_ResAnimGroup.GetMemberInfoSetIndex(key); }

    const char* GetName() const { return this->m_ResAnimGroup.GetName(); }

    s32 GetMemberCount() const { return this->m_ResAnimGroup.GetMemberInfoSetCount(); }

    const anim::AnimBlendOp* GetBlendOperationInGroup(int blendOpIdx) const
    {
        return m_BlendOperations[blendOpIdx];
    }

    void SetBlendOperationInGroup(int blendOpIdx, anim::AnimBlendOp* blendOp)
    {
        m_BlendOperations[blendOpIdx] = blendOp;
    }

    const anim::AnimBlendOp* GetBlendOperation(int memberIdx) const
    {
        return this->GetBlendOperationInGroup(this->GetResAnimGroupMember(memberIdx).GetBlendOperationIndex());
    }

    SceneNode* GetSceneNode() const { return m_SceneNode; }

    int GetTargetObjectIndex(int memberIdx) const
    {
        return m_TargetObjectIndicies[memberIdx];
    }

    void SetTargetObjectIndex(int memberIdx, const int targetObjIdx)
    {
        m_TargetObjectIndicies[memberIdx] = targetObjIdx;
    }

    void* GetTargetObject(int memberIdx) const
    {
        return m_TargetObjects[memberIdx];
    }

    void SetTargetObject(int memberIdx, void* object)
    {
        m_TargetObjects[memberIdx] = object;
    }

    void* GetTargetPtr(int memberIdx) const
    {
        return m_TargetPtrs[memberIdx];
    }

    void SetTargetPtr(int memberIdx, void* target)
    {
        m_TargetPtrs[memberIdx] = target;
    }

    bool HasOriginalValue() const { return this->m_OriginalValues.Size() != 0; }

    const void* GetOriginalValue(int memberIdx) const
    {
        return m_OriginalValues[memberIdx];
    }

    void SetOriginalValue(int memberIdx, const void* value)
    {
        m_OriginalValues[memberIdx] = value;
    }

    PreEvaluateCallback GetPreEvaluateCallback() const { return m_PreEvaluateCallback; }

    void SetPreEvaluateCallback(PreEvaluateCallback function) { m_PreEvaluateCallback = function; }

    bool GetFullBakedAnimEnabled() const { return m_FullBakedAnimEnabled; }

    void SetFullBakedAnimEnabled(bool enable) { m_FullBakedAnimEnabled = enable; }

    void Reset();
protected:

    AnimGroup(anim::ResAnimGroup resAnimGroup,SceneNode* sceneNode, nw::os::IAllocator* allocator);

    virtual ~AnimGroup() {}
    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, const anim::ResAnimGroup resAnimGroup, bool useOriginalValue);

    Result Initialize(bool useOriginalValue);

private:
    anim::ResAnimGroup m_ResAnimGroup;
    ut::MoveArray<anim::AnimBlendOp*> m_BlendOperations;

    SceneNode* m_SceneNode;
    nw::ut::MoveArray<int> m_TargetObjectIndicies;
    nw::ut::MoveArray<void*> m_TargetObjects;
    nw::ut::MoveArray<void*> m_TargetPtrs;
    nw::ut::MoveArray<const void*> m_OriginalValues;

    PreEvaluateCallback m_PreEvaluateCallback;

    bool m_FullBakedAnimEnabled;
};

}
}