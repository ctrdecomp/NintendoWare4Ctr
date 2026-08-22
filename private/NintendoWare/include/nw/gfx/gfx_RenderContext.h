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

#ifndef NW_GFX_RENDERCONTEXT_H_
#define NW_GFX_RENDERCONTEXT_H_

#include <GLES2/gl2.h>

#include <nw/types.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResShape.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/gfx_GraphicsDevice.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_SceneEnvironmentSetting.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class IRenderTarget;
class Camera;
class Model;
class LookupTableCache;
class Light;
class FragmentLight;
class VertexLight;
class AmbientLight;
class HemiSphereLight;
class Fog;
class ShaderProgram;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class RenderContext : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(RenderContext);

public:

    //
    enum
    {
        DEFAULT_MAX_CAMERAS = 32, //
        DEFAULT_MAX_LIGHT_SETS = 128, //
        DEFAULT_MAX_FOGS = 32, //
        ATTRIBUTE_SCALES_VECTOR_COUNT = 3,
        VERTEX_LIGHT_SET_COUNT = 6,
        VERTEX_LIGHT_AMBIENT = 0,
        VERTEX_LIGHT_DIFFUSE = 1,
        VERTEX_LIGHT_POSITION = 2,
        VERTEX_LIGHT_SPOT_DIRECTION = 3,
        VERTEX_LIGHT_DISTANCE_ATTENUATION = 4,
        VERTEX_LIGHT_SPOT_FACTOR = 5
    };

    //
    //
    //
    enum RenderMode
    {
        //
        RENDERMODE_DEFAULT = 0,
        //
        RENDERMODE_IGNORE_MATERIAL = 0x1 << 0,
        //
        RENDERMODE_IGNORE_SHADER = 0x1 << 1,
        //
        RENDERMODE_IGNORE_SCENEENVIRONMENT = 0x1 << 2
    };

    //
    //
    //
    enum ResetStateMode
    {
        //
        RESETSTATEMODE_MODEL_CACHE          = 0x1 << 0,
        //
        RESETSTATEMODE_MATERIAL             = 0x1 << 1,
        //
        RESETSTATEMODE_MATERIAL_CACHE       = 0x1 << 2,
        //
        RESETSTATEMODE_ACTIVE_CAMERA        = 0x1 << 3,
        //
        RESETSTATEMODE_CAMERA_CACHE         = 0x1 << 4,
        //
        RESETSTATEMODE_SCENE_ENVIRONMENT    = 0x1 << 5,
        //
        RESETSTATEMODE_SHADER_PROGRAM       = 0x1 << 6,
        //
        RESETSTATEMODE_LOOK_UP_TABLE        = 0x1 << 7,
        //
        RESETSTATEMODE_HASH                 = 0x1 << 8,
        //
        RESETSTATEMODE_VERTEX_ATTRIBUTE     = 0x1 << 9,

        //
        RESETSTATEMODE_ALL =
            RESETSTATEMODE_MODEL_CACHE       |
            RESETSTATEMODE_MATERIAL          |
            RESETSTATEMODE_MATERIAL_CACHE    |
            RESETSTATEMODE_ACTIVE_CAMERA     |
            RESETSTATEMODE_CAMERA_CACHE      |
            RESETSTATEMODE_SCENE_ENVIRONMENT |
            RESETSTATEMODE_SHADER_PROGRAM    |
            RESETSTATEMODE_LOOK_UP_TABLE     |
            RESETSTATEMODE_HASH              |
            RESETSTATEMODE_VERTEX_ATTRIBUTE
    };

    struct MaterialHash
    {
        MaterialHash()
        {
            ResetMaterialHash(Model::MULTI_FLAG_BUFFER_MATERIAL);
        }

        u32 shaderParameter;
        u32 shadingParameter;
        u32 materialColor;
        u32 rasterization;
        u32 textureCoordinator;
        u32 textureMapper;
        u32 fragmentLighting;
        u32 fragmentLightingTable;
        u32 textureCombiner;
        u32 fragmentOperation;
        u32 alphaTest;

        //
        //
        //
        //
        NW_INLINE void ResetMaterialHash(s32 hashMask);
    };

    //
    typedef ut::FixedSizeArray<ResVertexAttribute, VERTEX_ATTRIBUTE_COUNT> VertexAttributeArray;

    //
    typedef ut::FixedSizeArray<math::VEC4, ATTRIBUTE_SCALES_VECTOR_COUNT> AttributeScaleArray;

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        //
        Builder()
        :
        m_MaxCameras(DEFAULT_MAX_CAMERAS),
        m_MaxLightSets(DEFAULT_MAX_LIGHT_SETS),
        m_MaxFogs(DEFAULT_MAX_FOGS),
        m_MaxVertexLights(LightSet::DEFAULT_MAX_VERTEX_LIGHTS),
        m_ParticleMaterialActivator(NULL)
        {}

        //
        ~Builder() {}

        //
        Builder& MaxCameras(int maxCamera) { m_MaxCameras = maxCamera; return *this; }

        //
        Builder& MaxLights(s32 max) { m_MaxLightSets = max; return *this; }

        //
        Builder& MaxFogs(s32 max) { m_MaxFogs = max; return *this; }

        //
        Builder& MaxVertexLights(int maxVertexLights) { m_MaxVertexLights = maxVertexLights; return *this; }

        //
        //
        //
        Builder& ParticleMaterialActivator(IMaterialActivator* particleMaterialActivator)
        {
            m_ParticleMaterialActivator = particleMaterialActivator;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        RenderContext* Create(os::IAllocator* allocator);

    private:
        int m_MaxCameras;
        int m_MaxLightSets;
        int m_MaxFogs;
        int m_MaxVertexLights;
        IMaterialActivator* m_ParticleMaterialActivator;
    };

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    void SetRenderTarget(IRenderTarget* renderTarget, const Viewport& viewport);

    //
    //
    //
    //
    void SetRenderTarget(IRenderTarget* renderTarget);

    //
    //
    void ResetState();

    //
    //
    //
    //
    //
    void ResetState(s32 resetStateMode, s32 hashMask);

    //
    //
    //
    //
    //
    //
    void ClearBuffer(GLbitfield mask, const nw::ut::FloatColor& color, f32 depth);

    //
    IRenderTarget* GetRenderTarget() { return this->m_RenderTarget; }

    //
    const IRenderTarget* GetRenderTarget() const { return this->m_RenderTarget; }

    //
    //
    //
    //
    //
    //
    //
    //
    void ActivateContext(IMaterialActivator* userMaterialActivator = NULL);

    //
    //
    //
    //
    //
    //
    void ActivateParticleContext();

    //
    //
    //
    //
    void RenderPrimitive(ResPrimitive primitive);

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
    void SetActiveCamera(int index)
    {
        NW_MINMAXLT_ASSERT(index, 0, m_SceneEnvironment.m_Cameras.size());

        Camera* camera = m_SceneEnvironment.m_Cameras[index];
        m_SceneEnvironment.m_CameraIndex = index;

        if (m_SceneEnvironment.m_Camera != camera)
        {
            m_SceneEnvironment.m_Camera = camera;

            // Set the wScale.
            GraphicsDevice::SetWScale(camera->GetWScale());
        }
    }

    //
    Camera* GetActiveCamera()
    {
        return m_SceneEnvironment.m_Camera;
    }

    //
    const Camera* GetActiveCamera() const
    {
        return m_SceneEnvironment.m_Camera;
    }

    //
    s32 GetActiveCameraIndex() const
    {
        return m_SceneEnvironment.m_CameraIndex;
    }

    //
    //
    //
    //
    //
    //
    //
    //
    void SetCameraMatrix(Camera* camera, bool isForce = false);

    //
    //
    //
    //
    //
    //
    //
    //
    //
    void SetModelMatrixForModel(Model* model);

    //
    //
    //
    //
    //
    //
    //
    //
    //
    void SetModelMatrixForSkeletalModel(SkeletalModel* skeletalModel);

    //
    //
    //
    //
    Model* GetModelCache() { return this->m_ModelCache; }

    //
    //
    //
    //
    const Model* GetModelCache() const { return this->m_ModelCache; }

    //
    //
    //
    Material* GetMaterial() { return this->m_Material; }

    //
    //
    //
    const Material* GetMaterial() const { return this->m_Material; }

    //
    //
    //
    //
    void SetMaterial(Material* material)
    {
        NW_NULL_ASSERT(material);
        m_Material = material;
    }

    //
    //
    //
    //
    //
    Material* GetMaterialCache() { return this->m_MaterialCache; }

    //
    //
    //
    //
    //
    const Material* GetMaterialCache() const { return this->m_MaterialCache; }

    //
    //
    //
    ShaderProgram* GetShaderProgram() { return this->m_ShaderProgram.Get(); }

    //
    //
    //
    const ShaderProgram* GetShaderProgram() const { return this->m_ShaderProgram.Get(); }

    //
    SceneEnvironment& GetSceneEnvironment()
    {
        return this->m_SceneEnvironment;
    }

    //
    const SceneEnvironment& GetSceneEnvironment() const
    {
        return this->m_SceneEnvironment;
    }

    //
    void SetMaterialHash(const MaterialHash& materialHash) { this->m_MaterialHash = materialHash; }

    //
    MaterialHash GetMaterialHash() { return this->m_MaterialHash; }

    //
    const MaterialHash* GetMaterialHash() const { return &this->m_MaterialHash; }

    //
    u32 GetRenderMode() const
    {
        return this->m_RenderMode;
    }

    //
    void SetRenderMode(u32 renderMode)
    {
        this->m_RenderMode = renderMode;
    }


    //
    bool IsShaderProgramDirty() const
    {
        return this->m_IsShaderProgramDirty;
    }

#ifdef NW_GFX_MODEL_TRANSLATE_OFFSET_ENABLED
    //
    math::VEC3& ModelTranslateOffset()
    {
        return m_ModelTranslateOffset;
    }

    //
    const math::VEC3& ModelTranslateOffset() const
    {
        return m_ModelTranslateOffset;
    }
#endif

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void ActivateVertexAttribute( ResMesh mesh );

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void DeactivateVertexAttribute( ResMesh mesh );

    //

private:
    static const ut::FloatColor NULL_AMBIENT;

    //
    RenderContext(
        nw::os::IAllocator* pAllocator,
        GfxPtr<ShaderProgram> shaderProgram,
        GfxPtr<IMaterialActivator> particleMaterialActivator,
        const SceneEnvironment::Description& description);

    //
    void ActivateShaderProgram();

    //
    void ActivateSceneEnvironment();

    //
    void ActivateMaterial(IMaterialActivator* userMaterialActivator)
    {
        if (!ut::CheckFlag(m_RenderMode, RenderContext::RENDERMODE_IGNORE_MATERIAL))
        {
            IMaterialActivator* materialActivator = NULL;

            if (userMaterialActivator == NULL)
            {
                materialActivator = this->m_Material->GetOwnerModel()->GetMaterialActivator();
            }
            else
            {
                materialActivator = userMaterialActivator;
            }

            NW_NULL_ASSERT(materialActivator);
            materialActivator->Activate(this, this->m_Material);
        }
    }

    //
    void ActivateParticleMaterial()
    {
        if (!ut::CheckFlag(m_RenderMode, RenderContext::RENDERMODE_IGNORE_MATERIAL))
        {
            m_ParticleMaterialActivator.Get()->Activate(this, this->m_Material);
        }
    }

    //
    void ActivateFog();

    //
    void ActivateAmbientLight();

    //
    void ActivateHemiSphereLight();

    //
    void ActivateFragmentLights();

    //
    void ActivateFragmentLight(int index, const FragmentLight* light);

    //
    void ActivateVertexLights();

    //
    void ActivateVertexLight(int index, const VertexLight* light);

    //
    void TransformToViewCoordinate(
        math::VEC4* out,
        const math::MTX34* view,
        const math::VEC4* v);

    //
    u32 ToLutTexture(s32 index)
    {
        NW_ASSERT(0 <= index && index < LOOKUP_TABLE_COUNT);
        return index + GL_LUT_TEXTURE0_DMP;
    }

    NW_INLINE GLuint ToPrimitiveModeGL(u8 mode, bool isGeometryShaderEnabled);

    GfxPtr<ShaderProgram> m_ShaderProgram;

    bool m_IsVertexAlphaEnabled;
    bool m_IsBoneWeightWEnabled;
    bool m_IsVertexAttributeDirty;
    bool m_IsShaderProgramDirty;

    u32 m_RenderMode;

    IRenderTarget* m_RenderTarget;
    Model* m_ModelCache;
    Material* m_Material;
    Material* m_MaterialCache;
    Camera* m_CameraCache;

    SceneEnvironment m_SceneEnvironment;

    MaterialHash m_MaterialHash;
    GfxPtr<IMaterialActivator> m_ParticleMaterialActivator;

#ifdef NW_GFX_MODEL_TRANSLATE_OFFSET_ENABLED
    math::VEC3 m_ModelTranslateOffset;
#endif
};


//----------------------------------------
NW_INLINE void
RenderContext::MaterialHash::ResetMaterialHash(s32 hashMask)
{
    NW_UNUSED_VARIABLE(hashMask);
    const u32 resetValue = ~(0x0);

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_SHADER_PARAMETER))
    {
        shaderParameter = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_SHADING_PARAMETER))
    {
        shadingParameter = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_MATERIAL_COLOR))
    {
        materialColor = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_RASTERIZATION))
    {
        rasterization = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_TEXTURE_COORDINATOR))
    {
        textureCoordinator = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_TEXTURE_MAPPER))
    {
        textureMapper = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_FRAGMENT_LIGHTING))
    {
        fragmentLighting = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE))
    {
        fragmentLightingTable = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_TEXTURE_COMBINER))
    {
        textureCombiner = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_FRAGMENT_OPERATION))
    {
        fragmentOperation = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_ALPHA_TEST))
    {
        alphaTest = resetValue;
    }
}


//---------------------------------------------------------------------------
NW_INLINE void
RenderContext::DeactivateVertexAttribute( ResMesh mesh )
{
    NW_NULL_ASSERT( mesh.ref().m_DeactivateCommandCache );

    internal::NWUseCmdlist( mesh.ref().m_DeactivateCommandCache, mesh.ref().m_DeactivateCommandCacheSize );
}


} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_RENDERCONTEXT_H_
