// Filename: gfx_SimpleMaterialActivator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_SimpleMaterialActivator.h>
#include <nw/gfx/gfx_MaterialState.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nn/gx.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SimpleMaterialActivator, IMaterialActivator);

SimpleMaterialActivator::SimpleMaterialActivator(nw::os::IAllocator* allocator): 
    IMaterialActivator(allocator) {}

SimpleMaterialActivator::~SimpleMaterialActivator() {}

SimpleMaterialActivator* SimpleMaterialActivator::Create(os::IAllocator* allocator)
{
    void* memory = allocator->Alloc(sizeof(SimpleMaterialActivator));
    
    if (memory == NULL)
    {
        return NULL;
    }
    else{
        return new(memory) SimpleMaterialActivator(allocator);
    }
}

void SimpleMaterialActivator::Activate(RenderContext* renderContext, const Material* material)
{
    NW_NULL_ASSERT(renderContext);

    const SceneEnvironment& sceneEnvironment = renderContext->GetSceneEnvironment();
    bool areFragmentLightsDirty = sceneEnvironment.IsFragmentLightsDirty() || sceneEnvironment.IsAmbientLightDirty();
    bool areVertexLightsDirty = sceneEnvironment.IsVertexLightsDirty() || sceneEnvironment.IsHemiSphereLightDirty();

    const Material* cacheMaterial = renderContext->GetMaterialCache();
    if ( ( cacheMaterial != NULL) && 
         ( cacheMaterial->GetBufferCount() == 0) &&
         ( cacheMaterial->GetOriginal() == material->GetOriginal() ) &&
         ( !areFragmentLightsDirty ) &&
         ( !areVertexLightsDirty ) )
         {
        return;
    }

    const ShaderProgram* shaderProgram = renderContext->GetShaderProgram();
    NW_NULL_ASSERT(shaderProgram);

    ResMaterial original = material->GetOriginal();
    const Model* owner = material->GetOwnerModel();
    NW_NULL_ASSERT(owner);

    RenderContext::MaterialHash materialHash = renderContext->GetMaterialHash();

    ResMaterial resShaderParameterMaterial = original;
    u32 shaderParametersHash = resShaderParameterMaterial.GetShaderParametersHash();
    bool isShaderParametersEnabled =
        (shaderParametersHash == 0x0) ||
        (shaderParametersHash != materialHash.shaderParameter) ||
        renderContext->IsShaderProgramDirty();
    if (isShaderParametersEnabled)
    {
        internal::MaterialState::ActivateShaderParameter(shaderProgram, resShaderParameterMaterial);
        materialHash.shaderParameter = shaderParametersHash;
    }

    ResMaterial resShadingParametersMaterial = original;

    internal::MaterialState::ActivateFragmentLightParameters(sceneEnvironment, shaderProgram, resShadingParametersMaterial);

    u32 shadingParametersHash = resShadingParametersMaterial.GetShadingParameterHash();
    bool isShadingParametersEnabled = (shadingParametersHash == 0x0) || (shadingParametersHash != materialHash.shadingParameter) || areVertexLightsDirty;
    if (isShadingParametersEnabled)
    {
        internal::MaterialState::ActivateShadingParameters(sceneEnvironment, shaderProgram, resShadingParametersMaterial);
        materialHash.shadingParameter = shadingParametersHash;
    }

    ResRasterization resRasterization = original.GetRasterization();
    u32 rasterizationHash = original.GetRasterizationHash();
    bool isRasterizationEnabled = (rasterizationHash == 0x0) || (rasterizationHash != materialHash.rasterization);
    if (isRasterizationEnabled)
    {
        internal::MaterialState::ActivateRasterization(resRasterization);
        materialHash.rasterization = rasterizationHash;
    }

    ResMaterial resTextureCoordinatorsMaterial = original;
    u32 textureCoordinatorHash = resTextureCoordinatorsMaterial.GetTextureCoordinatorsHash();
    bool isTextureCoordinatorEnabled = (textureCoordinatorHash == 0x0) || (textureCoordinatorHash != materialHash.textureCoordinator);
    if (isTextureCoordinatorEnabled)
    {
        internal::MaterialState::ActivateTextureCoordinators(renderContext, shaderProgram, resTextureCoordinatorsMaterial);
        materialHash.textureCoordinator = textureCoordinatorHash;
    }

    ResMaterial resTextureMappersMaterial = original;
    u32 textureMappersHash = resTextureMappersMaterial.GetTextureMappersHash();
    bool isTextureMappersEnabled = (textureMappersHash == 0x0) || (textureMappersHash != materialHash.textureMapper);
    if (isTextureMappersEnabled)
    {
        internal::MaterialState::ActivateTextureMappers(resTextureMappersMaterial);
        materialHash.textureMapper = textureMappersHash;
    }

#if 0
    ResMaterial resProceduralTextureMapperMaterial = original;
    internal::MaterialState::ActivateProceduralTextureMapper(renderContext, shaderProgram, resProceduralTextureMapperMaterial);
#endif

    bool reflectionEnabled = false;
    bool lightEnabled = nw::ut::CheckFlag(original.GetFlags(), ResMaterialData::FLAG_FRAGMENTLIGHT_ENABLED);
    if (lightEnabled)
    {
        ResFragmentLighting resFragmentLighting =original.GetFragmentShader().GetFragmentLighting();
        reflectionEnabled = ut::CheckFlag(resFragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
        u32 fragmentLightingHash = original.GetFragmentLightingHash();
        bool isFragmentLightingEnabled = (fragmentLightingHash == 0x0) || (fragmentLightingHash != materialHash.fragmentLighting) || areFragmentLightsDirty;
        if (isFragmentLightingEnabled)
        {
            internal::MaterialState::ActivateFragmentLighting(sceneEnvironment, resFragmentLighting);
            materialHash.fragmentLighting = fragmentLightingHash;
        }

        ResFragmentLightingTable resFragmentLightingTable =original.GetFragmentShader().GetFragmentLightingTable();
        u32 fragmentLightingTableHash = original.GetFragmentLightingTableHash();
        bool isFragmentLightingTableEnabled = (fragmentLightingTableHash == 0x0) || (fragmentLightingTableHash != materialHash.fragmentLightingTable);
        if (isFragmentLightingTableEnabled)
        {
            internal::MaterialState::ActivateFragmentLightingTable(resFragmentLighting, resFragmentLightingTable);
            materialHash.fragmentLightingTable = fragmentLightingTableHash;
        }
    }

    ResMaterialColor resMaterialColor =  original.GetMaterialColor();
    u32 materialColorHash = original.GetMaterialColorHash();
    bool isMaterialColorEnabled = (materialColorHash == 0x0) || (materialColorHash != materialHash.materialColor) || areFragmentLightsDirty;
    if (!isMaterialColorEnabled && cacheMaterial != NULL)
    {
        bool isPreReflectionEnabled = nw::ut::CheckFlag(cacheMaterial->GetOriginal().GetFragmentShader().GetFragmentLighting().GetFlags(), ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
        isMaterialColorEnabled = (reflectionEnabled != isPreReflectionEnabled);
    }
    if (isMaterialColorEnabled)
    {
        internal::MaterialState::ActivateMaterialColor(sceneEnvironment, shaderProgram, resMaterialColor, reflectionEnabled);
        materialHash.materialColor = materialColorHash;
    }

    ResFragmentShader resTextureCombinerFragmentShader = original.GetFragmentShader();
    u32 textureCombinersHash = original.GetTextureCombinersHash();
    bool isTextureCombinersEnabled = (textureCombinersHash == 0x0) || (textureCombinersHash != materialHash.textureCombiner);
    if (isTextureCombinersEnabled || isMaterialColorEnabled)
    {
        internal::MaterialState::ActivateTextureCombiners(resTextureCombinerFragmentShader, resMaterialColor);
        materialHash.textureCombiner = textureCombinersHash;
    }

    ResAlphaTest resAlphaTest = original.GetFragmentShader().GetAlphaTest();
    u32 alphaTestHash = original.GetAlphaTestHash();
    bool isAlphaTestEnabled = (alphaTestHash == 0x0) || (alphaTestHash != materialHash.alphaTest);
    if (isAlphaTestEnabled)
    {
        internal::MaterialState::ActivateAlphaTest(resAlphaTest);
        materialHash.alphaTest = alphaTestHash;
    }

    ResFragmentOperation resFragmentOperation = original.GetFragmentOperation();
    u32 fragmentOperationHash = original.GetFragmentOperationHash();
    bool isFragmentOperationEnabled = (fragmentOperationHash == 0x0) || (fragmentOperationHash != materialHash.fragmentOperation);
    if (isFragmentOperationEnabled)
    {
        internal::MaterialState::ActivateFragmentOperation(resFragmentOperation);
        materialHash.fragmentOperation = fragmentOperationHash;
    }

    renderContext->SetMaterialHash(materialHash);\
}

}
}