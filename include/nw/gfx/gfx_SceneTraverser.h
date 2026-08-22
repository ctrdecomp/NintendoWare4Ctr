#pragma once

#include <nw/gfx/gfx_ISceneVisitor.h>

namespace nw{
namespace gfx{

class SceneContext;

class SceneTraverser  : public ISceneVisitor{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneTraverser);

public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder{
    public:
        SceneTraverser* Create(os::IAllocator* allocator);
    };

    void Begin(SceneContext* sceneContext);

    void End();
    
    virtual void VisitSceneNode(SceneNode* node);
    virtual void VisitTransformNode(TransformNode* node);
    virtual void VisitUserRenderNode(UserRenderNode* node);
    virtual void VisitModel(Model* model);
    virtual void VisitSkeletalModel(SkeletalModel* model);
    virtual void VisitCamera(Camera* camera);
    virtual void VisitFog(Fog* fog);
    virtual void VisitLight(Light* light);
    virtual void VisitFragmentLight(FragmentLight* light);
    virtual void VisitVertexLight(VertexLight* light);
    virtual void VisitAmbientLight(AmbientLight* light);
    virtual void VisitHemiSphereLight(HemiSphereLight* light);
    virtual void VisitParticleSet(ParticleSet* particleSet);
    virtual void VisitParticleEmitter(ParticleEmitter* particleEmitter);
    virtual void VisitParticleModel(ParticleModel* particleModel);
    
private:
    SceneTraverser(nw::os::IAllocator* allocator): 
        ISceneVisitor(allocator), 
        mSceneContext(NULL) 
    {}

    virtual ~SceneTraverser() {}

    SceneContext* mSceneContext;
};

}
}