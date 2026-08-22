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

#ifndef NW_GFX_MATERIALACTIVATOR_H_
#define NW_GFX_MATERIALACTIVATOR_H_

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
class MaterialActivator : public IMaterialActivator
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(MaterialActivator);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static MaterialActivator* Create(os::IAllocator* allocator);

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

        size += sizeof(MaterialActivator);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    virtual void Activate(RenderContext* renderContext, const Material* material);

private:
    //
    MaterialActivator(os::IAllocator* allocator);

    //
    virtual ~MaterialActivator();
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_MATERIALACTIVATOR_H_
