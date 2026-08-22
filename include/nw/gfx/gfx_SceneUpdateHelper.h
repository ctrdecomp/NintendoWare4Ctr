#pragma once

#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>
#include <nw/gfx/gfx_RenderQueue.h>

namespace nw{
namespace gfx{

class SceneUpdateHelper{
public:
    template<typename EnqueueModelMesh,typename EnqueueSkeletalModelMesh,typename IsVisibleModel>
    class SubmitViewFunctor : public std::unary_function<Model*, void>{
    public:
        SubmitViewFunctor(SkeletonUpdater* skeletonUpdater,BillboardUpdater* billboardUpdater,
            RenderQueue* renderQueue,const Camera* camera,u8 layerId,
            u8 particleLayerId,IsVisibleModel* isVisibleModel): 
            mSkeletonUpdater(skeletonUpdater),
            mBillboardUpdater(billboardUpdater),
            mRenderQueue(renderQueue),
            mCamera(camera),
            mLayerId(layerId),
            mParticleLayerId(particleLayerId),
            mIsVisibleModel(isVisibleModel) 
        {}

        void operator() (Model* model){
            SkeletalModel* skeletalModel = nw::ut::DynamicCast<SkeletalModel*>(model);

            if (skeletalModel){
                Skeleton* skeleton = skeletalModel->GetSkeleton();
                skeleton->SetUpdated(false);
            }

            if (!mIsVisibleModel->IsVisible(model)) { return; }
    
            if (skeletalModel){
                if (!skeletalModel->IsSharingSkeleton()){
                    this->mSkeletonUpdater->UpdateView(skeletalModel->GetSkeleton(),*this->mBillboardUpdater,*this->mCamera);
                }

                const Skeleton* skeleton = skeletalModel->GetSkeleton();
                ResSkeleton resSkeleton = skeleton->GetResSkeleton();
                
                bool isModelCoordinate =
                    ut::CheckFlag(resSkeleton.GetFlags(), ResSkeletonData::FLAG_MODEL_COORDINATE);
                model->UpdateNormalMatrix(this->mCamera->ViewMatrix(), isModelCoordinate);
                
                u8 meshLayerId = mLayerId | model->GetLayerId();

                gfx::ResMeshArray meshs = skeletalModel->GetResMeshes();
                std::for_each(
                    meshs.begin(),
                    meshs.end(),
                    EnqueueSkeletalModelMesh(this->mRenderQueue, skeletalModel, meshLayerId, *this->mCamera));
            }
            else{
                model->UpdateNormalMatrix(this->mCamera->ViewMatrix(), false);
                
                u8 meshLayerId =
                    nw::ut::IsTypeOf<ParticleModel>(model)? this->mParticleLayerId : this->mLayerId;
                meshLayerId |= model->GetLayerId();

                gfx::ResMeshArray meshs = model->GetResMeshes();
                std::for_each(meshs.begin(),meshs.end(),EnqueueModelMesh(this->mRenderQueue, model, meshLayerId, *this->mCamera));
            }
        }
        
    private:
        SkeletonUpdater* mSkeletonUpdater;
        BillboardUpdater* mBillboardUpdater;
        RenderQueue* mRenderQueue;
        const Camera* mCamera;
        u8 mLayerId;
        u8 mParticleLayerId;
        IsVisibleModel* mIsVisibleModel;
    };

    template<typename IsVisibleModelFunctor>
    static void SubmitView(SkeletonUpdater* skeletonUpdater,BillboardUpdater* billboardUpdater,RenderQueue* renderQueue,SceneContext* sceneContext,
        const Camera& camera,u8 layerId,u8 particleLayerId,ISceneUpdater::RenderSortMode renderSortMode,
        ISceneUpdater::DepthSortMode depthSortMode,IsVisibleModelFunctor* isVisibleModel){
        NW_NULL_ASSERT(sceneContext);
        
        if (depthSortMode == ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH){
            
            typedef SubmitViewFunctor<
                RenderQueue::FastEnqueueModelFunctor,
                RenderQueue::FastEnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> FastMeshBaseSubmitViewFunctor;
            
            typedef SubmitViewFunctor<
                RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> FastTranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT){
                std::for_each(
                    sceneContext->GetModelsBegin(),
                    sceneContext->GetModelsEnd(),
                    FastTranslucentModelBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel));
            }
            else{
                std::for_each(
                    sceneContext->GetModelsBegin(),
                    sceneContext->GetModelsEnd(),
                    FastMeshBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel));
            }
        }
        else{
            
            typedef SubmitViewFunctor<
                RenderQueue::EnqueueModelFunctor,
                RenderQueue::EnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> MeshBaseSubmitViewFunctor;
            
            typedef SubmitViewFunctor<
                RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> TranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT){
                std::for_each(
                    sceneContext->GetModelsBegin(),
                    sceneContext->GetModelsEnd(),
                    TranslucentModelBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel));
            }
            else{
                std::for_each(
                    sceneContext->GetModelsBegin(),
                    sceneContext->GetModelsEnd(),
                    MeshBaseSubmitViewFunctor(
                            skeletonUpdater, billboardUpdater,
                            renderQueue, &camera, layerId, particleLayerId, isVisibleModel));
            }
        }
    }

private:
    SceneUpdateHelper() {}
    ~SceneUpdateHelper() {}
};


}
}