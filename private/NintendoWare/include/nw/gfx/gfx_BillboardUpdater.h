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

#ifndef NW_GFX_BILLBOARDUPDATER_H_
#define NW_GFX_BILLBOARDUPDATER_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

namespace nw
{
namespace os
{
class IAllocator;
} // namesapce os

namespace gfx
{

class Matrix34;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class BillboardUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(BillboardUpdater);

public:
    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static BillboardUpdater* Create(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void Update(
        math::MTX34* worldMatrix,
        const math::MTX34& viewMatrix,
        const math::MTX34& inverseViewMatrix,
        const math::VEC3& cameraPosition,
        const CalculatedTransform& worldTransform,
        const CalculatedTransform& localTransform,
        ResBone::BillboardMode billboardMode) const;

private:
    //
    BillboardUpdater(os::IAllocator* allocator);

    //
    virtual ~BillboardUpdater();

    void CalculateLocalMatrix(
        math::MTX34* localMatrix,
        const CalculatedTransform& transform,
        math::VEC3 zAxis,
        bool recalculateYAxis = true) const;

    void CalculateScreenLocalMatrix(
        math::MTX34* localMatrix,
        const CalculatedTransform& transform,
        const math::MTX34& inverseViewMatrix,
        math::VEC3 yAxis,
        math::VEC3& zAxis) const;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_BILLBOARDUPDATER_H_
