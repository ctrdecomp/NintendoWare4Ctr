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

#ifndef NW_GFX_LOOKATTARGETVIEWUPDATER_H_
#define NW_GFX_LOOKATTARGETVIEWUPDATER_H_

#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/ut/ut_Preprocessor.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
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
class LookAtTargetViewUpdater : public CameraViewUpdater
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(LookAtTargetViewUpdater);

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
    static LookAtTargetViewUpdater* Create(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static LookAtTargetViewUpdater* Create(
        os::IAllocator* allocator,
        ResLookAtTargetViewUpdater resUpdater);

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        bool isDynamicBuild)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(LookAtTargetViewUpdater);
        if (isDynamicBuild)
        {
            size += sizeof(ResLookAtTargetViewUpdaterData);
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
    //
    //---------------------------------------------------------------------------
    void virtual Update(
        math::MTX34* viewMatrix,
        const math::MTX34& worldMatrix,
        const math::VEC3& cameraPosition);

    //

    //----------------------------------------
    //
    //

    //
    virtual ResCameraViewUpdater GetResource()
    {
        return this->m_Resource;
    }

    //
    virtual const ResCameraViewUpdater GetResource() const
    {
        return this->m_Resource;
    }

    //

    //
    virtual anim::ResCameraAnimData::ViewUpdaterKind Kind() const
    {
        return anim::ResCameraAnimData::VIEW_UPDATER_LOOKAT;
    }

private:
    //----------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    LookAtTargetViewUpdater(
        os::IAllocator* pAllocator,
        bool isDynamic,
        ResLookAtTargetViewUpdater resUpdater);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    virtual ~LookAtTargetViewUpdater();
    //

    ResLookAtTargetViewUpdater m_Resource;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_LOOKATTARGETVIEWUPDATER_H_
