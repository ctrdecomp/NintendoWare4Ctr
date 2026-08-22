/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/
#ifndef NW_GFX_SCENEENVIRONMENT_H_
#define NW_GFX_SCENEENVIRONMENT_H_

#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_SceneEnvironmentSetting.h>
#include <nw/gfx/gfx_LightSet.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class FragmentLight;
class VertexLight;
class AmbientLight;
class Fog;
class HemiSphereLight;
class Camera;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class SceneEnvironment
{
public:

    //
    struct Description
    {
        CameraArray cameras;
        FogArray fogs;
        LightSetArray lightSets;
        VertexLightArray vertexLights;
    };

    //
    SceneEnvironment(const Description& description)
        : m_AmbientLight(NULL),
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
        m_FragmentLights.resize(m_FragmentLights.capacity());
        m_VertexLights.resize(m_VertexLights.capacity());

        m_Cameras.resize(m_Cameras.capacity());
        std::fill(m_Cameras.begin(), m_Cameras.end(), static_cast<Camera*>(NULL));

        m_Fogs.resize(m_Fogs.capacity());
        std::fill(m_Fogs.begin(), m_Fogs.end(), static_cast<Fog*>(NULL));

        m_LightSets.resize(m_LightSets.capacity());
        std::fill(m_LightSets.begin(), m_LightSets.end(), static_cast<LightSet*>(NULL));
    }

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void ApplyFrom(const SceneEnvironmentSetting& setting);

    //
    void ClearSettings();

    //
    //
    //
    //
    //
    void SetFog(s32 index, Fog* fog)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Fogs.size());
        m_Fogs[index] = fog;
    }

    //
    //
    //
    //
    Fog* GetFog(s32 index)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Fogs.size());
        return m_Fogs[index];
    }

    //
    //
    //
    //
    const Fog* GetFog(s32 index) const
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Fogs.size());
        return m_Fogs[index];
    }

    //
    //
    //
    //
    //
    void SetCamera(int index, Camera* camera)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Cameras.size());
        m_Cameras[index] = camera;
    }

    //
    //
    //
    //
    Camera* GetCamera(int index)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Cameras.size());
        return m_Cameras[index];
    }

    //
    //
    //
    //
    const Camera* GetCamera(int index) const
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Cameras.size());
        return m_Cameras[index];
    }

    //
    //
    //
    //
    //
    void SetLightSet(int index, LightSet* lightSet)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_LightSets.size());
        m_LightSets[index] = lightSet;
    }

    //
    //
    //
    //
    LightSet* GetLightSet(int index)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_LightSets.size());
        return m_LightSets[index];
    }

    //
    //
    //
    //
    const LightSet* GetLightSet(int index) const
    {
        NW_MINMAXLT_ASSERT(index, 0, m_LightSets.size());
        return m_LightSets[index];
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    void SetFragmentLight(FragmentLight* light)
    {
        if (this->m_ActiveFragmentLightCount == LIGHT_COUNT)
        {
            return;
        }

        m_FragmentLights[m_ActiveFragmentLightCount] = (light);
        ++m_ActiveFragmentLightCount;
    }

    //
    //
    s32 GetFragmentLightCount() const
    {
        return this->m_ActiveFragmentLightCount;
    }

    //
    //
    //
    //
    //
    const FragmentLight* GetFragmentLight(int index) const
    {
        NW_MINMAXLT_ASSERT(index, 0, this->m_ActiveFragmentLightCount);
        return this->m_FragmentLights[ index ];
    }

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    void SetVertexLight(VertexLight* light)
    {
        if (m_ActiveVertexLightCount == m_VertexLights.capacity())
        {
            return;
        }

        this->m_VertexLights[m_ActiveVertexLightCount] = light;
        ++m_ActiveVertexLightCount;
    }

    //
    //
    //
    //
    //
    const VertexLight* GetVertexLight(int index) const
    {
        NW_MINMAXLT_ASSERT(index, 0, this->m_ActiveVertexLightCount);
        return this->m_VertexLights[ index ];
    }

    //
    //
    s32 GetVertexLightCount() const
    {
        return this->m_ActiveVertexLightCount;
    }

    //
    //
    //
    //
    void SetAmbientLight(AmbientLight* ambientLight)
    {
        this->m_AmbientLight = ambientLight;
    }

    //
    //
    //
    const AmbientLight* GetAmbientLight() const
    {
        return this->m_AmbientLight;
    }

    //
    //
    //
    //
    void SetHemiSphereLight(HemiSphereLight* hemiSphereLight)
    {
        this->m_HemiSphereLight = hemiSphereLight;
    }

    //
    //
    //
    //
    const HemiSphereLight* GetHemiSphereLight() const
    {
        return this->m_HemiSphereLight;
    }


    //
    //
    //
    //
    void SetActiveLightSet(int index);

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    void SetActiveFog(int index)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_Fogs.size());

        Fog* fog = m_Fogs[index];

        if (this->m_Fog != fog)
        {
            this->m_Fog = fog;
            this->m_FogDirty = true;
        }
    }

    //
    //
    //
    //
    Fog* GetActiveFog()
    {
        return this->m_Fog;
    }

    //
    //
    //
    //
    const Fog* GetActiveFog() const
    {
        return this->m_Fog;
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    bool IsFragmentLightsDirty() const
    {
        return m_FragmentLightsDirty;
    }

    //
    //
    void SetFragmentLightsDirty(bool fragmentLightsDirty)
    {
        m_FragmentLightsDirty = fragmentLightsDirty;
    }

    //
    //
    bool IsAmbientLightDirty() const
    {
        return m_AmbientLightDirty;
    }

    //
    //
    void SetAmbientLightDirty(bool ambientLightDirty)
    {
        m_AmbientLightDirty = ambientLightDirty;
    }

    //
    //
    bool IsVertexLightsDirty() const
    {
#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
        return m_VertexLightsDirty;
#else
        return false;
#endif
    }

    //
    //
    void SetVertexLightsDirty(bool vertexLightsDirty)
    {
        m_VertexLightsDirty = vertexLightsDirty;
    }

    //
    //
    bool IsHemiSphereLightDirty() const
    {
        return m_HemiSphereLightDirty;
    }

    //
    //
    void SetHemiSphereLightDirty(bool hemiSphereLightDirty)
    {
        m_HemiSphereLightDirty = hemiSphereLightDirty;
    }

    //
    //
    bool IsFogDirty() const
    {
        return m_FogDirty;
    }

    //
    //
    void SetFogDirty(bool fogDirty)
    {
        m_FogDirty = fogDirty;
    }

    //
    //
    void SetAllFlagsDirty(bool flagsDirty)
    {
        m_FogDirty = flagsDirty;
        m_AmbientLightDirty = flagsDirty;
        m_FragmentLightsDirty = flagsDirty;

#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
        m_VertexLightsDirty = flagsDirty;
#endif
        m_HemiSphereLightDirty = flagsDirty;
    }

    //

    //----------------------------------------
    //
    //

    //
    void Reset();

    //
    void ResetFragmentLights();

    //
    void ResetVertexLights();

    //
    void ResetHemiSphereLight();

    //
    void ResetAmbientLight();

    //
    void ResetFog();

    //
    void ResetLightSet();

    //

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

//
typedef SceneEnvironment RenderEnvironment;

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_SCENEENVIRONMENT_H_

