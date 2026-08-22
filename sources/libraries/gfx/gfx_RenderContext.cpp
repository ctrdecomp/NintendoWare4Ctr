#include <nw/math/math_Types.h>
#include <nw/types.h>
#include <nw/gfx/gfx_ShaderProgram.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_Material.h>
#include <nw/gfx/gfx_ParticleMaterialActivator.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nw/gfx/gfx_GraphicsDevice.h>
#include <nw/gfx/gfx_IRenderTarget.h>
#include <nw/gfx/gfx_OnScreenBuffer.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/res/gfx_ResLight.h>
#include <nw/gfx/res/gfx_ResFog.h>
#include <nw/gfx/gfx_ShaderUniforms.h>
#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nn/math/inline/math_Types.ipp>

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

namespace{
nw::math::VEC4 GetTranslate(const nw::math::MTX34& transform){
    nw::math::VEC3 translate = transform.GetColumn(3);
    return nw::math::VEC4(translate);
}
}

namespace nw{
namespace gfx{

const ut::FloatColor RenderContext::NULL_AMBIENT = ut::FloatColor();

RenderContext* RenderContext::Builder::Create(nw::os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);

    void* renderContextMemory = allocator->Alloc(sizeof(RenderContext));
    NW_NULL_ASSERT(renderContextMemory);

    if (!renderContextMemory){
        NW_WARNING(false, "Allocation failed\n");
        return NULL;
    }

    void* shaderProgramMemory = allocator->Alloc(sizeof(ShaderProgram));
    NW_NULL_ASSERT(shaderProgramMemory);

    if (!shaderProgramMemory){
        NW_WARNING(false, "Allocation failed\n");
        return NULL;
    }

    GfxPtr<ShaderProgram> shaderProgram(new(shaderProgramMemory) ShaderProgram(allocator));

    if (mParticleMaterialActivator == NULL){
        mParticleMaterialActivator = ParticleMaterialActivator::Create(allocator);
    }

    GfxPtr<IMaterialActivator> particleMaterialActivator(mParticleMaterialActivator);

    SceneEnvironment::Description description;
    description.vertexLights = VertexLightArray(mMaxVertexLights, allocator);
    description.cameras = CameraArray(mMaxCameras, allocator);
    description.fogs = FogArray(mMaxFogs, allocator);
    description.lightSets = LightSetArray(mMaxLightSets, allocator);

    return new(renderContextMemory) RenderContext(allocator, shaderProgram, particleMaterialActivator, description);
}

RenderContext::RenderContext(nw::os::IAllocator* allocator,GfxPtr<ShaderProgram> shaderProgram,GfxPtr<IMaterialActivator> particleMaterialActivator,const SceneEnvironment::Description& description):
    GfxObject(allocator),
    mShaderProgram(shaderProgram),
    mIsVertexAlphaEnabled(false),
    mIsBoneWeightWEnabled(false),
    mIsVertexAttributeDirty(true),
    mIsShaderProgramDirty(true),
    mRenderMode(RENDERMODE_DEFAULT),
    mRenderTarget(NULL),
    mModelCache(NULL),
    mMaterial(NULL),
    mMaterialCache(NULL),
    mCameraCache(NULL),
    mSceneEnvironment(description),
    mParticleMaterialActivator(particleMaterialActivator)
{}

void RenderContext::SetRenderTarget(IRenderTarget* renderTarget, const Viewport& viewport){
    mRenderTarget = renderTarget;

    if (mRenderTarget){
        const FrameBufferObject& fbo = mRenderTarget->GetBufferObject();
        fbo.ActivateBuffer();

        GraphicsDevice::SetRenderBufferSize(mRenderTarget->GetDescription().width, mRenderTarget->GetDescription().height);

        GraphicsDevice::ActivateViewport(viewport);
        GraphicsDevice::SetDepthRange(viewport.GetDepthNear(), viewport.GetDepthFar());
        GraphicsDevice::SetDepthFormat(mRenderTarget->GetDescription().depthFormat);
    }
}

void RenderContext::SetRenderTarget(IRenderTarget* renderTarget){
    f32 width = 0.0f;
    f32 height = 0.0f;
    if (renderTarget){
        width = static_cast<f32>(renderTarget->GetDescription().width);
        height = static_cast<f32>(renderTarget->GetDescription().height);
    }

    const float x = 0.0f;
    const float y = 0.0f;
    const float near = 0.0f;
    const float far = 1.0f;
    this->SetRenderTarget(renderTarget, Viewport(x, y, width, height, near, far));
}

void RenderContext::ResetState(){
    mModelCache = NULL;
    mMaterial = NULL;
    mMaterialCache = NULL;
    mCameraCache = NULL;
    mSceneEnvironment.Reset();

    mIsVertexAttributeDirty = true;

    mShaderProgram.Get()->DeactivateDescription();
    GraphicsDevice::InvalidateAllLookupTables();
    mMaterialHash.ResetMaterialHash(Model::MULTI_FLAG_BUFFER_MATERIAL);
}

void RenderContext::ResetState(s32 resetStateMode, s32 hashMask){
    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_MODEL_CACHE)){
        mModelCache = NULL;
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_MATERIAL)){
        mMaterial = NULL;
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_MATERIAL_CACHE)){
        mMaterialCache = NULL;
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_ACTIVE_CAMERA)){
        mSceneEnvironment.mCamera = NULL;
        mSceneEnvironment.mCameraIndex = -1;
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_CAMERA_CACHE)){
        mCameraCache = NULL;
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_SCENE_ENVIRONMENT)){
        mSceneEnvironment.Reset();
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_SHADER_PROGRAM)){
        mShaderProgram.Get()->DeactivateDescription();
        mIsShaderProgramDirty = true;
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_LOOK_UP_TABLE)){
        GraphicsDevice::InvalidateAllLookupTables();
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_HASH)){
        mMaterialHash.ResetMaterialHash(hashMask);
    }

    if (ut::CheckFlag(resetStateMode, RenderContext::RESETSTATEMODE_VERTEX_ATTRIBUTE)){
        mIsVertexAttributeDirty = true;
    }
}

void RenderContext::ClearBuffer(GLbitfield mask, const ut::FloatColor& color, f32 depth){
    const FrameBufferObject& fbo = mRenderTarget->GetBufferObject();

    {
        fbo.ClearBuffer(mask, color, depth, 0);
    }
}

void RenderContext::SetCameraMatrix(Camera* camera, bool isForce){
    if (mCameraCache != camera || isForce){
        mCameraCache = camera;

        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_VIEWMTX_INDEX, 3, camera->ViewMatrix());
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_PROJMTX_INDEX, 4, camera->ProjectionMatrix());

        internal::NWSetGeometryUniform4fv(VERTEX_SHADER_UNIFORM_VIEWMTX_INDEX, 3, camera->ViewMatrix());
        internal::NWSetGeometryUniform4fv(VERTEX_SHADER_UNIFORM_PROJMTX_INDEX, 4, camera->ProjectionMatrix());
    }
}

void RenderContext::SetModelMatrixForModel(Model* model){
    NW_NULL_ASSERT(model);
    NW_NULL_ASSERT(mShaderProgram);

    mShaderProgram->SetWorldMatrix(math::MTX34::Identity());
    mShaderProgram->SetModelNormalMatrix(model->NormalMatrix());

    mModelCache = model;
}

void RenderContext::SetModelMatrixForSkeletalModel(SkeletalModel* skeletalModel){
    NW_NULL_ASSERT(skeletalModel);
    NW_NULL_ASSERT(mShaderProgram);

    Skeleton* skeleton = skeletalModel->GetSkeleton();
    NW_NULL_ASSERT(skeleton);
    ResSkeleton resSkeleton = skeleton->GetResSkeleton();
    NW_ASSERT(resSkeleton.IsValid());

    if (ut::CheckFlag(resSkeleton.GetFlags(), ResSkeletonData::FLAG_MODEL_COORDINATE)){
        mShaderProgram->SetWorldMatrix(skeletalModel->WorldMatrix());
    }
    else{
        mShaderProgram->SetWorldMatrix(math::MTX34::Identity());
    }

    mShaderProgram->SetModelNormalMatrix(skeletalModel->NormalMatrix());

    mModelCache = skeletalModel;
}

void RenderContext::ActivateContext(IMaterialActivator* userMaterialActivator){
    if (mMaterialCache == mMaterial){
        return;
    }

    ActivateShaderProgram();
    ActivateSceneEnvironment();
    ActivateMaterial(userMaterialActivator);

    mSceneEnvironment.SetAllFlagsDirty(false);

    mMaterialCache = mMaterial;
}

void RenderContext::ActivateParticleContext(){
    if (mIsVertexAttributeDirty){
        internal::ClearVertexAttribute();
        mIsVertexAttributeDirty = false;
    }

    if (mMaterialCache == mMaterial){
        return;
    }

    bool isParticleMaterialEnabled = false;
    if (mMaterialCache != NULL){
        isParticleMaterialEnabled =
            ut::CheckFlag(mMaterialCache->GetOriginal().GetFlags(), ResMaterialData::FLAG_PARTICLE_MATERIAL_ENABLED) &&
            ut::CheckFlag(mMaterial->GetOriginal().GetFlags(), ResMaterialData::FLAG_PARTICLE_MATERIAL_ENABLED);
    }

    if (isParticleMaterialEnabled){
        mIsShaderProgramDirty = false;
        ActivateParticleMaterial();
    }
    else{
        ActivateContext();
    }

    mMaterialCache = mMaterial;
}

void RenderContext::ActivateVertexAttribute(ResMesh mesh){
    NW_NULL_ASSERT(mesh.ref().mActivateCommandCache);

    if (mIsVertexAttributeDirty){
        internal::ClearVertexAttribute();
        mIsVertexAttributeDirty = false;
    }

    internal::NWUseCmdlist(mesh.ref().mActivateCommandCache, mesh.ref().mActivateCommandCacheSize);
    internal::NWUseCmdlist(mesh.ref().mIrScaleCommand, sizeof(mesh.ref().mIrScaleCommand));

    const bool hasVertexAlpha = (mesh.ref().mFlags & ResMesh::FLAG_HAS_VERTEX_ALPHA) ? true : false;
    const bool hasBoneWeightW = (mesh.ref().mFlags & ResMesh::FLAG_HAS_BONE_WEIGHT_W) ? true : false;

    mShaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISVERTA), hasVertexAlpha);
    mShaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISBONEW), hasBoneWeightW);
}

void RenderContext::ActivateShaderProgram(){
    if (!ut::CheckFlag(mRenderMode, RenderContext::RENDERMODE_IGNORE_SHADER)){
        ResShaderProgramDescription description = mMaterial->GetDescription();

        NW_ASSERT(description.IsValid());

        ResShaderProgramDescription previousDescription = mShaderProgram->GetActiveDescription();

        if (previousDescription != description){
            if (previousDescription.IsValid() &&
                previousDescription.GetGeometryShaderIndex() >= 0 &&
                description.GetGeometryShaderIndex() < 0){
                mIsVertexAttributeDirty = true;

                const s32 hashMask = Model::FLAG_BUFFER_SHADER_PARAMETER |
                    Model::FLAG_BUFFER_MATERIAL_COLOR |
                    Model::FLAG_BUFFER_TEXTURE_COORDINATOR |
                    Model::FLAG_BUFFER_SCENE_ENVIRONMENT;
                mMaterialHash.ResetMaterialHash(hashMask);

                mSceneEnvironment.SetAllFlagsDirty(true);
            }

            mShaderProgram.Get()->ActivateDescription(description);

            mIsShaderProgramDirty = true;
        }
        else{
            mIsShaderProgramDirty = false;
        }
    }
}

void RenderContext::ActivateSceneEnvironment(){
    if (!ut::CheckFlag(mRenderMode, RenderContext::RENDERMODE_IGNORE_SCENEENVIRONMENT)){
        NW_NULL_ASSERT(mMaterial);
        const Model* owner = mMaterial->GetOwnerModel();
        NW_NULL_ASSERT(owner);

        ResMaterial resMaterial = mMaterial->GetSceneEnvironmentResMaterial();

        this->mSceneEnvironment.SetActiveLightSet(resMaterial.GetLightSetIndex());

        if (this->mSceneEnvironment.IsFragmentLightsDirty()){
            ActivateFragmentLights();
        }

        if (this->mSceneEnvironment.IsHemiSphereLightDirty()){
            ActivateHemiSphereLight();
        }
        if (this->mSceneEnvironment.IsVertexLightsDirty()){
            ActivateVertexLights();
        }
        this->mSceneEnvironment.SetActiveFog(resMaterial.GetFogIndex());

        if (this->mSceneEnvironment.IsFogDirty()){
            ActivateFog();
        }
    }
}

void RenderContext::ActivateFog(){
    if (mSceneEnvironment.GetActiveFog() != NULL){
        NW_ASSERT(mSceneEnvironment.GetActiveFog()->GetResFog().IsValid());
        NW_ASSERT(mSceneEnvironment.GetActiveFog()->GetResFog().GetFogSampler().IsValid());

        ResFog resFog = mSceneEnvironment.GetActiveFog()->GetResFog();

        ResImageLookupTable lut = resFog.GetFogSampler();

        enum{
            REG_FOG_ZFLIP = 0xe0,
            REG_FOG_COLOR = 0xe1,
            REG_FOG_ZFLIP_SHIFT = 16
        };

        ut::Color8 color8 = resFog.GetColor();

        const u32 HEADER_FOG_ZFLIP = internal::MakeCommandHeader(REG_FOG_ZFLIP, 1, false, 0x4);
        const u32 HEADER_FOG_COLOR = internal::MakeCommandHeader(REG_FOG_COLOR, 1, false, 0xf);

        u32 FOG_COMMAND[] ={
            (resFog.IsZFlip() ? (0x1 << REG_FOG_ZFLIP_SHIFT) : 0x0),
            HEADER_FOG_ZFLIP,
            color8.r | color8.g << 8 | color8.b << 16,
            HEADER_FOG_COLOR
        };

        internal::NWUseCmdlist<sizeof(FOG_COMMAND)>(&FOG_COMMAND[0]);

        GraphicsDevice::ActivateLookupTable(lut, GraphicsDevice::LUT_TARGET_FOG);
    }
}

void RenderContext::ActivateHemiSphereLight(){
    const HemiSphereLight* hemiSphereLight = mSceneEnvironment.GetHemiSphereLight();
    if (hemiSphereLight &&
        ut::CheckFlag(mShaderProgram->GetActiveDescription().GetFlags(), ResShaderProgramDescription::FLAG_IS_SUPPORTING_HEMISPHERE_LIGHTING)){
        ResHemiSphereLight resLight = hemiSphereLight->GetResHemiSphereLight();
        NW_ASSERT(resLight.IsValid());

        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_HSLGCOL_INDEX, 1, resLight.GetGroundColor());
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_HSLSCOL_INDEX, 1, resLight.GetSkyColor());

        math::VEC3 direction(resLight.GetDirection());
        if (ut::CheckFlag(resLight.GetFlags(), ResHemiSphereLightData::FLAG_IS_INHERITING_DIRECTION_ROTATE) &&
            hemiSphereLight->GetParent() != NULL){
            math::VEC3TransformNormal(&direction, &hemiSphereLight->TrackbackWorldMatrix(), &resLight.GetDirection());
        }

        const Camera* camera = GetActiveCamera();
        NW_NULL_ASSERT(camera);

        const math::MTX34& viewMatrix = camera->ViewMatrix();

        math::VEC3TransformNormal(&direction, &viewMatrix, &direction);
        math::VEC4 directionAndLerp(direction.x, direction.y, direction.z, resLight.GetLerpFactor());

        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_HSLSDIR_INDEX, 1, directionAndLerp);
    }
}

void RenderContext::ActivateFragmentLights(){
    GraphicsDevice::ResetFragmentLightEnabled();
    int lightCount = mSceneEnvironment.GetFragmentLightCount();

    for (int i = 0; i < lightCount; ++i){
        const FragmentLight* light = mSceneEnvironment.GetFragmentLight(i);
        ActivateFragmentLight(i, light);
    }

    GraphicsDevice::ActivateFragmentLightEnabled();
}

void RenderContext::ActivateFragmentLight(int index, const FragmentLight* light){
    ResFragmentLight resLight = light->GetResFragmentLight();
    NW_ASSERT(resLight.IsValid());

    s32 lightKind = resLight.GetLightKind();
    NW_ASSERT(lightKind < ResFragmentLight::KIND_COUNT);

    Camera* camera = GetActiveCamera();
    NW_NULL_ASSERT(camera);

    const math::MTX34& viewMatrix = camera->ViewMatrix();

    if (lightKind == ResFragmentLight::KIND_DIRECTIONAL){
        nw::math::VEC4 direction(light->Direction());
        direction.w = 0.0f;

        TransformToViewCoordinate(&direction, &viewMatrix, &direction);

        GraphicsDevice::ActivateFragmentLightPosition(index, -direction);
        GraphicsDevice::SetFragmentLightPositionW(index, (direction.w == 0.0f));

        GraphicsDevice::SetFragmentLightSpotEnabled(index, false);
        GraphicsDevice::SetFragmentLightDistanceAttnEnabled(index, false);
    }
    else if (lightKind == ResFragmentLight::KIND_POINT){
        GraphicsDevice::SetFragmentLightSpotEnabled(index, false);

        if (ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_DISTANCE_ATTENUATION_ENABLED) &&
            resLight.GetDistanceSampler().IsValid()){
            GraphicsDevice::SetFragmentLightDistanceAttnEnabled(index, true);
            GraphicsDevice::ActivateFragmentLightDistanceAttnTable(index, resLight.GetDistanceSampler().Dereference());

            GraphicsDevice::ActivateFragmentLightDistanceAttnScaleBias(
                index,
                resLight.ref().mDistanceAttenuationScale,
                resLight.ref().mDistanceAttenuationBias);
        }
        else{
            GraphicsDevice::SetFragmentLightDistanceAttnEnabled(index, false);
        }

        nw::math::VEC4 position = GetTranslate(light->WorldMatrix());
        position.w = 1.0f;
        TransformToViewCoordinate(&position, &viewMatrix, &position);

        GraphicsDevice::ActivateFragmentLightPosition(index, position);
        GraphicsDevice::SetFragmentLightPositionW(index, (position.w == 0.0f));
    }
    else if (lightKind == ResFragmentLight::KIND_SPOT){
        NW_ASSERTMSG(resLight.GetAngleSampler().IsValid(), "The spot light must contain angle sampler(lookuptable).");
        NW_ASSERTMSG(resLight.GetAngleSampler().GetSampler().IsValid(), "The spot light must contain angle sampler(lookuptable).");

        if (resLight.GetAngleSampler().IsValid()){
            GraphicsDevice::SetFragmentLightSpotEnabled(index, true);
            GraphicsDevice::ActivateFragmentLightSpotTable(index, resLight.GetAngleSampler().GetSampler().Dereference());
            GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_SP, resLight.GetAngleSampler().GetSampler().Dereference().IsAbs());
            GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_SP, resLight.GetAngleSampler().GetInput());
            GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_SP, resLight.GetAngleSampler().GetScale());
        }

        if (ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_DISTANCE_ATTENUATION_ENABLED) && resLight.GetDistanceSampler().IsValid()){
            GraphicsDevice::SetFragmentLightDistanceAttnEnabled(index, true);
            GraphicsDevice::ActivateFragmentLightDistanceAttnTable(index, resLight.GetDistanceSampler().Dereference());

            GraphicsDevice::ActivateFragmentLightDistanceAttnScaleBias(
                index,
                resLight.ref().mDistanceAttenuationScale,
                resLight.ref().mDistanceAttenuationBias);
        }
        else{
            GraphicsDevice::SetFragmentLightDistanceAttnEnabled(index, false);
        }

        nw::math::VEC4 direction(light->Direction());
        direction.w = 0.0f;

        TransformToViewCoordinate(&direction, &viewMatrix, &direction);

        nw::math::VEC4 position = GetTranslate(light->WorldMatrix());
        position.w = 1.0f;
        TransformToViewCoordinate(&position, &viewMatrix, &position);

        math::VEC3 spotDirection(direction);
        GraphicsDevice::ActivateFragmentLightPosition(index, position, spotDirection);
        GraphicsDevice::SetFragmentLightPositionW(index, (position.w == 0.0f));
    }
}

void RenderContext::ActivateVertexLights(){
    int size = mSceneEnvironment.GetVertexLightCount();
    if (size > 0){
        const ShaderProgram* shaderProgram = GetShaderProgram();

        if (shaderProgram->GetActiveDescription().GetMaxVertexLightCount() > 0){
            shaderProgram->SetVertexUniformInt(NW_GFX_VERTEX_UNIFORM(LIGHTCT), ut::Max<s32>(size - 1, 0), 0, 1);
        }

        for (int i = 0; i < size; i++){
            ActivateVertexLight(i, mSceneEnvironment.GetVertexLight(i));
        }
    }
}

void RenderContext::ActivateVertexLight(int index, const VertexLight* light){
    if (mShaderProgram->GetActiveDescription().GetMaxVertexLightCount() > index){
        const float LIGHT_INFINITY = 0.0f;
        const float LIGHT_FINITY = 1.0f;

        s32 endUniform = mShaderProgram->GetActiveDescription().GetVertexLightEndUniform();
        ResVertexLight resLight = light->GetResVertexLight();
        NW_ASSERT(resLight.IsValid());

        Camera* camera = GetActiveCamera();
        NW_NULL_ASSERT(camera);

        const math::MTX34& viewMatrix = camera->ViewMatrix();

        mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_AMBIENT, resLight.GetAmbient());
        mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_DIFFUSE, resLight.GetDiffuse());

        if (resLight.GetLightKind() == ResVertexLight::KIND_DIRECTIONAL){

            math::VEC4 position(light->Direction());
            position.w = LIGHT_INFINITY;

            TransformToViewCoordinate(&position, &viewMatrix, &position);
            mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_POSITION, -position);
        }
        else{
            math::VEC4 position = GetTranslate(light->WorldMatrix());
            position.w = LIGHT_FINITY;

            TransformToViewCoordinate(&position, &viewMatrix, &position);
            mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_POSITION, position);

            mShaderProgram->SetUniversal(
                endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_DISTANCE_ATTENUATION,
                resLight.GetDistanceAttenuationAndEnabled());

            if (resLight.GetLightKind() == ResVertexLight::KIND_SPOT){
                math::VEC4 spotDirection(light->Direction());
                spotDirection.w = 0.0f;

                TransformToViewCoordinate(&spotDirection, &viewMatrix, &spotDirection);

                math::VEC4 spotFactor(resLight.GetSpotFactor().x, resLight.GetSpotFactor().y, 0.0f, 0.0f);
                if (resLight.GetSpotFactor().y > 0.0f && resLight.GetSpotFactor().y < math::PI){
                    spotFactor.y = nw::math::CosRad(resLight.GetSpotFactor().y);
                    spotDirection.w = 1.0f;
                }
                else if (resLight.GetSpotFactor().y == 0.0f){
                    const float EPSILON = 1e-5f;
                    const float GREATER_THAN_ONE = 1.0f + EPSILON;
                    spotFactor.y = GREATER_THAN_ONE;
                    spotDirection.w = 1.0f;
                }
                else{
                    spotDirection.w = 0.0f;
                }

                mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_SPOT_DIRECTION, spotDirection);
                mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_SPOT_FACTOR, spotFactor);
            }
            else{
                mShaderProgram->SetUniversal(endUniform - (index + 1) * VERTEX_LIGHT_SET_COUNT + VERTEX_LIGHT_SPOT_DIRECTION, math::VEC4::Zero());
            }
        }
    }
}

void RenderContext::TransformToViewCoordinate(math::VEC4* out, const math::MTX34* __restrict view, const math::VEC4* v){
    f32 vx = v->x;
    f32 vy = v->y;
    f32 vz = v->z;
    f32 vw = v->w;

    out->x = view->matrix[0][0] * vx + view->matrix[0][1] * vy + view->matrix[0][2] * vz + view->matrix[0][3] * vw;
    out->y = view->matrix[1][0] * vx + view->matrix[1][1] * vy + view->matrix[1][2] * vz + view->matrix[1][3] * vw;
    out->z = view->matrix[2][0] * vx + view->matrix[2][1] * vy + view->matrix[2][2] * vz + view->matrix[2][3] * vw;
    out->w = vw;
}

void RenderContext::RenderPrimitive(ResPrimitive primitive){
    NW_ASSERT(primitive.IsValid());

    mShaderProgram->FlushUniform();

    if ((primitive.GetFlags() & ResPrimitive::FLAG_COMMAND_HAS_BEEN_SETUP) == 0){
        ResShaderProgramDescription description = mShaderProgram->GetActiveDescription();
        bool hasGeometryShader = (description.GetGeometryShaderObject() > 0);

        primitive.SetupDrawCommand(hasGeometryShader);
    }

    ResIndexStreamArray indexStreams = primitive.GetIndexStreams();
    ResIndexStreamArray::iterator end = indexStreams.end();
    for (ResIndexStreamArray::iterator stream = indexStreams.begin(); stream != end; ++stream){
        ResIndexStream indexStream = *stream;

        if (!indexStream.IsVisible()){
            continue;
        }

        NW_NULL_ASSERT(indexStream.ref().mCommandCache);
        NW_ASSERT(indexStream.ref().mCommandCacheSize > 0);
        internal::NWUseCmdlist(indexStream.ref().mCommandCache, indexStream.ref().mCommandCacheSize);
    }
}

}
}