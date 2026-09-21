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

class SceneEnvironment
{
public:
    struct Description
    {
        CameraArray cameras;
        FogArray fogs;
        LightSetArray lightSets;
        VertexLightArray vertexLights;
    };

    SceneEnvironment(const Description& description):
        m_AmbientLight(NULL),
        m_HemiSphereLight(NULL),
        m_VertexLights(description.vertexLights),
        m_Fog(NULL),
        m_Camera(NULL),
        m_Cameras(description.cameras),
        m_CameraIndex(-1),
        m_Fogs(description.fogs),
        m_LightSets(description.lightSets),
        m_LightSetIndex(-1),
        m_ActiveVertexLightCount(0),
        m_ActiveFragmentLightCount(0),
        m_FragmentLightsDirty(true),
        m_AmbientLightDirty(true),
        m_VertexLightsDirty(true),
        m_HemiSphereLightDirty(true),
        m_FogDirty(true)
        {
        this->m_FragmentLights.resize(this->m_FragmentLights.capacity());
        this->m_VertexLights.resize(this->m_VertexLights.capacity());

        this->m_Cameras.resize(this->m_Cameras.capacity());
        std::fill(this->m_Cameras.begin(), this->m_Cameras.end(), static_cast<Camera*>(NULL));

        this->m_Fogs.resize(this->m_Fogs.capacity());
        std::fill(this->m_Fogs.begin(), this->m_Fogs.end(), static_cast<Fog*>(NULL));

        this->m_LightSets.resize(this->m_LightSets.capacity());
        std::fill(this->m_LightSets.begin(), this->m_LightSets.end(), static_cast<LightSet*>(NULL));
    }

    void ApplyFrom(const SceneEnvironmentSetting& setting);

    void ClearSettings();

    void SetFog(s32 index, Fog* fog)
    {
        m_Fogs[index] = fog;
    }

    Fog* GetFog(s32 index)
    {
        return m_Fogs[index];
    }

    const Fog* GetFog(s32 index) const
    {
        return m_Fogs[index];
    }

    void SetCamera(int index, Camera* camera)
    {
        m_Cameras[index] = camera;
    }

    Camera* GetCamera(int index)
    {
        return m_Cameras[index];
    }

    const Camera* GetCamera(int index) const
    {
        return m_Cameras[index];
    }

    void SetLightSet(int index, LightSet* lightSet)
    {
        m_LightSets[index] = lightSet;
    }

    LightSet* GetLightSet(int index)
    {
        return m_LightSets[index];
    }

    const LightSet* GetLightSet(int index) const
    {
        return m_LightSets[index];
    }

    void SetFragmentLight(FragmentLight* light)
    {
        if (m_ActiveFragmentLightCount == LIGHT_COUNT)
        {
            return;
        }

        m_FragmentLights[m_ActiveFragmentLightCount] = light;
        ++this->m_ActiveFragmentLightCount;
    }

    s32 GetFragmentLightCount() const
    {
        return m_ActiveFragmentLightCount;
    }

    const FragmentLight* GetFragmentLight(int index) const
    {
        return m_FragmentLights[index];
    }

    void SetVertexLight(VertexLight* light)
    {
        if (m_ActiveVertexLightCount == m_VertexLights.capacity())
        {
            return;
        }

        m_VertexLights[m_ActiveVertexLightCount] = light;
        ++this->m_ActiveVertexLightCount;
    }

    const VertexLight* GetVertexLight(int index) const
    {
        return m_VertexLights[index];
    }

    s32 GetVertexLightCount() const
    {
        return m_ActiveVertexLightCount;
    }

    void SetAmbientLight(AmbientLight* ambientLight)
    {
        m_AmbientLight = ambientLight;
    }

    const AmbientLight* GetAmbientLight() const
    {
        return m_AmbientLight;
    }

    void SetHemiSphereLight(HemiSphereLight* hemiSphereLight)
    {
        m_HemiSphereLight = hemiSphereLight;
    }

    const HemiSphereLight* GetHemiSphereLight() const
    {
        return m_HemiSphereLight;
    }

    void SetActiveLightSet(int index);

    void SetActiveFog(int index)
    {

        Fog* fog = m_Fogs[index];

        if (m_Fog != fog)
        {
            m_Fog = fog;
            m_FogDirty = true;
        }
    }

    Fog* GetActiveFog()
    {
        return m_Fog;
    }

    const Fog* GetActiveFog() const
    {
        return m_Fog;
    }

    bool IsFragmentLightsDirty() const
    {
        return m_FragmentLightsDirty;
    }

    void SetFragmentLightsDirty(bool fragmentLightsDirty)
    {
        m_FragmentLightsDirty = fragmentLightsDirty;
    }

    bool IsAmbientLightDirty() const
    {
        return m_AmbientLightDirty;
    }

    void SetAmbientLightDirty(bool ambientLightDirty)
    {
        m_AmbientLightDirty = ambientLightDirty;
    }

    bool IsVertexLightsDirty() const
    {
        return m_VertexLightsDirty;
    }

    void SetVertexLightsDirty(bool vertexLightsDirty)
    {
        m_VertexLightsDirty = vertexLightsDirty;
    }

    bool IsHemiSphereLightDirty() const
    {
        return m_HemiSphereLightDirty;
    }

    void SetHemiSphereLightDirty(bool hemiSphereLightDirty)
    {
        m_HemiSphereLightDirty = hemiSphereLightDirty;
    }

    bool IsFogDirty() const
    {
        return m_FogDirty;
    }

    void SetFogDirty(bool fogDirty)
    {
        m_FogDirty = fogDirty;
    }

    void SetAllFlagsDirty(bool flagsDirty)
    {
        m_FogDirty = flagsDirty;
        m_AmbientLightDirty = flagsDirty;
        m_FragmentLightsDirty = flagsDirty;

        m_VertexLightsDirty = flagsDirty;
        m_HemiSphereLightDirty = flagsDirty;
    }

    void Reset();

    void ResetFragmentLights();

    void ResetVertexLights();

    void ResetHemiSphereLight();

    void ResetAmbientLight();

    void ResetFog();

    void ResetLightSet();

private:
    AmbientLight* m_AmbientLight;
    HemiSphereLight* m_HemiSphereLight;
    VertexLightArray m_VertexLights;
    FixedFragmentLightArray m_FragmentLights;
    Fog* m_Fog;
    Camera* m_Camera;
    CameraArray m_Cameras;
    s32 m_CameraIndex;
    FogArray m_Fogs;
    LightSetArray m_LightSets;

    s32 m_LightSetIndex;
    s32 m_ActiveVertexLightCount;
    s32 m_ActiveFragmentLightCount;

    bool m_FragmentLightsDirty;
    bool m_AmbientLightDirty;
    bool m_VertexLightsDirty;
    bool m_HemiSphereLightDirty;
    bool m_FogDirty;

    friend class RenderContext;
};

typedef SceneEnvironment RenderEnvironment;

}
}