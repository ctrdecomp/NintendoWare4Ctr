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

#ifndef NW_GFX_TRANSFORMANIMADDER_H_
#define NW_GFX_TRANSFORMANIMADDER_H_

#include <nw/gfx/gfx_AnimAdder.h>

namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class TransformAnimAdder : public AnimAdder
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

            size += sizeof(TransformAnimAdder);
            TransformAnimAdder::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        //
        //
        //
        //
        //
        //
        TransformAnimAdder* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimAdder));

            if (buf == NULL)
            {
                return NULL;
            }

            TransformAnimAdder* adder = new(buf) TransformAnimAdder(allocator);

            Result result = adder->Initialize(m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());

            return adder;
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
    virtual const anim::AnimResult* GetResult(
        void* target,
        int memberIdx) const;

    //

protected:
    //----------------------------------------
    //
    //

    //
    TransformAnimAdder(
        os::IAllocator* allocator)
    : AnimAdder(allocator)
    {}

    //
    virtual ~TransformAnimAdder() {}

    //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_TRANSFORMANIMADDER_H_
