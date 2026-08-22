#pragma once

#include <nw/types.h>

#include <gles2/gl2.h>
#include <gles2/gl2extern.h>
#include <nn/gx.h>

#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/res/gfx_ResMaterial.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>

namespace nw{
namespace gfx{

class ShaderProgram;

namespace internal{

void ClearVertexAttribute();
s32 SetupDrawIndexStreamCommand(CommandBufferInfo& bufferInfo,ResIndexStream indexStream,bool hasGeometryShader);
s32 CalcSetupDrawIndexStreamCommand(ResIndexStream indexStream);
s32 SetupShaderProgramMode(bool useGeometry);

class CommandCacheHelper{
public:
    inline static u32 MultU32Color(u32 lhs, u32 rhs){
        const u32 MASKx2 = (0xff << 1);

        u32 r = ((((lhs << 1) & MASKx2) + 1) * (((rhs << 1) & MASKx2) + 1)) >> 10;
        u32 g = ((((lhs >> 7) & MASKx2) + 1) * (((rhs >> 7) & MASKx2) + 1)) >> 10;
        u32 b = ((((lhs >> 15) & MASKx2) + 1) * (((rhs >> 15) & MASKx2) + 1)) >> 10;

        return (r << 20) | (g << 10) | b;
    }

    inline static u32 MultAddU32Color(u32 lhs, u32 rhs, u32 add){
        const u32 MASK = 0xff;
        const u32 MASKx2 = (0xff << 1);

        u32 r = (((((lhs << 1) & MASKx2) + 1) * (((rhs << 1) & MASKx2) + 1)) >> 10) + (add & MASK);
        u32 g = (((((lhs >> 7) & MASKx2) + 1) * (((rhs >> 7) & MASKx2) + 1)) >> 10) + ((add >> 8) & MASK);
        u32 b = (((((lhs >> 15) & MASKx2) + 1) * (((rhs >> 15) & MASKx2) + 1)) >> 10) + ((add >> 16) & MASK);

        return
            (nw::ut::Clamp<u32>(r, 0, 255) << 20) |
            (nw::ut::Clamp<u32>(g, 0, 255) << 10) |
            nw::ut::Clamp<u32>(b, 0, 255);
    }

    inline static u32 GetVertexFormat(s32 dimension, GLuint format){
        u32 result = 0;

        switch(format){
        case GL_BYTE:
            result = 0;
            break;

        case GL_UNSIGNED_BYTE:
            result = 1;
            break;

        case GL_SHORT:
            result = 2;
            break;

        case GL_FLOAT:
            result = 3;
            break;

        default:
            NW_FATAL_ERROR("unknown format");
        }

        return result + ((dimension - 1) << 2);
    }

    inline static u32 GetVertexSize(s32 dimension, GLuint format){
        switch(format){
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
            return dimension;

        case GL_SHORT:
            return dimension * 2;

        case GL_FLOAT:
            return dimension * 4;

        default:
            NW_FATAL_ERROR("unknown format");
        }

        return 0;
    }
};

inline void ActivateFragmentLightCount(s32 count){
    enum { 
        REG_FRAGMENT_LIGHT_COUNT    = 0x1c2,
        REG_FRAGMENT_LIGHT_MASK     = 0x1d9
    };
    
    const u32 COUNT_HEADER    = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_COUNT, 1, false, 0x1);
    const u32 LIGHT_MASK_HEADER = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_MASK, 1, false, 0xF);
    
    u32 LIGHT_COUNT_COMMAND[] ={
        0,
        COUNT_HEADER
        ,0,
        LIGHT_MASK_HEADER
    };
    
    LIGHT_COUNT_COMMAND[0] = (count > 0) ? count - 1 : 0;

    for (int i = 0; i < count; ++i){
        LIGHT_COUNT_COMMAND[2] |= i << (4 * i);
    }
    
    internal::NWUseCmdlist<sizeof(LIGHT_COUNT_COMMAND)>( &LIGHT_COUNT_COMMAND[0] );
}

inline void ActivateFragmentLight(s32 index, ResMaterialColor materialColor, const FragmentLight* light, bool useReflection){
    enum { REG_FRAGMENT_COLOR_BASE = 0x140 };
    
    const u32 HEADER = internal::MakeCommandHeader(REG_FRAGMENT_COLOR_BASE, 4, true, 0xF);
    
    NW_NULL_ASSERT( light );
    
    ResFragmentLight resLight = light->GetResFragmentLight();
    
    u32 specular0U32  = CommandCacheHelper::MultU32Color(resLight.GetSpecular0U32(), materialColor.GetSpecular0U32());
    u32 diffuseU32    = CommandCacheHelper::MultU32Color(resLight.GetDiffuseU32(), materialColor.GetDiffuseU32());
    u32 ambientU32    = CommandCacheHelper::MultU32Color(resLight.GetAmbientU32(), materialColor.GetAmbientU32());
    u32 specular1U32  = resLight.GetSpecular1U32();
    
    if (useReflection){
        const u32 MASK = 0xff;
        u8 r = (specular1U32 & MASK);
        u8 g = ((specular1U32 >> 8) & MASK);
        u8 b = ((specular1U32 >> 16) & MASK);
        specular1U32 = (static_cast<u32>(r) << 20) | (static_cast<u32>(g) << 10) | (static_cast<u32>(b));
    }
    else{
        specular1U32 = CommandCacheHelper::MultU32Color(resLight.GetSpecular1U32(), materialColor.GetSpecular1U32());
    }
    
    u32 LIGHT_COMMAND[6] ={
        specular0U32,
        HEADER + (0x10 * index),
        specular1U32,
        diffuseU32,
        ambientU32,
        0
    };
    
    internal::NWUseCmdlist<sizeof(LIGHT_COMMAND)>( &LIGHT_COMMAND[0] );
}

inline void ActivateFragmentAmbientLight(ResMaterialColor materialColor, const AmbientLight* light){
    enum { REG_FRAGMENT_COLOR_AMBIENT = 0x1c0 };
    
    u32 ambientU32 = 0x0;
    const u32 HEADER = internal::MakeCommandHeader(REG_FRAGMENT_COLOR_AMBIENT, 1, false, 0xF);
    
    if (light){
        ResAmbientLight resLight = light->GetResAmbientLight();
        ambientU32 = CommandCacheHelper::MultAddU32Color(resLight.GetAmbientU32(), materialColor.GetAmbientU32(), materialColor.GetEmissionU32());
    }
    else{
        u32 emissionU32 = materialColor.GetEmissionU32();
        const u32 MASK = 0xff;
        u8 r = (emissionU32 & MASK);
        u8 g = ((emissionU32 >> 8) & MASK);
        u8 b = ((emissionU32 >> 16) & MASK);
        ambientU32 = (static_cast<u32>(r) << 20) | (static_cast<u32>(g) << 10) | (static_cast<u32>(b));
    }
    
    u32 LIGHT_COMMAND[2] ={
        ambientU32,
        HEADER
    };
    
    internal::NWUseCmdlist<sizeof(LIGHT_COMMAND)>( &LIGHT_COMMAND[0] );
}

inline void ActivateFragmentLightParameters(s32 index, bool isDirectional, bool twoSided, bool geomFactor0, bool geomFactor1){
    enum {
        REG_FRAGMENT_LIGHT_TYPE = 0x149
    };
    
    const u32 HEADER = internal::MakeCommandHeader(REG_FRAGMENT_LIGHT_TYPE, 1, false, 0x1);
    
    u32 LIGHT_COMMAND[2] ={
        0, HEADER + 0x10 * index
    };
    
    if (isDirectional){
        LIGHT_COMMAND[0] |= 0x1;
    }
    
    if (twoSided){
        LIGHT_COMMAND[0] |= 0x2;
    }
    
    if (geomFactor0){
        LIGHT_COMMAND[0] |= 0x4;
    }
    
    if (geomFactor1){
        LIGHT_COMMAND[0] |= 0x8;
    }
    
    internal::NWUseCmdlist<sizeof(LIGHT_COMMAND)>( &LIGHT_COMMAND[0] );
}

inline void ActivateFragmentLighting(const ResFragmentLighting fragmentLighting){
    const u32 REG_LIGHT_ENV  = 0x1c3;
    const u32 REG_LIGHT_ENV2 = 0x1c4;

    const u32 HEADER  = internal::MakeCommandHeader(REG_LIGHT_ENV,  1, false, 0xf);
    const u32 HEADER2 = internal::MakeCommandHeader(REG_LIGHT_ENV2, 1, false, 0x4);
    
    u32 LIGHT_COMMAND[4] ={
        0, HEADER, 0, HEADER2
    };
    
    u32 layerConfig     = fragmentLighting.GetLayerConfig();
    u32 fresnelSelector = fragmentLighting.GetFresnelConfig();
    u32 bumpSelector    = fragmentLighting.GetBumpTextureIndex();
    u32 bumpMode        = fragmentLighting.GetBumpMode();

    bool bumpRenorm      = fragmentLighting.IsBumpRenormalize() || (bumpMode == 0);
    bool clampHighlights = ut::CheckFlag(fragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_CLAMP_HIGH_LIGHT);
    
    LIGHT_COMMAND[0] = fresnelSelector << 2 |layerConfig << 4 | bumpSelector << 22 | (clampHighlights ? 1 : 0) << 27 | bumpMode << 28 | 0x1u << 10 | (bumpRenorm ? 0 : 1) << 30 | 0x1u << 31;
    
    bool lutEnabledD0 = ut::CheckFlag(fragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_DISTRIBUTION0_ENABLED);
    bool lutEnabledD1 = ut::CheckFlag(fragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_DISTRIBUTION1_ENABLED);
    bool lutEnabledRefl = ut::CheckFlag(fragmentLighting.GetFlags(), ResFragmentLightingData::FLAG_REFLECTION_ENABLED);
    
    LIGHT_COMMAND[2] = (lutEnabledD0 ? 0 : 1) << 16 | (lutEnabledD1 ? 0 : 1) << 17 | 1 << 18 | ((fresnelSelector == 0) ? 1 : 0) << 19 | ((lutEnabledRefl ? 0 : 7) << 20);
    
    internal::NWUseCmdlist<sizeof(LIGHT_COMMAND)>( &LIGHT_COMMAND[0] );
}

}
}
}