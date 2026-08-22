#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/gfx/gfx_ParticleMaterialActivator.h>
#include <nw/gfx/gfx_MaterialState.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_RenderContext.h>
#include <nn/gx.h>


namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(ParticleMaterialActivator, IMaterialActivator);

ParticleMaterialActivator* ParticleMaterialActivator::Create(os::IAllocator* allocator){
    void* memory = allocator->Alloc(sizeof(ParticleMaterialActivator));
    
    if (memory == NULL){
        return NULL;
    }
    else{
        return new(memory) ParticleMaterialActivator(allocator);
    }
}

void ParticleMaterialActivator::Activate(RenderContext* renderContext, const Material* material){
    #define _NO_USE_MATERIAL_BUFFER (1)

    NW_NULL_ASSERT(renderContext);

    const SceneEnvironment& sceneEnvironment = renderContext->GetSceneEnvironment();

    const Material* cacheMaterial = renderContext->GetMaterialCache();
    if ((cacheMaterial != NULL) &&  (cacheMaterial->GetOriginal() == material->GetOriginal())){
        return;
    }

    const ShaderProgram* shaderProgram = renderContext->GetShaderProgram();
    NW_NULL_ASSERT(shaderProgram);

    ResMaterial original = material->GetOriginal();
    const Model* owner = material->GetOwnerModel();
    NW_NULL_ASSERT(owner);

    RenderContext::MaterialHash materialHash = renderContext->GetMaterialHash();

    ResMaterial resShadingParametersMaterial = original;

    internal::MaterialState::ActivateFogParameters(sceneEnvironment, resShadingParametersMaterial);

    ResRasterization resRasterization = original.GetRasterization();
    u32 rasterizationHash = original.GetRasterizationHash();
    bool isRasterizationEnabled = (rasterizationHash == 0x0) || (rasterizationHash != materialHash.rasterization);
    if (isRasterizationEnabled){
        internal::MaterialState::ActivateRasterization(resRasterization);
        materialHash.rasterization = rasterizationHash;
    }

    ResMaterial resTextureCoordinatorsMaterial = original;
    u32 textureCoordinatorHash = resTextureCoordinatorsMaterial.GetTextureCoordinatorsHash();
    if (textureCoordinatorHash != materialHash.textureCoordinator){
        internal::MaterialState::ActivateParticleTextureCoordinators(renderContext, shaderProgram, resTextureCoordinatorsMaterial);
        materialHash.textureCoordinator = textureCoordinatorHash;
    }

    ResMaterial resTextureMappersMaterial = original;
    u32 textureMappersHash = resTextureMappersMaterial.GetTextureMappersHash();
    bool isTextureMappersEnabled = (textureMappersHash == 0x0) || (textureMappersHash != materialHash.textureMapper);
    if (isTextureMappersEnabled){
        internal::MaterialState::ActivateParticleTextureMappers(resTextureMappersMaterial);
        materialHash.textureMapper = textureMappersHash;
    }

    ResMaterialColor resMaterialColor =  original.GetMaterialColor();

    u32 materialColorHash = original.GetMaterialColorHash();
    bool isMaterialColorEnabled = (materialColorHash == 0x0) || (materialColorHash != materialHash.materialColor);

    ResFragmentShader resTextureCombinerFragmentShader = original.GetFragmentShader();
    u32 textureCombinersHash = original.GetTextureCombinersHash();
    bool isTextureCombinersEnabled = (textureCombinersHash == 0x0) || (textureCombinersHash != materialHash.textureCombiner);
    if (isTextureCombinersEnabled || isMaterialColorEnabled){
        internal::MaterialState::ActivateTextureCombiners(resTextureCombinerFragmentShader, resMaterialColor);
        materialHash.textureCombiner = textureCombinersHash;
    }

    ResAlphaTest resAlphaTest = original.GetFragmentShader().GetAlphaTest();
    internal::MaterialState::ActivateAlphaTest(resAlphaTest);

    ResFragmentOperation resFragmentOperation = original.GetFragmentOperation();
    u32 fragmentOperationHash = original.GetFragmentOperationHash();
    bool isFragmentOperationEnabled = (fragmentOperationHash == 0x0) || (fragmentOperationHash != materialHash.fragmentOperation);
    if (isFragmentOperationEnabled){
        internal::MaterialState::ActivateFragmentOperation(resFragmentOperation);
        materialHash.fragmentOperation = fragmentOperationHash;
    }
    renderContext->SetMaterialHash(materialHash);
}

ParticleMaterialActivator::ParticleMaterialActivator(os::IAllocator* allocator): 
    IMaterialActivator(allocator)
{}

ParticleMaterialActivator::~ParticleMaterialActivator(){}

}
}