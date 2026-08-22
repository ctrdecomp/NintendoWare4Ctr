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

#ifndef NW_GFX_RESPARTICLESHAPE_H_
#define NW_GFX_RESPARTICLESHAPE_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/res/gfx_ResVertex.h>
#include <nw/gfx/res/gfx_ResShape.h>

namespace nw {
namespace gfx {
namespace res {

//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
//
struct ResParticleShapeData : public ResShapeData
{
    nw::ut::ResS32 m_VertexAttributesTableCount;    //
    nw::ut::Offset toVertexAttributesTable;         //
};


//--------------------------------------------------------------------------
//
//--------------------------------------------------------------------------
class ResParticleShape : public ResShape
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleShape) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PSHP') };

    NW_RES_CTOR_INHERIT( ResParticleShape, ResShape )

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
    NW_RES_FIELD_CLASS_LIST_DECL( ResVertexAttribute, VertexAttributes ) // GetVertexAttributes(int idx), GetVertexAttributesCount()

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void    Setup();
};

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESPARTICLESHAPE_H_
