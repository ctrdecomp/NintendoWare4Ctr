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

#ifndef NW_GFX_IMATERIALACTIVATOR_H_
#define NW_GFX_IMATERIALACTIVATOR_H_

#include <nw/gfx/gfx_GfxObject.h>

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
//---------------------------------------------------------------------------
class IMaterialActivator : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(IMaterialActivator);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    virtual void Activate(RenderContext* renderContext, const Material* material) = 0;

protected:
    //
    IMaterialActivator(os::IAllocator* allocator) : GfxObject(allocator) {}

    //
    virtual ~IMaterialActivator() = 0;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_IMATERIALACTIVATOR_H_
