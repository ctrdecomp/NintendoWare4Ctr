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

#ifndef NW_GFX_RESPROCEDURALTEXTURE_H_
#define NW_GFX_RESPROCEDURALTEXTURE_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

//
struct ResProceduralTextureData : public ResTextureData
{
    bool m_IsNoiseEnabled;
    bool m_IsAlphaSeparate;
    u8 _padding_0[2];
    nw::ut::ResF32 m_NoiseUFrequency;
    nw::ut::ResF32 m_NoiseUPhase;
    nw::ut::ResF32 m_NoiseUAmplitude;
    nw::ut::ResF32 m_NoiseVFrequency;
    nw::ut::ResF32 m_NoiseVPhase;
    nw::ut::ResF32 m_NoiseVAmplitude;

    nw::ut::ResU32 m_ClampU;
    nw::ut::ResU32 m_ClampV;
    nw::ut::ResU32 m_ShiftU;
    nw::ut::ResU32 m_ShiftV;

    nw::ut::ResU32 m_MappingFunction;
    nw::ut::ResU32 m_AlphaMappingFunction;

    nw::ut::ResS32 m_TextureWidth;
    nw::ut::ResS32 m_TextureOffset;
    nw::ut::ResF32 m_TextureLodBias;
    nw::ut::ResU32 m_MinFilter;

    nw::ut::ResU8 m_ParameterCommand[48];
    nw::ut::ResS32 m_NoiseTableCommandTableCount;
    nw::ut::Offset toNoiseTableCommandTable;
    nw::ut::ResS32 m_MappingTableCommandTableCount;
    nw::ut::Offset toMappingTableCommandTable;
    nw::ut::ResS32 m_AlphaMappingTableCommandTableCount;
    nw::ut::Offset toAlphaMappingTableCommandTable;
    nw::ut::ResS32 m_ColorTableCommandTableCount;
    nw::ut::Offset toColorTableCommandTable;
};


//--------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class ResProceduralTexture : public ResTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResProceduralTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXPR') };

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

    enum MinFilter
    {
        MINFILTER_NEAREST                = GL_NEAREST,
        MINFILTER_LINEAR                 = GL_LINEAR,
        MINFILTER_NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        MINFILTER_NEAREST_MIPMAP_LINEAR  = GL_NEAREST_MIPMAP_LINEAR,
        MINFILTER_LINEAR_MIPMAP_NEAREST  = GL_LINEAR_MIPMAP_NEAREST,
        MINFILTER_LINEAR_MIPMAP_LINEAR   = GL_LINEAR_MIPMAP_LINEAR
    };

    NW_RES_CTOR_INHERIT( ResProceduralTexture, ResTexture )

    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( NoiseEnabled ) // IsNoiseEnabled(), GetSetNoiseEnabled()
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( AlphaSeparate )  // IsAlphaSeparate(), SetAlphaSeparate()

    NW_RES_FIELD_PRIMITIVE_DECL( f32, NoiseUFrequency ) // GetNoiseUFrequency(), SetNoiseUFrequency()
    NW_RES_FIELD_PRIMITIVE_DECL( f32, NoiseUPhase )     // GetNoiseUPhase(), SetNoiseUPhase()
    NW_RES_FIELD_PRIMITIVE_DECL( f32, NoiseUAmplitude ) // GetNoiseUAmplitude(), SetNoiseUAmplitude()
    NW_RES_FIELD_PRIMITIVE_DECL( f32, NoiseVFrequency ) // GetNoiseVFrequency(), SetNoiseVFrequency()
    NW_RES_FIELD_PRIMITIVE_DECL( f32, NoiseVPhase )     // GetNoiseVPhase(), SetNoiseVPhase()
    NW_RES_FIELD_PRIMITIVE_DECL( f32, NoiseVAmplitude ) // GetNoiseVAmplitude(), SetNoiseVAmplitude()

    NW_RES_FIELD_PRIMITIVE_DECL( u32, ClampU ) // GetClampU(), SetClampU()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ClampV ) // GetClampV(), SetClampV()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ShiftU ) // GetShiftU(), SetShiftU()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ShiftV ) // GetShiftV(), SetShiftV()

    NW_RES_FIELD_PRIMITIVE_DECL( u32, MappingFunction )      // GetMappingFunction(), SetMappingFunction()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, AlphaMappingFunction ) // GetAlphaMappingFunction(), SetAlphaMappingFunction()

    NW_RES_FIELD_PRIMITIVE_DECL( s32, TextureWidth )    // GetTextureWidth(), SetTextureWidth()
    NW_RES_FIELD_PRIMITIVE_DECL( s32, TextureOffset )   // GetTextureOffset(), SetTextureOffset()
    NW_RES_FIELD_PRIMITIVE_DECL( f32, TextureLodBias )  // GetTextureLodBias(), SetTextureLodBias()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, MinFilter )       // GetMinFilter(), SetMinFilter()

    const u8* GetParameterCommand() const { return ref().m_ParameterCommand; }
    u8* GetParameterCommand() { return ref().m_ParameterCommand; }
    s32 GetParameterCommandCount() const { return sizeof( ref().m_ParameterCommand ); }

    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, NoiseTableCommand )        // GetNoiseTableCommand(), GetNoiseTableCommandCount()
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, MappingTableCommand )      // GetMappingTableCommand(), GetMappingTableCommandCount()
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, AlphaMappingTableCommand ) // GetAlphaMappingTableCommand(), GetAlphaMappingTableCommandCount()
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, ColorTableCommand )        // GetColorTableCommand(), GetColorTableCommandCount()

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    class DynamicBuilder
    {
    public:
        //
        DynamicBuilder() {}
        //
        ~DynamicBuilder() {}

        //
        //
        //
        //
        //
        //
        ResProceduralTexture Create(os::IAllocator* allocator);
    };

    //
    //
    void DynamicDestroy();

};

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESPROCEDURALTEXTURE_H_
