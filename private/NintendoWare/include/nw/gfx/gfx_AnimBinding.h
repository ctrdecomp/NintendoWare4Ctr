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

#ifndef NW_GFX_ANIMBINDING_H_
#define NW_GFX_ANIMBINDING_H_

#include <nw/gfx/gfx_AnimGroup.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {

class AnimEvaluator;
class TransformAnimEvaluator;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class AnimBinding : public GfxObject
{
public:

    //
    //
    //
    typedef ut::MoveArray<AnimGroup*> AnimGroupArray;

    //
    //
    //
    typedef ut::MoveArray<AnimObject*> AnimObjectArray;

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        //
        Builder()
        :
          m_MaxAnimGroups(1),
          m_MaxAnimObjectsPerGroup(1) {}

        //
        Builder& MaxAnimGroups(int maxAnimGroups)
        {
            NW_ASSERT(maxAnimGroups > 0);
            m_MaxAnimGroups = maxAnimGroups;
            return *this;
        }

        //
        //
        //
        Builder& MaxAnimObjectsPerGroup(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjectsPerGroup = maxAnimObjects;
            return *this;
        }

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

            size += sizeof(AnimBinding);
            AnimBinding::GetMemorySizeForInitialize(pSize, m_MaxAnimGroups, m_MaxAnimObjectsPerGroup);
        }

        //
        //
        //
        //
        //
        //
        AnimBinding* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimBinding));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimBinding* animBinding = new(buf) AnimBinding(allocator, m_MaxAnimObjectsPerGroup);

            Result result = animBinding->Initialize(m_MaxAnimGroups, m_MaxAnimObjectsPerGroup);

            if (result.IsSuccess())
            {
                return animBinding;
            }
            else
            {
                SafeDestroy(animBinding);
                return NULL;
            }
        }

    private:
        int m_MaxAnimGroups;
        int m_MaxAnimObjectsPerGroup;
    };

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
    void Evaluate(anim::ResGraphicsAnimGroup::EvaluationTiming timing);

    //
    //
    //
    //
    void UpdateFrame()
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
    int GetAnimGroupCount() const { return m_AnimGroups.Size(); }

    //
    const AnimGroup* GetAnimGroup(int groupIdx) const
    {
        NW_MINMAXLT_ASSERT(groupIdx, 0, m_AnimGroups.Size());
        return m_AnimGroups[groupIdx];
    }

    //
    AnimGroup* GetAnimGroup(int groupIdx)
    {
        NW_MINMAXLT_ASSERT(groupIdx, 0, m_AnimGroups.Size());
        return m_AnimGroups[groupIdx];
    }

    //
    //
    //
    //
    //
    //
    //
    int GetAnimGroupIndex(const char* animGroupName) const
    {
        for (int groupIdx = 0; groupIdx < m_AnimGroups.Size(); ++groupIdx)
        {
            if (m_AnimGroups[groupIdx] != NULL &&
                ::std::strcmp(m_AnimGroups[groupIdx]->GetName(), animGroupName) == 0)
            {
                return groupIdx;
            }
        }
        return -1;
    }

    //
    void SetAnimGroup(int groupIdx, AnimGroup* animGroup)
    {
        NW_MINMAXLT_ASSERT(groupIdx, 0, m_AnimGroups.Size());
        m_AnimGroups[groupIdx] = animGroup;
    }

    //
    const AnimObject* GetAnimObject(int groupIdx, int objectIdx = 0) const
    {
        NW_MINMAXLT_ASSERT(groupIdx, 0, m_AnimObjects.Size());
        NW_MINMAXLT_ASSERT(objectIdx, 0, m_AnimObjectCountPerGroup);

        const int index = groupIdx * m_AnimObjectCountPerGroup + objectIdx;
        return m_AnimObjects[index];
    }

    //
    AnimObject* GetAnimObject(int groupIdx, int objectIdx = 0)
    {
        NW_MINMAXLT_ASSERT(groupIdx, 0, m_AnimGroups.Size());
        NW_MINMAXLT_ASSERT(objectIdx, 0, m_AnimObjectCountPerGroup);

        const int index = groupIdx * m_AnimObjectCountPerGroup + objectIdx;
        return m_AnimObjects[index];
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
    //
    bool SetAnimObject(AnimObject* animObject, int objectIdx = 0)
    {
        const AnimGroup* animGroup = animObject->GetAnimGroup();
        NW_NULL_ASSERT(animGroup);
        return SetAnimObject(animGroup->GetName(), animObject, objectIdx);
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
    bool SetAnimObject(int groupIdx, AnimObject* animObject, int objectIdx = 0)
    {
        // When groupIdx is entered here, it means this is the target group index
        if ((0 <= groupIdx && groupIdx < m_AnimGroups.Size()) &&
            (0 <= objectIdx && objectIdx < m_AnimObjectCountPerGroup))
        {
            const int index = groupIdx * m_AnimObjectCountPerGroup + objectIdx;
            m_AnimObjects[index] = animObject;
            return true;
        }

        NW_WARNING(false, "groupIdx or objectIdx is out of bounds");
        return false;
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
    bool SetAnimObject(const char* animGroupName, AnimObject* animObject, int objectIdx = 0)
    {
        return SetAnimObject(GetAnimGroupIndex(animGroupName), animObject, objectIdx);
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    AnimBinding(
        os::IAllocator* allocator,
        int maxAnimObjects)
    : GfxObject(allocator)
    , m_AnimObjectCountPerGroup(maxAnimObjects)
    {
    }

    //
    //
    //
    virtual ~AnimBinding()
    {
        // The content of both AnimGroups and AnimObjects are deleted based on generation.
        // For AnimBinding it is just referencing; nothing is taking place.
    }

    //

    //
    //
    //
    static void GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize, int maxAnimGroups, int maxAnimObjects)
    {
        os::MemorySizeCalculator& size = *pSize;

        const int animObjectCount = maxAnimGroups * maxAnimObjects;

        size += sizeof(AnimGroup*) * maxAnimGroups;
        size += sizeof(AnimObject*) * animObjectCount;
    }

    //
    //
    //
    Result Initialize(int maxAnimGroups, int maxAnimObjects)
    {
        // The number of AnimObject items is equal to the number of AnimObjects items set in Builder * the number of Group items.
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
    //----------------------------------------
    // Evaluations

    // The evaluation process when the blender is not used.
    // Unneeded loops are omitted and processing occurs at a faster speed.
    NW_FORCE_INLINE void EvaluateSimple(AnimGroup* animGroup, AnimEvaluator* evaluator);
    NW_FORCE_INLINE void EvaluateTransformSimple(AnimGroup* animGroup, TransformAnimEvaluator* evaluator);

    // The evaluation process when the blender is used.
    NW_FORCE_INLINE void EvaluateBlender(AnimGroup* animGroup, AnimObject* animObj);

    NW_FORCE_INLINE void EvaluateTransformMember(AnimGroup* animGroup, int memberIdx, AnimObject* animObj); //
    NW_FORCE_INLINE void EvaluateTransformMemberFast(AnimGroup* animGroup, int memberIdx, TransformAnimEvaluator* evaluator); //
    NW_FORCE_INLINE void EvaluateMember(AnimGroup* animGroup, int memberIdx, AnimObject* animObj, int& lastTargetObjIdx, bool& targetObjSkipFlag); //

    AnimGroupArray m_AnimGroups; //
    AnimObjectArray m_AnimObjects; //
    const int m_AnimObjectCountPerGroup; //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_ANIMBINDING_H_
