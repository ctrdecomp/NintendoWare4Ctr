#pragma once

#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>
#include <nw/gfx/gfx_RenderQueue.h>

namespace nw{
namespace gfx{

class SceneUpdateHelper
{
public:
    template<typename EnqueueModelMesh,typename EnqueueSkeletalModelMesh,typename IsVisibleModel>
    class SubmitViewFunctor : public std::unary_function<Model*, void>
{
    public:
        SubmitViewFunctor(SkeletonUpdater* skeletonUpdater,BillboardUpdater* billboardUpdater,
            RenderQueue* renderQueue,const Camera* camera,u8 layerId,
            u8 particleLayerId,IsVisibleModel* isVisibleModel): 
            m_SkeletonUpdater(skeletonUpdater),
            m_BillboardUpdater(billboardUpdater),
            m_RenderQueue(renderQueue),
            m_Camera(camera),
            m_LayerId(layerId),
            m_ParticleLayerId(particleLayerId),
            m_IsVisibleModel(isVisibleModel) {}

        void operator() (Model* model)
        {
            SkeletalModel* skeletalModel = nw::ut::DynamicCast<SkeletalModel*>(model);

            if (skeletalModel)
            {
                Skeleton* skeleton = skeletalModel->GetSkeleton();
                skeleton->SetUpdated(false);
            }

            if (!m_IsVisibleModel->IsVisible(model)) { return; }
    
            if (skeletalModel)
            {
                if (!skeletalModel->IsSharingSkeleton())
                {
                    this->m_SkeletonUpdater->UpdateView(skeletalModel->GetSkeleton(),*this->m_BillboardUpdater,*this->m_Camera);
                }

                const Skeleton* skeleton = skeletalModel->GetSkeleton();
                ResSkeleton resSkeleton = skeleton->GetResSkeleton();
                
                bool isModelCoordinate =
                    ut::CheckFlag(resSkeleton.GetFlags(), ResSkeletonData::FLAG_MODEL_COORDINATE);
                model->UpdateNormalMatrix(this->m_Camera->ViewMatrix(), isModelCoordinate);
                
                u8 meshLayerId = m_LayerId | model->GetLayerId();

                gfx::ResMeshArray meshs = skeletalModel->GetResMeshes();
                std::for_each(
                    meshs.begin(),
                    meshs.end(),
                    EnqueueSkeletalModelMesh(this->m_RenderQueue, skeletalModel, meshLayerId, *this->m_Camera));
            }
            else{
                model->UpdateNormalMatrix(this->m_Camera->ViewMatrix(), false);
                
                u8 meshLayerId =
                    nw::ut::IsTypeOf<ParticleModel>(model)? this->m_ParticleLayerId : this->m_LayerId;
                meshLayerId |= model->GetLayerId();

                gfx::ResMeshArray meshs = model->GetResMeshes();
                std::for_each(meshs.begin(),meshs.end(),EnqueueModelMesh(this->m_RenderQueue, model, meshLayerId, *this->m_Camera));
            }
        }
        
    private:
        SkeletonUpdater* m_SkeletonUpdater;
        BillboardUpdater* m_BillboardUpdater;
        RenderQueue* m_RenderQueue;
        const Camera* m_Camera;
        u8 m_LayerId;
        u8 m_ParticleLayerId;
        IsVisibleModel* m_IsVisibleModel;
    };

    template<typename IsVisibleModelFunctor>
    static void SubmitView(SkeletonUpdater* skeletonUpdater,BillboardUpdater* billboardUpdater,RenderQueue* renderQueue,SceneContext* sceneContext,
        const Camera& camera,u8 layerId,u8 particleLayerId,ISceneUpdater::RenderSortMode renderSortMode,
        ISceneUpdater::DepthSortMode depthSortMode,IsVisibleModelFunctor* isVisibleModel)
    {
        NW_NULL_ASSERT(sceneContext);
        
        if (depthSortMode == ISceneUpdater::SORT_DEPTH_OF_TRANSLUCENT_MESH)
        {
            
            typedef SubmitViewFunctor<
                RenderQueue::FastEnqueueModelFunctor,
                RenderQueue::FastEnqueueSkeletalModelFunctor,
                IsVisibleModelFunctor> FastMeshBaseSubmitViewFunctor;
            
            typedef SubmitViewFunctor<
                RenderQueue::EnqueueModelTranslucentModelBaseFunctor,
                RenderQueue::EnqueueSkeletalModelTranslucentModelBaseFunctor,
                IsVisibleModelFunctor> FastTranslucentModelBaseSubmitViewFunctor;
            
            if (renderSortMode == ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT)
            {
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
            
            if (renderSortMode == ISceneUpdater::OPAQUE_MESH_BASE_AND_TRANSLUCENT_MODEL_BASE_SORT)
            {
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