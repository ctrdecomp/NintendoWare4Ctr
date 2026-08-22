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

#ifndef NW_GFX_TRANSFORMANIMINTERPOLATOR_H_
#define NW_GFX_TRANSFORMANIMINTERPOLATOR_H_

#include <nw/gfx/gfx_AnimInterpolator.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
class TransformAnimInterpolator : public AnimInterpolator
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    class Builder
    {
    public:
        //
        Builder()
        : m_MaxAnimObjects(2),
          m_IgnoreNoAnimMember(false) {}

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
        //
        //
        Builder& IgnoreNoAnimMember(bool ignoreNoAnimMember) { m_IgnoreNoAnimMember = ignoreNoAnimMember; return *this; }

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

            size += sizeof(TransformAnimInterpolator);
            TransformAnimInterpolator::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        //
        //
        //
        //
        //
        //
        TransformAnimInterpolator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimInterpolator));

            if (buf == NULL)
            {
                return NULL;
            }

            TransformAnimInterpolator* interpolator = new(buf) TransformAnimInterpolator(allocator);

            Result result = interpolator->Initialize(m_MaxAnimObjects, m_IgnoreNoAnimMember);
            NW_ASSERT(result.IsSuccess());

            return interpolator;
        }

    private:
        int m_MaxAnimObjects;
        bool m_IgnoreNoAnimMember;
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
    TransformAnimInterpolator(
        os::IAllocator* allocator)
    : AnimInterpolator(allocator)
    {}

    //
    //
    //
    virtual ~TransformAnimInterpolator() {}

    //
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_TRANSFORMANIMINTERPOLATOR_H_
