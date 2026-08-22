#include <nw/os/os_Memory.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <algorithm>

namespace nw{
namespace gfx{

void SceneEnvironment::ApplyFrom(const SceneEnvironmentSetting& setting){
    SceneEnvironmentSetting::CameraBinderArray::const_iterator cameraBinderEnd = setting.GetCameraEnd();
    for (SceneEnvironmentSetting::CameraBinderArray::const_iterator cameraBinder = setting.GetCameraBegin(); cameraBinder != cameraBinderEnd; ++cameraBinder){
        int cameraIndex = (*cameraBinder).index;
        Camera* camera = (*cameraBinder).camera;

        mCameras[cameraIndex] = camera;
    }

    SceneEnvironmentSetting::FogBinderArray::const_iterator fogBinderEnd = setting.GetFogEnd();
    for (SceneEnvironmentSetting::FogBinderArray::const_iterator fogBinder = setting.GetFogBegin(); fogBinder != fogBinderEnd; ++fogBinder){
        int fogIndex = (*fogBinder).index;
        Fog* fog = (*fogBinder).fog;

        mFogs[fogIndex] = fog;
    }

    SceneEnvironmentSetting::LightSetBinderArray::const_iterator lightSetBinderEnd = setting.GetLightSetEnd();
    for (SceneEnvironmentSetting::LightSetBinderArray::const_iterator lightSetBinder = setting.GetLightSetBegin(); lightSetBinder != lightSetBinderEnd; ++lightSetBinder){
        int lightSetIndex = (*lightSetBinder).index;
        LightSet* lightSet = (*lightSetBinder).lightSet.Get();

        mLightSets[lightSetIndex] = lightSet;
    }
}

void SceneEnvironment::ClearSettings(){
    std::fill(this->mCameras.begin(), this->mCameras.end(), static_cast<gfx::Camera*>(NULL));
    std::fill(this->mFogs.begin(), this->mFogs.end(), static_cast<gfx::Fog*>(NULL));
    std::fill(this->mLightSets.begin(), this->mLightSets.end(), static_cast<gfx::LightSet*>(NULL));
}

void SceneEnvironment::Reset(){
    this->ResetLightSet();
    this->ResetAmbientLight();
    this->ResetHemiSphereLight();
    this->ResetFragmentLights();
    this->ResetVertexLights();
    this->ResetFog();

    mCamera = NULL;
    mCameraIndex = -1;
}

void SceneEnvironment::ResetFragmentLights(){
    for (int i = 0; i < mActiveFragmentLightCount; ++i){
        mFragmentLights[i] = NULL;
    }
    mActiveFragmentLightCount = 0;
    mFragmentLightsDirty = true;
}

void SceneEnvironment::ResetVertexLights(){
    for (int i = 0; i < mActiveVertexLightCount; ++i){
        mVertexLights[i] = NULL;
    }
    mActiveVertexLightCount = 0;;
    mVertexLightsDirty = true;
}

void SceneEnvironment::ResetHemiSphereLight(){
    mHemiSphereLight = NULL;
    mHemiSphereLightDirty = true;
}

void SceneEnvironment::ResetAmbientLight(){
    mAmbientLight = NULL;
    mAmbientLightDirty = true;
}

void SceneEnvironment::ResetFog(){
    mFog = NULL;
    mFogDirty =true;
}

void SceneEnvironment::ResetLightSet(){
    mLightSetIndex = -1;
}

void SceneEnvironment::SetActiveLightSet(int index){
    NW_MINMAX_ASSERT(index, 0, this->mLightSets.size());
    LightSet* lightSet = this->mLightSets[index];

    if ((lightSet != NULL) && (mLightSetIndex != index)){
        mLightSetIndex = index;

        AmbientLight* ambientLight = lightSet->GetAmbientLight();
        if (mAmbientLight != ambientLight){
            mAmbientLightDirty = true;
            mAmbientLight = (ambientLight != NULL && ambientLight->GetResAmbientLight().IsLightEnabled()) ? ambientLight : NULL;
        }

        HemiSphereLight* hemiSphereLight = lightSet->GetHemiSphereLight();
        if (mHemiSphereLight != hemiSphereLight){
            mHemiSphereLightDirty = true;
            mHemiSphereLight = (hemiSphereLight != NULL && hemiSphereLight->GetResHemiSphereLight().IsLightEnabled()) ? hemiSphereLight : NULL;
        }

        mActiveVertexLightCount = 0;
        int vertexLightMaxCount = this->mVertexLights.capacity();
        VertexLightArray::iterator vertexLightEnd = lightSet->GetVertexLightEnd();
        for (VertexLightArray::iterator iter = lightSet->GetVertexLightBegin(); iter != vertexLightEnd; ++iter){
            mVertexLightsDirty = true;
            if (mActiveVertexLightCount == vertexLightMaxCount){
                break;
            }

            if ((*iter)->GetResVertexLight().IsLightEnabled()){
                mVertexLights[mActiveVertexLightCount] = (*iter);
                ++mActiveVertexLightCount;
            }
        }

        mActiveFragmentLightCount = 0;
        FixedFragmentLightArray::iterator fragmentLightEnd = lightSet->GetFragmentLightEnd();
        for (FixedFragmentLightArray::iterator iter = lightSet->GetFragmentLightBegin(); iter != fragmentLightEnd; ++iter){
            mFragmentLightsDirty = true;
            if (mActiveFragmentLightCount == LIGHT_COUNT){
                break;
            }

            if ((*iter)->GetResFragmentLight().IsLightEnabled()){
                mFragmentLights[mActiveFragmentLightCount] = (*iter);
                ++mActiveFragmentLightCount;
            }
        }
    }
}

}
}