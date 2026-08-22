#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_Model.h>

namespace nw{
namespace gfx{

class SceneNode;
class TransformNode;
class UserRenderNode;
class Model;
class SkeletalModel;
class Camera;
class Fog;
class Light;
class FragmentLight;
class AmbientLight;
class VertexLight;
class HemiSphereLight;
class ParticleSet;
class ParticleEmitter;
class ParticleModel;

class ISceneVisitor : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ISceneVisitor);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual void VisitSceneNode(SceneNode* node) = 0;
    virtual void VisitTransformNode(TransformNode* node) = 0;
    virtual void VisitUserRenderNode(UserRenderNode* node) = 0;
    virtual void VisitModel(Model* model) = 0;
    virtual void VisitSkeletalModel(SkeletalModel* model) = 0;
    virtual void VisitCamera(Camera* camera) = 0;
    virtual void VisitFog(Fog* fog) = 0;
    virtual void VisitLight(Light* light) = 0;
    virtual void VisitFragmentLight(FragmentLight* light) = 0;
    virtual void VisitVertexLight(VertexLight* light) = 0;
    virtual void VisitAmbientLight(AmbientLight* light) = 0;
    virtual void VisitHemiSphereLight(HemiSphereLight* light) = 0;
    virtual void VisitParticleSet(ParticleSet* particleSet) = 0;
    virtual void VisitParticleEmitter(ParticleEmitter* particleEmitter) = 0;
    virtual void VisitParticleModel(ParticleModel* particleModel) = 0;

protected:
    ISceneVisitor(os::IAllocator* allocator):GfxObject(allocator){}
};

}
}