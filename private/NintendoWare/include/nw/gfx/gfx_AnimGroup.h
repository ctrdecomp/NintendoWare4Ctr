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

#ifndef NW_GFX_ANIMGROUP_H_
#define NW_GFX_ANIMGROUP_H_

#include <nw/anim/res/anim_ResAnimGroup.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_MoveArray.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {

namespace anim {

class AnimBlendOp;

}

namespace gfx {

class SceneNode;

//---------------------------------------------------------------------------
//
//
//
//
//
//
//---------------------------------------------------------------------------
class AnimGroup : public GfxObject
{
public:
    //
    //
    typedef bool (*PreEvaluateCallback)(AnimGroup* animGroup, int targetObjIdx);

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        //
        Builder()
        : m_SceneNode(NULL),
          m_UseOriginalValue(false) {}

        //
        Builder& ResAnimGroup(anim::ResAnimGroup resAnimGroup)
        {
            m_ResAnimGroup = resAnimGroup;
            return *this;
        }

        //
        Builder& SetSceneNode(SceneNode* sceneNode) { m_SceneNode = sceneNode; return *this; }

        //
        Builder& UseOriginalValue(bool use) { m_UseOriginalValue = use; return *this; }

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        //
        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            os::MemorySizeCalculator& size = *pSize;

            size += sizeof(AnimGroup);
            AnimGroup::GetMemorySizeForInitialize(pSize, m_ResAnimGroup, m_UseOriginalValue);
        }

        //
        //
        //
        //
        //
        //
        AnimGroup* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimGroup));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimGroup* animGroup = new(buf) AnimGroup(m_ResAnimGroup, m_SceneNode, allocator);

            Result result = animGroup->Initialize(m_UseOriginalValue);

            if (result.IsSuccess())
            {
                return animGroup;
            }
            else
            {
                SafeDestroy(animGroup);
                return NULL;
            }
        }

    private:
        SceneNode* m_SceneNode;
        anim::ResAnimGroup m_ResAnimGroup;
        bool m_UseOriginalValue;
    };

    //

    //----------------------------------------
    //
    //

    //
    anim::ResAnimGroup GetResAnimGroup() const { return m_ResAnimGroup; }

    //
    anim::ResGraphicsAnimGroup GetResGraphicsAnimGroup() const
    {
        return *reinterpret_cast<const anim::ResGraphicsAnimGroup*>(&m_ResAnimGroup);
    }

    //
    anim::ResAnimGroupMember GetResAnimGroupMember(int memberIdx) const { return m_ResAnimGroup.GetMemberInfoSet(memberIdx); }

    //
    anim::ResAnimGroupMember GetResAnimGroupMember(const char* key) const { return m_ResAnimGroup.GetMemberInfoSet(key); }

    //
    int GetResAnimGroupMemberIndex(const char* key) const { return m_ResAnimGroup.GetMemberInfoSetIndex(key); }

    //
    const char* GetName() const { return m_ResAnimGroup.GetName(); }

    //
    s32 GetMemberCount() const { return m_ResAnimGroup.GetMemberInfoSetCount(); }

    //
    const anim::AnimBlendOp* GetBlendOperationInGroup(int blendOpIdx) const
    {
        NW_MINMAXLT_ASSERT(blendOpIdx, 0, m_BlendOperations.Size());
        return m_BlendOperations[blendOpIdx];
    }

    //
    void SetBlendOperationInGroup(int blendOpIdx, anim::AnimBlendOp* blendOp)
    {
        NW_MINMAXLT_ASSERT(blendOpIdx, 0, m_BlendOperations.Size());
        m_BlendOperations[blendOpIdx] = blendOp;
    }

    //
    const anim::AnimBlendOp* GetBlendOperation(int memberIdx) const
    {
        return GetBlendOperationInGroup(
            GetResAnimGroupMember(memberIdx).GetBlendOperationIndex());
    }

    //
    SceneNode* GetSceneNode() const { return m_SceneNode; }

    //
    int GetTargetObjectIndex(int memberIdx) const
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_TargetObjectIndicies.Size());
        return m_TargetObjectIndicies[memberIdx];
    }

    //
    void SetTargetObjectIndex(int memberIdx, const int targetObjIdx)
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_TargetObjectIndicies.Size());
        m_TargetObjectIndicies[memberIdx] = targetObjIdx;
    }

    //
    void* GetTargetObject(int memberIdx) const
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_TargetObjects.Size());
        return m_TargetObjects[memberIdx];
    }

    //
    void SetTargetObject(int memberIdx, void* object)
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_TargetObjects.Size());
        m_TargetObjects[memberIdx] = object;
    }

    //
    void* GetTargetPtr(int memberIdx) const
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_TargetPtrs.Size());
        return m_TargetPtrs[memberIdx];
    }

    //
    void SetTargetPtr(int memberIdx, void* target)
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_TargetPtrs.Size());
        m_TargetPtrs[memberIdx] = target;
    }

    //
    bool HasOriginalValue() const { return m_OriginalValues.Size() != 0; }

    //
    const void* GetOriginalValue(int memberIdx) const
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_OriginalValues.Size());
        return m_OriginalValues[memberIdx];
    }

    //
    void SetOriginalValue(int memberIdx, const void* value)
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_OriginalValues.Size());
        m_OriginalValues[memberIdx] = value;
    }

    //
    PreEvaluateCallback GetPreEvaluateCallback() const { return m_PreEvaluateCallback; }

    //
    void SetPreEvaluateCallback(PreEvaluateCallback function) { m_PreEvaluateCallback = function; }

    //
    //
    //
    bool GetFullBakedAnimEnabled() const { return m_FullBakedAnimEnabled; }

    //
    //
    //
    void SetFullBakedAnimEnabled(bool enable) { m_FullBakedAnimEnabled = enable; }

    //
    void Reset();

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    AnimGroup(
        anim::ResAnimGroup resAnimGroup,
        SceneNode* sceneNode,
        os::IAllocator* allocator);

    //
    //
    //
    virtual ~AnimGroup() {}

    //

    //
    //
    //
    static void GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize, const anim::ResAnimGroup resAnimGroup, bool useOriginalValue);

    //
    //
    //
    Result Initialize(bool useOriginalValue);

private:
    anim::ResAnimGroup m_ResAnimGroup; //
    ut::MoveArray<anim::AnimBlendOp*> m_BlendOperations; //

    SceneNode* m_SceneNode; //
    ut::MoveArray<int> m_TargetObjectIndicies; //
    ut::MoveArray<void*> m_TargetObjects; //
    ut::MoveArray<void*> m_TargetPtrs; //
    ut::MoveArray<const void*> m_OriginalValues; //

    PreEvaluateCallback m_PreEvaluateCallback; //

    bool m_FullBakedAnimEnabled; //
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_ANIMGROUP_H_
