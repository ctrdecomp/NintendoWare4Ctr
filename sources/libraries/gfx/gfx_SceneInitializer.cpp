#include <nw/gfx/gfx_SceneTraverser.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_SceneInitializer.h>
#include <nw/gfx/gfx_IMaterialIdGenerator.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SceneInitializer, ISceneVisitor);

SceneInitializer* SceneInitializer::Builder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* memory = allocator->Alloc(sizeof(SceneInitializer));
    NW_NULL_ASSERT(memory);
    
    NW_ASSERTMSG(this->mDescription.materialIdGenerator != NULL, "SceneInitializer need to set an IMaterialIdGenerator instance.");

    SceneInitializer* initializer = new(memory) SceneInitializer(allocator, this->mDescription);
    
    return initializer;
}

void SceneInitializer::VisitModel(Model* model){
    if (this->mMaterialIdGenerator.Get() != NULL){
        gfx::MaterialArray::iterator materialEnd = model->GetMaterials().second;
        for (gfx::MaterialArray::iterator material = model->GetMaterials().first;
            material != materialEnd;
            ++material){
            this->mMaterialIdGenerator.Get()->Accept(*material);
        }
    }
}

void SceneInitializer::VisitSkeletalModel(SkeletalModel* model){
    if (this->mMaterialIdGenerator.Get() != NULL){
        gfx::MaterialArray::iterator materialEnd = model->GetMaterials().second;
        for (gfx::MaterialArray::iterator material = model->GetMaterials().first;
            material != materialEnd;
            ++material){
            this->mMaterialIdGenerator.Get()->Accept(*material);
        }
    }
}

void SceneInitializer::VisitParticleModel(ParticleModel* model){
    if (this->mMaterialIdGenerator.Get() != NULL){
        gfx::MaterialArray::iterator materialEnd = model->GetMaterials().second;
        for (gfx::MaterialArray::iterator material = model->GetMaterials().first;
            material != materialEnd;
            ++material){
            this->mMaterialIdGenerator.Get()->Accept(*material);
        }
    }
}

void SceneInitializer::Begin(){ }

void SceneInitializer::End(){
    this->mMaterialIdGenerator.Get()->Generate();
}

}
}