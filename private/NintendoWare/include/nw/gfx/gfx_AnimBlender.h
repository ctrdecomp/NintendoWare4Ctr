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

#ifndef NW_GFX_ANIMBLENDER_H_
#define NW_GFX_ANIMBLENDER_H_

#include <nw/gfx/gfx_AnimObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
class AnimBlender : public AnimObject
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    //
    //
    typedef ut::MoveArray<AnimObject*> AnimObjectArray;

    //----------------------------------------
    //
    //

    //
    AnimBlender(
        os::IAllocator* allocator)
    : AnimObject(allocator, ANIMTYPE_BLENDER)
    {
    }

    //
    virtual ~AnimBlender() {}

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
    //
    virtual Result TryBind(AnimGroup* animGroup)
    {
        NW_NULL_ASSERT(animGroup);
        NW_ASSERT(m_AnimGroup == NULL);
        m_AnimGroup = animGroup;
        return Result(BIND_RESULT_OK);
    }

    //
    virtual void Release()
    {
        m_AnimGroup = NULL;
    }

    //
    virtual void UpdateFrame()
    {
        for (int animObjIdx = 0; animObjIdx < m_AnimObjects.Size(); ++animObjIdx)
        {
            if (m_AnimObjects[animObjIdx] != NULL)
            {
                m_AnimObjects[animObjIdx]->UpdateFrame();
            }
        }
    }

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
    virtual bool HasMemberAnim(int memberIdx) const
    {
        for (int animObjIdx = 0; animObjIdx < m_AnimObjects.Size(); ++animObjIdx)
        {
            if (m_AnimObjects[animObjIdx] != NULL &&
                m_AnimObjects[animObjIdx]->HasMemberAnim(memberIdx))
            {
                return true;
            }
        }
        return false;
    }

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
    void AddAnimObject(AnimObject* animObj)
    {
        NW_ASSERT(m_AnimObjects.Size() < m_AnimObjects.Capacity());
        m_AnimObjects.PushBack(animObj);
    }

    //
    //
    //
    //
    //
    //
    const AnimObject* GetAnimObject(int animObjIdx) const
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_AnimObjects.Size());
        return m_AnimObjects[animObjIdx];
    }

    //
    //
    //
    //
    //
    //
    AnimObject* GetAnimObject(int animObjIdx)
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_AnimObjects.Size());
        return m_AnimObjects[animObjIdx];
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
    AnimObject* ReplaceAnimObject(int animObjIdx, AnimObject* animObj)
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_AnimObjects.Size());
        AnimObject* oldObj = m_AnimObjects[animObjIdx];
        m_AnimObjects[animObjIdx] = animObj;
        return oldObj;
    }

    //
    void ClearAnimObjects()
    {
        m_AnimObjects.Clear();
    }

    //
    int GetAnimObjectCount() const
    {
        return m_AnimObjects.Size();
    }

    //
    int GetMaxAnimObjects() const
    {
        return m_AnimObjects.Capacity();
    }

    //

    //----------------------------------------
    //
    //

    //
    virtual void UpdateCache()
    {
        for (int animObjIdx = 0; animObjIdx < m_AnimObjects.Size(); ++animObjIdx)
        {
            if (m_AnimObjects[animObjIdx] != NULL)
            {
                m_AnimObjects[animObjIdx]->UpdateCache();
            }
        }
    }

    //

protected:
    //
    //
    //
    static void GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize, int maxAnimObjects)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(AnimObject*) * maxAnimObjects;
    }

    //
    Result Initialize(int maxAnimObjects)
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

    AnimObjectArray m_AnimObjects; //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_ANIMBLENDER_H_
