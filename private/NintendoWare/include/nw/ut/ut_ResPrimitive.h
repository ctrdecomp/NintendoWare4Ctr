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

#ifndef NW_UT_RESPRIMITIVE_H_
#define NW_UT_RESPRIMITIVE_H_

#include <nw/types.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDeclMacros.h>

namespace nw {
namespace ut {

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResBoundingVolumeData
{
    nw::ut::ResTypeInfo typeInfo;   //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResOrientedBoundingBoxData : public ResBoundingVolumeData
{
    ResVec3   m_CenterPosition;     //
    ResMtx33  m_OrientationMatrix;  //
    ResVec3   m_Size;               //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResAxisAlignedBoundingBoxData : public ResBoundingVolumeData
{
    ResVec3   m_CenterPosition;     //
    ResVec3   m_Size;               //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResBoundingVolume : public nw::ut::ResCommon<ResBoundingVolumeData>
{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResBoundingVolume) };

    NW_RES_CTOR( ResBoundingVolume )

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
class ResOrientedBoundingBox : public ResBoundingVolume
{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResOrientedBoundingBox) };

    NW_RES_CTOR_INHERIT( ResOrientedBoundingBox, ResBoundingVolume )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, CenterPosition ) // GetCenterPosition()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRUCT_DECL( nw::math::MTX33, OrientationMatrix ) // GetOrientationMatrix()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Size ) // GetSize()
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResAxisAlignedBoundingBox : public ResBoundingVolume
{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResAxisAlignedBoundingBox) };

    NW_RES_CTOR_INHERIT( ResAxisAlignedBoundingBox, ResBoundingVolume )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, CenterPosition ) // GetCenterPosition()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, Size ) // GetSize()
};

} /* namespace ut */
} /* namespace nw */

#endif /* NW_UT_RESPRIMITIVE_H_ */
