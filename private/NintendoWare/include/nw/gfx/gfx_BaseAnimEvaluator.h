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

#ifndef NW_GFX_BASEANIMEVALUATOR_H_
#define NW_GFX_BASEANIMEVALUATOR_H_

#include <nw/anim/anim_AnimFrameController.h>
#include <nw/anim/res/anim_ResAnim.h>
#include <nw/gfx/gfx_AnimGroup.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {
namespace gfx {

namespace internal
{
    //
    void ClearMaterialHash(anim::ResAnimGroupMember member);
}

//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class BaseAnimEvaluator : public AnimObject
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    //
    //
    static const int NotFoundIndex;

    //----------------------------------------
    //
    //

    //
    BaseAnimEvaluator(
        os::IAllocator* allocator, u32 animType)
    : AnimObject(allocator, animType),
      m_IsCacheDirty(true),
      m_IsCacheExternal(false),
      m_UseSharedCache(false)
    {
    }

    //
    virtual ~BaseAnimEvaluator() {}

    //

    //----------------------------------------
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
    virtual Result TryBind(AnimGroup* animGroup);

    //
    virtual void Release()
    {
        m_AnimGroup = NULL;
    }

    //
    virtual void UpdateFrame()
    {
        // When the update frame is 0, the cache has been updated in the SetStepFrame function.
        // 
        if (GetStepFrame() != 0.0f)
        {
            m_AnimFrameController.UpdateFrame();
            m_IsCacheDirty = true;
        }
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
    virtual Result ChangeAnim(const nw::anim::ResAnim animData)
    {
        NW_NULL_ASSERT(m_AnimGroup);

        // When this assert has failed, increase the value of Builder::MaxAnimMembers
        NW_ASSERT(animData.GetMemberAnimSetCount() <= m_ReverseBindIndexTable.Capacity());

        SetResAnim(animData);

        AnimGroup* animGroup = m_AnimGroup;
        Release();
        return TryBind(animGroup);
    }

    //
    //
    //
    //
    //
    //
    void Reset()
    {
        for (int i = (m_AnimGroup->GetMemberCount() - 1); i >= 0; --i)
        {
            this->ResetMember(i);
        }
    }

    //
    //
    void ResetMember(int memberIdx);

    //

    //----------------------------------------
    //
    //

    //
    float GetFrame() const { return m_AnimFrameController.GetFrame(); }

    //
    //
    //
    //
    void SetFrame(float frame)
    {
        m_AnimFrameController.SetFrame(frame);
        m_IsCacheDirty = true;
    }

    //
    //
    //
    //
    void ResetFrame(f32 frame)
    {
        m_AnimFrameController.GetAnimFrame().ResetFrame(frame);
        m_IsCacheDirty = true;
    }

    //
    //
    float GetStepFrame() const { return m_AnimFrameController.GetStepFrame(); }

    //
    //
    //
    //
    //
    //
    void SetStepFrame(float stepFrame)
    {
        m_AnimFrameController.SetStepFrame(stepFrame);
        if (stepFrame == 0.0f)
        {
            m_IsCacheDirty = true;
            UpdateCache();
        }
    }

    //
    //
    float GetStartFrame() const { return m_AnimFrameController.GetStartFrame(); }

    //
    //
    //
    //
    //
    //
    void SetStartFrame(float startFrame)
    {
        m_AnimFrameController.SetStartFrame(startFrame);
        if (startFrame > GetFrame())
        {
            SetFrame(startFrame);
        }
    }

    //
    //
    float GetEndFrame() const { return m_AnimFrameController.GetEndFrame(); }

    //
    //
    //
    //
    //
    //
    void SetEndFrame(float endFrame)
    {
        m_AnimFrameController.SetEndFrame(endFrame);
        if (endFrame < GetFrame())
        {
            SetFrame(endFrame);
        }
    }

    //
    nw::anim::AnimFrameController::PlayPolicy GetPlayPolicy() const
    {
        return m_AnimFrameController.GetPlayPolicy();
    }

    //
    //
    //
    //
    void SetPlayPolicy(nw::anim::AnimFrameController::PlayPolicy playPolicy)
    {
        m_AnimFrameController.SetPlayPolicy(playPolicy);
        m_IsCacheDirty = true;
    }

    //
    const nw::anim::AnimFrameController& AnimFrameController() const { return m_AnimFrameController; }

    //

    //----------------------------------------
    //
    //

    //
    const void* GetUserData() const { return m_AnimFrameController.GetUserData(); }

    //
    void* GetUserData() { return m_AnimFrameController.GetUserData(); }

    //
    //
    //
    //
    //
    void SetUserData(void* userData) { m_AnimFrameController.SetUserData(userData); }

    //
    const ut::MoveArray<int>& BindIndexTable() const { return m_BindIndexTable; }

    //
    ut::MoveArray<int>& BindIndexTable() { return m_BindIndexTable; }

    //
    const ut::MoveArray<int>& ReverseBindIndexTable() const { return m_ReverseBindIndexTable; }

    //
    ut::MoveArray<int>& ReverseBindIndexTable() { return m_ReverseBindIndexTable; }

    //
    const nw::anim::ResAnim GetAnimData() const { return m_AnimData; }

    //

    //----------------------------------------
    //
    //

    //
    virtual const void* GetCacheBuffer() const { return NULL; }

    //
    virtual int GetCacheBufferSizeNeeded() const { return 0; }

    //
    //
    //
    //
    //
    //
    //
    virtual void SetCacheBuffer(void* buf, int size)
    {
        (void)buf;
        (void)size;
    }

    //

protected:
    //
    //
    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        const int maxMembers,
        const int maxAnimMembers)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(int) * maxMembers;
        size += sizeof(int) * maxAnimMembers;
    }

    //
    //
    //
    Result Initialize(
        const nw::anim::ResAnim& animData,
        const int maxMembers,
        const int maxAnimMembers)
    {
        Result result = INITIALIZE_RESULT_OK;

        SetResAnim(animData);

        {
            void* memory = GetAllocator().Alloc(sizeof(int) * maxMembers);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_BindIndexTable = ut::MoveArray<int>(memory, maxMembers, &GetAllocator());
        }

        {
            void* memory = GetAllocator().Alloc(sizeof(int) * maxAnimMembers);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_ReverseBindIndexTable = ut::MoveArray<int>(memory, maxAnimMembers, &GetAllocator());
        }

        return result;
    }

    //
    //
    //
    //
    //
    //
    //
    template <class T>
    Result
    TryBindTemplate(AnimGroup* animGroup, T indexGetter)
    {
        NW_NULL_ASSERT(animGroup);
        NW_ASSERT(std::strcmp(m_AnimData.GetTargetAnimGroupName(), animGroup->GetName()) == 0);
        NW_ASSERT(m_AnimGroup == NULL);

        const int memberCount = animGroup->GetMemberCount();
        bool resultResize = m_BindIndexTable.Resize(memberCount);
        NW_ASSERTMSG(resultResize,
            "Member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxMembers.");

        const int animMemberCount = m_AnimData.GetMemberAnimSetCount();
        bool resultReverseResize = m_ReverseBindIndexTable.Resize(animMemberCount);
        NW_ASSERTMSG(resultReverseResize,
            "Animation member count exceeded upper limit. Increase AnimEvaluator::Builder::MaxAnimMembers.");

        NW_ASSERTMSG(animGroup->GetFullBakedAnimEnabled() == m_AnimData.IsFullBakedAnim(),
            "To use full baked animation, call SkeletalModel::SetFullBakedAnimEnabled(true) before Bind.");

        int boundAnimCount = 0;
        for (int memberIdx = 0; memberIdx < memberCount; ++memberIdx)
        {
            // Fill in with the index when it couldn't be found
            m_BindIndexTable[memberIdx] = NotFoundIndex;
        }

        for (int animIdx = 0; animIdx < animMemberCount; ++animIdx)
        {
            anim::ResMemberAnim member = m_AnimData.GetMemberAnimSet(animIdx);
            const int bindTargetIdx = indexGetter(animGroup, member);

            // When it couldn't be found in the dictionary, there is no target for the model
            if (bindTargetIdx == -1)
            {
                m_ReverseBindIndexTable[animIdx] = NotFoundIndex;
                continue;
            }

            m_BindIndexTable[bindTargetIdx] = animIdx;
            m_ReverseBindIndexTable[animIdx] = bindTargetIdx;
            ++boundAnimCount;

            //HACK
            anim::ResAnimGroupMember resAnimGroupMember =
                animGroup->GetResAnimGroupMember(bindTargetIdx);
            internal::ClearMaterialHash(resAnimGroupMember);
        }

        m_AnimGroup = animGroup;

        // All the member animations were bound.
        if (boundAnimCount == m_AnimData.GetMemberAnimSetCount())
        {
            return Result(BIND_RESULT_OK);
        }

        // There were no bound animations and so it failed.
        if (boundAnimCount == 0)
        {
            return Result(BIND_RESULT_NO_MEMBER_BOUND | Result::MASK_FAIL_BIT);
        }

        // When neither of the above was the case, binding succeeded but there is a notification that the member animations were not bound.
        // 
        return Result(BIND_RESULT_NOT_ALL_ANIM_MEMBER_BOUND);
    }

    //
    //
    //
    ut::MoveArray<int> m_BindIndexTable;

    //
    //
    //
    ut::MoveArray<int> m_ReverseBindIndexTable;

    anim::AnimFrameController m_AnimFrameController; //
    anim::ResAnim m_AnimData; //

    //
    //
    //
    bool m_IsCacheDirty;

    //
    //
    //
    bool m_IsCacheExternal;

    //
    //
    //
    bool m_UseSharedCache;

private:
    //
    //
    //
    class BasicIndexGetterFunctor
    {
    public:
        int operator() (AnimGroup* animGroup, anim::ResMemberAnim member)
        {
            return animGroup->GetResAnimGroupMemberIndex(member.GetPath());
        }
    };

    void SetResAnim(const nw::anim::ResAnim animData)
    {
        m_AnimData = animData;
        m_AnimFrameController.SetStepFrame(1.0f);
        m_AnimFrameController.SetStartFrame(0.0f);
        m_AnimFrameController.SetEndFrame(animData.GetFrameSize());
        m_AnimFrameController.GetAnimFrame().ResetFrame(0.0f);

        switch (animData.GetLoopMode())
        {
            case nw::anim::ResAnimData::LOOP_MODE_ONETIME:
                m_AnimFrameController.SetPlayPolicy(nw::anim::PlayPolicy_Onetime);
                break;

            case nw::anim::ResAnimData::LOOP_MODE_LOOP:
                m_AnimFrameController.SetPlayPolicy(nw::anim::PlayPolicy_Loop);
                break;

            default:
                NW_ASSERT(false);
        }

        m_IsCacheDirty = true;
    }
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_BASEANIMEVALUATOR_H_
