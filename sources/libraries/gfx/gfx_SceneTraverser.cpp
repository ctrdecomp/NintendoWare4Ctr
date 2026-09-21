// Filename: gfx_SceneTraverser.cpp
//
// Project: NintendoWare4Ctr

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

SceneTraverser* SceneTraverser::Builder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    void* memory = allocator->Alloc(sizeof(SceneTraverser));
    NW_NULL_ASSERT(memory);
    
    SceneTraverser* traverser = new(memory) SceneTraverser(allocator);
    
    return traverser;
}

void SceneTraverser::Begin(SceneContext* sceneContext)
{
    NW_NULL_ASSERT(sceneContext);
    
    this->m_SceneContext = sceneContext;
    this->m_SceneContext->Clear();
}

void SceneTraverser::End()
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext = NULL;
}

void SceneTraverser::VisitSceneNode(SceneNode* node)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(node);
}

void SceneTraverser::VisitTransformNode(TransformNode* node)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(node);
}

void SceneTraverser::VisitUserRenderNode(UserRenderNode* node)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(node);
    this->m_SceneContext->PushUserRenderNode(node);
}

void SceneTraverser::VisitModel(Model* model)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(model);
    this->m_SceneContext->PushModel(model);
    this->m_SceneContext->PushAnimatableNode(model);
}

void SceneTraverser::VisitSkeletalModel(SkeletalModel* model)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(model);
    this->m_SceneContext->PushModel(model);
    this->m_SceneContext->PushSkeletalModel(model);
    this->m_SceneContext->PushAnimatableNode(model);
}

void SceneTraverser::VisitCamera(Camera* camera)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(camera);
    this->m_SceneContext->PushCamera(camera);
    this->m_SceneContext->PushAnimatableNode(camera);
}

void SceneTraverser::VisitFog(Fog* fog)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(fog);
    this->m_SceneContext->PushFog(fog);
    this->m_SceneContext->PushAnimatableNode(fog);
}

void SceneTraverser::VisitLight(Light* light)
{
    NW_UNUSED_VARIABLE(light);
    this->m_SceneContext->PushSceneNode(light);
    this->m_SceneContext->PushLight(light);
    this->m_SceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitFragmentLight(FragmentLight* light)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(light);
    this->m_SceneContext->PushLight(light);
    this->m_SceneContext->PushFragmentLight(light);
    this->m_SceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitVertexLight(VertexLight* light)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(light);
    this->m_SceneContext->PushLight(light);
    this->m_SceneContext->PushVertexLight(light);
    this->m_SceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitAmbientLight(AmbientLight* light)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(light);
    this->m_SceneContext->PushLight(light);
    this->m_SceneContext->PushAmbientLight(light);
    this->m_SceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitHemiSphereLight(HemiSphereLight* light)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(light);
    this->m_SceneContext->PushLight(light);
    this->m_SceneContext->PushHemiSphereLight(light);
    this->m_SceneContext->PushAnimatableNode(light);
}

void SceneTraverser::VisitParticleSet(ParticleSet* particleSet)
{
    particleSet->CopyTraversalResults(particleSet->GetParent());

    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(particleSet);
    this->m_SceneContext->PushParticleSet(particleSet);
}

void SceneTraverser::VisitParticleEmitter(ParticleEmitter* particleEmitter)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(reinterpret_cast<SceneNode*>(particleEmitter));
    this->m_SceneContext->PushParticleEmitter(particleEmitter);
}

void SceneTraverser::VisitParticleModel(ParticleModel* model)
{
    NW_NULL_ASSERT(this->m_SceneContext);
    this->m_SceneContext->PushSceneNode(model);
    this->m_SceneContext->PushModel(model);
    this->m_SceneContext->PushParticleModel(model);
    this->m_SceneContext->PushAnimatableNode(model);
}

}
}