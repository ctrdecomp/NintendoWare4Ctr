#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResProceduralNoiseData{
    nw::ut::ResBool mIsNoiseEnabled;
    u8 _padding_0[3];
    nw::ut::ResVec3 mNoiseU;
    nw::ut::ResVec3 mNoiseV;
    nw::ut::Offset toNoiseTable;
};

struct ResProceduralClampShiftData{
    nw::ut::ResU32 mClampU;
    nw::ut::ResU32 mClampV;
    nw::ut::ResU32 mShiftU;
    nw::ut::ResU32 mShiftV;
};

struct ResProceduralMappingData{
    nw::ut::ResBool mIsAlphaSeparate;
    u8 _padding_0[3];
    nw::ut::ResU32 mMappingFunction;
    nw::ut::ResU32 mAlphaMappingFunction;
    nw::ut::Offset toMappingTable;
    nw::ut::Offset toAlphaMappingTable;
};

struct ResProceduralColorData{
    nw::ut::ResS32 mTextureWidth;
    nw::ut::ResS32 mTextureOffset;
    nw::ut::ResF32 mTextureLodBias;
    nw::ut::ResU32 mMinFilter;
    nw::ut::Offset toColorTables[4];
};

struct ResProceduralTextureData : public ResTextureData{
    ResProceduralNoiseData mNoise;
    ResProceduralClampShiftData mClampShift;
    ResProceduralMappingData mMapping;
    ResProceduralColorData mColor;
};

class ResProceduralNoise : public nw::ut::ResCommon<ResProceduralNoiseData>{
public:
    NW_RES_CTOR(ResProceduralNoise)

    NW_RES_FIELD_BOOL_PRIMITIVE_DECL(NoiseEnabled)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, NoiseU)
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, NoiseV)
    NW_RES_FIELD_CLASS_DECL(ResImageLookupTable, NoiseTable)
};

class ResProceduralClampShift : public nw::ut::ResCommon<ResProceduralClampShiftData>{
public:
    enum Clamp{
        CLAMP_REPEAT    = GL_SYMMETRICAL_REPEAT_DMP,
        CLAMP_MIRRORED  = GL_MIRRORED_REPEAT,
        CLAMP_PULSE     = GL_PULSE_DMP,
        CLAMP_EDGE      = GL_CLAMP_TO_EDGE,
        CLAMP_ZERO      = GL_CLAMP_TO_ZERO_DMP
    };
    
    enum Shift{
        SHIFT_EVEN = GL_EVEN_DMP,
        SHIFT_ODD  = GL_ODD_DMP,
        SHIFT_NONE = GL_NONE_DMP
    };
    
    NW_RES_CTOR( ResProceduralClampShift )

    NW_RES_FIELD_PRIMITIVE_DECL(Clamp, ClampU)
    NW_RES_FIELD_PRIMITIVE_DECL(Clamp, ClampV)
    NW_RES_FIELD_PRIMITIVE_DECL(Shift, ShiftU)
    NW_RES_FIELD_PRIMITIVE_DECL(Shift, ShiftV)
};

class ResProceduralMapping : public nw::ut::ResCommon<ResProceduralMappingData>{
public:
    enum Function{
        FUNCTION_U        = GL_PROCTEX_U_DMP,
        FUNCTION_V        = GL_PROCTEX_V_DMP,
        FUNCTION_U2       = GL_PROCTEX_U2_DMP,
        FUNCTION_V2       = GL_PROCTEX_V2_DMP,
        FUNCTION_ADD      = GL_PROCTEX_ADD_DMP,
        FUNCTION_ADD2     = GL_PROCTEX_ADD2_DMP,
        FUNCTION_ADDSQRT2 = GL_PROCTEX_ADDSQRT2_DMP,
        FUNCTION_MIN      = GL_PROCTEX_MIN_DMP,
        FUNCTION_MAX      = GL_PROCTEX_MAX_DMP,
        FUNCTION_RMAX     = GL_PROCTEX_RMAX_DMP
    };
    
    NW_RES_CTOR( ResProceduralMapping )

    NW_RES_FIELD_BOOL_PRIMITIVE_DECL(AlphaSeparate)
    NW_RES_FIELD_PRIMITIVE_DECL(Function, MappingFunction)
    NW_RES_FIELD_PRIMITIVE_DECL(Function, AlphaMappingFunction)
    NW_RES_FIELD_CLASS_DECL(ResImageLookupTable, MappingTable)
    NW_RES_FIELD_CLASS_DECL(ResImageLookupTable, AlphaMappingTable)
};

class ResProceduralColor : public nw::ut::ResCommon<ResProceduralColorData>{
public:
    enum MinFilter{
        MINFILTER_NEAREST                = GL_NEAREST,
        MINFILTER_LINEAR                 = GL_LINEAR,
        MINFILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        MINFILTER_NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
        MINFILTER_LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
        MINFILTER_LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_LINEAR
    };

    NW_RES_CTOR( ResProceduralColor )

    NW_RES_FIELD_PRIMITIVE_DECL(s32, TextureWidth)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, TextureOffset)
    NW_RES_FIELD_PRIMITIVE_DECL(f32, TextureLodBias)
    NW_RES_FIELD_PRIMITIVE_DECL(MinFilter, MinFilter)
    NW_RES_FIELD_CLASS_FIXED_LIST_DECL(ResImageLookupTable, ColorTables)
};

class ResProceduralTexture : public ResTexture{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResProceduralTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXPR') };
    
    NW_RES_CTOR_INHERIT(ResProceduralTexture, ResTexture)

    NW_RES_FIELD_RESSTRUCT_DECL(ResProceduralNoise, Noise)
    NW_RES_FIELD_RESSTRUCT_DECL(ResProceduralClampShift, ClampShift)
    NW_RES_FIELD_RESSTRUCT_DECL(ResProceduralMapping, Mapping)
    NW_RES_FIELD_RESSTRUCT_DECL(ResProceduralColor, Color)
};

}
}
}