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

#ifndef NW_GFX_RESPARTICLECOLLECTION_H_
#define NW_GFX_RESPARTICLECOLLECTION_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>

namespace nw {
namespace gfx {
namespace res {

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResParticleAttributeData
{
    nw::ut::ResTypeInfo typeInfo;   //
    nw::ut::ResS32 m_Usage;         //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResParticleAttribute : public nw::ut::ResCommon< ResParticleAttributeData >
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleAttribute) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PSAT') };

    NW_RES_CTOR( ResParticleAttribute )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Usage )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

//
typedef nw::ut::ResArrayClass<ResParticleAttribute>::type  ResParticleAttributeArray;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResParticleStreamAttributeData : public ResParticleAttributeData
{
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResParticleStreamAttribute : public ResParticleAttribute
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleStreamAttribute) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PSST') };

    NW_RES_CTOR_INHERIT( ResParticleStreamAttribute, ResParticleAttribute )
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResParticleParameterAttributeData : public ResParticleAttributeData
{
    nw::ut::ResS32 m_Dimension;
    nw::ut::ResS32 m_DataTableCount;
    nw::ut::Offset toDataTable;
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResParticleParameterAttribute : public ResParticleAttribute
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleParameterAttribute) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PSPA') };

    NW_RES_CTOR_INHERIT( ResParticleParameterAttribute, ResParticleAttribute )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Dimension )

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( f32, Data )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResParticleCollectionData
{
    nw::ut::ResS32 m_Capacity;              //
    nw::ut::ResS32 m_AttributesTableCount;  //
    nw::ut::Offset toAttributesTable;       //
};



//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResParticleCollection : public nw::ut::ResCommon< ResParticleCollectionData >
{
public:
    NW_RES_CTOR( ResParticleCollection )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Capacity )

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
    NW_RES_FIELD_CLASS_LIST_DECL( ResParticleAttribute, Attributes )
};

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESPARTICLECOLLECTION_H_
