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

#ifndef NW_GFX_ISCENEUPDATER_H_
#define NW_GFX_ISCENEUPDATER_H_

#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_RenderQueue.h>

namespace nw
{
namespace gfx
{

class SceneContext;
class SceneNode;
class Camera;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ISceneUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ISceneUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    enum RenderSortMode
    {
        ALL_MESH_BASE_SORT, //
        OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT //
    };

    //
    enum DepthSortMode
    {
        SORT_DEPTH_OF_ALL_MESH, //
        SORT_DEPTH_OF_TRANSLUCENT_MESH //
    };

    //
    //
    //
    //
    class IsVisibleModelFunctor
    {
    public:
        virtual bool IsVisible(const nw::gfx::Model* model)
        {
            NW_UNUSED_VARIABLE(model);
            return true;
        }
    };

    //----------------------------------------
    //
    //

    //
    virtual DepthSortMode GetDepthSortMode() const = 0;

    //
    virtual void SetDepthSortMode(DepthSortMode depthSortMode) = 0;

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void UpdateAll(SceneContext* sceneContext) = 0;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual void SubmitView(
        RenderQueue* renderQueue,
        SceneContext* sceneContext,
        const Camera& camera,
        u8 layerId,
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT) = 0;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual void SubmitView(
        RenderQueue* renderQueue,
        SceneContext* sceneContext,
        const Camera& camera,
        u8 layerId,
        u8 particleLayerId,
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT) = 0;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    virtual void SubmitView(
        RenderQueue* renderQueue,
        SceneContext* sceneContext,
        const Camera& camera,
        u8 layerId,
        u8 particleLayerId,
        IsVisibleModelFunctor* isVisibleModel,
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT) = 0;

    //

protected:
    //----------------------------------------
    //
    //

    //
    ISceneUpdater(os::IAllocator* allocator) : GfxObject(allocator) {}

    //
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_ISCENEUPDATER_H_
