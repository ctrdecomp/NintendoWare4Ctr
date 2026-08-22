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

#ifndef NW_GFX_ANIMADDER_H_
#define NW_GFX_ANIMADDER_H_

#include <nw/gfx/gfx_AnimBlender.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class AnimAdder : public AnimBlender
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        //
        Builder()
        : m_MaxAnimObjects(2) {}

        //
        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
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

            size += sizeof(AnimAdder);
            AnimAdder::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        //
        //
        //
        //
        //
        //
        AnimAdder* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimAdder));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimAdder* animAdder = new(buf) AnimAdder(allocator);

            Result result = animAdder->Initialize(m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());

            return animAdder;
        }

    private:
        int m_MaxAnimObjects;
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
    //
    virtual const anim::AnimResult* GetResult(
        void* target,
        int memberIdx) const;

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
    float GetWeight(int animObjIdx) const
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_Weights.Size());
        return m_Weights[animObjIdx];
    }

    //
    //
    //
    //
    //
    void SetWeight(int animObjIdx, float weight)
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_Weights.Size());
        m_Weights[animObjIdx] = weight;
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    AnimAdder(
        os::IAllocator* allocator)
    : AnimBlender(allocator)
    {
    }

    //
    virtual ~AnimAdder() {}

    //

    //
    //
    //
    static void GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize, int maxAnimObjects)
    {
        os::MemorySizeCalculator& size = *pSize;

        AnimBlender::GetMemorySizeForInitialize(pSize, maxAnimObjects);
        size += sizeof(float) * maxAnimObjects;
    }

    Result Initialize(int maxAnimObjects)
    {
        Result result = AnimBlender::Initialize(maxAnimObjects);
        NW_ENSURE_AND_RETURN(result);

        void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
        if (memory == NULL)
        {
            result |= Result::MASK_FAIL_BIT;
        }
        NW_ENSURE_AND_RETURN(result);

        m_Weights = ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
        for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx)
        {
            m_Weights.PushBackFast(1.0f);
        }

        return result;
    }

    ut::MoveArray<float> m_Weights;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_ANIMADDER_H_
