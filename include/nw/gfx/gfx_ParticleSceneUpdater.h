#pragma once

#include <nw/gfx/gfx_ISceneUpdater.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>

namespace nw{
namespace gfx{

class SceneContext;
class ParticleContext;
class ParticleSet;

class ParticleSceneUpdater  : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleSceneUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::MoveArray<SceneNode*> SceneNodeArray;

    typedef nw::ut::MoveArray<ParticleSet*> ParticleSetArray;

    class Builder
    {
    public:
        Builder() {}
        ParticleSceneUpdater* Create(nw::os::IAllocator* allocator);
    };

    void SetStepFrame(SceneContext* sceneContext, f32 stepFrame);
    virtual void UpdateNode(SceneContext* sceneContext, ParticleContext* particleContext, bool enableSwapBuffer = true);
    virtual void SwapBuffer(SceneContext*) {}

protected:
    ParticleSceneUpdater(nw::os::IAllocator* allocator): 
        GfxObject(allocator) {}

    virtual ~ParticleSceneUpdater() {}
};

}
}