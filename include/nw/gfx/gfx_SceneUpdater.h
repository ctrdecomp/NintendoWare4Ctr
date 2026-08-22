#pragma once

#include <nw/gfx/gfx_ISceneUpdater.h>
#include <nw/gfx/gfx_RenderQueue.h>
#include <nw/gfx/gfx_BillboardUpdater.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_SceneContext.h>

namespace nw{
namespace gfx{

class SkeletonUpdater;
class WorldMatrixUpdater;
class BillboardUpdater;

class SceneContext;

class SceneUpdater  : public ISceneUpdater{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder{
    public:
        Builder() {}

        Builder& SkeletonUpdaterPtr(SkeletonUpdater* skeletonUpdater){
            this->mSkeletonUpdater.Reset(skeletonUpdater);
            return *this;
        }

        Builder& WorldMatrixUpdaterPtr(WorldMatrixUpdater* worldMatrixUpdater){
            this->mWorldMatrixUpdater.Reset(worldMatrixUpdater);
            return *this;
        }

        Builder& BillboardUpdaterPtr(BillboardUpdater* billboardUpdater){
            this->mBillboardUpdater.Reset(billboardUpdater);
            return *this;
        }

        SceneUpdater* Create(os::IAllocator* allocator);

    private:
        GfxPtr<SkeletonUpdater> mSkeletonUpdater;
        GfxPtr<WorldMatrixUpdater> mWorldMatrixUpdater;
        GfxPtr<BillboardUpdater> mBillboardUpdater;
    };

    virtual DepthSortMode GetDepthSortMode() const { return mDepthSortMode; }

    virtual void SetDepthSortMode(DepthSortMode depthSortMode){
        mDepthSortMode = depthSortMode;
    }

    virtual void UpdateAll(SceneContext* sceneContext);

    void UpdateTransformNode(SceneContext* sceneContext) const;

    void UpdateSkeletalModel(SceneContext* sceneContext) const;

    void UpdateAnim(SceneContext* sceneContext) const;

    void EvaluateAnim(SceneContext* sceneContext, anim::ResGraphicsAnimGroup::EvaluationTiming timing) const;

    virtual void SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,
        u8 layerId,RenderSortMode renderSortMode = ALL_MESH_BASE_SORT);

    virtual void SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,
        u8 layerId,u8 particleLayerId,RenderSortMode renderSortMode = ALL_MESH_BASE_SORT);
    
    virtual void SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,
        u8 layerId,u8 particleLayerId,IsVisibleModelFunctor* isVisibleModel,RenderSortMode renderSortMode = ALL_MESH_BASE_SORT);
protected:
    SceneUpdater(nw::os::IAllocator* allocator,GfxPtr<SkeletonUpdater> skeletonUpdater,GfxPtr<WorldMatrixUpdater> worldMatrixUpdater,
    GfxPtr<BillboardUpdater> billboardUpdater): 
        ISceneUpdater(allocator),
        mSkeletonUpdater(skeletonUpdater),
        mWorldMatrixUpdater(worldMatrixUpdater),
        mBillboardUpdater(billboardUpdater),
        mDepthSortMode(SORT_DEPTH_OF_ALL_MESH)
    {}

    virtual ~SceneUpdater() {}

    GfxPtr<SkeletonUpdater> mSkeletonUpdater;
    GfxPtr<WorldMatrixUpdater> mWorldMatrixUpdater;
    GfxPtr<BillboardUpdater> mBillboardUpdater;
    DepthSortMode mDepthSortMode;
};

}
}