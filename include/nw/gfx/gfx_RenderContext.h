#pragma once

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
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_SceneEnvironmentSetting.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

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

class RenderContext : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(RenderContext);

public:
    enum{
        DEFAULT_MAX_CAMERAS = 32,
        DEFAULT_MAX_LIGHT_SETS = 128,
        DEFAULT_MAX_FOGS = 32,
        ATTRIBUTE_SCALES_VECTOR_COUNT = 3,
        VERTEX_LIGHT_SET_COUNT = 6,
        VERTEX_LIGHT_AMBIENT = 0,
        VERTEX_LIGHT_DIFFUSE = 1,
        VERTEX_LIGHT_POSITION = 2,
        VERTEX_LIGHT_SPOT_DIRECTION = 3,
        VERTEX_LIGHT_DISTANCE_ATTENUATION = 4,
        VERTEX_LIGHT_SPOT_FACTOR = 5
    };

    enum RenderMode{
        RENDERMODE_DEFAULT = 0,
        RENDERMODE_IGNORE_MATERIAL = 0x1 << 0,
        RENDERMODE_IGNORE_SHADER = 0x1 << 1,
        RENDERMODE_IGNORE_SCENEENVIRONMENT = 0x1 << 2
    };

    enum ResetStateMode{
        RESETSTATEMODE_MODEL_CACHE = 0x1 << 0,
        RESETSTATEMODE_MATERIAL = 0x1 << 1,
        RESETSTATEMODE_MATERIAL_CACHE = 0x1 << 2,
        RESETSTATEMODE_ACTIVE_CAMERA = 0x1 << 3,
        RESETSTATEMODE_CAMERA_CACHE = 0x1 << 4,
        RESETSTATEMODE_SCENE_ENVIRONMENT = 0x1 << 5,
        RESETSTATEMODE_SHADER_PROGRAM = 0x1 << 6,
        RESETSTATEMODE_LOOK_UP_TABLE = 0x1 << 7,
        RESETSTATEMODE_HASH = 0x1 << 8,
        RESETSTATEMODE_VERTEX_ATTRIBUTE = 0x1 << 9,

        RESETSTATEMODE_ALL =
            RESETSTATEMODE_MODEL_CACHE |
            RESETSTATEMODE_MATERIAL |
            RESETSTATEMODE_MATERIAL_CACHE |
            RESETSTATEMODE_ACTIVE_CAMERA |
            RESETSTATEMODE_CAMERA_CACHE |
            RESETSTATEMODE_SCENE_ENVIRONMENT |
            RESETSTATEMODE_SHADER_PROGRAM |
            RESETSTATEMODE_LOOK_UP_TABLE |
            RESETSTATEMODE_HASH |
            RESETSTATEMODE_VERTEX_ATTRIBUTE
    };

    struct MaterialHash{
        MaterialHash(){
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

        inline void ResetMaterialHash(s32 hashMask);
    };

    typedef ut::FixedSizeArray<ResVertexAttribute, VERTEX_ATTRIBUTE_COUNT> VertexAttributeArray;
    typedef ut::FixedSizeArray<math::VEC4, ATTRIBUTE_SCALES_VECTOR_COUNT> AttributeScaleArray;

    class Builder
    {
    public:
        Builder():
            mMaxCameras(DEFAULT_MAX_CAMERAS),
            mMaxLightSets(DEFAULT_MAX_LIGHT_SETS),
            mMaxFogs(DEFAULT_MAX_FOGS),
            mMaxVertexLights(LightSet::DEFAULT_MAX_VERTEX_LIGHTS),
            mParticleMaterialActivator(NULL)
        {}

        ~Builder() {}

        Builder& MaxCameras(int maxCamera) { mMaxCameras = maxCamera; return *this; }
        Builder& MaxLights(s32 max) { mMaxLightSets = max; return *this; }
        Builder& MaxFogs(s32 max) { mMaxFogs = max; return *this; }
        Builder& MaxVertexLights(int maxVertexLights) { mMaxVertexLights = maxVertexLights; return *this; }

        Builder& ParticleMaterialActivator(IMaterialActivator* particleMaterialActivator){
            mParticleMaterialActivator = particleMaterialActivator;
            return *this;
        }

        RenderContext* Create(nw::os::IAllocator* allocator);

    private:
        int mMaxCameras;
        int mMaxLightSets;
        int mMaxFogs;
        int mMaxVertexLights;
        IMaterialActivator* mParticleMaterialActivator;
    };

    void SetRenderTarget(IRenderTarget* renderTarget, const Viewport& viewport);

    void SetRenderTarget(IRenderTarget* renderTarget);

    void ResetState();

    void ResetState(s32 resetStateMode, s32 hashMask);

    void ClearBuffer(GLbitfield mask, const nw::ut::FloatColor& color, f32 depth);

    IRenderTarget* GetRenderTarget() { return mRenderTarget; }
    const IRenderTarget* GetRenderTarget() const { return mRenderTarget; }

    void ActivateContext(IMaterialActivator* userMaterialActivator = NULL);

    void ActivateParticleContext();

    void RenderPrimitive(ResPrimitive primitive);

    void SetActiveCamera(int index){
        Camera* camera = mSceneEnvironment.mCameras[index];
        mSceneEnvironment.mCameraIndex = index;

        if (mSceneEnvironment.mCamera != camera)
        {
            mSceneEnvironment.mCamera = camera;
            GraphicsDevice::SetWScale(camera->GetWScale());
        }
    }

    Camera* GetActiveCamera(){
        return mSceneEnvironment.mCamera;
    }

    const Camera* GetActiveCamera() const{
        return mSceneEnvironment.mCamera;
    }

    s32 GetActiveCameraIndex() const{
        return mSceneEnvironment.mCameraIndex;
    }

    void SetCameraMatrix(Camera* camera, bool isForce = false);

    NW_DEPRECATED_FUNCTION(void SetModelMatrix(Model* model)){
        if (model == NULL){
            mModelCache = NULL;
            return;
        }

        SkeletalModel* skeletalModel = ut::DynamicCast<SkeletalModel*>(model);
        if (skeletalModel){
            this->SetModelMatrixForSkeletalModel(skeletalModel);
        }
        else{
            this->SetModelMatrixForModel(model);
        }
    }

    void SetModelMatrixForModel(Model* model);

    void SetModelMatrixForSkeletalModel(SkeletalModel* skeletalModel);

    Model* GetModelCache() { return mModelCache; }
    const Model* GetModelCache() const { return mModelCache; }

    Material* GetMaterial() { return mMaterial; }
    const Material* GetMaterial() const { return mMaterial; }

    void SetMaterial(Material* material){
        NW_NULL_ASSERT(material);
        mMaterial = material;
    }

    Material* GetMaterialCache() { return mMaterialCache; }
    const Material* GetMaterialCache() const { return mMaterialCache; }

    ShaderProgram* GetShaderProgram() { return this->mShaderProgram.Get(); }
    const ShaderProgram* GetShaderProgram() const { return this->mShaderProgram.Get(); }

    SceneEnvironment& GetSceneEnvironment(){
        return mSceneEnvironment;
    }

    const SceneEnvironment& GetSceneEnvironment() const{
        return mSceneEnvironment;
    }

    NW_DEPRECATED_FUNCTION(void SetMatrixPaletteCount(int count)) { NW_UNUSED_VARIABLE(count); }

    void SetMaterialHash(const MaterialHash& materialHash) { mMaterialHash = materialHash; }

    MaterialHash GetMaterialHash() { return mMaterialHash; }
    const MaterialHash* GetMaterialHash() const { return &mMaterialHash; }

    u32 GetRenderMode() const{
        return mRenderMode;
    }

    void SetRenderMode(u32 renderMode){
        mRenderMode = renderMode;
    }

    bool IsShaderProgramDirty() const{
        return mIsShaderProgramDirty;
    }

    math::VEC3& ModelTranslateOffset(){
        return mModelTranslateOffset;
    }

    const math::VEC3& ModelTranslateOffset() const{
        return mModelTranslateOffset;
    }

    void ActivateVertexAttribute(ResMesh mesh);

    void DeactivateVertexAttribute(ResMesh mesh);

private:
    static const ut::FloatColor NULL_AMBIENT;

    RenderContext(
        nw::os::IAllocator* pAllocator,
        GfxPtr<ShaderProgram> shaderProgram,
        GfxPtr<IMaterialActivator> particleMaterialActivator,
        const SceneEnvironment::Description& description);

    void ActivateShaderProgram();

    void ActivateSceneEnvironment();

    void ActivateMaterial(IMaterialActivator* userMaterialActivator){
        if (!ut::CheckFlag(mRenderMode, RenderContext::RENDERMODE_IGNORE_MATERIAL)){
            IMaterialActivator* materialActivator = NULL;

            if (userMaterialActivator == NULL){
                materialActivator = mMaterial->GetOwnerModel()->GetMaterialActivator();
            }
            else{
                materialActivator = userMaterialActivator;
            }

            NW_NULL_ASSERT(materialActivator);
            materialActivator->Activate(this, mMaterial);
        }
    }

    void ActivateParticleMaterial(){
        if (!ut::CheckFlag(mRenderMode, RenderContext::RENDERMODE_IGNORE_MATERIAL)){
            mParticleMaterialActivator.Get()->Activate(this, mMaterial);
        }
    }

    void ActivateFog();

    void ActivateAmbientLight();

    void ActivateHemiSphereLight();

    void ActivateFragmentLights();

    void ActivateFragmentLight(int index, const FragmentLight* light);

    void ActivateVertexLights();

    void ActivateVertexLight(int index, const VertexLight* light);

    void TransformToViewCoordinate(math::VEC4* out, const math::MTX34* view, const math::VEC4* v);

    u32 ToLutTexture(s32 index){
        NW_ASSERT(0 <= index && index < LOOKUP_TABLE_COUNT);
        return index + GL_LUT_TEXTURE0_DMP;
    }

    inline GLuint ToPrimitiveModeGL(u8 mode, bool isGeometryShaderEnabled);

    GfxPtr<ShaderProgram> mShaderProgram;

    bool mIsVertexAlphaEnabled;
    bool mIsBoneWeightWEnabled;
    bool mIsVertexAttributeDirty;
    bool mIsShaderProgramDirty;

    u32 mRenderMode;

    IRenderTarget* mRenderTarget;
    Model* mModelCache;
    Material* mMaterial;
    Material* mMaterialCache;
    Camera* mCameraCache;

    SceneEnvironment mSceneEnvironment;

    MaterialHash mMaterialHash;
    GfxPtr<IMaterialActivator> mParticleMaterialActivator;

    nw::math::VEC3 mModelTranslateOffset;
};

inline void RenderContext::MaterialHash::ResetMaterialHash(s32 hashMask){
    NW_UNUSED_VARIABLE(hashMask);
    const u32 resetValue = ~(0x0);

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_SHADER_PARAMETER)){
        shaderParameter = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_SHADING_PARAMETER)){
        shadingParameter = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_MATERIAL_COLOR)){
        materialColor = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_RASTERIZATION)){
        rasterization = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_TEXTURE_COORDINATOR)){
        textureCoordinator = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_TEXTURE_MAPPER)){
        textureMapper = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_FRAGMENT_LIGHTING)){
        fragmentLighting = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_FRAGMENT_LIGHTING_TABLE)){
        fragmentLightingTable = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_TEXTURE_COMBINER)){
        textureCombiner = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_FRAGMENT_OPERATION)){
        fragmentOperation = resetValue;
    }

    if (ut::CheckFlag(hashMask, Model::FLAG_BUFFER_ALPHA_TEST)){
        alphaTest = resetValue;
    }
}

inline void RenderContext::DeactivateVertexAttribute(ResMesh mesh){
    NW_NULL_ASSERT(mesh.ref().mDeactivateCommandCache);
    internal::NWUseCmdlist(mesh.ref().mDeactivateCommandCache, mesh.ref().mDeactivateCommandCacheSize);
}

}
}