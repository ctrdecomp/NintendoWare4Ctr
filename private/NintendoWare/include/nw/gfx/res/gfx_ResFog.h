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

#ifndef NW_GFX_RESFOG_H_
#define NW_GFX_RESFOG_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/res/gfx_ResRevision.h>

namespace nw {
namespace gfx {
namespace res {

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResFogUpdaterData
{
    nw::ut::ResS32 m_FogUpdaterType;    //
    nw::ut::ResF32 m_MinFogDepth;       //
    nw::ut::ResF32 m_MaxFogDepth;       //
    nw::ut::ResF32 m_Density;           //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResFogData : public ResTransformNodeData
{
    //
    //--------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_ZFLIP_ENABLED_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,    //
        FLAG_ATTENUATE_DISTANCE_ENABLED_SHIFT,                          //
        FLAG_SHIFT_MAX,                                                 //

        FLAG_ZFLIP_ENABLED              = 0x1 << FLAG_ZFLIP_ENABLED_SHIFT,              //
        FLAG_ATTENUATE_DISTANCE_ENABLED = 0x1 << FLAG_ATTENUATE_DISTANCE_ENABLED_SHIFT  //
    };

    nw::ut::ResFloatColor m_Color;  //
    nw::ut::Offset toFogSampler;    //
    nw::ut::Offset toFogUpdater;    //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResFogUpdater : public nw::ut::ResCommon<ResFogUpdaterData>
{
public:
    //--------------------------------------------------------------------------
    //
    //--------------------------------------------------------------------------
    enum FogUpdaterType
    {
        FOG_UPDATER_TYPE_NONE,              //
        FOG_UPDATER_TYPE_LINEAR,            //
        FOG_UPDATER_TYPE_EXPONENT,          //
        FOG_UPDATER_TYPE_EXPONENT_SQUARE    //
    };

    NW_RES_CTOR( ResFogUpdater )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( FogUpdaterType, FogUpdaterType )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, MinFogDepth )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, MaxFogDepth )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Density )
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResFog : public ResTransformNode
{
public:
    enum { BINARY_REVISION = REVISION_RES_FOG };
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResFog) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CFOG') };

    //--------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_ZFLIP_ENABLED_SHIFT = ResTransformNode::FLAG_SHIFT_MAX,    //
        FLAG_ATTENUATE_DISTANCE_ENABLED_SHIFT,                          //
        FLAG_SHIFT_MAX,                                                 //

        FLAG_ZFLIP_ENABLED              = 0x1 << FLAG_ZFLIP_ENABLED_SHIFT,              //
        FLAG_ATTENUATE_DISTANCE_ENABLED = 0x1 << FLAG_ATTENUATE_DISTANCE_ENABLED_SHIFT  //
    };

    NW_RES_CTOR_INHERIT( ResFog, ResTransformNode )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_COLOR_DECL( nw::ut::FloatColor, Color )   // FloatColor& GetColor()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResImageLookupTable, FogSampler )  // GetFogSampler()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResFogUpdater, FogUpdater )    // GetFogUpdater

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetRevision() const { return this->GetHeader().revision; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool    IsZFlip() const
        { return (ref().m_Flags & ResFogData::FLAG_ZFLIP_ENABLED) != 0; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool    IsAttenuateDistance() const
        { return (ref().m_Flags & ResFogData::FLAG_ATTENUATE_DISTANCE_ENABLED) != 0; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Result Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();
};

//
typedef nw::ut::ResArrayPatricia<ResFog>::type  ResFogArray;

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESFOG_H_
