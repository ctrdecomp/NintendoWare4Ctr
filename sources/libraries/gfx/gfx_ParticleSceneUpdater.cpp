#include <nw/gfx/gfx_ParticleSceneUpdater.h>

#include <nw/gfx/gfx_SceneContext.h>
#include <nw/gfx/gfx_ParticleContext.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleEmitter.h>

#include <nw/ut/ut_Foreach.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(ParticleSceneUpdater);

ParticleSceneUpdater* ParticleSceneUpdater::Builder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    void* memory = allocator->Alloc(sizeof(ParticleSceneUpdater));
    if (memory == NULL)
    {
        return NULL;
    }

    return new(memory) ParticleSceneUpdater(allocator);
}

void ParticleSceneUpdater::SetStepFrame(SceneContext* sceneContext,f32 stepFrame)
{
{
        ParticleModelArray::iterator end = sceneContext->GetParticleModelEnd();
        for (ParticleModelArray::iterator i = sceneContext->GetParticleModelBegin(); i != end;)
        {
            ParticleModel* model = *i++;
            NW_NULL_ASSERT(model);

            model->ParticleAnimFrameController().SetStepFrame(stepFrame);
        }
    }

    {
        ParticleEmitterArray::iterator end = sceneContext->GetParticleEmitterEnd();
        for (ParticleEmitterArray::iterator i = sceneContext->GetParticleEmitterBegin(); i != end;)
        {
            ParticleEmitter* emitter = *i++;
            NW_NULL_ASSERT(emitter);

            emitter->ParticleAnimFrameController().SetStepFrame(stepFrame);
        }
    }
}

void ParticleSceneUpdater::UpdateNode(SceneContext* sceneContext,ParticleContext* particleContext,bool enableSwapBuffer)
{
{
        ParticleModelArray::iterator end = sceneContext->GetParticleModelEnd();
        for (ParticleModelArray::iterator i = sceneContext->GetParticleModelBegin(); i != end;)
        {
            ParticleModel* model = *i++;
            NW_NULL_ASSERT(model);

            model->UpdateParticleFrame();
        }
    }

    {
        ParticleEmitterArray::iterator end = sceneContext->GetParticleEmitterEnd();
        for (ParticleEmitterArray::iterator i = sceneContext->GetParticleEmitterBegin(); i != end;)
        {
            ParticleEmitter* emitter = *i++;
            NW_NULL_ASSERT(emitter);

            emitter->UpdateParticleFrame();
            emitter->Emission(particleContext);
        }
    }

    {
        ParticleSetArray::iterator end = sceneContext->GetParticleSetEnd();
        for (ParticleSetArray::iterator i = sceneContext->GetParticleSetBegin(); i != end;)
        {
            ParticleSet* particleSet = *i++;
            particleSet->UpdateParticles(particleContext, enableSwapBuffer);
        }
    }
}

}
}