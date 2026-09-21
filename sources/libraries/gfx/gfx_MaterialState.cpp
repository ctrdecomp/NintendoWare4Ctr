// Filename: gfx_MaterialState.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_MaterialState.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nw/math/inlines/math_Matrix44.ipp>

namespace nw{
namespace gfx{
namespace internal{

nn::math::MTX34* CreateMatrixForLinearShadowMapTexture(nn::math::MTX34* pOut, f32 coeff, f32 nearp, f32 farp)
{
    f32 (*const m)[4] = pOut->matrix;
    f32 scaleZ = 1.0f / (farp - nearp);

    m[0][0] = 0.5f * coeff * scaleZ;
    m[0][1] = 0.0f;
    m[0][2] = -0.5f * scaleZ;
    m[0][3] = 0.0f;

    m[1][0] = 0.0f;
    m[1][1] = 0.5f * coeff * scaleZ;
    m[1][2] = -0.5f * scaleZ;
    m[1][3] = 0.0f;

    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -scaleZ;
    m[2][3] = 0.0f;

    return pOut;
}

math::MTX44* MaterialState::SetupTextureMatrix(math::MTX44* textureMatrix,ResTextureCoordinator::MappingMatrixMode mode,float scaleS, float scaleT,float rotate,float translateS, float translateT)
{
    NW_NULL_ASSERT(textureMatrix);

    switch (mode)
    {
    case ResTextureCoordinator::MAPPINGMATRIXMODE_MAYA:
        math::MTX44TextureMatrixForMaya(textureMatrix, scaleS, scaleT, rotate, translateS, translateT);
        break;
    case ResTextureCoordinator::MAPPINGMATRIXMODE_SOFTIMAGE:
        math::MTX44TextureMatrixForSoftimage(textureMatrix, scaleS, scaleT, rotate, translateS, translateT);
        break;
    case ResTextureCoordinator::MAPPINGMATRIXMODE_3DSMAX:
        math::MTX44TextureMatrixForMax(textureMatrix, scaleS, scaleT, rotate, translateS, translateT);
        break;
    default:
        math::MTX44Identity(textureMatrix);
        break;
    }

    return textureMatrix;
}

void MaterialState::ActivateFragmentLightingTable(const ResFragmentLighting fragmentLighting, const ResFragmentLightingTable fragmentLightingTable)
{
    s32 flags = fragmentLighting.GetFlags();
    bool isDistribution0Enbaled = ut::CheckFlag(flags, ResFragmentLightingData::FLAG_DISTRIBUTION0_ENABLED);
    bool isDistribution1Enbaled = ut::CheckFlag(flags, ResFragmentLightingData::FLAG_DISTRIBUTION1_ENABLED);
    bool isReflectionEnabled = ut::CheckFlag(flags, ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
    bool isFresnelEnabled = (fragmentLighting.GetFresnelConfig() != ResFragmentLighting::CONFIG_NO_FRESNEL);

    u32 absLutInput = 0;
    u32 lutInput = 0;
    u32 lutScale = 0;

    ResLightingLookupTable lightingLookupTable;
    ResImageLookupTable lookupTable;

    lightingLookupTable = fragmentLightingTable.GetReflectanceRSampler();
    if (lightingLookupTable.IsValid() && isReflectionEnabled)
    {
        NW_NULL_ASSERT(lightingLookupTable.GetSampler().IsValid());

        lookupTable = lightingLookupTable.GetSampler().Dereference();

        GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_RR, lookupTable.IsAbs());
        GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_RR, lightingLookupTable.GetInput());
        GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_RR, lightingLookupTable.GetScale());

        GraphicsDevice::ActivateLookupTable(lookupTable, GraphicsDevice::LUT_TARGET_RR);
    }

    lightingLookupTable = fragmentLightingTable.GetReflectanceGSampler();
    if (lightingLookupTable.IsValid() && isReflectionEnabled)
    {
        NW_NULL_ASSERT(lightingLookupTable.GetSampler().IsValid());

        lookupTable = lightingLookupTable.GetSampler().Dereference();

        GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_RG, lookupTable.IsAbs());
        GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_RG, lightingLookupTable.GetInput());
        GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_RG, lightingLookupTable.GetScale());

        GraphicsDevice::ActivateLookupTable(lookupTable, GraphicsDevice::LUT_TARGET_RG);
    }

    lightingLookupTable = fragmentLightingTable.GetReflectanceBSampler();
    if (lightingLookupTable.IsValid() && isReflectionEnabled)
    {
        NW_NULL_ASSERT(lightingLookupTable.GetSampler().IsValid());

        lookupTable = lightingLookupTable.GetSampler().Dereference();

        GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_RB, lookupTable.IsAbs());
        GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_RB, lightingLookupTable.GetInput());
        GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_RB, lightingLookupTable.GetScale());

        GraphicsDevice::ActivateLookupTable(lookupTable, GraphicsDevice::LUT_TARGET_RB);
    }

    lightingLookupTable = fragmentLightingTable.GetDistribution0Sampler();
    if (lightingLookupTable.IsValid() && isDistribution0Enbaled)
    {
        NW_NULL_ASSERT(lightingLookupTable.GetSampler().IsValid());

        lookupTable = lightingLookupTable.GetSampler().Dereference();

        GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_D0, lookupTable.IsAbs());
        GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_D0, lightingLookupTable.GetInput());
        GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_D0, lightingLookupTable.GetScale());

        GraphicsDevice::ActivateLookupTable(lookupTable, GraphicsDevice::LUT_TARGET_D0);
    }

    lightingLookupTable = fragmentLightingTable.GetDistribution1Sampler();
    if (lightingLookupTable.IsValid() && isDistribution1Enbaled)
    {
        NW_NULL_ASSERT(lightingLookupTable.GetSampler().IsValid());

        lookupTable = lightingLookupTable.GetSampler().Dereference();

        GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_D1, lookupTable.IsAbs());
        GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_D1, lightingLookupTable.GetInput());
        GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_D1, lightingLookupTable.GetScale());

        GraphicsDevice::ActivateLookupTable(lookupTable, GraphicsDevice::LUT_TARGET_D1);
    }

    lightingLookupTable = fragmentLightingTable.GetFresnelSampler();
    if (lightingLookupTable.IsValid() && isFresnelEnabled)
    {
        NW_NULL_ASSERT(lightingLookupTable.GetSampler().IsValid());

        lookupTable = lightingLookupTable.GetSampler().Dereference();

        GraphicsDevice::SetLutIsAbs(GraphicsDevice::LUT_TARGET_FR, lookupTable.IsAbs());
        GraphicsDevice::SetLutInput(GraphicsDevice::LUT_TARGET_FR, lightingLookupTable.GetInput());
        GraphicsDevice::SetLutScale(GraphicsDevice::LUT_TARGET_FR, lightingLookupTable.GetScale());

        GraphicsDevice::ActivateLookupTable(lookupTable, GraphicsDevice::LUT_TARGET_FR);
    }

    GraphicsDevice::ActivateLutParameters();
}

void MaterialState::ActivateTextureCoordinators(RenderContext* renderContext, const ShaderProgram* shaderProgram, const ResMaterial texCoordMaterial)
{
    enum
    {
        TEXCOORD2_SHIFT = 13,
        TEXCOORD3_SHIFT = 8
    };

    const u32 TEXCOORD_SETTING[] ={
        0,
        (1 << TEXCOORD2_SHIFT),
        (1 << TEXCOORD2_SHIFT) | (1 << TEXCOORD3_SHIFT),
        (1 << TEXCOORD2_SHIFT) | (2 << TEXCOORD3_SHIFT),
        (1 << TEXCOORD3_SHIFT),
        (2 << TEXCOORD3_SHIFT)
    };

    u32 textureSetting = 0;

    NW_ASSERT(texCoordMaterial.GetTextureCoordinateConfig() < ResMaterial::CONFIG_NUM);
    textureSetting = TEXCOORD_SETTING[texCoordMaterial.GetTextureCoordinateConfig()];

    const u32 HEADER = internal::MakeCommandHeader(PICA_REG_TEXTURE_FUNC, 1, false, 0xa);

    u32 TEXTYPE_COMMAND[] ={
        textureSetting | (0x1 << 12),
        HEADER
    };

    internal::NWUseCmdlist<sizeof(TEXTYPE_COMMAND)>(&TEXTYPE_COMMAND[0]);

    const int TEXTURE_UNIT_COUNT = 3;
    GLfloat textureMappings[TEXTURE_UNIT_COUNT] = { 0.0f, 0.0f, 0.0f };

    int coordinatorsCount = texCoordMaterial.GetActiveTextureCoordinatorsCount();

    for (int unit = 0; unit < TEXTURE_UNIT_COUNT; ++unit)
    {
        shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(UVMAP0) + unit, false);

        if (unit == 1 || unit == 2)
        {
            if (unit < coordinatorsCount)
            {
                shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISTEX1) + unit - 1, true);
            }
            else
            {
                shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISTEX1) + unit - 1, false);
            }
        }
    }

    math::VEC4 projectionTranslate;
    bool isProjectionEnabled = false;

    for (int i = 0; i < coordinatorsCount; ++i)
    {
        ResTextureCoordinator coordinator = texCoordMaterial.GetTextureCoordinators(i);
        NW_ASSERT(coordinator.IsValid());

        if (!coordinator.IsEnabled())
        {
            continue;
        }

        math::MTX34 texMtx34;

        if (coordinator.IsDirty())
        {
            math::MTX44 texMtx;
            SetupTextureMatrix(&texMtx,coordinator.GetMatrixMode(),coordinator.GetScale().x,coordinator.GetScale().y,coordinator.GetRotate(),coordinator.GetTranslate().x,coordinator.GetTranslate().y);

            texMtx34.v[0] = texMtx.v[0];
            texMtx34.v[1] = texMtx.v[1];
            texMtx34.v[2] = texMtx.v[2];

            coordinator.GetTextureMatrix().v[0] = texMtx34.v[0];
            coordinator.GetTextureMatrix().v[1] = texMtx34.v[1];
            coordinator.GetTextureMatrix().v[2] = texMtx34.v[2];
            coordinator.SetDirty(false);
        }
        else
        {
            texMtx34 = coordinator.GetTextureMatrix();
        }

        NW_ASSERTMSG(i < 1 || coordinator.GetMappingMethod() != ResTextureCoordinator::MAPPINGMETHOD_CAMERA_CUBE_ENV, "Cube Map is supported just on Texture Unit 0.");

        switch (coordinator.GetMappingMethod())
        {
        case ResTextureCoordinator::MAPPINGMETHOD_UV_COORDINATE:{
                textureMappings[i] = static_cast<GLfloat>(coordinator.GetSourceCoordinate());
                shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(UVMAP0) + i, true);
            }
            break;

        case ResTextureCoordinator::MAPPINGMETHOD_PROJECTION:{
                NW_ASSERTMSG(i == 0 || i == 1, "Projection mapping can use coordinator0 or coordinator1.");

                SceneEnvironment& sceneEnvironment = renderContext->GetSceneEnvironment();
                int cameraIndex = coordinator.GetReferenceCamera();
                const Camera* camera = ((cameraIndex < 0) ? renderContext->GetActiveCamera() : sceneEnvironment.GetCamera(cameraIndex));

                NW_NULL_ASSERT(camera);

                const math::MTX34& referenceViewMatrix = camera->ViewMatrix();

                if (i == 0)
                {
                    projectionTranslate.x = texMtx34.matrix[0][3];
                    projectionTranslate.y = texMtx34.matrix[1][3];
                }
                else
                {
                    projectionTranslate.z = texMtx34.matrix[0][3];
                    projectionTranslate.w = texMtx34.matrix[1][3];
                }

                texMtx34.matrix[0][3] = 0.0f;
                texMtx34.matrix[1][3] = 0.0f;

                math::MTX34Mult(&texMtx34, &texMtx34, &camera->TextureProjectionMatrix());
                math::MTX34Mult(&texMtx34, &texMtx34, &referenceViewMatrix);

                isProjectionEnabled = true;
            }
            break;

        case ResTextureCoordinator::MAPPINGMETHOD_SHADOW:{
                NW_ASSERTMSG(i == 0, "Projection mapping can use coordinator0.");

                SceneEnvironment& sceneEnvironment = renderContext->GetSceneEnvironment();
                int cameraIndex = coordinator.GetReferenceCamera();
                const Camera* camera = ((cameraIndex < 0) ? renderContext->GetActiveCamera() : sceneEnvironment.GetCamera(cameraIndex));

                NW_NULL_ASSERT(camera);

                projectionTranslate.x = texMtx34.matrix[0][3];
                projectionTranslate.y = texMtx34.matrix[1][3];

                texMtx34.matrix[0][3] = 0.0f;
                texMtx34.matrix[1][3] = 0.0f;

                f32 near = camera->GetNear();
                f32 far = camera->GetFar();
                ResCameraProjectionUpdater updater = camera->GetProjectionUpdater()->GetResource();

                math::MTX34 projection = camera->TextureProjectionMatrix();
                switch (updater.ref().typeInfo)
                {
                case ResPerspectiveProjectionUpdater::TYPE_INFO:{
                        f32 scaleFactor = 1.0f / (far - near);
                        math::MTX34 scaleMatrix = math::MTX34::Identity();
                        math::VEC3 scale(scaleFactor, scaleFactor, scaleFactor);
                        math::MTX34Scale(&scaleMatrix, &scale);
                        math::MTX34Mult(&texMtx34, &texMtx34, &scaleMatrix);
                    }
                    break;

                case ResFrustumProjectionUpdater::TYPE_INFO:{
                        f32 scaleFactor = 1.0f / (far - near);
                        math::MTX34 scaleMatrix = math::MTX34::Identity();
                        math::VEC3 scale(scaleFactor, scaleFactor, scaleFactor);
                        math::MTX34Scale(&scaleMatrix, &scale);
                        math::MTX34Mult(&texMtx34, &texMtx34, &scaleMatrix);
                    }
                    break;

                case ResOrthoProjectionUpdater::TYPE_INFO:{
                        f32 scaleFactor = -1.0f / (far - near);
                        projection.matrix[2][2] = scaleFactor;
                        projection.matrix[2][3] = near * scaleFactor;
                    }
                    break;

                default:
                    NW_FATAL_ERROR("Unsupported camera updater type.\n");
                    break;
                }

                math::MTX34Mult(&texMtx34, &texMtx34, &projection);

                const math::MTX34& referenceViewMatrix = camera->ViewMatrix();
                math::MTX34Mult(&texMtx34, &texMtx34, &referenceViewMatrix);

                isProjectionEnabled = true;
            }
            break;

        case ResTextureCoordinator::MAPPINGMETHOD_CAMERA_CUBE_ENV:{
                textureMappings[i] = static_cast<GLfloat>(TEXTURE_COORDINATE_COUNT - 1) + static_cast<GLfloat>(coordinator.GetMappingMethod());

                const Camera* camera = renderContext->GetActiveCamera();
                if (camera)
                {
                    texMtx34 = camera->InverseViewMatrix();
                }
            }
            break;

        case ResTextureCoordinator::MAPPINGMETHOD_CAMERA_SPHERE_ENV:{
                textureMappings[i] = static_cast<GLfloat>(TEXTURE_COORDINATE_COUNT - 1) + static_cast<GLfloat>(coordinator.GetMappingMethod());
            }
            break;

        default:
            NW_FATAL_ERROR("Invalid texture coordinator type.");
            break;
        }

        int matrixCount = 3;

        if (i == 2)
        {
            matrixCount = 2;
        }

        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_TEXMTX0_INDEX + 3 * i, matrixCount, texMtx34);
    }

    if (isProjectionEnabled)
    {
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_TEXTRAN_INDEX, 1, projectionTranslate);
    }

    internal::NWSetVertexUniform3fv(VERTEX_SHADER_UNIFORM_TEXCMAP_INDEX, 1, textureMappings);
}

void MaterialState::ActivateParticleTextureCoordinators(RenderContext* renderContext, const ShaderProgram* shaderProgram, const ResMaterial texCoordMaterial)
{
    NW_UNUSED_VARIABLE(renderContext);

    enum
    {
        TEXCOORD2_SHIFT = 13,
        TEXCOORD3_SHIFT = 8
    };

    const u32 TEXCOORD_SETTING[] ={
        0,
        (1 << TEXCOORD2_SHIFT),
        (1 << TEXCOORD2_SHIFT) | (1 << TEXCOORD3_SHIFT),
        (1 << TEXCOORD2_SHIFT) | (2 << TEXCOORD3_SHIFT),
        (1 << TEXCOORD3_SHIFT),
        (2 << TEXCOORD3_SHIFT)
    };

    u32 textureSetting = 0;

    NW_ASSERT(texCoordMaterial.GetTextureCoordinateConfig() < ResMaterial::CONFIG_NUM);
    textureSetting = TEXCOORD_SETTING[texCoordMaterial.GetTextureCoordinateConfig()];

    const u32 HEADER = internal::MakeCommandHeader(PICA_REG_TEXTURE_FUNC, 1, false, 0xa);

    u32 TEXTYPE_COMMAND[] ={
        textureSetting | (0x1 << 12),
        HEADER
    };

    internal::NWUseCmdlist<sizeof(TEXTYPE_COMMAND)>(&TEXTYPE_COMMAND[0]);

    const int TEXTURE_UNIT_COUNT = 3;
    GLfloat textureMappings[TEXTURE_UNIT_COUNT] = { 0.0f, 0.0f, 0.0f };

    shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(UVMAP0), false);

    int coordinatorsCount = texCoordMaterial.GetActiveTextureCoordinatorsCount();

    ResTextureCoordinator coordinator = texCoordMaterial.GetTextureCoordinators(0);
    NW_ASSERT(coordinator.IsValid());

    if (coordinator.IsEnabled())
    {
        math::MTX34 texMtx34;

        if (coordinator.IsDirty())
        {
            math::MTX44 texMtx;
            SetupTextureMatrix(
                &texMtx,
                coordinator.GetMatrixMode(),
                coordinator.GetScale().x,
                coordinator.GetScale().y,
                coordinator.GetRotate(),
                coordinator.GetTranslate().x,
                coordinator.GetTranslate().y);

            texMtx34.v[0] = texMtx.v[0];
            texMtx34.v[1] = texMtx.v[1];
            texMtx34.v[2] = texMtx.v[2];

            coordinator.GetTextureMatrix().v[0] = texMtx34.v[0];
            coordinator.GetTextureMatrix().v[1] = texMtx34.v[1];
            coordinator.GetTextureMatrix().v[2] = texMtx34.v[2];
            coordinator.SetDirty(false);
        }
        else{
            texMtx34 = coordinator.GetTextureMatrix();
        }

        NW_ASSERT(coordinator.GetMappingMethod() == ResTextureCoordinator::MAPPINGMETHOD_UV_COORDINATE);
        {
            textureMappings[0] = static_cast<GLfloat>(coordinator.GetSourceCoordinate());
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(UVMAP0), true);
        }

        int matrixCount = 3;
        internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_TEXMTX0_INDEX, matrixCount, texMtx34);
    }

    internal::NWSetVertexUniform3fv(VERTEX_SHADER_UNIFORM_TEXCMAP_INDEX, 1, textureMappings);
}

}
}
}