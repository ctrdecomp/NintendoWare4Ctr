#pragma once

#include <nw/types.h>
#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/gfx/gfx_GraphicsDevice.h>
#include <nw/gfx/gfx_SceneEnvironment.h>
#include <nw/gfx/gfx_ShaderUniforms.h>
#include <nw/gfx/gfx_ShaderProgram.h>

#define MATERIAL_SET_ENABLED

namespace mw{
namespace os{
    class IAllocator;
}
namespace gfx{
namespace internal{

nn::math::MTX34* CreateMatrixForLinearShadowMapTexture(nn::math::MTX34* pOut, f32 coeff, f32 nearp, f32 farp);

class MaterialState{
public:
    static inline void ActivateShaderParameter(const ShaderProgram* shaderProgram, 
        const ResMaterial material);

    static inline void ActivateFragmentLightParameters(const SceneEnvironment& sceneEnvironment,
        const ShaderProgram* shaderProgram,
        const ResMaterial material);

    static inline void ActivateShadingParameters(const SceneEnvironment& sceneEnvironment,
        const ShaderProgram* shaderProgram,
        const ResMaterial material);

    static inline void ActivateRasterization(const ResRasterization rasterization);

    static void ActivateFragmentLightingTable(const ResFragmentLighting fragmentLighting,
        const ResFragmentLightingTable fragmentLightingTable);

    static void ActivateTextureCoordinators(RenderContext* renderContext,const ShaderProgram* shaderProgram,
        const ResMaterial texCoordMaterial);

    static void ActivateParticleTextureCoordinators(RenderContext* renderContext,const ShaderProgram* shaderProgram,
        const ResMaterial texCoordMaterial);

    static inline void ActivateMaterialColor(const SceneEnvironment& sceneEnvironment,const ShaderProgram* shaderProgram,
        ResMaterialColor materialColor,
        bool useReflection);

    static inline void ActivateFragmentLighting(
        const SceneEnvironment& sceneEnvironment,
        const ResFragmentLighting fragmentLighting);

    static inline void ActivateTextureMappers(const ResMaterial texMapperMaterial);

    static inline void ActivateParticleTextureMappers(const ResMaterial texMapperMaterial);

    static inline void ActivateProceduralTextureMapper(
        RenderContext* renderContext,
        const ShaderProgram* shaderProgram,
        const ResMaterial material);

    static inline void ActivateTextureCombiners(const ResFragmentShader fragmentShader, const ResMaterialColor materialColor);

    static inline void ActivateAlphaTest(const ResAlphaTest alphaTest);

    static inline void ActivateFragmentOperation(const ResFragmentOperation fragmentOperation);

    static inline void ActivateFogParameters(const SceneEnvironment& sceneEnvironment, const ResMaterial material);

private:
    static inline u32 GetConstantColorU32(ResTextureCombiner::Constant constant, const ResMaterialColor materialColor);

    static inline void ActivateDepthOperation(const ResDepthOperation depthOperation);

    static inline void ActivateStencilOperation(const ResStencilOperation stencilOperation);

    static inline void ActivateBlendOperation(const ResBlendOperation blendOperation);

    static math::MTX44* SetupTextureMatrix(
        nw::math::MTX44* textureMatrix,
        ResTextureCoordinator::MappingMatrixMode mode,
        float scaleS, float scaleT,
        float rotate,
        float translateS, float translateT);
};

inline u32 MaterialState::GetConstantColorU32(ResTextureCombiner::Constant constant, const ResMaterialColor materialColor){
    switch (constant){
    case ResTextureCombiner::CONSTANT0: return materialColor.GetConstant0U32();
    case ResTextureCombiner::CONSTANT1: return materialColor.GetConstant1U32();
    case ResTextureCombiner::CONSTANT2: return materialColor.GetConstant2U32();
    case ResTextureCombiner::CONSTANT3: return materialColor.GetConstant3U32();
    case ResTextureCombiner::CONSTANT4: return materialColor.GetConstant4U32();
    case ResTextureCombiner::CONSTANT5: return materialColor.GetConstant5U32();
    case ResTextureCombiner::EMISSION:  return materialColor.GetEmissionU32();
    case ResTextureCombiner::AMBIENT:   return materialColor.GetAmbientU32();
    case ResTextureCombiner::DIFFUSE:   return materialColor.GetDiffuseU32();
    case ResTextureCombiner::SPECULAR0: return materialColor.GetSpecular0U32();
    case ResTextureCombiner::SPECULAR1: return materialColor.GetSpecular1U32();
    default:
        NW_FATAL_ERROR("Illegal constant name");
        return materialColor.GetConstant0U32();
    }
}

inline void MaterialState::ActivateFogParameters(const SceneEnvironment& sceneEnvironment, const ResMaterial material){
    u32 flags = material.GetFlags();

    enum{
        FOG_MODE_FALSE = 0,
        FOG_MODE_FOG = 5,
        FOG_MODE_GAS = 7,
        REG_FOG_MODE = 0xe0
    };

    s32 fogMode = FOG_MODE_FALSE;
    if (ut::CheckFlag(flags, ResMaterialData::FLAG_FOG_ENABLED)){
        if (sceneEnvironment.GetActiveFog() != NULL){
            fogMode = FOG_MODE_FOG;
        }
    }

    {
        const u32 HEADER_FOG_MODE = internal::MakeCommandHeader(REG_FOG_MODE, 1, false, 0x1);

        u32 FOG_COMMAND[] ={
            fogMode,
            HEADER_FOG_MODE
        };

        internal::NWUseCmdlist<sizeof(FOG_COMMAND)>(&FOG_COMMAND[0]);
    }
}

inline void MaterialState::ActivateDepthOperation(const ResDepthOperation depthOperation){
    GraphicsDevice::SetDepthTestEnabled(nw::ut::CheckFlag(depthOperation.GetFlags(), ResDepthOperationData::FLAG_TEST_ENABLED));
    GraphicsDevice::SetDepthMaskEnabled(nw::ut::CheckFlag(depthOperation.GetFlags(), ResDepthOperationData::FLAG_MASK_ENABLED));
    GraphicsDevice::ActivateMask();

    internal::NWUseCmdlist<sizeof(depthOperation.ref().mCommandBuffer)>(&depthOperation.ref().mCommandBuffer[0]);
}

inline void MaterialState::ActivateStencilOperation(const ResStencilOperation stencilOperation){
    if (!stencilOperation.IsValid()){
        return;
    }

    if (stencilOperation.IsTestEnabled()){
        GraphicsDevice::SetStencilTestEnabled(true);
    }
    else{
        GraphicsDevice::SetStencilTestEnabled(false);
    }

    internal::NWUseCmdlist<sizeof(stencilOperation.ref().mCommandBuffer)>(&stencilOperation.ref().mCommandBuffer[0]);
}

inline void MaterialState::ActivateBlendOperation(const ResBlendOperation blendOperation){
    ResBlendOperation::Mode mode = blendOperation.GetMode();

    if (mode == ResBlendOperation::MODE_NOT_USE){
        GraphicsDevice::SetBlendEnabled(false);
    }
    else{
        GraphicsDevice::SetBlendEnabled(true);
    }

    internal::NWUseCmdlist<sizeof(blendOperation.ref().mCommandBuffer)>(blendOperation.ref().mCommandBuffer);
}

inline void MaterialState::ActivateShaderParameter(const ShaderProgram* shaderProgram, const ResMaterial material){
    ResShaderProgramDescription description = shaderProgram->GetActiveDescription();
    ResShaderSymbolArray symbolArray = description.GetSymbols();
    int symbolCount = description.GetSymbolsCount();

    bool symbolHasBeenSetFlag[64] = { false };

    ResShaderParameterArrayConst::const_iterator end = material.GetShaderParameters().end();
    for (ResShaderParameterArrayConst::const_iterator parameter = material.GetShaderParameters().begin(); parameter != end; ++parameter){
        int symbolIndex = (*parameter).GetSymbolIndex();

        if (0 <= symbolIndex && symbolIndex < symbolCount){
            ResShaderSymbol shaderSymbol = symbolArray[symbolIndex];

            if (shaderSymbol.IsEnabled() && shaderSymbol.GetLocation() >= 0){
                NW_ASSERT(shaderSymbol.GetLocation() < 96);

                if (shaderSymbol.IsGeometryUniform()){
                    shaderProgram->SetUserGeometryUniform(shaderSymbol.GetLocation(), (*parameter).GetParameter());
                }
                else{
                    shaderProgram->SetUserVertexUniform(shaderSymbol.GetLocation(), (*parameter).GetParameter());
                }
            }

            symbolHasBeenSetFlag[symbolIndex] = true;
        }
    }

    for (int symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex){
        if (!symbolHasBeenSetFlag[symbolIndex]){
            ResShaderSymbol shaderSymbol = symbolArray[symbolIndex];

            if (shaderSymbol.IsEnabled() && shaderSymbol.GetLocation() >= 0){
                NW_ASSERT(shaderSymbol.GetLocation() < 96);

                if (shaderSymbol.IsGeometryUniform()){
                    shaderProgram->SetUserGeometryUniform(shaderSymbol.GetLocation(), shaderSymbol.GetDefaultValue());
                }
                else{
                    shaderProgram->SetUserVertexUniform(shaderSymbol.GetLocation(), shaderSymbol.GetDefaultValue());
                }
            }
        }
    }
}

inline void MaterialState::ActivateFragmentLightParameters(const SceneEnvironment& sceneEnvironment,const ShaderProgram* shaderProgram,const ResMaterial material){
    enum{
        REG_FRAGMENT_LIGHT_ENABLED = 0x1c6,
        REG_FRAGMENT_LIGHT_ENABLED2 = 0x8F
    };

    const u32 ENABLED_HEADER = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_ENABLED, 1, false, 0x1);
    const u32 ENABLED_HEADER2 = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_ENABLED2, 1, false, 0x1);

    bool isFragmentLightEnabled =
        ut::CheckFlag(material.GetFlags(), ResMaterialData::FLAG_FRAGMENTLIGHT_ENABLED) &&
        (sceneEnvironment.GetFragmentLightCount() != 0);

    u32 LIGHT_COUNT_COMMAND[] ={
        isFragmentLightEnabled ? 0 : 1,
        ENABLED_HEADER,
        isFragmentLightEnabled ? 1 : 0,
        ENABLED_HEADER2
    };

    shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISQUATE), isFragmentLightEnabled);

    internal::NWUseCmdlist<sizeof(LIGHT_COUNT_COMMAND)>(&LIGHT_COUNT_COMMAND[0]);
}

inline void MaterialState::ActivateShadingParameters(const SceneEnvironment& sceneEnvironment,const ShaderProgram* shaderProgram,const ResMaterial material){
    u32 flags = material.GetFlags();

    if (shaderProgram->GetActiveDescription().GetMaxVertexLightCount() > 0){
        if (sceneEnvironment.GetVertexLightCount() > 0 && ut::CheckFlag(flags, ResMaterialData::FLAG_VERTEXLIGHT_ENABLED)){
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISVERTL), true);
        }
        else{
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISVERTL), false);
        }
    }

    if (ut::CheckFlag(shaderProgram->GetActiveDescription().GetFlags(), ResShaderProgramDescription::FLAG_IS_SUPPORTING_HEMISPHERE_LIGHTING)){
        if (sceneEnvironment.GetHemiSphereLight() != NULL && ut::CheckFlag(flags, ResMaterialData::FLAG_HEMISPHERELIGHT_ENABLED)){
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISHEMIL), true);
            shaderProgram->SetVertexUniformBool(
                NW_GFX_VERTEX_UNIFORM(ISHEMIO),
                ut::CheckFlag(flags, ResMaterialData::FLAG_HEMISPHERE_OCCLUSION_ENABLED));
        }
        else{
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISHEMIL), false);
        }
    }

    enum{
        FOG_MODE_FALSE = 0,
        FOG_MODE_FOG = 5,
        FOG_MODE_GAS = 7,
        REG_FOG_MODE = 0xe0
    };

    s32 fogMode = FOG_MODE_FALSE;
    if (ut::CheckFlag(flags, ResMaterialData::FLAG_FOG_ENABLED)){
        if (sceneEnvironment.GetActiveFog() != NULL){
            fogMode = FOG_MODE_FOG;
        }
    }

    {
        const u32 HEADER_FOG_MODE = internal::MakeCommandHeader(REG_FOG_MODE, 1, false, 0x1);

        u32 FOG_COMMAND[] ={
            fogMode,
            HEADER_FOG_MODE
        };

        internal::NWUseCmdlist<sizeof(FOG_COMMAND)>(&FOG_COMMAND[0]);
    }
}

inline void MaterialState::ActivateRasterization(const ResRasterization rasterization){
    NW_ASSERT(rasterization.IsValid());

    GraphicsDevice::SetPolygonOffsetEnabled(ut::CheckFlag(rasterization.GetFlags(), ResRasterizationData::FLAG_POLYGON_OFFSET_ENABLED));
    GraphicsDevice::SetPolygonOffsetUnit(rasterization.GetPolygonOffsetUnit());
    GraphicsDevice::ActivatePolygonOffset();

    internal::NWUseCmdlist<sizeof(rasterization.ref().mCommandBuffer)>(rasterization.ref().mCommandBuffer);
}

inline void MaterialState::ActivateMaterialColor(const SceneEnvironment& sceneEnvironment,const ShaderProgram* shaderProgram,ResMaterialColor materialColor,bool useReflection){
    NW_ASSERT(materialColor.IsValid());

    ResShaderProgramDescription resShaderProgram = shaderProgram->GetActiveDescription();

    internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_MATDIFF_INDEX, 1, materialColor.GetDiffuse());
    internal::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_MATAMBI_INDEX, 1, materialColor.GetAmbient());

    int lightCount = ut::Min(sceneEnvironment.GetFragmentLightCount(), MAX_PER_PIXEL_LIGHTS);
    internal::ActivateFragmentLightCount(lightCount);

    internal::ActivateFragmentAmbientLight(materialColor, sceneEnvironment.GetAmbientLight());

    for (int i = 0; i < lightCount; ++i){
        const FragmentLight* light = sceneEnvironment.GetFragmentLight(i);
        internal::ActivateFragmentLight(i, materialColor, light, useReflection);
    }
}

inline void MaterialState::ActivateFragmentLighting(const SceneEnvironment& sceneEnvironment, const ResFragmentLighting fragmentLighting){
    NW_ASSERT(fragmentLighting.IsValid());

    internal::ActivateFragmentLighting(fragmentLighting);

    bool geomFactor0 = ut::CheckFlag(fragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_GEOMETRIC_FACTOR0_ENABLED);
    bool geomFactor1 = ut::CheckFlag(fragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_GEOMETRIC_FACTOR1_ENABLED);

    int lightCount = sceneEnvironment.GetFragmentLightCount();
    for (int i = 0; i < lightCount; ++i){
        const FragmentLight* light = sceneEnvironment.GetFragmentLight(i);
        ResFragmentLight resLight = light->GetResFragmentLight();
        s32 lightKind = resLight.GetLightKind();

        bool twoSideDiffuse = ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_TWO_SIDE_DIFFUSE_ENABLED);
        bool isDirectional = (lightKind == ResFragmentLight::KIND_DIRECTIONAL) || GraphicsDevice::GetFragmentLightPositionW(i);

        internal::ActivateFragmentLightParameters(i, isDirectional, twoSideDiffuse, geomFactor0, geomFactor1);
    }
}

inline void MaterialState::ActivateTextureMappers(const ResMaterial texMapperMaterial){
    u32 samplerSetting = 0;

    for (s32 textureUnit = 0; textureUnit < texMapperMaterial.GetTextureMappersCount(); textureUnit++){
        const ResTextureMapper textureMapper = texMapperMaterial.GetTextureMappers(textureUnit);
        if (!textureMapper.IsValid()){
            continue;
        }

        samplerSetting |= 1 << textureUnit;

        const ResPixelBasedTextureMapper pixTexMapper = ResStaticCast<const ResPixelBasedTextureMapper>(textureMapper);

        const u32* command = pixTexMapper.GetCommandCache();
        u32 commandSize = pixTexMapper.GetCommandSizeToSend();

        internal::NWUseCmdlist(command, commandSize);
    }

    const u32 HEADER = internal::MakeCommandHeader(PICA_REG_TEXTURE_FUNC, 1, false, 0x5);

    u32 INVALIDATE_TEXTURE_CACHE[] ={
        samplerSetting | 0x00010000,
        HEADER
    };

    internal::NWUseCmdlist<sizeof(INVALIDATE_TEXTURE_CACHE)>(&INVALIDATE_TEXTURE_CACHE[0]);
}

inline void MaterialState::ActivateParticleTextureMappers(const ResMaterial texMapperMaterial){
    const ResTextureMapper textureMapper = texMapperMaterial.GetTextureMappers(0);
    if (textureMapper.IsValid()){
        const ResPixelBasedTextureMapper pixTexMapper = ResStaticCast<const ResPixelBasedTextureMapper>(textureMapper);

        const u32* command = pixTexMapper.GetCommandCache();
        u32 commandSize = pixTexMapper.GetCommandSizeToSend();

        internal::NWUseCmdlist(command, commandSize);
    }

    const u32 HEADER = internal::MakeCommandHeader(PICA_REG_TEXTURE_FUNC, 1, false, 0x5);

    const u32 INVALIDATE_TEXTURE_CACHE[] ={
        0x00010001,
        HEADER
    };

    internal::NWUseCmdlist<sizeof(INVALIDATE_TEXTURE_CACHE)>(&INVALIDATE_TEXTURE_CACHE[0]);
}

inline void MaterialState::ActivateProceduralTextureMapper(RenderContext* renderContext,const ShaderProgram* shaderProgram,const ResMaterial material){
    NW_UNUSED_VARIABLE(renderContext);
    NW_UNUSED_VARIABLE(shaderProgram);
    NW_UNUSED_VARIABLE(material);
}

inline void MaterialState::ActivateTextureCombiners(const ResFragmentShader fragmentShader, const ResMaterialColor materialColor){
    internal::NWUseCmdlist<sizeof(fragmentShader.ref().mBufferCommand)>(&(fragmentShader.ref().mBufferCommand[0]));

    for (uint stage = 0; stage < COMBINER_AVAILABLE_COUNT; stage++){
        ResTextureCombiner textureCombiner = fragmentShader.GetTextureCombiners(stage);

        u32 constantU32 = GetConstantColorU32(textureCombiner.GetConstant(), materialColor);

        textureCombiner.SetConstantCmd(constantU32);

        internal::NWUseCmdlist<sizeof(textureCombiner.ref().mCommandBuffer)>(reinterpret_cast<const u32*>(textureCombiner.ref().mCommandBuffer));
    }
}

inline void MaterialState::ActivateAlphaTest(const ResAlphaTest alphaTest){
    NW_ASSERT(alphaTest.IsValid());
    internal::NWUseCmdlist<sizeof(alphaTest.ref().mCommandBuffer)>(&alphaTest.ref().mCommandBuffer[0]);
}

inline void MaterialState::ActivateFragmentOperation(const ResFragmentOperation fragmentOperation){
    ResDepthOperation resDepthOperation = fragmentOperation.GetDepthOperation();
    ResStencilOperation resStencilOperation = fragmentOperation.GetStencilOperation();
    ResBlendOperation resBlendOperation = fragmentOperation.GetBlendOperation();

    ActivateDepthOperation(resDepthOperation);
    ActivateStencilOperation(resStencilOperation);
    ActivateBlendOperation(resBlendOperation);

    GraphicsDevice::SetFragmentOperationMode(fragmentOperation.GetFragmentOperationMode());
    GraphicsDevice::ActivateFrameBuffer();
}

}
}
}