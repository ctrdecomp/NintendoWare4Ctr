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

#ifndef NW_GFX_MATERIALSTATE_H_
#define NW_GFX_MATERIALSTATE_H_

#include <nw/types.h>
#include <nw/gfx/gfx_ActivateCommand.h>
#include <nw/gfx/gfx_GraphicsDevice.h>
#include <nw/gfx/gfx_Config.h>

// If defined, this macro conducts a profile of the individual element settings of the materials.
// #define NW_MATERIAL_PROFILE

// When this macro is defined, the material is set.
#define MATERIAL_SET_ENABLED

namespace nw
{
namespace os
{
class IAllocator;
}

namespace gfx
{

namespace internal
{

// Generates a standard OpenGL projection transformation matrix
nn::math::Matrix34*
CreateMatrixForLinearShadowMapTexture(nn::math::Matrix34* pOut, f32 coeff, f32 nearp, f32 farp);

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class MaterialState
{
public:
    //----------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateShaderParameter(
        const ShaderProgram* shaderProgram,
        const ResMaterial material);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateFragmentLightParameters(
        const SceneEnvironment& sceneEnvironment,
        const ShaderProgram* shaderProgram,
        const ResMaterial material);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateShadingParameters(
        const SceneEnvironment& sceneEnvironment,
        const ShaderProgram* shaderProgram,
        const ResMaterial material);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateRasterization(const ResRasterization rasterization);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void ActivateFragmentLightingTable(
        const ResFragmentLighting fragmentLighting,
        const ResFragmentLightingTable fragmentLightingTable);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void ActivateTextureCoordinators(
        RenderContext* renderContext,
        const ShaderProgram* shaderProgram,
        const ResMaterial texCoordMaterial);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void ActivateParticleTextureCoordinators(
        RenderContext* renderContext,
        const ShaderProgram* shaderProgram,
        const ResMaterial texCoordMaterial);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateMaterialColor(
        const SceneEnvironment& sceneEnvironment,
        const ShaderProgram* shaderProgram,
        ResMaterialColor materialColor,
        bool useReflection);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateFragmentLighting(
        const SceneEnvironment& sceneEnvironment,
        const ResFragmentLighting fragmentLighting);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateTextureMappers(const ResMaterial texMapperMaterial);


    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateParticleTextureMappers(const ResMaterial texMapperMaterial);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateProceduralTextureMapper(
        const ResMaterial material);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateTextureCombiners(
        const ResFragmentShader fragmentShader,
        const ResMaterialColor materialColor);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateAlphaTest(const ResAlphaTest alphaTest);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateFragmentOperation(const ResFragmentOperation fragmentOperation);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateFogParameters(
        const SceneEnvironment& sceneEnvironment,
        const ResMaterial material);

    //

private:
    //
    static NW_INLINE u32 GetConstantColorU32(
        ResTextureCombiner::Constant constant,
        const ResMaterialColor materialColor);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateDepthOperation(const ResDepthOperation depthOperation);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateStencilOperation(const ResStencilOperation stencilOperation);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    static NW_INLINE void ActivateBlendOperation(const ResBlendOperation blendOperation);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static math::MTX44* SetupTextureMatrix(
        math::MTX44* textureMatrix,
        ResTextureCoordinator::MappingMatrixMode mode,
        float scaleS, float scaleT,
        float rotate,
        float translateS, float translateT);
};

//----------------------------------------
NW_INLINE u32
MaterialState::GetConstantColorU32(
    ResTextureCombiner::Constant constant,
    const ResMaterialColor materialColor)
{
    switch ( constant )
    {
    case ResTextureCombiner::CONSTANT0: return  materialColor.GetConstant0U32();
    case ResTextureCombiner::CONSTANT1: return  materialColor.GetConstant1U32();
    case ResTextureCombiner::CONSTANT2: return  materialColor.GetConstant2U32();
    case ResTextureCombiner::CONSTANT3: return  materialColor.GetConstant3U32();
    case ResTextureCombiner::CONSTANT4: return  materialColor.GetConstant4U32();
    case ResTextureCombiner::CONSTANT5: return  materialColor.GetConstant5U32();
    case ResTextureCombiner::EMISSION:  return  materialColor.GetEmissionU32();
    case ResTextureCombiner::AMBIENT:   return  materialColor.GetAmbientU32();
    case ResTextureCombiner::DIFFUSE:   return  materialColor.GetDiffuseU32();
    case ResTextureCombiner::SPECULAR0: return  materialColor.GetSpecular0U32();
    case ResTextureCombiner::SPECULAR1: return  materialColor.GetSpecular1U32();
    default:
        NW_FATAL_ERROR("Illegal constant name");
        return materialColor.GetConstant0U32();
    }
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateFogParameters(const SceneEnvironment& sceneEnvironment, const ResMaterial material)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateFogParameters");
#endif

#if defined(MATERIAL_SET_ENABLED)
    u32 flags = material.GetFlags();

    enum
    {
        FOG_MODE_FALSE = 0,
        FOG_MODE_FOG   = 5,
        FOG_MODE_GAS   = 7,
        REG_FOG_MODE   = 0xe0
    };

    // Configures fog.
    s32 fogMode = FOG_MODE_FALSE;
    if (ut::CheckFlag(flags, ResMaterialData::FLAG_FOG_ENABLED))
    {
        if (sceneEnvironment.GetActiveFog() != NULL)
        {
            fogMode = FOG_MODE_FOG;
        }
    }

    {
        const u32 HEADER_FOG_MODE = i::MakeCommandHeader(REG_FOG_MODE, 1, false, 0x1);

        u32 FOG_COMMAND[] =
        {
            fogMode,
            HEADER_FOG_MODE
        };

        i::NWUseCmdlist<sizeof(FOG_COMMAND)>(&FOG_COMMAND[0]);
    }
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateDepthOperation(const ResDepthOperation depthOperation)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateDepthOperation");
#endif

    GraphicsDevice::SetDepthTestEnabled(
        ut::CheckFlag(
            depthOperation.GetFlags(),
            ResDepthOperationData::FLAG_TEST_ENABLED));

    GraphicsDevice::SetDepthMaskEnabled(
        ut::CheckFlag(
            depthOperation.GetFlags(),
            ResDepthOperationData::FLAG_MASK_ENABLED));

    GraphicsDevice::ActivateMask();

    i::NWUseCmdlist<sizeof(depthOperation.ref().m_CommandBuffer)>( &depthOperation.ref().m_CommandBuffer[0] );
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateStencilOperation(const ResStencilOperation stencilOperation)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateStencilOperation");
#endif

    if (!stencilOperation.IsValid())
    {
        return;
    }

    if (stencilOperation.IsTestEnabled())
    {
        GraphicsDevice::SetStencilTestEnabled(true);
    }
    else
    {
        GraphicsDevice::SetStencilTestEnabled(false);
    }
    i::NWUseCmdlist<sizeof(stencilOperation.ref().m_CommandBuffer)>( &stencilOperation.ref().m_CommandBuffer[0] );
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateBlendOperation(const ResBlendOperation blendOperation)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateBlendOperation");
#endif

    GraphicsDevice::SetDstColorReferred(blendOperation.IsDstColorReferred());

    i::NWUseCmdlist<sizeof(blendOperation.ref().m_CommandBuffer)>(blendOperation.ref().m_CommandBuffer);
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateShaderParameter(
        const ShaderProgram* shaderProgram,
        const ResMaterial material)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateShaderParameter");
#endif

#if defined(MATERIAL_SET_ENABLED)
    ResShaderProgramDescription description = shaderProgram->GetActiveDescription();
    ResShaderSymbolArray symbolArray = description.GetSymbols();
    int symbolCount = description.GetSymbolsCount();

    // 
    // Sets the default values.

    bool symbolHasBeenSetFlag[64] = { false };

    ResShaderParameterArrayConst::const_iterator end = material.GetShaderParameters().end();
    for ( ResShaderParameterArrayConst::const_iterator parameter = material.GetShaderParameters().begin();
          parameter != end; ++parameter )
    {
        int symbolIndex = (*parameter).GetSymbolIndex();
        if ( 0 <= symbolIndex && symbolIndex < symbolCount)
        {
            ResShaderSymbol shaderSymbol = symbolArray[symbolIndex];
            if (shaderSymbol.IsEnabled() && shaderSymbol.GetLocation() >= 0)
            {
                NW_ASSERT(shaderSymbol.GetLocation() < 96);

                if (shaderSymbol.IsGeometryUniform())
                {
                    shaderProgram->SetUserGeometryUniform(shaderSymbol.GetLocation(), (*parameter).GetParameter());
                }
                else
                {
                    shaderProgram->SetUserVertexUniform(shaderSymbol.GetLocation(), (*parameter).GetParameter());
                }
            }

            // Even when the shader symbol itself is invalid, there is no need to set the default value so set the flag.
            symbolHasBeenSetFlag[symbolIndex] = true;
        }
    }

    for (int symbolIndex = 0; symbolIndex < symbolCount; ++symbolIndex)
    {
        if ( ! symbolHasBeenSetFlag[symbolIndex] )
        {
            ResShaderSymbol shaderSymbol = symbolArray[symbolIndex];

            if (shaderSymbol.IsEnabled() && shaderSymbol.GetLocation() >= 0)
            {
                NW_ASSERT(shaderSymbol.GetLocation() < 96);

                if (shaderSymbol.IsGeometryUniform())
                {
                    shaderProgram->SetUserGeometryUniform(shaderSymbol.GetLocation(), shaderSymbol.GetDefaultValue());
                }
                else
                {
                    shaderProgram->SetUserVertexUniform(shaderSymbol.GetLocation(), shaderSymbol.GetDefaultValue());
                }
            }
        }
    }
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateFragmentLightParameters(
    const SceneEnvironment& sceneEnvironment,
    const ShaderProgram* shaderProgram,
    const ResMaterial material)
{
    // Sets the on/off for each type of light.
    enum
    {
        REG_FRAGMENT_LIGHT_ENABLED  = 0x1c6,
        REG_FRAGMENT_LIGHT_ENABLED2 = 0x8F
    };
    const u32 ENABLED_HEADER  = i::MakeCommandHeader(REG_FRAGMENT_LIGHT_ENABLED, 1, false, 0x1);
    const u32 ENABLED_HEADER2 = i::MakeCommandHeader(REG_FRAGMENT_LIGHT_ENABLED2, 1, false, 0x1);
    bool isFragmentLightEnabled =
        ut::CheckFlag(material.GetFlags(), ResMaterialData::FLAG_FRAGMENTLIGHT_ENABLED) &&
        (sceneEnvironment.GetFragmentLightCount() != 0);

    u32 LIGHT_COUNT_COMMAND[] =
    {
        isFragmentLightEnabled ? 0 : 1,
        ENABLED_HEADER,
        isFragmentLightEnabled ? 1 : 0,
        ENABLED_HEADER2
    };

    // When fragment lighting will not occur, quaternion calculation can be omitted.
    shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISQUATE), isFragmentLightEnabled);

    i::NWUseCmdlist<sizeof(LIGHT_COUNT_COMMAND)>( &LIGHT_COUNT_COMMAND[0] );
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateShadingParameters(
    const SceneEnvironment& sceneEnvironment,
    const ShaderProgram* shaderProgram,
    const ResMaterial material)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateShadingParameters");
#endif

#if defined(MATERIAL_SET_ENABLED)
    u32 flags = material.GetFlags();

#if defined(NW_GFX_VERTEX_LIGHT_ENABLED)
    if (shaderProgram->GetActiveDescription().GetMaxVertexLightCount() > 0)
    {
        if (sceneEnvironment.GetVertexLightCount() > 0 &&
            ut::CheckFlag(flags, ResMaterialData::FLAG_VERTEXLIGHT_ENABLED))
        {
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISVERTL), true);
        }
        else
        {
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISVERTL), false);
        }
    }
#endif

    if (ut::CheckFlag(
        shaderProgram->GetActiveDescription().GetFlags(),
        ResShaderProgramDescription::FLAG_IS_SUPPORTING_HEMISPHERE_LIGHTING))
    {
        if (sceneEnvironment.GetHemiSphereLight() != NULL &&
            ut::CheckFlag(flags, ResMaterialData::FLAG_HEMISPHERELIGHT_ENABLED))
        {
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISHEMIL), true);
            shaderProgram->SetVertexUniformBool(
                NW_GFX_VERTEX_UNIFORM(ISHEMIO),
                ut::CheckFlag(flags, ResMaterialData::FLAG_HEMISPHERE_OCCLUSION_ENABLED));
        }
        else
        {
            shaderProgram->SetVertexUniformBool(NW_GFX_VERTEX_UNIFORM(ISHEMIL), false);
        }
    }

    enum
    {
        FOG_MODE_FALSE = 0,
        FOG_MODE_FOG   = 5,
        FOG_MODE_GAS   = 7,
        REG_FOG_MODE   = 0xe0
    };

    // Configures fog.
    s32 fogMode = FOG_MODE_FALSE;
    if (ut::CheckFlag(flags, ResMaterialData::FLAG_FOG_ENABLED))
    {
        if (sceneEnvironment.GetActiveFog() != NULL)
        {
            fogMode = FOG_MODE_FOG;
        }
    }

    {
        const u32 HEADER_FOG_MODE = i::MakeCommandHeader(REG_FOG_MODE, 1, false, 0x1);

        u32 FOG_COMMAND[] =
        {
            fogMode,
            HEADER_FOG_MODE
        };

        i::NWUseCmdlist<sizeof(FOG_COMMAND)>(&FOG_COMMAND[0]);
    }
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateRasterization(const ResRasterization rasterization)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateRasterization");
#endif

#if defined(MATERIAL_SET_ENABLED)
    NW_ASSERT(rasterization.IsValid());

    GraphicsDevice::SetPolygonOffsetEnabled(ut::CheckFlag(rasterization.GetFlags(), ResRasterizationData::FLAG_POLYGON_OFFSET_ENABLED));
    GraphicsDevice::SetPolygonOffsetUnit(rasterization.GetPolygonOffsetUnit());
    GraphicsDevice::ActivatePolygonOffset();

    // CullMode settings
    i::NWUseCmdlist<sizeof(rasterization.ref().m_CommandBuffer)>( rasterization.ref().m_CommandBuffer );
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateMaterialColor(
    const SceneEnvironment& sceneEnvironment,
    const ShaderProgram* shaderProgram,
    ResMaterialColor materialColor,
    bool useReflection)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateMaterialColor");
#endif

#if defined(MATERIAL_SET_ENABLED)
    NW_ASSERT(materialColor.IsValid());

    ResShaderProgramDescription resShaderProgram = shaderProgram->GetActiveDescription();

    // Sets a material color to the vertex shader.
    i::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_MATDIFF_INDEX, 1, materialColor.GetDiffuse());
    i::NWSetVertexUniform4fv(VERTEX_SHADER_UNIFORM_MATAMBI_INDEX, 1, materialColor.GetAmbient());

    int lightCount = ut::Min(sceneEnvironment.GetFragmentLightCount(), MAX_PER_PIXEL_LIGHTS);
    i::ActivateFragmentLightCount( lightCount );

    i::ActivateFragmentAmbientLight( materialColor, sceneEnvironment.GetAmbientLight() );

    for (int i = 0; i < lightCount; ++i)
    {
        const FragmentLight* light = sceneEnvironment.GetFragmentLight(i);

        i::ActivateFragmentLight(i, materialColor, light, useReflection);
    }
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateFragmentLighting(
    const SceneEnvironment& sceneEnvironment,
    const ResFragmentLighting fragmentLighting)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateFragmentLighting");
#endif

#if defined(MATERIAL_SET_ENABLED)
    NW_ASSERT(fragmentLighting.IsValid());

    // NOTE: Consider whether we can't have the resource-side have a command in advance.
    //       There is no problem with the portions held by the buffers for fragmentLighting and fragmentShader(LayerConfig), so attempt a measurement and if there are results, there might be movement for members within the FragmentShader class.
    // 
    // 
    i::ActivateFragmentLighting(fragmentLighting);

    bool geomFactor0 = ut::CheckFlag(
            fragmentLighting.GetFlags(),
            ResFragmentLightingData::FLAG_GEOMETRIC_FACTOR0_ENABLED);

    bool geomFactor1 = ut::CheckFlag(
            fragmentLighting.GetFlags(),
            ResFragmentLightingData::FLAG_GEOMETRIC_FACTOR1_ENABLED);

    int lightCount = sceneEnvironment.GetFragmentLightCount();
    for (int i = 0; i < lightCount; ++i)
    {
        const FragmentLight* light = sceneEnvironment.GetFragmentLight(i);
        ResFragmentLight resLight = light->GetResFragmentLight();
        s32 lightKind = resLight.GetLightKind();

        bool twoSideDiffuse = ut::CheckFlag(resLight.GetFlags(), ResFragmentLightData::FLAG_TWO_SIDE_DIFFUSE_ENABLED);
        bool isDirectional = (lightKind == ResFragmentLight::KIND_DIRECTIONAL) || GraphicsDevice::GetFragmentLightPositionW(i);

        i::ActivateFragmentLightParameters(i, isDirectional, twoSideDiffuse, geomFactor0, geomFactor1);
    }
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateTextureMappers(const ResMaterial texMapperMaterial)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateTextureMappers");
#endif

#if defined(MATERIAL_SET_ENABLED)
    u32 samplerSetting = 0;

    for ( s32 textureUnit = 0; textureUnit < texMapperMaterial.GetTextureMappersCount(); textureUnit++)
    {
        const ResTextureMapper textureMapper = texMapperMaterial.GetTextureMappers(textureUnit);
        if ( ! textureMapper.IsValid() )
        {
            continue;
        }

        samplerSetting |= 1 << textureUnit;

        // TODO: ShadowTextureMapper is not yet supported.
        const ResPixelBasedTextureMapper pixTexMapper = ResStaticCast<const ResPixelBasedTextureMapper>(textureMapper);

        // The content of 0x81-0x8a, 0x8e[3:0] is included here
        const u32* command = pixTexMapper.GetCommandCache();
        u32  commandSize   = pixTexMapper.GetCommandSizeToSend();

        i::NWUseCmdlist(command, commandSize);
    }

    const u32 HEADER = i::MakeCommandHeader(PICA_REG_TEXTURE_FUNC, 1, false, 0x5);

    u32 INVALIDATE_TEXTURE_CACHE[] =
    {
        samplerSetting | 0x00010000,
        HEADER
    };

    i::NWUseCmdlist<sizeof(INVALIDATE_TEXTURE_CACHE)>(&INVALIDATE_TEXTURE_CACHE[0]);
#endif
}


//----------------------------------------
NW_INLINE void
MaterialState::ActivateParticleTextureMappers(const ResMaterial texMapperMaterial)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateParticleTextureMappers");
#endif

#if defined(MATERIAL_SET_ENABLED)
    // TODO: Procedural textures are not yet supported
    const ResTextureMapper textureMapper = texMapperMaterial.GetTextureMappers(0);
    if ( textureMapper.IsValid() )
    {
        // TODO: ShadowTextureMapper is not yet supported.
        const ResPixelBasedTextureMapper pixTexMapper = ResStaticCast<const ResPixelBasedTextureMapper>(textureMapper);

        // The content of 0x81-0x8a, 0x8e[3:0] is included here
        const u32* command = pixTexMapper.GetCommandCache();
        u32  commandSize   = pixTexMapper.GetCommandSizeToSend();

        i::NWUseCmdlist(command, commandSize);
    }

    const u32 HEADER = i::MakeCommandHeader(PICA_REG_TEXTURE_FUNC, 1, false, 0x5);

    const u32 INVALIDATE_TEXTURE_CACHE[] =
    {
        0x00010001,
        HEADER
    };
    i::NWUseCmdlist<sizeof(INVALIDATE_TEXTURE_CACHE)>(&INVALIDATE_TEXTURE_CACHE[0]);
#endif
}


//----------------------------------------
NW_INLINE void
MaterialState::ActivateProceduralTextureMapper(const ResMaterial material)
{

#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateProceduralTextureMapper");
#endif

    const ResProceduralTextureMapper textureMapper = material.GetProceduralTextureMapper();

    if ( ! textureMapper.IsValid() )
    {
        return;
    }

    const ResProceduralTexture texture = ResStaticCast<const ResProceduralTexture>( textureMapper.GetTexture().Dereference() );

    i::NWUseCmdlist( texture.GetParameterCommand(), texture.GetParameterCommandCount() );
    i::NWUseCmdlist( texture.GetNoiseTableCommand(), texture.GetNoiseTableCommandCount() );
    i::NWUseCmdlist( texture.GetMappingTableCommand(), texture.GetMappingTableCommandCount() );
    i::NWUseCmdlist( texture.GetAlphaMappingTableCommand(), texture.GetAlphaMappingTableCommandCount() );
    i::NWUseCmdlist( texture.GetColorTableCommand(), texture.GetColorTableCommandCount() );
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateTextureCombiners(
    const ResFragmentShader fragmentShader,
    const ResMaterialColor materialColor)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateTextureCombiners");
#endif

#if defined(MATERIAL_SET_ENABLED)
#ifdef NW_TARGET_CTR_GL_FINAL
    i::NWUseCmdlist<sizeof(fragmentShader.ref().m_BufferCommand)>(&(fragmentShader.ref().m_BufferCommand[0]));
#endif

    for (uint stage = 0; stage < COMBINER_AVAILABLE_COUNT; stage++)
    {
        ResTextureCombiner textureCombiner = fragmentShader.GetTextureCombiners(stage);

        // When we attempted to measure it, at present there was no significant difference in the processing overhead, so make it such that constant colors are referenced from material colors.
        // 
        u32 constantU32 = GetConstantColorU32(
            textureCombiner.GetConstant(),
            materialColor);

        textureCombiner.SetConstantCmd( constantU32 );

        i::NWUseCmdlist<sizeof(textureCombiner.ref().m_CommandBuffer)>( reinterpret_cast<const u32*>(textureCombiner.ref().m_CommandBuffer) );
    }
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateAlphaTest(const ResAlphaTest alphaTest)
{
#if defined(NW_MATERIAL_PROFILE)
    NW_PROFILE("MaterialState::ActivateAlphaTest");
#endif

#if defined(MATERIAL_SET_ENABLED)
    NW_ASSERT(alphaTest.IsValid());
    i::NWUseCmdlist<sizeof(alphaTest.ref().m_CommandBuffer)>( &alphaTest.ref().m_CommandBuffer[0] );
#endif
}

//----------------------------------------
NW_INLINE void
MaterialState::ActivateFragmentOperation(const ResFragmentOperation fragmentOperation)
{
#if defined(MATERIAL_SET_ENABLED)
    ResDepthOperation resDepthOperation = fragmentOperation.GetDepthOperation();
    ResStencilOperation resStencilOperation = fragmentOperation.GetStencilOperation();
    ResBlendOperation resBlendOperation = fragmentOperation.GetBlendOperation();

    ActivateDepthOperation(resDepthOperation);
    ActivateStencilOperation(resStencilOperation);
    ActivateBlendOperation(resBlendOperation);

    GraphicsDevice::SetFragmentOperationMode(fragmentOperation.GetFragmentOperationMode());
    GraphicsDevice::ActivateFrameBuffer();

    // NOTE:
    // With the GraphicsDevice::ActivateFrameBuffer function, after issuing a command for clearing the frame buffer cache, read/write flag setting is issued for the 0x112-0x115 frame buffer.
    // 
    //
    // A case has been reported of being able to solve the problem of having block noise in the frame buffer while the hardware is still being scanned by issuing another command to clear the frame buffer cache after setting this read/write flag.
    // 
    // 
#endif
}

} // namespace internal
} // namespace gfx
} // namespace nw

#endif // NW_GFX_MATERIALSTATE_H_
