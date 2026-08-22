#pragma once

#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_SceneEnvironmentSetting.h>
#include <nw/gfx/gfx_LightSet.h>

namespace nw{
namespace gfx{

class FragmentLight;
class VertexLight;
class AmbientLight;
class Fog;
class HemiSphereLight;
class Camera;

class SceneEnvironment{
public:
    struct Description{
        CameraArray cameras;
        FogArray fogs;
        LightSetArray lightSets;
        VertexLightArray vertexLights;
    };

    SceneEnvironment(const Description& description):
        mAmbientLight(NULL),
        mHemiSphereLight(NULL),
        mVertexLights(description.vertexLights),
        mFog(NULL),
        mCamera(NULL),
        mCameras(description.cameras),
        mCameraIndex(-1),
        mFogs(description.fogs),
        mLightSets(description.lightSets),
        mLightSetIndex(-1),
        mActiveVertexLightCount(0),
        mActiveFragmentLightCount(0),
        mFragmentLightsDirty(true),
        mAmbientLightDirty(true),
        mVertexLightsDirty(true),
        mHemiSphereLightDirty(true),
        mFogDirty(true){
        this->mFragmentLights.resize(this->mFragmentLights.capacity());
        this->mVertexLights.resize(this->mVertexLights.capacity());

        this->mCameras.resize(this->mCameras.capacity());
        std::fill(this->mCameras.begin(), this->mCameras.end(), static_cast<Camera*>(NULL));

        this->mFogs.resize(this->mFogs.capacity());
        std::fill(this->mFogs.begin(), this->mFogs.end(), static_cast<Fog*>(NULL));

        this->mLightSets.resize(this->mLightSets.capacity());
        std::fill(this->mLightSets.begin(), this->mLightSets.end(), static_cast<LightSet*>(NULL));
    }

    void ApplyFrom(const SceneEnvironmentSetting& setting);

    void ClearSettings();

    void SetFog(s32 index, Fog* fog){
        mFogs[index] = fog;
    }

    Fog* GetFog(s32 index){
        return mFogs[index];
    }

    const Fog* GetFog(s32 index) const{
        return mFogs[index];
    }

    void SetCamera(int index, Camera* camera){
        mCameras[index] = camera;
    }

    Camera* GetCamera(int index){
        return mCameras[index];
    }

    const Camera* GetCamera(int index) const{
        return mCameras[index];
    }

    void SetLightSet(int index, LightSet* lightSet){
        mLightSets[index] = lightSet;
    }

    LightSet* GetLightSet(int index){
        return mLightSets[index];
    }

    const LightSet* GetLightSet(int index) const{
        return mLightSets[index];
    }

    void SetFragmentLight(FragmentLight* light){
        if (mActiveFragmentLightCount == LIGHT_COUNT){
            return;
        }

        mFragmentLights[mActiveFragmentLightCount] = light;
        ++this->mActiveFragmentLightCount;
    }

    s32 GetFragmentLightCount() const{
        return mActiveFragmentLightCount;
    }

    const FragmentLight* GetFragmentLight(int index) const{
        return mFragmentLights[index];
    }

    void SetVertexLight(VertexLight* light){
        if (mActiveVertexLightCount == mVertexLights.capacity()){
            return;
        }

        mVertexLights[mActiveVertexLightCount] = light;
        ++this->mActiveVertexLightCount;
    }

    const VertexLight* GetVertexLight(int index) const{
        return mVertexLights[index];
    }

    s32 GetVertexLightCount() const{
        return mActiveVertexLightCount;
    }

    void SetAmbientLight(AmbientLight* ambientLight){
        mAmbientLight = ambientLight;
    }

    const AmbientLight* GetAmbientLight() const{
        return mAmbientLight;
    }

    void SetHemiSphereLight(HemiSphereLight* hemiSphereLight){
        mHemiSphereLight = hemiSphereLight;
    }

    const HemiSphereLight* GetHemiSphereLight() const{
        return mHemiSphereLight;
    }

    void SetActiveLightSet(int index);

    void SetActiveFog(int index){

        Fog* fog = mFogs[index];

        if (mFog != fog)
        {
            mFog = fog;
            mFogDirty = true;
        }
    }

    Fog* GetActiveFog(){
        return mFog;
    }

    const Fog* GetActiveFog() const{
        return mFog;
    }

    bool IsFragmentLightsDirty() const{
        return mFragmentLightsDirty;
    }

    void SetFragmentLightsDirty(bool fragmentLightsDirty){
        mFragmentLightsDirty = fragmentLightsDirty;
    }

    bool IsAmbientLightDirty() const{
        return mAmbientLightDirty;
    }

    void SetAmbientLightDirty(bool ambientLightDirty){
        mAmbientLightDirty = ambientLightDirty;
    }

    bool IsVertexLightsDirty() const{
        return mVertexLightsDirty;
    }

    void SetVertexLightsDirty(bool vertexLightsDirty){
        mVertexLightsDirty = vertexLightsDirty;
    }

    bool IsHemiSphereLightDirty() const{
        return mHemiSphereLightDirty;
    }

    void SetHemiSphereLightDirty(bool hemiSphereLightDirty){
        mHemiSphereLightDirty = hemiSphereLightDirty;
    }

    bool IsFogDirty() const{
        return mFogDirty;
    }

    void SetFogDirty(bool fogDirty){
        mFogDirty = fogDirty;
    }

    void SetAllFlagsDirty(bool flagsDirty){
        mFogDirty = flagsDirty;
        mAmbientLightDirty = flagsDirty;
        mFragmentLightsDirty = flagsDirty;

        mVertexLightsDirty = flagsDirty;
        mHemiSphereLightDirty = flagsDirty;
    }

    void Reset();

    void ResetFragmentLights();

    void ResetVertexLights();

    void ResetHemiSphereLight();

    void ResetAmbientLight();

    void ResetFog();

    void ResetLightSet();

private:
    AmbientLight* mAmbientLight;
    HemiSphereLight* mHemiSphereLight;
    VertexLightArray mVertexLights;
    FixedFragmentLightArray mFragmentLights;
    Fog* mFog;
    Camera* mCamera;
    CameraArray mCameras;
    s32 mCameraIndex;
    FogArray mFogs;
    LightSetArray mLightSets;

    s32 mLightSetIndex;
    s32 mActiveVertexLightCount;
    s32 mActiveFragmentLightCount;

    bool mFragmentLightsDirty;
    bool mAmbientLightDirty;
    bool mVertexLightsDirty;
    bool mHemiSphereLightDirty;
    bool mFogDirty;

    friend class RenderContext;
};

typedef SceneEnvironment RenderEnvironment;

}
}