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

#ifndef NW_GFX_SKELETONUPDATER_H_
#define NW_GFX_SKELETONUPDATER_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw
{
namespace os
{
class IAllocator;
} // namesapce os

namespace gfx
{

class Skeleton;
class WorldMatrixUpdater;
class BillboardUpdater;
class Camera;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SkeletonUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SkeletonUpdater);

public:
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    class Builder
    {
    public:
        //
        Builder() {}

        //---------------------------------------------------------------------------
        //
        //
        //
        //
        //
        //---------------------------------------------------------------------------
        SkeletonUpdater* Create(os::IAllocator* allocator);
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void UpdateWorld(
        Skeleton* skeleton,
        const WorldMatrixUpdater& worldMatrixUpdater) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void UpdateView(
        Skeleton* skeleton,
        const BillboardUpdater& billboardUpdater,
        const Camera& camera) const;

private:
    //
    SkeletonUpdater(os::IAllocator* allocator);

    //
    virtual ~SkeletonUpdater();
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_SKELETONUPDATER_H_
