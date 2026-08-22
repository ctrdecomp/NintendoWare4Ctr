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

#ifndef NW_GFX_IMATERIALIDGENERATOR_H_
#define NW_GFX_IMATERIALIDGENERATOR_H_

namespace nw
{
namespace gfx
{

class Material;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class IMaterialIdGenerator : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(IMaterialIdGenerator);

public:
    NW_UT_RUNTIME_TYPEINFO;



    //
    //
    //
    //
    //
    virtual void Accept(Material* material) = 0;

    //----------------------------------------
    //
    //

    //
    //
    virtual void Generate() = 0;

    //

protected:
    //----------------------------------------
    //
    //

    //
    IMaterialIdGenerator(os::IAllocator* allocator) : GfxObject(allocator) {}

    //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_IMATERIALIDGENERATOR_H_
