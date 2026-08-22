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

#ifndef NW_GFX_CAMERAPROJECTIONUPDATER_H_
#define NW_GFX_CAMERAPROJECTIONUPDATER_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResCamera.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{

namespace math
{
    struct VEC2;
    struct Transform3;
    struct MTX34;
}

namespace ut
{
    struct Rect;
}

namespace gfx
{

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class CameraProjectionUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(CameraProjectionUpdater);

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
    //---------------------------------------------------------------------------
    virtual void Update(math::MTX44* projectionMatrix, math::Matrix34* textureProjectionMatrix) = 0;

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    void SetPivotDirection( math::PivotDirection pivot) { this->m_Pivot = pivot; }

    //
    //
    //
    math::PivotDirection GetPivotDirection() const { return this->m_Pivot; }

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
    virtual ResCameraProjectionUpdater GetResource() = 0;

    //
    virtual const ResCameraProjectionUpdater GetResource() const = 0;

    //

    //----------------------------------------
    //
    //

    //
    math::VEC2& TextureScale() { return m_TextureScale; }

    //
    const math::VEC2& TextureScale() const { return m_TextureScale; }

    //
    math::VEC2& TextureTranslate() { return m_TextureTranslate; }

    //
    const math::VEC2& TextureTranslate() const { return m_TextureTranslate; }

    //

    //
    virtual anim::ResCameraAnimData::ProjectionUpdaterKind Kind() const = 0;

protected:

    //
    CameraProjectionUpdater(os::IAllocator* allocator, bool isDynamic)
        : GfxObject(allocator),
        m_Pivot(math::PIVOT_NONE),
        m_IsDynamic(isDynamic),
        m_TextureScale(0.5f, 0.5f),
        m_TextureTranslate(0.5f, 0.5f)
    {}

    //
    virtual ~CameraProjectionUpdater(){}

private:
    math::PivotDirection m_Pivot;
    bool m_IsDynamic;
    math::VEC2 m_TextureScale;
    math::VEC2 m_TextureTranslate;

protected:
    //
    static const float PROJECTION_NEAR_CLIP;
    //
    static const float PROJECTION_FAR_CLIP;
    //
    static const float PROJECTION_FOVY_RADIAN;
    //
    static const float PROJECTION_ASPECT_RATIO;
    //
    static const math::VEC2 PROJECTION_CENTER;
    //
    static const float PROJECTION_HEIGHT;
    //
    static const ResProjectionRect PROJECTION_RECT;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_CAMERAPROJECTIONUPDATER_H_
