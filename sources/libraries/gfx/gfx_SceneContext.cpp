#include <nw/gfx/gfx_SceneContext.h>

namespace nw{
namespace gfx{

SceneContext* SceneContext::Builder::Create(nw::os::IAllocator* allocator){
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

    if (mIsFixedSizeMemory){
        sceneNodes = SceneNodeArray(this->mMaxSceneNodes, allocator);
        userRenderNodes = UserRenderNodeArray(this->mMaxUserRenderNodes, allocator);
        models = ModelArray(this->mMaxModels, allocator);
        skeletalModels = SkeletalModelArray(this->mMaxSkeletalModels, allocator);
        lights = LightArray(this->mMaxLights, allocator);
        fragmentLights = FragmentLightArray(this->mMaxFragmentLights, allocator);
        vertexLights = VertexLightArray(this->mMaxVertexLights, allocator);
        hemiSphereLights = HemiSphereLightArray(this->mMaxHemiSphereLights, allocator);
        ambientLights = AmbientLightArray(this->mMaxAmbientLights, allocator);
        cameras = CameraArray(this->mMaxCameras, allocator);
        fogs = FogArray(this->mMaxFogs, allocator);
        particleSets = ParticleSetArray(this->mMaxParticleSets, allocator);
        particleEmitters = ParticleEmitterArray(this->mMaxParticleEmitters, allocator);
        particleModels = ParticleModelArray(this->mMaxParticleModels, allocator);
        animatableNodes = AnimatableNodeArray(this->mMaxAnimatableNodes, allocator);
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