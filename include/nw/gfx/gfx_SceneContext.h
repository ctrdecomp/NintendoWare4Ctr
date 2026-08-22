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

class SceneContext : public GfxObject{
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

    enum{
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

    class Builder{
    public:
        Builder():
            mIsFixedSizeMemory(true),
            mMaxSceneNodes(DEFAULT_MAX_SCENE_NODES),
            mMaxUserRenderNodes(DEFAULT_MAX_USER_RENDER_NODES),
            mMaxModels(DEFAULT_MAX_MODELS),
            mMaxSkeletalModels(DEFAULT_MAX_SKELETAL_MODELS),
            mMaxCameras(DEFAULT_MAX_CAMERAS),
            mMaxLights(DEFAULT_MAX_LIGHTS),
            mMaxFragmentLights(DEFAULT_MAX_FRAGMENT_LIGHTS),
            mMaxVertexLights(DEFAULT_MAX_VERTEX_LIGHTS),
            mMaxHemiSphereLights(DEFAULT_MAX_HEMISPHERE_LIGHTS),
            mMaxAmbientLights(DEFAULT_MAX_AMBIENT_LIGHTS),
            mMaxFogs(DEFAULT_MAX_FOGS),
            mMaxParticleSets(DEFAULT_MAX_PARTICLESETS),
            mMaxParticleEmitters(DEFAULT_MAX_PARTICLEEMITTERS),
            mMaxParticleModels(DEFAULT_MAX_PARTICLEMODELS),
            mMaxAnimatableNodes(DEFAULT_MAX_ANIMATABLE_NODES)
        {}

        Builder& IsFixedSizeMemory(bool isFixedSizeMemory) { mIsFixedSizeMemory = isFixedSizeMemory; return *this; }
        Builder& MaxSceneNodes(s32 max) { mMaxSceneNodes = max; return *this; }
        Builder& MaxUserRenderNodes(s32 max) { mMaxUserRenderNodes = max; return *this; }
        Builder& MaxModels(s32 max) { mMaxModels = max; return *this; }
        Builder& MaxSkeletalModels(s32 max) { mMaxSkeletalModels = max; return *this; }
        Builder& MaxCameras(s32 max) { mMaxCameras = max; return *this; }
        Builder& MaxLights(s32 max) { mMaxLights = max; return *this; }
        Builder& MaxFragmentLights(s32 max) { mMaxFragmentLights = max; return *this; }
        Builder& MaxVertexLights(s32 max) { mMaxVertexLights = max; return *this; }
        Builder& MaxHemiSphereLights(s32 max) { mMaxHemiSphereLights = max; return *this; }
        Builder& MaxAmbientLights(s32 max) { mMaxAmbientLights = max; return *this; }
        Builder& MaxFogs(s32 max) { mMaxFogs = max; return *this; }
        Builder& MaxParticleSets(s32 max) { mMaxParticleSets = max; return *this; }
        Builder& MaxParticleEmitters(s32 max) { mMaxParticleEmitters = max; return *this; }
        Builder& MaxParticleModels(s32 max) { mMaxParticleModels = max; return *this; }
        Builder& MaxAnimatableNodes(s32 max) { mMaxAnimatableNodes = max; return *this; }

        SceneContext* Create(nw::os::IAllocator* allocator);

    private:
        bool mIsFixedSizeMemory;
        s32 mMaxSceneNodes;
        s32 mMaxUserRenderNodes;
        s32 mMaxModels;
        s32 mMaxSkeletalModels;
        s32 mMaxCameras;
        s32 mMaxLights;
        s32 mMaxFragmentLights;
        s32 mMaxVertexLights;
        s32 mMaxHemiSphereLights;
        s32 mMaxAmbientLights;
        s32 mMaxFogs;
        s32 mMaxParticleSets;
        s32 mMaxParticleEmitters;
        s32 mMaxParticleModels;
        s32 mMaxAnimatableNodes;
    };

    void Clear(){
        this->mSceneNodes.clear();
        this->mUserRenderNodes.clear();
        this->mModels.clear();
        this->mSkeletalModels.clear();
        this->mLights.clear();
        this->mFragmentLights.clear();
        this->mVertexLights.clear();
        this->mHemiSphereLights.clear();
        this->mAmbientLights.clear();
        this->mCameras.clear();
        this->mFogs.clear();
        this->mParticleSets.clear();
        this->mParticleEmitters.clear();
        this->mParticleModels.clear();
        this->mAnimatableNodes.clear();
    }

    void PushSceneNode(SceneNode* node){
        bool isPushed = mSceneNodes.push_back(node);
        NW_ASSERT(isPushed);
    }

    bool TryPushSceneNode(SceneNode* node){
        return mSceneNodes.push_back(node);
    }

    SceneNodeArray::iterator GetSceneNodesBegin() { return mSceneNodes.begin(); }
    SceneNodeArray::const_iterator GetSceneNodesBegin() const { return mSceneNodes.begin(); }
    SceneNodeArray::iterator GetSceneNodesEnd() { return mSceneNodes.end(); }
    SceneNodeArray::const_iterator GetSceneNodesEnd() const { return mSceneNodes.end(); }

    std::pair<SceneNodeArray::iterator, SceneNodeArray::iterator> GetSceneNodes(){
        return std::make_pair(mSceneNodes.begin(), mSceneNodes.end());
    }

    std::pair<SceneNodeArray::const_iterator, SceneNodeArray::const_iterator> GetSceneNodes() const{
        return std::make_pair(mSceneNodes.begin(), mSceneNodes.end());
    }

    void PushUserRenderNode(UserRenderNode* node){
        bool isPushed = mUserRenderNodes.push_back(node);
        NW_ASSERT(isPushed);
    }

    bool TryPushUserRenderNode(UserRenderNode* node){
        return mUserRenderNodes.push_back(node);
    }

    UserRenderNodeArray::iterator GetUserRenderNodesBegin() { return mUserRenderNodes.begin(); }
    UserRenderNodeArray::const_iterator GetUserRenderNodesBegin() const { return mUserRenderNodes.begin(); }
    UserRenderNodeArray::iterator GetUserRenderNodesEnd() { return mUserRenderNodes.end(); }
    UserRenderNodeArray::const_iterator GetUserRenderNodesEnd() const { return mUserRenderNodes.end(); }

    std::pair<UserRenderNodeArray::iterator, UserRenderNodeArray::iterator> GetUserRenderNodes(){
        return std::make_pair(mUserRenderNodes.begin(), mUserRenderNodes.end());
    }

    std::pair<UserRenderNodeArray::const_iterator, UserRenderNodeArray::const_iterator> GetUserRenderNodes() const{
        return std::make_pair(mUserRenderNodes.begin(), mUserRenderNodes.end());
    }

    void PushModel(Model* model){
        bool isPushed = mModels.push_back(model);
        NW_ASSERT(isPushed);
    }

    bool TryPushModel(Model* model){
        return mModels.push_back(model);
    }

    ModelArray::iterator GetModelsBegin() { return mModels.begin(); }
    ModelArray::const_iterator GetModelsBegin() const { return mModels.begin(); }
    ModelArray::iterator GetModelsEnd() { return mModels.end(); }
    ModelArray::const_iterator GetModelsEnd() const { return mModels.end(); }

    void PushSkeletalModel(SkeletalModel* skeletalModel){
        bool isPushed = mSkeletalModels.push_back(skeletalModel);
        NW_ASSERT(isPushed);
    }

    bool TryPushSkeletalModel(SkeletalModel* skeletalModel){
        return mSkeletalModels.push_back(skeletalModel);
    }

    SkeletalModelArray::iterator GetSkeletalModelsBegin() { return mSkeletalModels.begin(); }
    SkeletalModelArray::const_iterator GetSkeletalModelsBegin() const { return mSkeletalModels.begin(); }
    SkeletalModelArray::iterator GetSkeletalModelsEnd() { return mSkeletalModels.end(); }
    SkeletalModelArray::const_iterator GetSkeletalModelsEnd() const { return mSkeletalModels.end(); }

    void PushLight(Light* light){
        bool isPushed = mLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushLight(Light* light){
        return mLights.push_back(light);
    }

    LightArray::iterator GetLightsBegin() { return mLights.begin(); }
    LightArray::const_iterator GetLightsBegin() const { return mLights.begin(); }
    LightArray::iterator GetLightsEnd() { return mLights.end(); }
    LightArray::const_iterator GetLightsEnd() const { return mLights.end(); }

    void PushFragmentLight(FragmentLight* light){
        bool isPushed = mFragmentLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushFragmentLight(FragmentLight* light){
        return mFragmentLights.push_back(light);
    }

    FragmentLightArray::iterator GetFragmentLightsBegin() { return mFragmentLights.begin(); }
    FragmentLightArray::const_iterator GetFragmentLightsBegin() const { return mFragmentLights.begin(); }
    FragmentLightArray::iterator GetFragmentLightsEnd() { return mFragmentLights.end(); }
    FragmentLightArray::const_iterator GetFragmentLightsEnd() const { return mFragmentLights.end(); }

    void PushVertexLight(VertexLight* light){
        bool isPushed = mVertexLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushVertexLight(VertexLight* light){
        return mVertexLights.push_back(light);
    }

    VertexLightArray::iterator GetVertexLightsBegin() { return mVertexLights.begin(); }
    VertexLightArray::const_iterator GetVertexLightsBegin() const { return mVertexLights.begin(); }
    VertexLightArray::iterator GetVertexLightsEnd() { return mVertexLights.end(); }
    VertexLightArray::const_iterator GetVertexLightsEnd() const { return mVertexLights.end(); }

    void PushHemiSphereLight(HemiSphereLight* light){
        bool isPushed = mHemiSphereLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushHemiSphereLight(HemiSphereLight* light){
        return mHemiSphereLights.push_back(light);
    }

    HemiSphereLightArray::iterator GetHemiSphereLightsBegin() { return mHemiSphereLights.begin(); }
    HemiSphereLightArray::const_iterator GetHemiSphereLightsBegin() const { return mHemiSphereLights.begin(); }
    HemiSphereLightArray::iterator GetHemiSphereLightsEnd() { return mHemiSphereLights.end(); }
    HemiSphereLightArray::const_iterator GetHemiSphereLightsEnd() const { return mHemiSphereLights.end(); }

    void PushAmbientLight(AmbientLight* light){
        bool isPushed = mAmbientLights.push_back(light);
        NW_ASSERT(isPushed);
    }

    bool TryPushAmbientLight(AmbientLight* light){
        return mAmbientLights.push_back(light);
    }

    AmbientLightArray::iterator GetAmbientLightsBegin() { return mAmbientLights.begin(); }
    AmbientLightArray::const_iterator GetAmbientLightsBegin() const { return mAmbientLights.begin(); }
    AmbientLightArray::iterator GetAmbientLightsEnd() { return mAmbientLights.end(); }
    AmbientLightArray::const_iterator GetAmbientLightsEnd() const { return mAmbientLights.end(); }

    void PushCamera(Camera* camera){
        bool isPushed = mCameras.push_back(camera);
        NW_ASSERT(isPushed);
    }

    bool TryPushCamera(Camera* camera){
        return mCameras.push_back(camera);
    }

    CameraArray::iterator GetCameraBegin() { return mCameras.begin(); }
    CameraArray::const_iterator GetCameraBegin() const { return mCameras.begin(); }
    CameraArray::iterator GetCameraEnd() { return mCameras.end(); }
    CameraArray::const_iterator GetCameraEnd() const { return mCameras.end(); }

    void PushFog(Fog* fog){
        bool isPushed = mFogs.push_back(fog);
        NW_ASSERT(isPushed);
    }

    bool TryPushFog(Fog* fog){
        return mFogs.push_back(fog);
    }

    FogArray::iterator GetFogBegin() { return mFogs.begin(); }
    FogArray::const_iterator GetFogBegin() const { return mFogs.begin(); }
    FogArray::iterator GetFogEnd() { return mFogs.end(); }
    FogArray::const_iterator GetFogEnd() const { return mFogs.end(); }

    void PushParticleSet(ParticleSet* particleSet){
        bool isPushed = mParticleSets.push_back(particleSet);
        NW_ASSERT(isPushed);
    }

    bool TryPushParticleSet(ParticleSet* particleSet){
        return mParticleSets.push_back(particleSet);
    }

    ParticleSetArray::iterator GetParticleSetBegin() { return mParticleSets.begin(); }
    ParticleSetArray::const_iterator GetParticleSetBegin() const { return mParticleSets.begin(); }
    ParticleSetArray::iterator GetParticleSetEnd() { return mParticleSets.end(); }
    ParticleSetArray::const_iterator GetParticleSetEnd() const { return mParticleSets.end(); }

    void PushParticleEmitter(ParticleEmitter* particleEmitter){
        bool isPushed = mParticleEmitters.push_back(particleEmitter);
        NW_ASSERT(isPushed);
    }

    bool TryPushParticleEmitter(ParticleEmitter* particleEmitter){
        return mParticleEmitters.push_back(particleEmitter);
    }

    ParticleEmitterArray::iterator GetParticleEmitterBegin() { return mParticleEmitters.begin(); }
    ParticleEmitterArray::const_iterator GetParticleEmitterBegin() const { return mParticleEmitters.begin(); }
    ParticleEmitterArray::iterator GetParticleEmitterEnd() { return mParticleEmitters.end(); }
    ParticleEmitterArray::const_iterator GetParticleEmitterEnd() const { return mParticleEmitters.end(); }

    void PushParticleModel(ParticleModel* particleModel){
        bool isPushed = mParticleModels.push_back(particleModel);
        NW_ASSERT(isPushed);
    }

    bool TryPushParticleModel(ParticleModel* particleModel){
        return mParticleModels.push_back(particleModel);
    }

    ParticleModelArray::iterator GetParticleModelBegin() { return mParticleModels.begin(); }
    ParticleModelArray::const_iterator GetParticleModelBegin() const { return mParticleModels.begin(); }
    ParticleModelArray::iterator GetParticleModelEnd() { return mParticleModels.end(); }
    ParticleModelArray::const_iterator GetParticleModelEnd() const { return mParticleModels.end(); }

    void PushAnimatableNode(SceneNode* sceneNode){
        if (sceneNode->GetAnimBinding() != NULL){
            bool isPushed = mAnimatableNodes.push_back(sceneNode);
            NW_ASSERT(isPushed);
        }
    }

    bool TryPushAnimatableNode(SceneNode* sceneNode){
        bool isPushed = true;
        if (sceneNode->GetAnimBinding() != NULL){
            isPushed = mAnimatableNodes.push_back(sceneNode);
        }
        return isPushed;
    }

    AnimatableNodeArray::iterator GetAnimatableNodesBegin() { return mAnimatableNodes.begin(); }
    AnimatableNodeArray::const_iterator GetAnimatableNodesBegin() const { return mAnimatableNodes.begin(); }
    AnimatableNodeArray::iterator GetAnimatableNodesEnd() { return mAnimatableNodes.end(); }
    AnimatableNodeArray::const_iterator GetAnimatableNodesEnd() const { return mAnimatableNodes.end(); }

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
        mSceneNodes(sceneNodes),
        mUserRenderNodes(userRenderNodes),
        mModels(models),
        mSkeletalModels(skeletalModels),
        mCameras(cameras),
        mFogs(fogs),
        mLights(lights),
        mFragmentLights(fragmentLights),
        mVertexLights(vertexLights),
        mHemiSphereLights(hemiSphereLights),
        mAmbientLights(ambientLights),
        mParticleSets(particleSets),
        mParticleEmitters(particleEmitters),
        mParticleModels(particleModels),
        mAnimatableNodes(animatableNodes)
    {}

    virtual ~SceneContext() {}

    SceneNodeArray mSceneNodes;
    UserRenderNodeArray mUserRenderNodes;
    ModelArray mModels;
    SkeletalModelArray mSkeletalModels;
    CameraArray mCameras;
    FogArray mFogs;
    LightArray mLights;
    FragmentLightArray mFragmentLights;
    VertexLightArray mVertexLights;
    HemiSphereLightArray mHemiSphereLights;
    AmbientLightArray mAmbientLights;
    ParticleSetArray mParticleSets;
    ParticleEmitterArray mParticleEmitters;
    ParticleModelArray mParticleModels;
    AnimatableNodeArray mAnimatableNodes;
};

}
}