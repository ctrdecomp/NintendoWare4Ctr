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

#ifndef NW_GFX_RESMATERIAL_H_
#define NW_GFX_RESMATERIAL_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <nw/types.h>
#include <nw/ut/ut_Flag.h>
#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTextureMapper.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/res/gfx_ResShader.h>
#include <nw/gfx/res/gfx_ResFragmentShader.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/gfx_CommandUtil.h>

namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResMaterialColorData
{
    nw::ut::ResFloatColor m_Emission;       //
    nw::ut::ResFloatColor m_AmbientAndVertexColorScale; //
    nw::ut::ResFloatColor m_Diffuse;        //
    nw::ut::ResFloatColor m_Specular0;      //
    nw::ut::ResFloatColor m_Specular1;      //
    nw::ut::ResFloatColor m_Constant0;      //
    nw::ut::ResFloatColor m_Constant1;      //
    nw::ut::ResFloatColor m_Constant2;      //
    nw::ut::ResFloatColor m_Constant3;      //
    nw::ut::ResFloatColor m_Constant4;      //
    nw::ut::ResFloatColor m_Constant5;      //
    nw::ut::ResU32 m_EmissionU32;           //
    nw::ut::ResU32 m_AmbientU32;            //
    nw::ut::ResU32 m_DiffuseU32;            //
    nw::ut::ResU32 m_Specular0U32;          //
    nw::ut::ResU32 m_Specular1U32;          //
    nw::ut::ResU32 m_Constant0U32;          //
    nw::ut::ResU32 m_Constant1U32;          //
    nw::ut::ResU32 m_Constant2U32;          //
    nw::ut::ResU32 m_Constant3U32;          //
    nw::ut::ResU32 m_Constant4U32;          //
    nw::ut::ResU32 m_Constant5U32;          //

    void* m_CommandCache;                   //
};

// The format for ResRasterizationData::m_CommandBuffer is in the following format.
//
// [0] The data for 0x040
// [1] The header to 0x040 (BE:1)
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResRasterizationData
{
    //
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_POLYGON_OFFSET_ENABLED = 0x1 << 0      //
    };

    nw::ut::ResU32 m_Flags;                 //
    nw::ut::ResU32 m_CullingMode;           //
    nw::ut::ResF32 m_PolygonOffsetUnit;     //

    nw::ut::ResU32 m_CommandBuffer[2];      //
};

// The format for ResBlendOperationData::m_CommandBuffer is in the following format.
//
// [0] The data for 0x100
// [1] The header to 0x100 (BE:0xf)
// [2] The data for 0x101
// [3] The data for 0x102
// [4] The data for 0x103
// [5] padding
//
// If it is necessary to reference the destination color, the uppermost bit of m_Mode is enabled in runtime.
// 
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResBlendOperationData
{
    nw::ut::ResS32          m_Mode;         //
    nw::ut::ResFloatColor   m_BlendColor;   //

    nw::ut::ResU32 m_CommandBuffer[6];      //
};

// The format for ResDepthOperationData::m_CommandBuffer is in the following format.
//
// [0] The [7:0] for 0x107
// [1] The header to 0x107 (BE:1)
// [2] The [31:24] for 0x126
// [3] The header to 0x126 (BE:8)
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResDepthOperationData
{
    //
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_TEST_ENABLED = 0x1 << 0,   //
        FLAG_MASK_ENABLED = 0x1 << 1    //
    };

    nw::ut::ResU32 m_Flags;             //

    nw::ut::ResU32 m_CommandBuffer[4];  //
};

// The format for ResStencilOperationData::m_CommandBuffer is in the following format.
//
// [0] The data for 0x105
// [1] The header to 0x105 (BE:0xd)
// [2] The data for 0x106
// [3] The header to 0x106 (BE:0xf)
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResStencilOperationData
{
    nw::ut::ResU32 m_CommandBuffer[4];  //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResFragmentOperationData
{
    ResDepthOperationData     m_DepthOperation;     //
    ResBlendOperationData     m_BlendOperation;     //
    ResStencilOperationData   m_StencilOperation;   //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResTextureCoordinatorData
{
    nw::ut::ResS32    m_SourceCoordinate;   //
    nw::ut::ResS32    m_MappingMethod;      //
    nw::ut::ResS32    m_ReferenceCamera;    //
    nw::ut::ResS32    m_MatrixMode;         //
    nw::ut::ResVec2   m_Scale;              //
    nw::ut::ResF32    m_Rotate;             //
    nw::ut::ResVec2   m_Translate;          //
    nw::ut::ResBool   m_IsDirty;            //
    nw::ut::ResBool   m_IsEnabled;          //
    u8 _padding_0[2];                       //
    nw::ut::ResMtx34  m_TextureMatrix;      //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResMaterialData : public ResSceneObjectData
{
    //
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_FRAGMENTLIGHT_ENABLED = 0x1 << 0,          //
        FLAG_VERTEXLIGHT_ENABLED   = 0x1 << 1,          //
        FLAG_HEMISPHERELIGHT_ENABLED   = 0x1 << 2,      //
        FLAG_HEMISPHERE_OCCLUSION_ENABLED = 0x1 << 3,   //
        FLAG_FOG_ENABLED = 0x1 << 4,                    //
        FLAG_PARTICLE_MATERIAL_ENABLED = 0x1 << 5,      //
        FLAG_HAS_BEEN_SETUP = 0x1 << 6,                 //
        FLAG_HAS_BEEN_SETUP_SHADER = 0x1 << 7,          //
        FLAG_HAS_BEEN_SETUP_TEXTURE = 0x1 << 8,         //
        FLAG_HAS_BEEN_SETUP_FRAGMENTSHADER = 0x1 << 9   //
    };

    nw::ut::ResU32 m_Flags;                                 //
    nw::ut::ResS32 m_TextureCoordinateConfig;               //
    nw::ut::ResS32 m_TranslucencyKind;                      //
    ResMaterialColorData m_MaterialColor;                   //
    ResRasterizationData m_Rasterization;                   //
    ResFragmentOperationData m_FragmentOperation;           //
    nw::ut::ResS32 m_ActiveTextureCoordinatorsCount;        //
    ResTextureCoordinatorData m_TextureCoordinators[3];     //
    nw::ut::Offset toTextureMappers[3];                     //
    nw::ut::Offset toProceduralTextureMapper;               //
    nw::ut::Offset toShader;                                //
    nw::ut::Offset toFragmentShader;                        //
    nw::ut::ResS32 m_ShaderProgramDescriptionIndex;         //
    nw::ut::ResS32 m_ShaderParametersTableCount;            //
    nw::ut::Offset toShaderParametersTable;                 //
    nw::ut::ResS32 m_LightSetIndex;                         //
    nw::ut::ResS32 m_FogIndex;                              //
    nw::ut::ResU32 m_ShadingParameterHash;                  //
    nw::ut::ResU32 m_ShaderParametersHash;                  //
    nw::ut::ResU32 m_TextureCoordinatorsHash;               //
    nw::ut::ResU32 m_TextureSamplersHash;                   //
    nw::ut::ResU32 m_TextureMappersHash;                    //
    nw::ut::ResU32 m_MaterialColorHash;                     //
    nw::ut::ResU32 m_RasterizationHash;                     //
    nw::ut::ResU32 m_FragmentLightingHash;                  //
    nw::ut::ResU32 m_FragmentLightingTableHash;             //
    nw::ut::ResU32 m_FragmentLightingTableParametersHash;   //
    nw::ut::ResU32 m_TextureCombinersHash;                  //
    nw::ut::ResU32 m_AlphaTestHash;                         //
    nw::ut::ResU32 m_FragmentOperationHash;                 //
    nw::ut::ResU32 m_MaterialId;                            //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResMaterialColor : public nw::ut::ResCommon< ResMaterialColorData >
{
public:
    NW_RES_CTOR( ResMaterialColor )

    //----------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const nw::ut::FloatColor& GetAmbient() const { return ref().m_AmbientAndVertexColorScale; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    f32 GetVertexColorScale() const { return ref().m_AmbientAndVertexColorScale.a; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetVertexColorScale(f32 scale) { ref().m_AmbientAndVertexColorScale.a = scale; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetAmbientU32() const { return ref().m_AmbientU32; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetAmbient( f32 r, f32 g, f32 b )
    {
        ref().m_AmbientAndVertexColorScale.SetColor(r, g, b);
        ref().m_AmbientU32 = ref().m_AmbientAndVertexColorScale.ToPicaU32();
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetAmbient( const nw::ut::FloatColor& value )
    {
        this->SetAmbient(value.r, value.g, value.b);
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Emission )  // GetEmission()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Diffuse )   // GetDiffuse(), SetDiffuse(r,g,b), SetDiffuse(r,g,b,a), SetDiffuse(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Specular0 ) // GetSpecular0(), SetSpecular0(r,g,b), SetSpecular0(r,g,b,a), SetSpecular0(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Specular1 ) // GetSpecular1(), SetSpecular1(r,g,b), SetSpecular1(r,g,b,a), SetSpecular1(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Constant0 ) // GetConstant0(), SetConstant0(r,g,b), SetConstant0(r,g,b,a), SetConstant0(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Constant1 ) // GetConstant1(), SetConstant1(r,g,b), SetConstant1(r,g,b,a), SetConstant1(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Constant2 ) // GetConstant2(), SetConstant2(r,g,b), SetConstant2(r,g,b,a), SetConstant2(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Constant3 ) // GetConstant3(), SetConstant3(r,g,b), SetConstant3(r,g,b,a), SetConstant3(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Constant4 ) // GetConstant4(), SetConstant4(r,g,b), SetConstant4(r,g,b,a), SetConstant4(FloatColor)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLOAT_U32_COLOR_DECL( nw::ut::FloatColor, Constant5 ) // GetConstant5(), SetConstant5(r,g,b), SetConstant5(r,g,b,a), SetConstant5(FloatColor)

    //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResRasterization : public nw::ut::ResCommon< ResRasterizationData >
{
public:

    //
    enum CullingMode
    {
        CULLINGMODE_FRONT = 0,  //
        CULLINGMODE_BACK  = 1,  //
        CULLINGMODE_NONE  = 3   //
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_POLYGON_OFFSET_ENABLED = 0x1 << 0      //
    };

    NW_RES_CTOR( ResRasterization )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )               // GetFlags(), SetFlags(), EnableFlags(), DisableFlags()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, PolygonOffsetUnit )   // GetPolygonOffsetUnit(), SetPolygonOffsetUnit()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_COMMAND_BUFFER() // GetCommandBuffer()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    CullingMode GetCullingMode() const
    {
        return static_cast<CullingMode>(this->ref().m_CullingMode);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetCullingMode(CullingMode value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x3, CMD_CULLING_MODE_NONE = 0, CMD_CULLING_MODE_OTHER = 1, CMD_CULLING_MODE_BACK = 2 };

        this->ref().m_CullingMode = static_cast<u32>(value);

        static const u32 table[] =
        {
            CMD_CULLING_MODE_OTHER,
            CMD_CULLING_MODE_BACK,
            CMD_CULLING_MODE_OTHER,
            CMD_CULLING_MODE_NONE
        };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], table[value], CMD_MASK, CMD_SHIFT );
    }
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResBlendOperation : public nw::ut::ResCommon< ResBlendOperationData >
{
public:

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
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    enum Mode
    {
        MODE_NOT_USE,           //
        MODE_BLEND,             //
        MODE_SEPARATE_BLEND,    //
        MODE_LOGIC              //
    };

    //
    enum LogicOp
    {
        LOGICOP_CLEAR           = 0,    //
        LOGICOP_AND             = 1,    //
        LOGICOP_AND_REVERSE     = 2,    //
        LOGICOP_COPY            = 3,    //
        LOGICOP_SET             = 4,    //
        LOGICOP_COPY_INVERTED   = 5,    //
        LOGICOP_NOOP            = 6,    //
        LOGICOP_INVERT          = 7,    //
        LOGICOP_NAND            = 8,    //
        LOGICOP_OR              = 9,    //
        LOGICOP_NOR             = 10,   //
        LOGICOP_XOR             = 11,   //
        LOGICOP_EQUIV           = 12,   //
        LOGICOP_AND_INVERTED    = 13,   //
        LOGICOP_OR_REVERSE      = 14,   //
        LOGICOP_OR_INVERTED     = 15    //
    };

    //
    //
    //
    enum BlendFactor
    {
        BLENDFACTOR_ZERO                     = 0,   //
        BLENDFACTOR_ONE                      = 1,   //
        BLENDFACTOR_SRC_COLOR                = 2,   //
        BLENDFACTOR_ONE_MINUS_SRC_COLOR      = 3,   //
        BLENDFACTOR_DST_COLOR                = 4,   //
        BLENDFACTOR_ONE_MINUS_DST_COLOR      = 5,   //
        BLENDFACTOR_SRC_ALPHA                = 6,   //
        BLENDFACTOR_ONE_MINUS_SRC_ALPHA      = 7,   //
        BLENDFACTOR_DST_ALPHA                = 8,   //
        BLENDFACTOR_ONE_MINUS_DST_ALPHA      = 9,   //
        BLENDFACTOR_CONSTANT_COLOR           = 10,  //
        BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR = 11,  //
        BLENDFACTOR_CONSTANT_ALPHA           = 12,  //
        BLENDFACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,  //
        BLENDFACTOR_SRC_ALPHA_SATURATE       = 14   //
    };

    //
    enum BlendEquation
    {
        BLENDEQ_FUNC_ADD              = 0,  //
        BLENDEQ_FUNC_SUBTRACT         = 1,  //
        BLENDEQ_FUNC_REVERSE_SUBTRACT = 2,  //
        BLENDEQ_MIN                   = 3,  //
        BLENDEQ_MAX                   = 4   //
    };

    NW_RES_CTOR( ResBlendOperation )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    Mode GetMode() const
    {
        return static_cast<Mode>(ref().m_Mode & ~MODE_DST_REFERRED_DISABLED);
    }

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
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_DEPRECATED_FUNCTION(void SetMode(Mode value))
    {
        enum { CMD_SHIFT = 8, CMD_MASK = 0x1, CMD_BLEND_MODE_LOGIC = 0, CMD_BLEND_MODE_OTHER = 1 };

        ref().m_Mode = value;

        const u32 table[] =
        {
            CMD_BLEND_MODE_OTHER,
            CMD_BLEND_MODE_OTHER,
            CMD_BLEND_MODE_OTHER,
            CMD_BLEND_MODE_LOGIC
        };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], table[value], CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetBlendMode(bool isBlendMode)
    {
        Mode value = isBlendMode ? MODE_SEPARATE_BLEND : MODE_LOGIC;

        enum { CMD_SHIFT = 8, CMD_MASK = 0x1, CMD_BLEND_MODE_LOGIC = 0, CMD_BLEND_MODE_OTHER = 1 };

        ref().m_Mode = value;

        const u32 table[] =
        {
            CMD_BLEND_MODE_OTHER,
            CMD_BLEND_MODE_OTHER,
            CMD_BLEND_MODE_OTHER,
            CMD_BLEND_MODE_LOGIC
        };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], table[value], CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetBlendColor( f32 r, f32 g, f32 b )
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0xffffffff };

        ref().m_BlendColor.Set(r, g, b);
        u32 colorByte = ref().m_BlendColor.ToPicaU32();
        internal::SetCmdValue( &ref().m_CommandBuffer[4], colorByte, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetBlendColor( f32 r, f32 g, f32 b, f32 a )
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0xffffffff };

        ref().m_BlendColor.Set(r, g, b, a);
        u32 colorByte = ref().m_BlendColor.ToPicaU32();
        internal::SetCmdValue( &ref().m_CommandBuffer[4], colorByte, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const nw::ut::FloatColor& GetBlendColor() const
    {
        return ref().m_BlendColor;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetBlendColor(const nw::ut::FloatColor& value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0xffffffff };

        ref().m_BlendColor = value;
        u32 colorByte = ref().m_BlendColor.ToPicaU32();
        internal::SetCmdValue( &ref().m_CommandBuffer[4], colorByte, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    LogicOp GetLogicOperation() const
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0xf };

        return static_cast<LogicOp>(internal::GetCmdValue( ref().m_CommandBuffer[3], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetLogicOperation(LogicOp value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0xf };

        internal::SetCmdValue( &ref().m_CommandBuffer[3], value, CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    BlendFactor GetBlendFuncSrcRgb() const
    {
        enum { CMD_SHIFT = 16, CMD_MASK = 0xf };

        return static_cast<BlendFactor>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

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
    //
    //---------------------------------------------------------------------------
    void SetBlendFuncSrcRgb(BlendFactor value)
    {
        enum { CMD_SHIFT = 16, CMD_MASK = 0xf };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    BlendFactor GetBlendFuncSrcAlpha() const
    {
        enum { CMD_SHIFT = 24, CMD_MASK = 0xf };

        return static_cast<BlendFactor>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

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
    //
    //---------------------------------------------------------------------------
    void SetBlendFuncSrcAlpha(BlendFactor value)
    {
        enum { CMD_SHIFT = 24, CMD_MASK = 0xf };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    BlendFactor GetBlendFuncDstRgb() const
    {
        enum { CMD_SHIFT = 20, CMD_MASK = 0xf };

        return static_cast<BlendFactor>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

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
    //
    //---------------------------------------------------------------------------
    void SetBlendFuncDstRgb(BlendFactor value)
    {
        enum { CMD_SHIFT = 20, CMD_MASK = 0xf };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    BlendFactor GetBlendFuncDstAlpha() const
    {
        enum { CMD_SHIFT = 28, CMD_MASK = 0xf };

        return static_cast<BlendFactor>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

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
    //
    //---------------------------------------------------------------------------
    void SetBlendFuncDstAlpha(BlendFactor value)
    {
        enum { CMD_SHIFT = 28, CMD_MASK = 0xf };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );

        UpdateDstColorReferredFlag();
    }

    //------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    BlendEquation GetBlendEquationRgb() const
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x7 };

        return static_cast<BlendEquation>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

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
    //
    //---------------------------------------------------------------------------
    void SetBlendEquationRgb(BlendEquation value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    BlendEquation GetBlendEquationAlpha() const
    {
        enum { CMD_SHIFT = 8, CMD_MASK = 0x7 };

        return static_cast<BlendEquation>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

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
    //
    //---------------------------------------------------------------------------
    void SetBlendEquationAlpha(BlendEquation value)
    {
        enum { CMD_SHIFT = 8, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool IsDstColorReferred() const
    {
        return !ut::CheckFlag(ref().m_Mode, static_cast<u32>(MODE_DST_REFERRED_DISABLED));
    }

    //
    void UpdateDstColorReferredFlag()
    {
        if (this->GetMode() == MODE_LOGIC)
        {
            switch (this->GetLogicOperation())
            {
            case LOGICOP_CLEAR:
            case LOGICOP_COPY:
            case LOGICOP_SET:
            case LOGICOP_COPY_INVERTED:
                ref().m_Mode |= MODE_DST_REFERRED_DISABLED;
                return;

            default:
                ref().m_Mode &= ~MODE_DST_REFERRED_DISABLED;
                return;
            }
        }
        else
        {
            switch (this->GetBlendFuncSrcRgb())
            {
            case BLENDFACTOR_DST_COLOR:
            case BLENDFACTOR_ONE_MINUS_DST_COLOR:
            case BLENDFACTOR_DST_ALPHA:
            case BLENDFACTOR_ONE_MINUS_DST_ALPHA:
            case BLENDFACTOR_SRC_ALPHA_SATURATE:
                ref().m_Mode &= ~MODE_DST_REFERRED_DISABLED;
                return;

            default:
                break;
            }

            switch (this->GetBlendFuncSrcAlpha())
            {
            case BLENDFACTOR_DST_COLOR:
            case BLENDFACTOR_ONE_MINUS_DST_COLOR:
            case BLENDFACTOR_DST_ALPHA:
            case BLENDFACTOR_ONE_MINUS_DST_ALPHA:
            case BLENDFACTOR_SRC_ALPHA_SATURATE:
                ref().m_Mode &= ~MODE_DST_REFERRED_DISABLED;
                return;

            default:
                break;
            }

            if (this->GetBlendFuncDstRgb() != BLENDFACTOR_ZERO ||
                this->GetBlendFuncDstAlpha() != BLENDFACTOR_ZERO)
            {
                ref().m_Mode &= ~MODE_DST_REFERRED_DISABLED;
                return;
            }
            else
            {
                ref().m_Mode |= MODE_DST_REFERRED_DISABLED;
                return;
            }
        }
    }

private:
    enum
    {
        MODE_DST_REFERRED_DISABLED = 0x80000000     //
    };

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_COMMAND_BUFFER();
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResDepthOperation : public nw::ut::ResCommon< ResDepthOperationData >
{
public:

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum TestFunc
    {
        TESTFUNC_NEVER      = 0,    //
        TESTFUNC_ALWAYS     = 1,    //
        TESTFUNC_EQUAL      = 2,    //
        TESTFUNC_NOTEQUAL   = 3,    //
        TESTFUNC_LESS       = 4,    //
        TESTFUNC_LEQUAL     = 5,    //
        TESTFUNC_GREATER    = 6,    //
        TESTFUNC_GEQUAL     = 7     //
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_TEST_ENABLED = 0x1 << 0,   //
        FLAG_MASK_ENABLED = 0x1 << 1    //
    };

    NW_RES_CTOR( ResDepthOperation )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetFlags() const
    {
        return static_cast<u32>(ref().m_Flags);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetFlags(u32 value)
    {
        ref().m_Flags = value;

        enum { CMD_SHIFT = 0, CMD_MASK = 0x1 };

        // Here only DepthTestEnabled is set.
        // For DepthMask, it is set from a flag value at runtime.
        internal::SetCmdValue( &ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    TestFunc GetTestFunc() const
    {
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };

        return static_cast<TestFunc>(internal::GetCmdValue( ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetTestFunc(TestFunc value)
    {
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };
        enum { CMD_SHIFT126 = 24, CMD_MASK126 = 0x3, CMD_NEVER = 0, CMD_ALWAYS = 1, CMD_GEATERORGEQUAL = 2, CMD_OTHERS = 3 };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );

        static const u32 table[] =
        {
            CMD_NEVER,
            CMD_ALWAYS,
            CMD_OTHERS,
            CMD_OTHERS,
            CMD_OTHERS,
            CMD_OTHERS,
            CMD_GEATERORGEQUAL,
            CMD_GEATERORGEQUAL
        };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], table[value], CMD_MASK126, CMD_SHIFT126 );
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_COMMAND_BUFFER() // GetCommandBuffer()
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResStencilOperation : public nw::ut::ResCommon< ResStencilOperationData >
{
public:

    //
    enum TestFunc
    {
        TESTFUNC_NEVER,     //
        TESTFUNC_ALWAYS,    //
        TESTFUNC_EQUAL,     //
        TESTFUNC_NOTEQUAL,  //
        TESTFUNC_LESS,      //
        TESTFUNC_LEQUAL,    //
        TESTFUNC_GREATER,   //
        TESTFUNC_GEQUAL     //
    };

    //
    enum StencilOp
    {
        STENCILOP_KEEP,             //
        STENCILOP_ZERO,             //
        STENCILOP_REPLACE,          //
        STENCILOP_INCREASE,         //
        STENCILOP_DECREASE,         //
        STENCILOP_INVERT,           //
        STENCILOP_INCREASE_WRAP,    //
        STENCILOP_DECREASE_WRAP     //
    };

    NW_RES_CTOR( ResStencilOperation )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_COMMAND_BUFFER();

    //
    bool IsTestEnabled() const
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x1 };

        return (internal::GetCmdValue( ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT )) ? true : false;
    }

    //
    void SetTestEnabled(bool value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x1 };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    //
    TestFunc GetTestFunction() const
    {
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };

        return static_cast<TestFunc>(internal::GetCmdValue( ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT ));
    }

    //
    void SetTestFunction(TestFunc value)
    {
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    //
    s32 GetTestReference() const
    {
        enum { CMD_SHIFT = 16, CMD_MASK = 0xff };

        return static_cast<s32>(internal::GetCmdValue( ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT ));
    }

    //
    void SetTestReference(s32 value)
    {
        enum { CMD_SHIFT = 16, CMD_MASK = 0xff };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    //
    u32 GetTestMask() const
    {
        enum { CMD_SHIFT = 24, CMD_MASK = 0xff };

        return static_cast<u32>(internal::GetCmdValue( ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT ));
    }

    //
    void SetTestMask(u32 value)
    {
        enum { CMD_SHIFT = 24, CMD_MASK = 0xff };

        internal::SetCmdValue( &ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    //
    StencilOp GetFailOperation() const
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x7 };

        return static_cast<StencilOp>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

    //
    void SetFailOperation(StencilOp value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );
    }

     //
    StencilOp GetZFailOperation() const
    {
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };

        return static_cast<StencilOp>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

     //
    void SetZFailOperation(StencilOp value)
    {
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );
    }

    //
    StencilOp GetPassOperation() const
    {
        enum { CMD_SHIFT = 8, CMD_MASK = 0x7 };

        return static_cast<StencilOp>(internal::GetCmdValue( ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT ));
    }

    //
    void SetPassOperation(StencilOp value)
    {
        enum { CMD_SHIFT = 8, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT );
    }
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResFragmentOperation : public nw::ut::ResCommon< ResFragmentOperationData >
{
public:
    NW_RES_CTOR( ResFragmentOperation )

    //
    enum FragmentOperationMode
    {
        FRAGMENT_OPERATION_MODE_GL = 0,
        FRAGMENT_OPERATION_MODE_GAS = 1,
        FRAGMENT_OPERATION_MODE_SHADOW = 3
    };

    //
    void SetFragmentOperationMode(FragmentOperationMode value)
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x3 };

        ResBlendOperation blendOperation = GetBlendOperation();
        NW_ASSERT(blendOperation.IsValid());

        internal::SetCmdValue( &blendOperation.ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    //
    FragmentOperationMode GetFragmentOperationMode() const
    {
        enum { CMD_SHIFT = 0, CMD_MASK = 0x3 };

        ResBlendOperation blendOperation = GetBlendOperation();
        NW_ASSERT(blendOperation.IsValid());

        return static_cast<FragmentOperationMode>(internal::GetCmdValue( blendOperation.ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_RESSTRUCT_DECL( ResDepthOperation, DepthOperation )         // GetDepthOperation(), GetDepthOperationData()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_RESSTRUCT_DECL( ResBlendOperation, BlendOperation )         // GetBlendOperation(), GetBlendOperationData()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_RESSTRUCT_DECL( ResStencilOperation, StencilOperation )     // GetStencilOperation(), GetStencilOperationData()
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResTextureCoordinator : public nw::ut::ResCommon< ResTextureCoordinatorData >
{
public:

    //
    enum MappingMethod
    {
        MAPPINGMETHOD_UV_COORDINATE,        //
        MAPPINGMETHOD_CAMERA_CUBE_ENV,      //
        MAPPINGMETHOD_CAMERA_SPHERE_ENV,    //
        MAPPINGMETHOD_PROJECTION,           //
        MAPPINGMETHOD_SHADOW,               //
        MAPPINGMETHOD_SHADOW_CUBE           //
    };

    //
    enum MappingMatrixMode
    {
        MAPPINGMATRIXMODE_MAYA,             //
        MAPPINGMATRIXMODE_SOFTIMAGE,        //
        MAPPINGMATRIXMODE_3DSMAX            //
    };

    NW_RES_CTOR( ResTextureCoordinator )

    //
    const nw::math::VEC2& GetScale() const
    {
        return this->ref().m_Scale;
    }

    //
    //
    void SetScale(const nw::math::VEC2& scale)
    {
        if (this->ref().m_Scale != scale)
        {
            this->ref().m_Scale.Set(scale.x, scale.y);
            this->SetDirty(true);
        }
    }

    //
    f32 GetRotate()
    {
        return this->ref().m_Rotate;
    }

    //
    //
    void SetRotate(f32 rotate)
    {
        if (this->ref().m_Rotate != rotate)
        {
            this->ref().m_Rotate = rotate;
            this->SetDirty(true);
        }
    }

    //
    const nw::math::VEC2& GetTranslate() const
    {
        return this->ref().m_Translate;
    }

    //
    //
    void SetTranslate(const nw::math::VEC2& translate)
    {
        if (this->ref().m_Translate != translate)
        {
            this->ref().m_Translate.Set(translate.x, translate.y);
            this->SetDirty(true);
        }
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, SourceCoordinate )         // GetSourceCoordinate(), SetSourceCoordinate()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( MappingMethod, MappingMethod )  // GetMappingMethod(), SetMappingMethod()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, ReferenceCamera )          // GetReferenceCamera(), SetReferenceCamera()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( MappingMatrixMode, MatrixMode ) // GetMatrixMode(), SetMatrixMode()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Dirty )                    // IsDirty(), SetDirty()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Enabled )                  // IsEnabled(), SetEnabled()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRUCT_DECL( nw::math::MTX34, TextureMatrix )   // GetTextureMatrix()
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResMaterial : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResMaterial) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('MTOB') };
    enum { BINARY_REVISION = REVISION_RES_MATERIAL };

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    enum TextureCoordinateConfig
    {
        CONFIG_0120,    //
        CONFIG_0110,    //
        CONFIG_0111,    //
        CONFIG_0112,    //
        CONFIG_0121,    //
        CONFIG_0122,    //
        CONFIG_NUM      //
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum TranslucencyKind
    {
        TRANSLUCENCY_KIND_LAYER0,   //
        TRANSLUCENCY_KIND_LAYER1,   //
        TRANSLUCENCY_KIND_LAYER2,   //
        TRANSLUCENCY_KIND_LAYER3,   //
        TRANSLUCENCY_KIND_OPAQUE = TRANSLUCENCY_KIND_LAYER0,        //
        TRANSLUCENCY_KIND_TRANSLUCENT = TRANSLUCENCY_KIND_LAYER1,   //
        TRANSLUCENCY_KIND_SUBTRACTIVE = TRANSLUCENCY_KIND_LAYER2,   //
        TRANSLUCENCY_KIND_ADDITIVE = TRANSLUCENCY_KIND_LAYER3,      //

        TRANSLUCENCY_KIND_END = TRANSLUCENCY_KIND_LAYER3   //
    };

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_FRAGMENTLIGHT_ENABLED = 0x1 << 0,          //
        FLAG_VERTEXLIGHT_ENABLED   = 0x1 << 1,          //
        FLAG_HEMISPHERELIGHT_ENABLED   = 0x1 << 2,      //
        FLAG_HEMISPHERE_OCCLUSION_ENABLED = 0x1 << 3,   //
        FLAG_FOG_ENABLED = 0x1 << 4,                    //
        FLAG_PARTICLE_MATERIAL_ENABLED = 0x1 << 5,      //
        FLAG_HAS_BEEN_SETUP = 0x1 << 6,                 //
        FLAG_HAS_BEEN_SETUP_SHADER = 0x1 << 7,          //
        FLAG_HAS_BEEN_SETUP_TEXTURE = 0x1 << 8,         //
        FLAG_HAS_BEEN_SETUP_FRAGMENTSHADER = 0x1 << 9   //
    };

    NW_RES_CTOR_INHERIT( ResMaterial, ResSceneObject )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )               // GetFlags(), SetFlags(), EnableFlags(), DisableFlags()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( TextureCoordinateConfig, TextureCoordinateConfig )     // GetTextureCoordinateConfig(), SetTextureCoordinateConfig()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( TranslucencyKind, TranslucencyKind )     // GetTranslucencyKind(), SetTranslucencyKind()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_RESSTRUCT_DECL( ResMaterialColor, MaterialColor )  // GetMaterialColor(), GetMaterialColorData()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_RESSTRUCT_DECL( ResRasterization, Rasterization )  // GetRasterization(), GetRasterizationData()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_RESSTRUCT_DECL( ResFragmentOperation, FragmentOperation )              // GetFragmentOperation(), GetFragmentOperationData()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, ActiveTextureCoordinatorsCount )   // SetActiveTextureCoordinatorsCount() GetActiveTextureCoordinatorsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRUCT_FIXED_LIST_DECL( ResTextureCoordinator, TextureCoordinators )   // GetTextureCoordinators(int idx), GetTextureCoodinatorsData(), GetTextureCoordinatorsData(int idx), GetTextureCoordinatorsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_FIXED_LIST_DECL( ResPixelBasedTextureMapper, TextureMappers )    // GetTextureMappers(int idx), GetTextureMappersCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //
    NW_RES_FIELD_CLASS_DECL( ResProceduralTextureMapper, ProceduralTextureMapper )      // GetProceduralTextureMapper()

    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResShader, Shader )                    // GetShader()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResFragmentShader, FragmentShader )    // GetFragmentShader()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, ShaderProgramDescriptionIndex ) // GetShaderProgramDescriptionIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResShaderParameter, ShaderParameters ) // GetParameters(int idx), GetParametersCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, LightSetIndex ) // GetLightSetIndex(), SetLightSetIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, FogIndex ) // GetFogIndex(), SetFogIndex()

    NW_RES_FIELD_PRIMITIVE_DECL( u32, ShadingParameterHash ) // GetShadingParameterHash(), SetShadingParameterHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ShaderParametersHash ) // GetShaderParametersHash(), SetShaderParametersHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, TextureCoordinatorsHash ) // GetTextureCoordinatorsHash(), SetTextureCoordinatorsHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, TextureSamplersHash ) // GetTextureSamplersHash(), SetTextureSamplersHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, TextureMappersHash ) // GetTextureMappersHash(), SetTextureMappersHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, MaterialColorHash ) // GetMaterialColorHash(), SetMaterialColorHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, RasterizationHash ) // GetRasterizationHash(), SetRasterizationHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FragmentLightingHash ) // GetFragmentLightingHash(), SetFragmentLightingHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FragmentLightingTableHash ) // GetFragmentLightingTableHash(), SetFragmentLightingTableHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FragmentLightingTableParametersHash ) // GetFragmentLightingTableParametersHash(), SetFragmentLightingTableParametersHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, AlphaTestHash ) // GetAlphaTestHash(), SetAlphaTestHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, TextureCombinersHash ) // GetTextureCombinersHash(), SetTextureCombinersHash()
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FragmentOperationHash ) // GetFragmentOperationHash(), SetFragmentOperationHash()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, MaterialId ) // GetMaterialId(), SetMaterialId()

    //
    //
    //
    u32 GetRevision() const { return this->GetHeader().revision; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Result Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();

    //
    //
    //
    //
    void SetShader(ResShader resShader);

    //
    //
    void ForceSetupTexture(const char* targetName, ResTexture texture);

    //
    //
    //
    void ForceSetupShader(const char* targetName, ResShader resShader);

    //
    //
    void ForceSetupLookupTable(const char* targetName, ResLookupTable lut);

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    ResPixelBasedTextureMapper AddTextureMapper(
        s32* pMapperIndex,
        os::IAllocator* allocator,
        const ResPixelBasedTextureMapper::Description& description
    );

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
private:
    //
    Result SetupShader(os::IAllocator* allocator, ResShader resShader, ResGraphicsFile graphicsFile);

    //
    Result SetupTextures(os::IAllocator* allocator, ResMaterial resMaterial, ResGraphicsFile graphicsFile);

    //
    //
    //
    //
    void CacheUserUniformIndex(ResShaderSymbolArray symbols);

    //
    Result SetupFragmentLightingTable(os::IAllocator* allocator, ResFragmentLightingTable resLightingTable, ResGraphicsFile graphicsFile);

    //
    void CalcFragmentLightingTableHash();
};

//
typedef nw::ut::ResArrayPatricia<ResMaterial>::type ResMaterialArray;

//
typedef nw::ut::ResArrayPatricia<const ResMaterial>::type ResMaterialArrayConst;

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESMATERIAL_H_
