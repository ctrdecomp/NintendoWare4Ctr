#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_SceneNode.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class SceneNode;
class UserRenderNode;
class Model;
class SkeletalModel;
class Light;
class FragmentLight;
class VertexLight;
class HemiSphereLight;
class AmbientLight;
class Camera;
class Fog;
class ParticleSet;
class ParticleEmitter;
class ParticleModel;
class LightSet;

typedef nw::ut::MoveArray<SceneNode*> SceneNodeArray;
typedef nw::ut::MoveArray<UserRenderNode*> UserRenderNodeArray;
typedef nw::ut::MoveArray<Model*> ModelArray;
typedef nw::ut::MoveArray<SkeletalModel*> SkeletalModelArray;
typedef nw::ut::MoveArray<Light*> LightArray;
typedef nw::ut::MoveArray<FragmentLight*> FragmentLightArray;
typedef nw::ut::MoveArray<VertexLight*> VertexLightArray;
typedef nw::ut::MoveArray<HemiSphereLight*> HemiSphereLightArray;
typedef nw::ut::MoveArray<AmbientLight*> AmbientLightArray;
typedef nw::ut::MoveArray<Camera*> CameraArray;
typedef nw::ut::MoveArray<Fog*> FogArray;
typedef nw::ut::MoveArray<ParticleSet*> ParticleSetArray;
typedef nw::ut::MoveArray<ParticleEmitter*> ParticleEmitterArray;
typedef nw::ut::MoveArray<ParticleModel*> ParticleModelArray;
typedef nw::ut::MoveArray<LightSet*> LightSetArray;
typedef nw::ut::MoveArray<SceneNode*> AnimatableNodeArray;

class SceneContext : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneContext);

public:
    typedef SceneNodeArray SceneNodeArray;
    typedef UserRenderNodeArray UserRenderNodeArray;
    typedef ModelArray ModelArray;
    typedef SkeletalModelArray SkeletalModelArray;
    typedef LightArray LightArray;
    typedef FragmentLightArray FragmentLightArray;
    typedef VertexLightArray VertexLightArray;
    typedef HemiSphereLightArray HemiSphereLightArray;
    typedef AmbientLightArray AmbientLightArray;
    typedef CameraArray CameraArray;
    typedef FogArray FogArray;
    typedef ParticleSetArray ParticleSetArray;
    typedef ParticleEmitterArray ParticleEmitterArray;
    typedef ParticleModelArray ParticleModelArray;
    typedef AnimatableNodeArray AnimatableNodeArray;

    enum
    {
        DEFAULT_MAX_SCENE_NODES = 64,
        DEFAULT_MAX_MODELS = 64,
        DEFAULT_MAX_SKELETAL_MODELS = 64,
        DEFAULT_MAX_CAMERAS = 4,
        DEFAULT_MAX_LIGHTS = 8,
        DEFAULT_MAX_FRAGMENT_LIGHTS = 8,
        DEFAULT_MAX_VERTEX_LIGHTS = 8,
        DEFAULT_MAX_HEMISPHERE_LIGHTS = 4,
        DEFAULT_MAX_AMBIENT_LIGHTS = 4,
        DEFAULT_MAX_FOGS = 4,
        DEFAULT_MAX_PARTICLESETS = 64,
        DEFAULT_MAX_PARTICLEEMITTERS = 64,
        DEFAULT_MAX_PARTICLEMODELS = 64,
        DEFAULT_MAX_ANIMATABLE_NODES = 64,
        DEFAULT_MAX_USER_RENDER_NODES = 0
    };

    class Builder
    {
    public:
        Builder():
            m_IsFixedSizeMemory(true),
            m_MaxSceneNodes(DEFAULT_MAX_SCENE_NODES),
            m_MaxUserRenderNodes(DEFAULT_MAX_USER_RENDER_NODES),
            m_MaxModels(DEFAULT_MAX_MODELS),
            m_MaxSkeletalModels(DEFAULT_MAX_SKELETAL_MODELS),
            m_MaxCameras(DEFAULT_MAX_CAMERAS),
            m_MaxLights(DEFAULT_MAX_LIGHTS),
            m_MaxFragmentLights(DEFAULT_MAX_FRAGMENT_LIGHTS),
            m_MaxVertexLights(DEFAULT_MAX_VERTEX_LIGHTS),
            m_MaxHemiSphereLights(DEFAULT_MAX_HEMISPHERE_LIGHTS),
            m_MaxAmbientLights(DEFAULT_MAX_AMBIENT_LIGHTS),
            m_MaxFogs(DEFAULT_MAX_FOGS),
            m_MaxParticleSets(DEFAULT_MAX_PARTICLESETS),
            m_MaxParticleEmitters(DEFAULT_MAX_PARTICLEEMITTERS),
            m_MaxParticleModels(DEFAULT_MAX_PARTICLEMODELS),
            m_MaxAnimatableNodes(DEFAULT_MAX_ANIMATABLE_NODES) {}

        Builder& IsFixedSizeMemory(bool isFixedSizeMemory) { m_IsFixedSizeMemory = isFixedSizeMemory; return *this; }
        Builder& MaxSceneNodes(s32 max) { m_MaxSceneNodes = max; return *this; }
        Builder& MaxUserRenderNodes(s32 max) { m_MaxUserRenderNodes = max; return *this; }
        Builder& MaxModels(s32 max) { m_MaxModels = max; return *this; }
        Builder& MaxSkeletalModels(s32 max) { m_MaxSkeletalModels = max; return *this; }
        Builder& MaxCameras(s32 max) { m_MaxCameras = max; return *this; }
        Builder& MaxLights(s32 max) { m_MaxLights = max; return *this; }
        Builder& MaxFragmentLights(s32 max) { m_MaxFragmentLights = max; return *this; }
        Builder& MaxVertexLights(s32 max) { m_MaxVertexLights = max; return *this; }
        Builder& MaxHemiSphereLights(s32 max) { m_MaxHemiSphereLights = max; return *this; }
        Builder& MaxAmbientLights(s32 max) { m_MaxAmbientLights = max; return *this; }
        Builder& MaxFogs(s32 max) { m_MaxFogs = max; return *this; }
        Builder& MaxParticleSets(s32 max) { m_MaxParticleSets = max; return *this; }
        Builder& MaxParticleEmitters(s32 max) { m_MaxParticleEmitters = max; return *this; }
        Builder& MaxParticleModels(s32 max) { m_MaxParticleModels = max; return *this; }
        Builder& MaxAnimatableNodes(s32 max) { m_MaxAnimatableNodes = max; return *this; }

        SceneContext* Create(nw::os::IAllocator* allocator);

    private:
        bool m_IsFixedSizeMemory;
        s32 m_MaxSceneNodes;
        s32 m_MaxUserRenderNodes;
        s32 m_MaxModels;
        s32 m_MaxSkeletalModels;
        s32 m_MaxCameras;
        s32 m_MaxLights;
        s32 m_MaxFragmentLights;
        s32 m_MaxVertexLights;
        s32 m_MaxHemiSphereLights;
        s32 m_MaxAmbientLights;
        s32 m_MaxFogs;
        s32 m_MaxParticleSets;
        s32 m_MaxParticleEmitters;
        s32 m_MaxParticleModels;
        s32 m_MaxAnimatableNodes;
    };

    void Clear()
    {
        this->m_SceneNodes.clear();
        this->m_UserRenderNodes.clear();
        this->m_Models.clear();
        this->m_SkeletalModels.clear();
        this->m_Lights.clear();
        this->m_FragmentLights.clear();
        this->m_VertexLights.clear();
        this->m_HemiSphereLights.clear();
        this->m_AmbientLights.clear();
        this->m_Cameras.clear();
        this->m_Fogs.clear();
        this->m_ParticleSets.clear();
        this->m_ParticleEmitters.clear();
        this->m_ParticleModels.clear();
        this->m_AnimatableNodes.clear();
    }

    void PushSceneNode(SceneNode* node)
    {
        bool isPushed = m_SceneNodes.push_back(node);
        NW_ASSERT(isPushed);
    }

    bool TryPushSceneNode(SceneNode* node)
    {
        return m_SceneNodes.push_back(node);
    }

    SceneNodeArray::iterator GetSceneNodesBegin() { return m_SceneNodes.begin(); }
    SceneNodeArray::const_iterator GetSceneNodesBegin() const { return m_SceneNodes.begin(); }
    SceneNodeArray::iterator GetSceneNodesEnd() { return m_SceneNodes.end(); }
    SceneNodeArray::const_iterator GetSceneNodesEnd() const { return m_SceneNodes.end(); }

    std::pair<SceneNodeArray::iterator, SceneNodeArray::iterator> GetSceneNodes()
    {
        return std::make_pair(m_SceneNodes.begin(), m_SceneNodes.end());
    }

    std::pair<SceneNodeArray::const_iterator, SceneNodeArray::const_iterator> GetSceneNodes() const
    {
        return std::make_pair(m_SceneNodes.begin(), m_SceneNodes.end());
    }

    void PushUserRenderNode(UserRenderNode* node)
    {
        bool isPushed = m_UserRenderNodes.push_back(node);
        NW_ASSERT(isPushed);
    }

    bool TryPushUserRenderNode(UserRenderNode* node)
    {
        return m_UserRenderNodes.push_back(node);
    }

    UserRenderNodeArray::iterator GetUserRenderNodesBegin() { return m_UserRenderNodes.begin(); }
    UserRenderNodeArray::const_iterator GetUserRenderNodesBegin() const { return m_UserRenderNodes.begin(); }
    UserRenderNodeArray::iterator GetUserRenderNodesEnd() { return m_UserRenderNodes.end(); }
    UserRenderNodeArray::const_iterator GetUserRenderNodesEnd() const { return m_UserRenderNodes.end(); }

    std::pair<UserRenderNodeArray::iterator, UserRenderNodeArray::iterator> GetUserRenderNodes()
    {
        return std::make_pair(m_UserRenderNodes.begin(), m_UserRenderNodes.end());
    }

    std::pair<UserRenderNodeArray::const_iterator, UserRenderNodeArray::const_iterator> GetUserRenderNodes() const
    {
        return std::make_pair(m_UserRenderNodes.begin(), m_UserRenderNodes.end());
    }

    void PushModel(Model* model)
    {
        bool isPushed = m_Models.push_back(model);
        NW_ASSERT(isPushed);
    }

    bool TryPushModel(Model* model)
    {
        return m_Models.push_back(model);
    }

    ModelArray::iterator GetModelsBegin() { return m_Models.begin(); }
    ModelArray::const_iterator GetModelsBegin() const { return m_Models.begin(); }
    ModelArray::iterator GetModelsEnd() { return m_Models.end(); }
    ModelArray::const_iterator GetModelsEnd() const { return m_Models.end(); }

    void PushSkeletalModel(SkeletalModel* skeletalModel)
    {
        bool isPushed = m_SkeletalModels.push_back(skeletalModel);
        NW_ASSERT(isPushed);
    }

    bool TryPushSkeletalModel(SkeletalModel* skeletalModel)
    {
        return m_SkeletalModels.push_back(skeletalModel);
    }

    SkeletalModelArray::iterator GetSkeletalModelsBegin() { return m_SkeletalModels.begin(); }
    SkeletalModelArray::const_iterator GetSkeletalModelsBegin() const { return m_SkeletalModels.begin(); }
    SkeletalModelArray::iterator GetSkeletalModelsEnd() { return m_SkeletalModels.end(); }
    SkeletalModelArray::const_iterator GetSkeletalModelsEnd() const { return m_SkeletalModels.end(); }

    void PushLight(Light* light)
    {
        bool isPushed = m_Lights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushLight(Light* light)
    {
        return m_Lights.push_back(light);
    }

    LightArray::iterator GetLightsBegin() { return m_Lights.begin(); }
    LightArray::const_iterator GetLightsBegin() const { return m_Lights.begin(); }
    LightArray::iterator GetLightsEnd() { return m_Lights.end(); }
    LightArray::const_iterator GetLightsEnd() const { return m_Lights.end(); }

    void PushFragmentLight(FragmentLight* light)
    {
        bool isPushed = m_FragmentLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushFragmentLight(FragmentLight* light)
    {
        return m_FragmentLights.push_back(light);
    }

    FragmentLightArray::iterator GetFragmentLightsBegin() { return m_FragmentLights.begin(); }
    FragmentLightArray::const_iterator GetFragmentLightsBegin() const { return m_FragmentLights.begin(); }
    FragmentLightArray::iterator GetFragmentLightsEnd() { return m_FragmentLights.end(); }
    FragmentLightArray::const_iterator GetFragmentLightsEnd() const { return m_FragmentLights.end(); }

    void PushVertexLight(VertexLight* light)
    {
        bool isPushed = m_VertexLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushVertexLight(VertexLight* light)
    {
        return m_VertexLights.push_back(light);
    }

    VertexLightArray::iterator GetVertexLightsBegin() { return m_VertexLights.begin(); }
    VertexLightArray::const_iterator GetVertexLightsBegin() const { return m_VertexLights.begin(); }
    VertexLightArray::iterator GetVertexLightsEnd() { return m_VertexLights.end(); }
    VertexLightArray::const_iterator GetVertexLightsEnd() const { return m_VertexLights.end(); }

    void PushHemiSphereLight(HemiSphereLight* light)
    {
        bool isPushed = m_HemiSphereLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushHemiSphereLight(HemiSphereLight* light)
    {
        return m_HemiSphereLights.push_back(light);
    }

    HemiSphereLightArray::iterator GetHemiSphereLightsBegin() { return m_HemiSphereLights.begin(); }
    HemiSphereLightArray::const_iterator GetHemiSphereLightsBegin() const { return m_HemiSphereLights.begin(); }
    HemiSphereLightArray::iterator GetHemiSphereLightsEnd() { return m_HemiSphereLights.end(); }
    HemiSphereLightArray::const_iterator GetHemiSphereLightsEnd() const { return m_HemiSphereLights.end(); }

    void PushAmbientLight(AmbientLight* light)
    {
        bool isPushed = m_AmbientLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushAmbientLight(AmbientLight* light)
    {
        return m_AmbientLights.push_back(light);
    }

    AmbientLightArray::iterator GetAmbientLightsBegin() { return m_AmbientLights.begin(); }
    AmbientLightArray::const_iterator GetAmbientLightsBegin() const { return m_AmbientLights.begin(); }
    AmbientLightArray::iterator GetAmbientLightsEnd() { return m_AmbientLights.end(); }
    AmbientLightArray::const_iterator GetAmbientLightsEnd() const { return m_AmbientLights.end(); }

    void PushCamera(Camera* camera)
    {
        bool isPushed = m_Cameras.push_back(camera);
        NW_ASSERT(isPushed);
    }

    bool TryPushCamera(Camera* camera)
    {
        return m_Cameras.push_back(camera);
    }

    CameraArray::iterator GetCameraBegin() { return m_Cameras.begin(); }
    CameraArray::const_iterator GetCameraBegin() const { return m_Cameras.begin(); }
    CameraArray::iterator GetCameraEnd() { return m_Cameras.end(); }
    CameraArray::const_iterator GetCameraEnd() const { return m_Cameras.end(); }

    void PushFog(Fog* fog)
    {
        bool isPushed = m_Fogs.push_back(fog);
        NW_ASSERT(isPushed);
    }

    bool TryPushFog(Fog* fog)
    {
        return m_Fogs.push_back(fog);
    }

    FogArray::iterator GetFogBegin() { return m_Fogs.begin(); }
    FogArray::const_iterator GetFogBegin() const { return m_Fogs.begin(); }
    FogArray::iterator GetFogEnd() { return m_Fogs.end(); }
    FogArray::const_iterator GetFogEnd() const { return m_Fogs.end(); }

    void PushParticleSet(ParticleSet* particleSet)
    {
        bool isPushed = m_ParticleSets.push_back(particleSet);
        NW_ASSERT(isPushed);
    }

    bool TryPushParticleSet(ParticleSet* particleSet)
    {
        return m_ParticleSets.push_back(particleSet);
    }

    ParticleSetArray::iterator GetParticleSetBegin() { return m_ParticleSets.begin(); }
    ParticleSetArray::const_iterator GetParticleSetBegin() const { return m_ParticleSets.begin(); }
    ParticleSetArray::iterator GetParticleSetEnd() { return m_ParticleSets.end(); }
    ParticleSetArray::const_iterator GetParticleSetEnd() const { return m_ParticleSets.end(); }

    void PushParticleEmitter(ParticleEmitter* particleEmitter)
    {
        bool isPushed = m_ParticleEmitters.push_back(particleEmitter);
        NW_ASSERT(isPushed);
    }

    bool TryPushParticleEmitter(ParticleEmitter* particleEmitter)
    {
        return m_ParticleEmitters.push_back(particleEmitter);
    }

    ParticleEmitterArray::iterator GetParticleEmitterBegin() { return m_ParticleEmitters.begin(); }
    ParticleEmitterArray::const_iterator GetParticleEmitterBegin() const { return m_ParticleEmitters.begin(); }
    ParticleEmitterArray::iterator GetParticleEmitterEnd() { return m_ParticleEmitters.end(); }
    ParticleEmitterArray::const_iterator GetParticleEmitterEnd() const { return m_ParticleEmitters.end(); }

    void PushParticleModel(ParticleModel* particleModel)
    {
        bool isPushed = m_ParticleModels.push_back(particleModel);
        NW_ASSERT(isPushed);
    }

    bool TryPushParticleModel(ParticleModel* particleModel)
    {
        return m_ParticleModels.push_back(particleModel);
    }

    ParticleModelArray::iterator GetParticleModelBegin() { return m_ParticleModels.begin(); }
    ParticleModelArray::const_iterator GetParticleModelBegin() const { return m_ParticleModels.begin(); }
    ParticleModelArray::iterator GetParticleModelEnd() { return m_ParticleModels.end(); }
    ParticleModelArray::const_iterator GetParticleModelEnd() const { return m_ParticleModels.end(); }

    void PushAnimatableNode(SceneNode* sceneNode)
    {
        if (sceneNode->GetAnimBinding() != NULL)
        {
            bool isPushed = m_AnimatableNodes.push_back(sceneNode);
            NW_ASSERT(isPushed);
        }
    }

    bool TryPushAnimatableNode(SceneNode* sceneNode)
    {
        bool isPushed = true;
        if (sceneNode->GetAnimBinding() != NULL)
        {
            isPushed = m_AnimatableNodes.push_back(sceneNode);
        }
        return isPushed;
    }

    AnimatableNodeArray::iterator GetAnimatableNodesBegin() { return m_AnimatableNodes.begin(); }
    AnimatableNodeArray::const_iterator GetAnimatableNodesBegin() const { return m_AnimatableNodes.begin(); }
    AnimatableNodeArray::iterator GetAnimatableNodesEnd() { return m_AnimatableNodes.end(); }
    AnimatableNodeArray::const_iterator GetAnimatableNodesEnd() const { return m_AnimatableNodes.end(); }

private:
    SceneContext(
        nw::os::IAllocator* allocator,
        SceneNodeArray sceneNodes,
        UserRenderNodeArray userRenderNodes,
        ModelArray models,
        SkeletalModelArray skeletalModels,
        CameraArray cameras,
        FogArray fogs,
        LightArray lights,
        FragmentLightArray fragmentLights,
        VertexLightArray vertexLights,
        HemiSphereLightArray hemiSphereLights,
        AmbientLightArray ambientLights,
        ParticleSetArray particleSets,
        ParticleEmitterArray particleEmitters,
        ParticleModelArray particleModels,
        AnimatableNodeArray animatableNodes):
        GfxObject(allocator),
        m_SceneNodes(sceneNodes),
        m_UserRenderNodes(userRenderNodes),
        m_Models(models),
        m_SkeletalModels(skeletalModels),
        m_Cameras(cameras),
        m_Fogs(fogs),
        m_Lights(lights),
        m_FragmentLights(fragmentLights),
        m_VertexLights(vertexLights),
        m_HemiSphereLights(hemiSphereLights),
        m_AmbientLights(ambientLights),
        m_ParticleSets(particleSets),
        m_ParticleEmitters(particleEmitters),
        m_ParticleModels(particleModels),
        m_AnimatableNodes(animatableNodes) {}

    virtual ~SceneContext() {}

    SceneNodeArray m_SceneNodes;
    UserRenderNodeArray m_UserRenderNodes;
    ModelArray m_Models;
    SkeletalModelArray m_SkeletalModels;
    CameraArray m_Cameras;
    FogArray m_Fogs;
    LightArray m_Lights;
    FragmentLightArray m_FragmentLights;
    VertexLightArray m_VertexLights;
    HemiSphereLightArray m_HemiSphereLights;
    AmbientLightArray m_AmbientLights;
    ParticleSetArray m_ParticleSets;
    ParticleEmitterArray m_ParticleEmitters;
    ParticleModelArray m_ParticleModels;
    AnimatableNodeArray m_AnimatableNodes;
};

}
}