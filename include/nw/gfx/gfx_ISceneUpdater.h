#pragma once

#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_RenderQueue.h>

namespace nw{
namespace gfx{

class SceneContext;
class SceneNode;
class Camera;

class ISceneUpdater : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ISceneUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    enum RenderSortMode{
        ALL_MESH_BASE_SORT,
        OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT
    };

    enum DepthSortMode{
        SORT_DEPTH_OF_ALL_MESH,
        SORT_DEPTH_OF_TRANSLUCENT_MESH
    };

    class IsVisibleModelFunctor{
    public:
        virtual bool IsVisible(const Model* model){
            NW_UNUSED_VARIABLE(model);
            return true;
        }
    };

    virtual DepthSortMode GetDepthSortMode() const = 0;

    virtual void SetDepthSortMode(DepthSortMode depthSortMode) = 0;

    virtual void UpdateAll(SceneContext* sceneContext) = 0;

    virtual void SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,u8 layerId,
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT) = 0;

    virtual void SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,u8 layerId,
        u8 particleLayerId,
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT) = 0;

    virtual void SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,u8 layerId,
        u8 particleLayerId,
        IsVisibleModelFunctor* isVisibleModel,
        RenderSortMode renderSortMode = ALL_MESH_BASE_SORT) = 0;


protected:

    ISceneUpdater(nw::os::IAllocator* allocator) : GfxObject(allocator) {}
};

}
}