// Filename: gfx_MaterialActivator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_MaterialActivator.h>
#include <nw/gfx/gfx_MaterialState.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nn/gx.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(MaterialActivator, IMaterialActivator);

MaterialActivator::MaterialActivator(os::IAllocator* allocator): 
    IMaterialActivator(allocator) {}

MaterialActivator::~MaterialActivator() { }

MaterialActivator* MaterialActivator::Create(os::IAllocator* allocator)
{
    void* memory = allocator->Alloc(sizeof(MaterialActivator));

    if (memory == NULL)
    {
        return NULL;
    }
    else{
        return new(memory) MaterialActivator(allocator);
    }
}

void MaterialActivator::Activate(RenderContext* renderContext, const Material* material)
{
    NW_NULL_ASSERT(renderContext);
    const ShaderProgram* shaderProgram = renderContext->GetShaderProgram();
    NW_NULL_ASSERT(shaderProgram);

    const Model* owner = material->GetOwnerModel();
    NW_NULL_ASSERT(owner);

    const SceneEnvironment& sceneEnvironment = renderContext->GetSceneEnvironment();
    bool areFragmentLightsDirty = sceneEnvironment.IsFragmentLightsDirty() || sceneEnvironment.IsAmbientLightDirty();
    bool areVertexLightsDirty = sceneEnvironment.IsVertexLightsDirty() || sceneEnvironment.IsHemiSphereLightDirty();

    RenderContext::MaterialHash materialHash = renderContext->GetMaterialHash();

    const ResMaterial resShaderParameterMaterial = material->GetShaderParameterResMaterial();
    u32 shaderParametersHash = resShaderParameterMaterial.GetShaderParametersHash();
    bool isShaderParametersEnabled = (shaderParametersHash != materialHash.shaderParameter) || (shaderParametersHash == 0x0) || renderContext->IsShaderProgramDirty();
    if (isShaderParametersEnabled)
    {
        internal::MaterialState::ActivateShaderParameter(shaderProgram, resShaderParameterMaterial);
        materialHash.shaderParameter = shaderParametersHash;
    }

    const ResMaterial resShadingParameterResMaterial = material->GetShadingParameterResMaterial();

    internal::MaterialState::ActivateFragmentLightParameters(sceneEnvironment, shaderProgram, resShadingParameterResMaterial);

    u32 shadingParametersHash = resShadingParameterResMaterial.GetShadingParameterHash();
    bool isShadingParametersEnabled = (shadingParametersHash != materialHash.shadingParameter) || shadingParametersHash == 0x0 || areVertexLightsDirty;

    if (isShadingParametersEnabled)
    {
        internal::MaterialState::ActivateShadingParameters(sceneEnvironment, shaderProgram, resShadingParameterResMaterial);
        materialHash.shadingParameter = shadingParametersHash;
    }
    
    const ResMaterial resRasterizationMaterial = material->GetRasterizationResMaterial();
    u32 rasterizationHash = resRasterizationMaterial.GetRasterizationHash();
    bool isRasterizationEnabled = (rasterizationHash != materialHash.rasterization) || rasterizationHash == 0x0;

    if (isRasterizationEnabled)
    {
        internal::MaterialState::ActivateRasterization(resRasterizationMaterial.GetRasterization());
        materialHash.rasterization = rasterizationHash;
    }

    const ResMaterial resTextureCoordinatorsMaterial = material->GetTextureCoordinatorResMaterial();
    u32 textureCoordinatorHash = resTextureCoordinatorsMaterial.GetTextureCoordinatorsHash();
    bool isTextureCoordinatorEnabled = (textureCoordinatorHash != materialHash.textureCoordinator) || textureCoordinatorHash == 0x0;

    if (isTextureCoordinatorEnabled)
    {
        internal::MaterialState::ActivateTextureCoordinators(renderContext, shaderProgram, resTextureCoordinatorsMaterial);
        materialHash.textureCoordinator = textureCoordinatorHash;
    }

    const ResMaterial resTextureMappersMaterial = material->GetTextureMapperResMaterial();
    u32 textureMappersHash = resTextureMappersMaterial.GetTextureMappersHash();
    bool isTextureMappersEnabled = (textureMappersHash != materialHash.textureMapper) || textureMappersHash == 0x0;

    if (isTextureMappersEnabled)
    {
        internal::MaterialState::ActivateTextureMappers(resTextureMappersMaterial);
        materialHash.textureMapper = textureMappersHash;
    }

#if 0
    const ResMaterial resProceduralTextureMapperMaterial = material->GetProceduralTextureMapperResMaterial();
    internal::MaterialState::ActivateProceduralTextureMapper(renderContext, shaderProgram, resProceduralTextureMapperMaterial);
#endif

    bool reflectionEnabled = false;
    bool lightEnabled = ut::CheckFlag(resShaderParameterMaterial.GetFlags(), ResMaterialData::FLAG_FRAGMENTLIGHT_ENABLED);

    if (lightEnabled)
    {
        const ResMaterial resFragmentLightingResMaterial = material->GetFragmentLightingResMaterial();
        const ResFragmentLighting resFragmentLighting = resFragmentLightingResMaterial.GetFragmentShader().GetFragmentLighting();
        reflectionEnabled = ut::CheckFlag(resFragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
        u32 fragmentLightingHash = resFragmentLightingResMaterial.GetFragmentLightingHash();
        bool isFragmentLightingEnabled = (fragmentLightingHash != materialHash.fragmentLighting) || fragmentLightingHash == 0x0 || areFragmentLightsDirty;

        if (isFragmentLightingEnabled)
        {
            internal::MaterialState::ActivateFragmentLighting(sceneEnvironment, resFragmentLighting);
            materialHash.fragmentLighting = fragmentLightingHash;
        }

        const ResMaterial resFramgnetLightingTableResMaterial = material->GetFragmentLightingTableResMaterial();
        u32 fragmentLightingTableHash = resFramgnetLightingTableResMaterial.GetFragmentLightingTableHash();
        bool isFragmentLightingTableEnabled = (fragmentLightingTableHash != materialHash.fragmentLightingTable) || fragmentLightingTableHash == 0x0;

        if (isFragmentLightingTableEnabled)
        {
            const ResFragmentLightingTable resFragmentLightingTable = resFramgnetLightingTableResMaterial.GetFragmentShader().GetFragmentLightingTable();
            internal::MaterialState::ActivateFragmentLightingTable(resFragmentLighting, resFragmentLightingTable);
            materialHash.fragmentLightingTable = fragmentLightingTableHash;
        }
    }

    const ResMaterial resMaterialColorResMaterial = material->GetMaterialColorResMaterial();
    const ResMaterialColor resMaterialColor = resMaterialColorResMaterial.GetMaterialColor();
    u32 materialColorHash = resMaterialColorResMaterial.GetMaterialColorHash();
    bool isMaterialColorEnabled = (materialColorHash != materialHash.materialColor) || materialColorHash == 0x0 || areFragmentLightsDirty;

    Material* cacheMaterial = renderContext->GetMaterialCache();
    if (!isMaterialColorEnabled && cacheMaterial != NULL)
    {
        ResMaterial resCacheMaterial = cacheMaterial->GetFragmentLightingTableResMaterial();
        bool isPreReflectionEnabled = ut::CheckFlag(resCacheMaterial.GetFragmentShader().GetFragmentLighting().GetFlags(), ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
        isMaterialColorEnabled = (reflectionEnabled != isPreReflectionEnabled);
    }
    if (isMaterialColorEnabled)
    {
        internal::MaterialState::ActivateMaterialColor(sceneEnvironment, shaderProgram, resMaterialColor, reflectionEnabled);
        materialHash.materialColor = materialColorHash;
    }

    const ResMaterial resTextureCombinerResMaterial = material->GetTextureCombinerResMaterial();
    u32 textureCombinersHash = resTextureCombinerResMaterial.GetTextureCombinersHash();
    bool isTextureCombinersEnabled = (textureCombinersHash != materialHash.textureCombiner) || textureCombinersHash == 0x0;

    if (isTextureCombinersEnabled || isMaterialColorEnabled)
    {
        const ResFragmentShader resTextureCombinerFragmentShader = resTextureCombinerResMaterial.GetFragmentShader();
        internal::MaterialState::ActivateTextureCombiners(resTextureCombinerFragmentShader, resMaterialColor);
        materialHash.textureCombiner = textureCombinersHash;
    }

    const ResMaterial resAlphaTestResMaterial = material->GetAlphaTestResMaterial();
    u32 alphaTestHash = resAlphaTestResMaterial.GetAlphaTestHash();
    bool isAlphaTestEnabled = (alphaTestHash != materialHash.alphaTest) || alphaTestHash == 0x0;

    if (isAlphaTestEnabled)
    {
        const ResAlphaTest resAlphaTest = resAlphaTestResMaterial.GetFragmentShader().GetAlphaTest();
        internal::MaterialState::ActivateAlphaTest(resAlphaTest);
        materialHash.alphaTest = alphaTestHash;
    }

    const ResMaterial resFragmentOperationResMaterial = material->GetFragmentOperationResMaterial();
    u32 fragmentOperationHash = resFragmentOperationResMaterial.GetFragmentOperationHash();
    bool isFragmentOperationEnabled = (fragmentOperationHash != materialHash.fragmentOperation) || fragmentOperationHash == 0x0;
    
    if (isFragmentOperationEnabled)
    {
        const ResFragmentOperation resFragmentOperation = resFragmentOperationResMaterial.GetFragmentOperation();
        internal::MaterialState::ActivateFragmentOperation(resFragmentOperation);
        materialHash.fragmentOperation = fragmentOperationHash;
    }

    renderContext->SetMaterialHash(materialHash);
}

}
}