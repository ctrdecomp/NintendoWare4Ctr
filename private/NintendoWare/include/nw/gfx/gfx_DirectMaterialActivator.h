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

#ifndef NW_GFX_DIRECTMATERIALACTIVATOR_H_
#define NW_GFX_DIRECTMATERIALACTIVATOR_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_IMaterialActivator.h>

namespace nw
{
namespace os
{
class IAllocator;
} // namesapce os

namespace gfx
{

class RenderContext;
class Material;

//---------------------------------------------------------------------------
//
//
//
//
//---------------------------------------------------------------------------
class DirectMaterialActivator : public IMaterialActivator
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(DirectMaterialActivator);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static DirectMaterialActivator* Create(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size);

        return size.GetSizeWithPadding(alignment);
    }


    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(DirectMaterialActivator);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    virtual void Activate(RenderContext* renderContext, const Material* material);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetActivateFlags() const
    {
        return this->m_ActivateFlags;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetActivateFlags(u32 flags)
    {
        this->m_ActivateFlags = flags;
    }

private:
    //
    DirectMaterialActivator(os::IAllocator* allocator);

    //
    virtual ~DirectMaterialActivator();

    u32 m_ActivateFlags;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_DIRECTMATERIALACTIVATOR_H_
