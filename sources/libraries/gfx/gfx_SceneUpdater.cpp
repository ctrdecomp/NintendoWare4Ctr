#include <nw/gfx/gfx_SceneUpdater.h>

#include <nw/gfx/gfx_SceneUpdateHelper.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/res/gfx_ResShape.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SceneUpdater, ISceneUpdater);

SceneUpdater* SceneUpdater::Builder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    if (!this->mSkeletonUpdater){
        this->mSkeletonUpdater.Reset(SkeletonUpdater::Builder().Create(allocator));
    }

    if (!this->mWorldMatrixUpdater){
        this->mWorldMatrixUpdater.Reset(WorldMatrixUpdater::Builder().Create(allocator));
    }
    
    if (!this->mBillboardUpdater){
        this->mBillboardUpdater.Reset(BillboardUpdater::Create(allocator));
    }
    
    void* memory = allocator->Alloc(sizeof(SceneUpdater));
    NW_NULL_ASSERT(memory);
    
    return new(memory) SceneUpdater(allocator,this->mSkeletonUpdater,this->mWorldMatrixUpdater,this->mBillboardUpdater);
}

void SceneUpdater::UpdateAll(SceneContext* sceneContext){
    NW_NULL_ASSERT(sceneContext);

    this->EvaluateAnim(sceneContext, anim::ResGraphicsAnimGroup::EVALUATION_BEFORE_WORLD_UPDATE);
    this->UpdateTransformNode(sceneContext);
    this->EvaluateAnim(sceneContext, anim::ResGraphicsAnimGroup::EVALUATION_AFTER_SCENE_CULLING);
    this->UpdateSkeletalModel(sceneContext);
    this->UpdateAnim(sceneContext);
}

void SceneUpdater::SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,u8 layerId,RenderSortMode renderSortMode){
    Model::IsVisibleModelDefaultFunctor isVisibleModel;
    
    SceneUpdateHelper::SubmitView(this->mSkeletonUpdater.Get(),this->mBillboardUpdater.Get(),renderQueue,sceneContext,camera,layerId,
        layerId,renderSortMode,this->mDepthSortMode,&isVisibleModel);
}

void SceneUpdater::SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,u8 layerId,u8 particleLayerId,RenderSortMode renderSortMode){
    Model::IsVisibleModelDefaultFunctor isVisibleModel;
    
    SceneUpdateHelper::SubmitView(this->mSkeletonUpdater.Get(),this->mBillboardUpdater.Get(),renderQueue,sceneContext,camera,layerId,
        particleLayerId,renderSortMode,this->mDepthSortMode,&isVisibleModel);
}

void SceneUpdater::SubmitView(RenderQueue* renderQueue,SceneContext* sceneContext,const Camera& camera,u8 layerId,u8 particleLayerId,SceneUpdater::IsVisibleModelFunctor* isVisibleModel,RenderSortMode renderSortMode){
    SceneUpdateHelper::SubmitView(this->mSkeletonUpdater.Get(),this->mBillboardUpdater.Get(),renderQueue,sceneContext,camera,layerId,
        particleLayerId,renderSortMode,this->mDepthSortMode,isVisibleModel);
}

void SceneUpdater::UpdateTransformNode(SceneContext* sceneContext) const{
    NW_NULL_ASSERT(sceneContext);
    
    SceneNodeArray::iterator end = sceneContext->GetSceneNodesEnd();
    for (SceneNodeArray::iterator node = sceneContext->GetSceneNodesBegin(); node != end; ++node){
        (*node)->PreUpdateSignal()(*node, sceneContext);
        (*node)->InheritTraversalResults();
        (*node)->UpdateTransform(this->mWorldMatrixUpdater.Get(), sceneContext);
    }
}

void SceneUpdater::UpdateSkeletalModel(SceneContext* sceneContext) const{
    NW_NULL_ASSERT(sceneContext);
    
    SkeletalModelArray::iterator end = sceneContext->GetSkeletalModelsEnd();
    for (SkeletalModelArray::iterator model = sceneContext->GetSkeletalModelsBegin(); model != end; ++model){

        if ((*model)->GetFullBakedAnimEnabled()) { continue; }

        Skeleton* skeleton = (*model)->GetSkeleton();

        if (skeleton->IsUpdated()) { continue; }

        this->mSkeletonUpdater->UpdateWorld(skeleton,*(this->mWorldMatrixUpdater.Get()));

        skeleton->SetUpdated(true);
    }
}

void SceneUpdater::UpdateAnim(SceneContext* sceneContext) const{
    NW_NULL_ASSERT(sceneContext);
    
    std::for_each(sceneContext->GetAnimatableNodesBegin(),sceneContext->GetAnimatableNodesEnd(),std::mem_fun(&SceneNode::UpdateFrame));
}

void SceneUpdater::EvaluateAnim(SceneContext* sceneContext, anim::ResGraphicsAnimGroup::EvaluationTiming timing) const{
    NW_NULL_ASSERT(sceneContext);
    
    AnimatableNodeArray::iterator animatableNodesEnd = sceneContext->GetAnimatableNodesEnd();
    for (AnimatableNodeArray::iterator animNode = sceneContext->GetAnimatableNodesBegin();
        animNode != animatableNodesEnd; ++animNode){
        (*animNode)->GetAnimBinding()->Evaluate(timing);
    }
}

}
}