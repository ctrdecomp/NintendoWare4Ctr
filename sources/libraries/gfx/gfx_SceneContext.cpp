#include <nw/gfx/gfx_SceneContext.h>

namespace nw{
namespace gfx{

SceneContext* SceneContext::Builder::Create(nw::os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    SceneNodeArray sceneNodes(allocator);
    UserRenderNodeArray userRenderNodes(allocator);
    ModelArray models(allocator);
    SkeletalModelArray skeletalModels(allocator);
    LightArray lights(allocator);
    FragmentLightArray fragmentLights(allocator);
    VertexLightArray vertexLights(allocator);
    HemiSphereLightArray hemiSphereLights(allocator);
    AmbientLightArray ambientLights(allocator);
    CameraArray cameras(allocator);
    FogArray fogs(allocator);
    ParticleSetArray particleSets(allocator);
    ParticleEmitterArray particleEmitters(allocator);
    ParticleModelArray particleModels(allocator);
    AnimatableNodeArray animatableNodes(allocator);

    if (m_IsFixedSizeMemory)
    {
        sceneNodes = SceneNodeArray(this->m_MaxSceneNodes, allocator);
        userRenderNodes = UserRenderNodeArray(this->m_MaxUserRenderNodes, allocator);
        models = ModelArray(this->m_MaxModels, allocator);
        skeletalModels = SkeletalModelArray(this->m_MaxSkeletalModels, allocator);
        lights = LightArray(this->m_MaxLights, allocator);
        fragmentLights = FragmentLightArray(this->m_MaxFragmentLights, allocator);
        vertexLights = VertexLightArray(this->m_MaxVertexLights, allocator);
        hemiSphereLights = HemiSphereLightArray(this->m_MaxHemiSphereLights, allocator);
        ambientLights = AmbientLightArray(this->m_MaxAmbientLights, allocator);
        cameras = CameraArray(this->m_MaxCameras, allocator);
        fogs = FogArray(this->m_MaxFogs, allocator);
        particleSets = ParticleSetArray(this->m_MaxParticleSets, allocator);
        particleEmitters = ParticleEmitterArray(this->m_MaxParticleEmitters, allocator);
        particleModels = ParticleModelArray(this->m_MaxParticleModels, allocator);
        animatableNodes = AnimatableNodeArray(this->m_MaxAnimatableNodes, allocator);
    }
    
    void* memory = allocator->Alloc(sizeof(SceneContext));
    NW_NULL_ASSERT(memory);
    
    SceneContext* context = new(memory) SceneContext(
        allocator, sceneNodes, userRenderNodes, models, skeletalModels, cameras, fogs,
        lights, fragmentLights, vertexLights, hemiSphereLights, ambientLights,
        particleSets, particleEmitters, particleModels, animatableNodes);
    
    return context;
}

}
}