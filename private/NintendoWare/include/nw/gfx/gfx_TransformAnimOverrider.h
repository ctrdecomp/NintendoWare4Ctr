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

#ifndef NW_GFX_TRANSFORMANIMOVERRIDER_H_
#define NW_GFX_TRANSFORMANIMOVERRIDER_H_

#include <nw/gfx/gfx_AnimOverrider.h>

namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
class TransformAnimOverrider : public AnimOverrider
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

            size += sizeof(TransformAnimOverrider);
            TransformAnimOverrider::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        //
        //
        //
        //
        //
        //
        TransformAnimOverrider* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimOverrider));

            if (buf == NULL)
            {
                return NULL;
            }

            TransformAnimOverrider* overrider = new(buf) TransformAnimOverrider(allocator);

            Result result = overrider->Initialize(m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());

            return overrider;
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
    //
    //
    TransformAnimOverrider(
        os::IAllocator* allocator)
    : AnimOverrider(allocator)
    {}

    //
    //
    //
    virtual ~TransformAnimOverrider() {}

    //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_TRANSFORMANIMOVERRIDER_H_
