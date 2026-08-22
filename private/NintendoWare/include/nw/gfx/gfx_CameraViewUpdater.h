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

#ifndef NW_GFX_CAMERAVIEWUPDATER_H_
#define NW_GFX_CAMERAVIEWUPDATER_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResCamera.h>
#include <nw/anim/res/anim_ResAnim.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{

namespace math
{
    struct VEC3;
    struct MTX34;
}

namespace gfx
{

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class CameraViewUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(CameraViewUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void virtual Update(
        math::MTX34* viewMatrix,
        const math::MTX34& worldMatrix,
        const math::VEC3& cameraPosition) = 0;

    //

    //
    bool IsDynamic()
    {
        return this->m_IsDynamic;
    }

    //----------------------------------------
    //
    //

    //
    virtual ResCameraViewUpdater GetResource() = 0;

    //
    virtual const ResCameraViewUpdater GetResource() const = 0;

    //

    //
    virtual anim::ResCameraAnimData::ViewUpdaterKind Kind() const = 0;


protected:

    //
    CameraViewUpdater(os::IAllocator* allocator, bool isDynamic)
        : GfxObject(allocator),
          m_IsDynamic(isDynamic)
    {}

protected:
    //
    static const math::VEC3 VIEW_TARGET_POSITION;
    //
    static const math::VEC3 VIEW_UPWARD_VECTOR;
    //
    static const math::VEC3 VIEW_VIEW_ROTATE;
    //
    static const float VIEW_TWIST;

private:
    bool m_IsDynamic;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_CAMERAVIEWUPDATER_H_
