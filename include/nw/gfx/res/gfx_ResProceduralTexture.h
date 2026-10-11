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

struct ResProceduralNoiseData
{
    nw::ut::ResBool m_IsNoiseEnabled;
    u8 _padding_0[3];
    nw::ut::ResVec3 m_NoiseU;
    nw::ut::ResVec3 m_NoiseV;
    nw::ut::Offset toNoiseTable;
};

struct ResProceduralClampShiftData
{
    nw::ut::ResU32 m_ClampU;
    nw::ut::ResU32 m_ClampV;
    nw::ut::ResU32 m_ShiftU;
    nw::ut::ResU32 m_ShiftV;
};

struct ResProceduralMappingData
{
    nw::ut::ResBool m_IsAlphaSeparate;
    u8 _padding_0[3];
    nw::ut::ResU32 m_MappingFunction;
    nw::ut::ResU32 m_AlphaMappingFunction;
    nw::ut::Offset toMappingTable;
    nw::ut::Offset toAlphaMappingTable;
};

struct ResProceduralColorData
{
    nw::ut::ResS32 m_TextureWidth;
    nw::ut::ResS32 m_TextureOffset;
    nw::ut::ResF32 m_TextureLodBias;
    nw::ut::ResU32 m_MinFilter;
    nw::ut::Offset toColorTables[4];
};

struct ResProceduralTextureData : public ResTextureData
{
    ResProceduralNoiseData m_Noise;
    ResProceduralClampShiftData m_ClampShift;
    ResProceduralMappingData m_Mapping;
    ResProceduralColorData m_Color;
};

class ResProceduralNoise : public nw::ut::ResCommon<ResProceduralNoiseData>
{
public:
    NW_RES_CTOR(ResProceduralNoise)


    bool IsNoiseEnabled() const { return static_cast<bool>(ref().m_IsNoiseEnabled); }
    void SetNoiseEnabled(bool value) { ref().m_IsNoiseEnabled = value; }

    void SetNoiseU(f32 x, f32 y, f32 z) { ref().m_NoiseU.Set(x, y, z); }
    const nw::math::VEC3& GetNoiseU() const { return ref().m_NoiseU; }
    void SetNoiseU(const nw::math::VEC3& value) { ref().m_NoiseU = value; }

    void SetNoiseV(f32 x, f32 y, f32 z) { ref().m_NoiseV.Set(x, y, z); }
    const nw::math::VEC3& GetNoiseV() const { return ref().m_NoiseV; }
    void SetNoiseV(const nw::math::VEC3& value) { ref().m_NoiseV = value; }

    ResImageLookupTable GetNoiseTable() { return ResImageLookupTable(ref().toNoiseTable.to_ptr()); }
    const ResImageLookupTable GetNoiseTable() const { return ResImageLookupTable(ref().toNoiseTable.to_ptr()); }
};

class ResProceduralClampShift : public nw::ut::ResCommon<ResProceduralClampShiftData>
{
public:
    enum Clamp
    {
        CLAMP_REPEAT    = GL_SYMMETRICAL_REPEAT_DMP,
        CLAMP_MIRRORED  = GL_MIRRORED_REPEAT,
        CLAMP_PULSE     = GL_PULSE_DMP,
        CLAMP_EDGE      = GL_CLAMP_TO_EDGE,
        CLAMP_ZERO      = GL_CLAMP_TO_ZERO_DMP
    };
    
    enum Shift
    {
        SHIFT_EVEN = GL_EVEN_DMP,
        SHIFT_ODD  = GL_ODD_DMP,
        SHIFT_NONE = GL_NONE_DMP
    };
    
    NW_RES_CTOR( ResProceduralClampShift )


    Clamp GetClampU() const { return static_cast<Clamp>(ref().m_ClampU); }
    void SetClampU(Clamp value) { ref().m_ClampU = value; }

    Clamp GetClampV() const { return static_cast<Clamp>(ref().m_ClampV); }
    void SetClampV(Clamp value) { ref().m_ClampV = value; }

    Shift GetShiftU() const { return static_cast<Shift>(ref().m_ShiftU); }
    void SetShiftU(Shift value) { ref().m_ShiftU = value; }

    Shift GetShiftV() const { return static_cast<Shift>(ref().m_ShiftV); }
    void SetShiftV(Shift value) { ref().m_ShiftV = value; }
};

class ResProceduralMapping : public nw::ut::ResCommon<ResProceduralMappingData>
{
public:
    enum Function
    {
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


    bool IsAlphaSeparate() const { return static_cast<bool>(ref().m_IsAlphaSeparate); }
    void SetAlphaSeparate(bool value) { ref().m_IsAlphaSeparate = value; }

    Function GetMappingFunction() const { return static_cast<Function>(ref().m_MappingFunction); }
    void SetMappingFunction(Function value) { ref().m_MappingFunction = value; }

    Function GetAlphaMappingFunction() const { return static_cast<Function>(ref().m_AlphaMappingFunction); }
    void SetAlphaMappingFunction(Function value) { ref().m_AlphaMappingFunction = value; }

    ResImageLookupTable GetMappingTable() { return ResImageLookupTable(ref().toMappingTable.to_ptr()); }
    const ResImageLookupTable GetMappingTable() const { return ResImageLookupTable(ref().toMappingTable.to_ptr()); }

    ResImageLookupTable GetAlphaMappingTable() { return ResImageLookupTable(ref().toAlphaMappingTable.to_ptr()); }
    const ResImageLookupTable GetAlphaMappingTable() const { return ResImageLookupTable(ref().toAlphaMappingTable.to_ptr()); }
};

class ResProceduralColor : public nw::ut::ResCommon<ResProceduralColorData>
{
public:
    enum MinFilter
    {
        MINFILTER_NEAREST                = GL_NEAREST,
        MINFILTER_LINEAR                 = GL_LINEAR,
        MINFILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        MINFILTER_NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
        MINFILTER_LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
        MINFILTER_LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_LINEAR
    };

    NW_RES_CTOR( ResProceduralColor )


    s32 GetTextureWidth() const { return static_cast<s32>(ref().m_TextureWidth); }
    void SetTextureWidth(s32 value) { ref().m_TextureWidth = value; }

    s32 GetTextureOffset() const { return static_cast<s32>(ref().m_TextureOffset); }
    void SetTextureOffset(s32 value) { ref().m_TextureOffset = value; }

    f32 GetTextureLodBias() const { return static_cast<f32>(ref().m_TextureLodBias); }
    void SetTextureLodBias(f32 value) { ref().m_TextureLodBias = value; }

    MinFilter GetMinFilter() const { return static_cast<MinFilter>(ref().m_MinFilter); }
    void SetMinFilter(MinFilter value) { ref().m_MinFilter = value; }

    s32 GetColorTablesCount() const
        { return sizeof(ref().toColorTables) / sizeof(nw::ut::Offset); }
    ResImageLookupTable GetColorTables(int idx)
        { NW_INDEX_ASSERT(ColorTables, idx); return ResImageLookupTable(ref().toColorTables[idx].to_ptr()); }
    const ResImageLookupTable GetColorTables(int idx) const
        { return ResImageLookupTable(ref().toColorTables[idx].to_ptr()); }
    nw::ut::internal::ResArray<ResImageLookupTable, nw::ut::internal::ResArrayClassTraits> GetColorTables()
        { return nw::ut::internal::ResArray<ResImageLookupTable, nw::ut::internal::ResArrayClassTraits>(&ref().toColorTables[0], GetColorTablesCount()); }
    const nw::ut::internal::ResArray<const ResImageLookupTable, nw::ut::internal::ResArrayClassTraits> GetColorTables() const
        { return nw::ut::internal::ResArray<const ResImageLookupTable, nw::ut::internal::ResArrayClassTraits>(&ref().toColorTables[0], GetColorTablesCount()); }
};

class ResProceduralTexture : public ResTexture
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResProceduralTexture)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('TXPR')
    };
    
    NW_RES_CTOR_INHERIT(ResProceduralTexture, ResTexture)


    ResProceduralNoise GetNoise() { return ResProceduralNoise(&(ref().m_Noise)); }
    const ResProceduralNoise GetNoise() const { return ResProceduralNoise(&(ref().m_Noise)); }
    ResProceduralNoiseData& GetNoiseData() { return ref().m_Noise; }
    const ResProceduralNoiseData& GetNoiseData() const { return ref().m_Noise; }

    ResProceduralClampShift GetClampShift() { return ResProceduralClampShift(&(ref().m_ClampShift)); }
    const ResProceduralClampShift GetClampShift() const { return ResProceduralClampShift(&(ref().m_ClampShift)); }
    ResProceduralClampShiftData& GetClampShiftData() { return ref().m_ClampShift; }
    const ResProceduralClampShiftData& GetClampShiftData() const { return ref().m_ClampShift; }

    ResProceduralMapping GetMapping() { return ResProceduralMapping(&(ref().m_Mapping)); }
    const ResProceduralMapping GetMapping() const { return ResProceduralMapping(&(ref().m_Mapping)); }
    ResProceduralMappingData& GetMappingData() { return ref().m_Mapping; }
    const ResProceduralMappingData& GetMappingData() const { return ref().m_Mapping; }

    ResProceduralColor GetColor() { return ResProceduralColor(&(ref().m_Color)); }
    const ResProceduralColor GetColor() const { return ResProceduralColor(&(ref().m_Color)); }
    ResProceduralColorData& GetColorData() { return ref().m_Color; }
    const ResProceduralColorData& GetColorData() const { return ref().m_Color; }
};

}
}
}

