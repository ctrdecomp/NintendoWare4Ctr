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

#ifndef NW_GFX_PERSPECTIVEPROJECTIONUPDATER_H_
#define NW_GFX_PERSPECTIVEPROJECTIONUPDATER_H_

#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw
{
namespace os
{
class IAllocator;
} // namespace os
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class PerspectiveProjectionUpdater : public CameraProjectionUpdater
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(PerspectiveProjectionUpdater);

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
    static PerspectiveProjectionUpdater* Create(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static PerspectiveProjectionUpdater* Create(
        os::IAllocator* allocator,
        ResPerspectiveProjectionUpdater resUpdater);

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        bool isDynamicBuild)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(PerspectiveProjectionUpdater);
        if (isDynamicBuild)
        {
            size += sizeof(ResPerspectiveProjectionUpdaterData);
        }
    }

    //

    //----------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void virtual Update(math::MTX44* projectionMatrix, math::Matrix34* textureProjectionMatrix);

    //

    //----------------------------------------
    //
    //

    //
    virtual ResCameraProjectionUpdater GetResource()
    {
        return this->m_Resource;
    }

    //
    virtual const ResCameraProjectionUpdater GetResource() const
    {
        return this->m_Resource;
    }

    //


    //
    virtual anim::ResCameraAnimData::ProjectionUpdaterKind Kind() const
    {
        return anim::ResCameraAnimData::PROJECTION_UPDATER_PERSPECTIVE;
    }

private:
    //----------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    PerspectiveProjectionUpdater(
        os::IAllocator* pAllocator,
        bool isDynamic,
        ResPerspectiveProjectionUpdater resUpdater);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    virtual ~PerspectiveProjectionUpdater();
    //

    ResPerspectiveProjectionUpdater m_Resource;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_PERSPECTIVEPROJECTIONUPDATER_H_
