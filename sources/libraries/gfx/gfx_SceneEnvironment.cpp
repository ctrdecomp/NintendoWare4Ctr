#include <nw/os/os_Memory.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_SceneContext.h>
#include <algorithm>

namespace nw{
namespace gfx{

void SceneEnvironment::ApplyFrom(const SceneEnvironmentSetting& setting)
{
    SceneEnvironmentSetting::CameraBinderArray::const_iterator cameraBinderEnd = setting.GetCameraEnd();
    for (SceneEnvironmentSetting::CameraBinderArray::const_iterator cameraBinder = setting.GetCameraBegin(); cameraBinder != cameraBinderEnd; ++cameraBinder)
    {
        int cameraIndex = (*cameraBinder).index;
        Camera* camera = (*cameraBinder).camera;

        m_Cameras[cameraIndex] = camera;
    }

    SceneEnvironmentSetting::FogBinderArray::const_iterator fogBinderEnd = setting.GetFogEnd();
    for (SceneEnvironmentSetting::FogBinderArray::const_iterator fogBinder = setting.GetFogBegin(); fogBinder != fogBinderEnd; ++fogBinder)
    {
        int fogIndex = (*fogBinder).index;
        Fog* fog = (*fogBinder).fog;

        m_Fogs[fogIndex] = fog;
    }

    SceneEnvironmentSetting::LightSetBinderArray::const_iterator lightSetBinderEnd = setting.GetLightSetEnd();
    for (SceneEnvironmentSetting::LightSetBinderArray::const_iterator lightSetBinder = setting.GetLightSetBegin(); lightSetBinder != lightSetBinderEnd; ++lightSetBinder)
    {
        int lightSetIndex = (*lightSetBinder).index;
        LightSet* lightSet = (*lightSetBinder).lightSet.Get();

        m_LightSets[lightSetIndex] = lightSet;
    }
}

void SceneEnvironment::ClearSettings()
{
    std::fill(this->m_Cameras.begin(), this->m_Cameras.end(), static_cast<gfx::Camera*>(NULL));
    std::fill(this->m_Fogs.begin(), this->m_Fogs.end(), static_cast<gfx::Fog*>(NULL));
    std::fill(this->m_LightSets.begin(), this->m_LightSets.end(), static_cast<gfx::LightSet*>(NULL));
}

void SceneEnvironment::Reset()
{
    this->ResetLightSet();
    this->ResetAmbientLight();
    this->ResetHemiSphereLight();
    this->ResetFragmentLights();
    this->ResetVertexLights();
    this->ResetFog();

    m_Camera = NULL;
    m_CameraIndex = -1;
}

void SceneEnvironment::ResetFragmentLights()
{
    for (int i = 0; i < m_ActiveFragmentLightCount; ++i)
    {
        m_FragmentLights[i] = NULL;
    }
    m_ActiveFragmentLightCount = 0;
    m_FragmentLightsDirty = true;
}

void SceneEnvironment::ResetVertexLights()
{
    for (int i = 0; i < m_ActiveVertexLightCount; ++i)
    {
        m_VertexLights[i] = NULL;
    }
    m_ActiveVertexLightCount = 0;;
    m_VertexLightsDirty = true;
}

void SceneEnvironment::ResetHemiSphereLight()
{
    m_HemiSphereLight = NULL;
    m_HemiSphereLightDirty = true;
}

void SceneEnvironment::ResetAmbientLight()
{
    m_AmbientLight = NULL;
    m_AmbientLightDirty = true;
}

void SceneEnvironment::ResetFog()
{
    m_Fog = NULL;
    m_FogDirty =true;
}

void SceneEnvironment::ResetLightSet()
{
    m_LightSetIndex = -1;
}

void SceneEnvironment::SetActiveLightSet(int index)
{
    NW_MINMAX_ASSERT(index, 0, this->m_LightSets.size());
    LightSet* lightSet = this->m_LightSets[index];

    if ((lightSet != NULL) && (m_LightSetIndex != index))
    {
        m_LightSetIndex = index;

        AmbientLight* ambientLight = lightSet->GetAmbientLight();
        if (m_AmbientLight != ambientLight)
        {
            m_AmbientLightDirty = true;
            m_AmbientLight = (ambientLight != NULL && ambientLight->GetResAmbientLight().IsLightEnabled()) ? ambientLight : NULL;
        }

        HemiSphereLight* hemiSphereLight = lightSet->GetHemiSphereLight();
        if (m_HemiSphereLight != hemiSphereLight)
        {
            m_HemiSphereLightDirty = true;
            m_HemiSphereLight = (hemiSphereLight != NULL && hemiSphereLight->GetResHemiSphereLight().IsLightEnabled()) ? hemiSphereLight : NULL;
        }

        m_ActiveVertexLightCount = 0;
        int vertexLightMaxCount = this->m_VertexLights.capacity();
        VertexLightArray::iterator vertexLightEnd = lightSet->GetVertexLightEnd();
        for (VertexLightArray::iterator iter = lightSet->GetVertexLightBegin(); iter != vertexLightEnd; ++iter)
        {
            m_VertexLightsDirty = true;
            if (m_ActiveVertexLightCount == vertexLightMaxCount)
            {
                break;
            }

            if ((*iter)->GetResVertexLight().IsLightEnabled())
            {
                m_VertexLights[m_ActiveVertexLightCount] = (*iter);
                ++m_ActiveVertexLightCount;
            }
        }

        m_ActiveFragmentLightCount = 0;
        FixedFragmentLightArray::iterator fragmentLightEnd = lightSet->GetFragmentLightEnd();
        for (FixedFragmentLightArray::iterator iter = lightSet->GetFragmentLightBegin(); iter != fragmentLightEnd; ++iter)
        {
            m_FragmentLightsDirty = true;
            if (m_ActiveFragmentLightCount == LIGHT_COUNT)
            {
                break;
            }

            if ((*iter)->GetResFragmentLight().IsLightEnabled())
            {
                m_FragmentLights[m_ActiveFragmentLightCount] = (*iter);
                ++m_ActiveFragmentLightCount;
            }
        }
    }
}

}
}