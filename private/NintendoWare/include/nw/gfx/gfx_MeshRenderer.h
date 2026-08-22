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

#ifndef NW_GFX_MESHRENDERER_H_
#define NW_GFX_MESHRENDERER_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResMesh.h>

namespace nw
{
namespace os
{
class IAllocator;
} // namespace os
namespace gfx
{

class Model;
class SkeletalModel;
class Mesh;
class RenderContext;
class PrimitiveSet;
class Primitive;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class MeshRenderer : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(MeshRenderer);

public:
    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    static MeshRenderer* Create(nw::os::IAllocator* pAllocator);

    //

    //----------------------------------------
    //
    //

    //
    void SetRenderContext(RenderContext* renderContext)
    {
        m_RenderContext = renderContext;
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    void RenderMesh(ResMesh mesh, Model* model);

    //

private:
    //
    MeshRenderer(nw::os::IAllocator* allocator)
    : GfxObject(allocator),
      m_RenderContext(NULL) {}

    //
    virtual ~MeshRenderer() {}

    //
    //
    //
    //
    //
    //
    void RenderSeparateDataShape(
        Model* model,
        ResSeparateDataShape shape,
        s32 currentPrimitiveIndex);

    //
    //
    //
    //
    //
    //
    void RenderParticleShape(
        Model* model,
        ResParticleShape shape,
        int index);

    //
    //
    //
    //
    //
    //
    void SetMatrixPalette(
        SkeletalModel* skeletalModel,
        ResPrimitiveSet primitiveSet,
        s32 boneIndexCount);

    RenderContext*      m_RenderContext;
};


} // namespace gfx
} // namespace nw

#endif // NW_GFX_MESH_RENDERER_H_
