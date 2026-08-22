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

#ifndef NW_GFX_SCENEUPDATER_H_
#define NW_GFX_SCENEUPDATER_H_

#include <nw/gfx/gfx_ISceneUpdater.h>
#include <nw/gfx/gfx_RenderQueue.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class SkeletonUpdater;
class WorldMatrixUpdater;
class BillboardUpdater;

class SceneContext;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class SceneUpdater  : public ISceneUpdater
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        Builder() {}

        //
        //
        Builder& SkeletonUpdaterPtr(SkeletonUpdater* skeletonUpdater)
        {
            m_SkeletonUpdater.Reset(skeletonUpdater);
            return *this;
        }

        //
        //
        Builder& WorldMatrixUpdaterPtr(WorldMatrixUpdater* worldMatrixUpdater)
        {
            m_WorldMatrixUpdater.Reset(worldMatrixUpdater);
            return *this;
        }

        //
        //
        Builder& BillboardUpdaterPtr(BillboardUpdater* billboardUpdater)
        {
            m_BillboardUpdater.Reset(billboardUpdater);
            return *this;
        }

        //
        //
        //
        //
        //
        //
        SceneUpdater* Create(os::IAllocator* allocator);

    private:
        GfxPtr<SkeletonUpdater> m_SkeletonUpdater;
        GfxPtr<WorldMatrixUpdater> m_WorldMatrixUpdater;
        GfxPtr<BillboardUpdater> m_BillboardUpdater;
    };

    //

    //----------------------------------------
    //
    //

    //
    virtual DepthSortMode GetDepthSortMode() const { return m_DepthSortMode; }

    //
    virtual void SetDepthSortMode(DepthSortMode depthSortMode)
    {
        m_DepthSortMode = depthSortMode;
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
    virtual void UpdateAll(SceneContext* sceneContext);

    //
    //
    //
    //
    void UpdateTransformNode(SceneContext* sceneContext) const;

    //
    //
    //
    //
    //
    //
    //
    //
    //
    void UpdateSkeletalModel(SceneContext* sceneContext, bool ignoreUpdatedFlag = true) const;

    //
    //
    //
    //
    void UpdateAnim(SceneContext* sceneContext) const;

    //
    //
    //
    //
    //
    //
    void EvaluateAnim(SceneContext* sceneContext, anim::ResGraphicsAnimGroup::EvaluationTiming timing) const;

    //

    //----------------------------------------
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
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT);

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
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT);

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
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT);

    //

protected:
    //----------------------------------------
    //
    //

    //
    SceneUpdater(os::IAllocator* allocator,
                 GfxPtr<SkeletonUpdater> skeletonUpdater,
                 GfxPtr<WorldMatrixUpdater> worldMatrixUpdater,
                 GfxPtr<BillboardUpdater> billboardUpdater)
    : ISceneUpdater(allocator),
      m_SkeletonUpdater(skeletonUpdater),
      m_WorldMatrixUpdater(worldMatrixUpdater),
      m_BillboardUpdater(billboardUpdater),
      m_DepthSortMode(SORT_DEPTH_OF_ALL_MESH)
    {}

    //
    virtual ~SceneUpdater() {}

    //

    GfxPtr<SkeletonUpdater> m_SkeletonUpdater;
    GfxPtr<WorldMatrixUpdater> m_WorldMatrixUpdater;
    GfxPtr<BillboardUpdater> m_BillboardUpdater;
    DepthSortMode m_DepthSortMode;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_SCENEUPDATER_H_
