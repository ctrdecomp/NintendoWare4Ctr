#include <nw/gfx/gfx_SceneTraverser.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_UserRenderNode.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_ParticleSet.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SceneTraverser, ISceneVisitor);

SceneTraverser* SceneTraverser::Builder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* memory = allocator->Alloc(sizeof(SceneTraverser));
    NW_NULL_ASSERT(memory);
    
    SceneTraverser* traverser = new(memory) SceneTraverser(allocator);
    
    return traverser;
}

void SceneTraverser::Begin(SceneContext* sceneContext){
    NW_NULL_ASSERT(sceneContext);
    
    this->mSceneContext = sceneContext;
    this->mSceneContext->Clear();
}

void SceneTraverser::End(){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext = NULL;
}

void SceneTraverser::VisitSceneNode(SceneNode* node){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(node);
}

void SceneTraverser::VisitTransformNode(TransformNode* node){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(node);
}

void SceneTraverser::VisitUserRenderNode(UserRenderNode* node){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(node);
    this->mSceneContext->PushUserRenderNode(node);
}

void SceneTraverser::VisitModel(Model* model){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(model);
    this->mSceneContext->PushModel(model);
    this->mSceneContext->PushAnimatableNode(model);
}

void SceneTraverser::VisitSkeletalModel(SkeletalModel* model){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(model);
    this->mSceneContext->PushModel(model);
    this->mSceneContext->PushSkeletalModel(model);
    this->mSceneContext->PushAnimatableNode(model);
}

void SceneTraverser::VisitCamera(Camera* camera){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(camera);
    this->mSceneContext->PushCamera(camera);
    this->mSceneContext->PushAnimatableNode(camera);
}

void SceneTraverser::VisitFog(Fog* fog){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(fog);
    this->mSceneContext->PushFog(fog);
    this->mSceneContext->PushAnimatableNode(fog);
}

void SceneTraverser::VisitLight(Light* light){
    NW_UNUSED_VARIABLE(light);
    this->mSceneContext->PushSceneNode(light);
    this->mSceneContext->PushLight(light);
    this->mSceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitFragmentLight(FragmentLight* light){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(light);
    this->mSceneContext->PushLight(light);
    this->mSceneContext->PushFragmentLight(light);
    this->mSceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitVertexLight(VertexLight* light){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(light);
    this->mSceneContext->PushLight(light);
    this->mSceneContext->PushVertexLight(light);
    this->mSceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitAmbientLight(AmbientLight* light){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(light);
    this->mSceneContext->PushLight(light);
    this->mSceneContext->PushAmbientLight(light);
    this->mSceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitHemiSphereLight(HemiSphereLight* light){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(light);
    this->mSceneContext->PushLight(light);
    this->mSceneContext->PushHemiSphereLight(light);
    this->mSceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitParticleSet(ParticleSet* particleSet){
    particleSet->CopyTraversalResults(particleSet->GetParent());

    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(particleSet);
    this->mSceneContext->PushParticleSet(particleSet);
}

void SceneTraverser::VisitParticleEmitter(ParticleEmitter* particleEmitter){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(reinterpret_cast<SceneNode*>(particleEmitter));
    this->mSceneContext->PushParticleEmitter(particleEmitter);
}

void SceneTraverser::VisitParticleModel(ParticleModel* model){
    NW_NULL_ASSERT(this->mSceneContext);
    this->mSceneContext->PushSceneNode(model);
    this->mSceneContext->PushModel(model);
    this->mSceneContext->PushParticleModel(model);
    this->mSceneContext->PushAnimatableNode(model);
}

}
}