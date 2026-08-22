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
#ifndef NW_GFX_RESSCENEENVIRONMENTSETTING_H_
#define NW_GFX_RESSCENEENVIRONMENTSETTING_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResFog.h>

namespace nw
{
namespace gfx
{
namespace res
{

class ResCamera;
class ResLight;


//
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResReferenceSceneObjectData
{
    nw::ut::ResS32 m_Index;     //
    nw::ut::BinString toPath;   //
    nw::ut::Offset toTarget;    //
};


//
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResLightSetData
{
    nw::ut::ResS32 m_Index;             //
    nw::ut::ResS32 m_LightsTableCount;  //
    nw::ut::Offset toLightsTable;       //
};

//
//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResSceneEnvironmentSettingData : public ResSceneObjectData
{
    nw::ut::ResS32 m_CamerasTableCount;     //
    nw::ut::Offset toCamerasTable;          //
    nw::ut::ResS32 m_LightSetsTableCount;   //
    nw::ut::Offset toLightSetsTable;        //
    nw::ut::ResS32 m_FogsTableCount;        //
    nw::ut::Offset toFogsTable;             //
};


//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResReferenceSceneObject : public nw::ut::ResCommon<ResReferenceSceneObjectData>
{
public:
    NW_RES_CTOR( ResReferenceSceneObject )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Index ) // GetIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( Path ) // GetPath()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResSceneObject, Target ) // GetTarget()
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResLightSet : public nw::ut::ResCommon<ResLightSetData>
{
public:
    NW_RES_CTOR(ResLightSet);

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Index ) // GetIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL(ResReferenceSceneObject, Lights) // GetLights(), GetLights(int idx), GetLightsCount()
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResSceneEnvironmentSetting : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneEnvironmentSetting) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CENV') };
    enum { BINARY_REVISION = REVISION_RES_RENDER_ENV_SETTING };

    NW_RES_CTOR_INHERIT( ResSceneEnvironmentSetting, ResSceneObject )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetRevision() const { return this->GetHeader().revision; }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResReferenceSceneObject, Cameras ) // GetCameras(int idx), GetCamerasCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResLightSet, LightSets ) // GetLightSets(int idx), GetLightSetsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResReferenceSceneObject, Fogs ) // GetFogs(int idx), GetFogsCount()
};

//
typedef nw::ut::ResArrayPatricia<ResSceneEnvironmentSetting>::type  ResSceneEnvironmentSettingArray;
//
typedef nw::ut::ResArrayClass<ResReferenceSceneObject>::type ResReferenceSceneObjectArray;
//
typedef nw::ut::ResArrayClass<ResLightSet>::type ResLightSetArray;

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESSCENEENVIRONMENTSETTING_H_

