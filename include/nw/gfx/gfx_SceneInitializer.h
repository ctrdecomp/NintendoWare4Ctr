#pragma once

#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_IMaterialIdGenerator.h>

namespace nw{
namespace gfx{


class SceneInitializer : public ISceneVisitor{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneInitializer);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description{
        IMaterialIdGenerator* materialIdGenerator;

        Description(): 
            materialIdGenerator(NULL)
        {}
    };

    class Builder{
    public:
        Builder& MaterialIdGenerator(IMaterialIdGenerator* materialIdGenerator){
            mDescription.materialIdGenerator = materialIdGenerator;
            return *this;
        }

        SceneInitializer* Create(nw::os::IAllocator* allocator);

    private:
        Description mDescription;
    };

    void Begin();
    void End();
    
    virtual void VisitSceneNode(SceneNode*) {}
    virtual void VisitTransformNode(TransformNode*) {}
    virtual void VisitUserRenderNode(UserRenderNode*) {}
    virtual void VisitModel(Model* model);
    virtual void VisitSkeletalModel(SkeletalModel* model);
    virtual void VisitCamera(Camera*) {}
    virtual void VisitFog(Fog*) {}
    virtual void VisitLight(Light*) {}
    virtual void VisitFragmentLight(FragmentLight*) {}
    virtual void VisitVertexLight(VertexLight*) {}
    virtual void VisitAmbientLight(AmbientLight*) {}
    virtual void VisitHemiSphereLight(HemiSphereLight*) {}
    virtual void VisitParticleSet(ParticleSet*) {}
    virtual void VisitParticleEmitter(ParticleEmitter*) {}
    virtual void VisitParticleModel(ParticleModel*);
    
private:

    SceneInitializer(nw::os::IAllocator* allocator, Description description): 
        ISceneVisitor(allocator),
        mMaterialIdGenerator(description.materialIdGenerator)
    {}

    virtual ~SceneInitializer() {}

    GfxPtr<IMaterialIdGenerator> mMaterialIdGenerator;
};

}
}