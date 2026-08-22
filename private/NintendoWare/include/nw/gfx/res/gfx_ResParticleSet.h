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

#ifndef NW_GFX_RESPARTICLESET_H_
#define NW_GFX_RESPARTICLESET_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResParticleCollection.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResParticleInitializer.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>
#include <nw/gfx/res/gfx_ResParticleShapeBuilder.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
namespace nw {
namespace gfx {
namespace res {

//
typedef ut::ResArrayClass<ResParticleInitializer>::type::iterator ResParticleInitializerArrayIterator;
//
typedef ut::ResArrayClass<const ResParticleInitializer>::type::const_iterator ResParticleInitializerArrayConstIterator;
//
typedef ut::ResArrayClass<ResParticleInitializer>::type ResParticleInitializerArray;
//
typedef ut::ResArrayClass<const ResParticleInitializer>::type ResParticleInitializerArrayConst;


//
typedef ut::ResArrayClass<ResParticleUpdater>::type::iterator ResParticleUpdaterArrayIterator;
//
typedef ut::ResArrayClass<const ResParticleUpdater>::type::const_iterator ResParticleUpdaterArrayConstIterator;

//
typedef ut::ResArrayClass<ResParticleUpdater>::type ResParticleUpdaterArray;
//
typedef ut::ResArrayClass<const ResParticleUpdater>::type ResParticleUpdaterArrayConst;

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
//
struct ResParticleSetData : public ResSceneNodeData
{
    nw::ut::Offset toParticleCollection;                //
    nw::ut::ResS32 m_ParticleInitializersTableCount;    //
    nw::ut::Offset toParticleInitializersTable;         //
    nw::ut::ResS32 m_ParticleUpdatersTableCount;        //
    nw::ut::Offset toParticleUpdatersTable;             //
    nw::ut::Offset toParticleShapeBuilder;              //
    nw::ut::ResS32 m_ParticleShapeIndex;                //
    nw::ut::ResU32 m_ParticleSetPriority;               //
    nw::ut::ResBool m_IsForceWorld;                     //
    nw::ut::ResBool m_IsBufferFlushEnabled;             //
    nw::ut::ResBool m_IsInheritParentVelocity;          //
};


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
class ResParticleSet : public ResSceneNode
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleSet) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PSET') };

    NW_RES_CTOR_INHERIT( ResParticleSet, ResSceneNode )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResParticleCollection, ParticleCollection )         // GetParticleCollection()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResParticleInitializer, ParticleInitializers ) // GetParticleInitializers(int idx), GetParticleInitializersCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResParticleUpdater, ParticleUpdaters )         // GetParticleUpdaters(int idx), GetParticleUpdatersCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResParticleShapeBuilder, ParticleShapeBuilder )         // GetParticleShpaBuilder()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, ParticleShapeIndex)                            // GetParticleShapeIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ParticleSetPriority)                       // GetParticleSetPriority()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsForceWorld)                             // GetIsForceWorld()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsBufferFlushEnabled)                     // GetIsBufferFlushEnabled()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( bool, IsInheritParentVelocity)                     // GetIsInheritParentVelocity()

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Setup();
};

//
typedef nw::ut::ResArrayPatricia<ResParticleSet>::type  ResParticleSetArray;

} // namespace res
} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif
#endif

#endif // NW_GFX_RESPARTICLESET_H_
