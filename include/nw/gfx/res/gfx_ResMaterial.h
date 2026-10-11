#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>
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

struct ResMaterialColorData
{
    nw::ut::ResFloatColor m_Emission;
    nw::ut::ResFloatColor m_AmbientAndVertexColorScale;
    nw::ut::ResFloatColor m_Diffuse;
    nw::ut::ResFloatColor m_Specular0;
    nw::ut::ResFloatColor m_Specular1;
    nw::ut::ResFloatColor m_Constant0;
    nw::ut::ResFloatColor m_Constant1;
    nw::ut::ResFloatColor m_Constant2;
    nw::ut::ResFloatColor m_Constant3;
    nw::ut::ResFloatColor m_Constant4;
    nw::ut::ResFloatColor m_Constant5;
    nw::ut::ResU32 m_EmissionU32;
    nw::ut::ResU32 m_AmbientU32;
    nw::ut::ResU32 m_DiffuseU32;
    nw::ut::ResU32 m_Specular0U32;
    nw::ut::ResU32 m_Specular1U32;
    nw::ut::ResU32 m_Constant0U32;
    nw::ut::ResU32 m_Constant1U32;
    nw::ut::ResU32 m_Constant2U32;
    nw::ut::ResU32 m_Constant3U32;
    nw::ut::ResU32 m_Constant4U32;
    nw::ut::ResU32 m_Constant5U32;
    void* m_CommandCache;
};

struct ResRasterizationData
{
    enum Flag
    {
        FLAG_POLYGON_OFFSET_ENABLED = 0x1 << 0
    };
    nw::ut::ResU32 m_Flags;
    nw::ut::ResU32 m_CullingMode;
    nw::ut::ResF32 m_PolygonOffsetUnit;
    nw::ut::ResU32 m_CommandBuffer[2];
};

struct ResBlendOperationData
{
    nw::ut::ResS32        m_Mode;
    nw::ut::ResFloatColor m_BlendColor;
    nw::ut::ResU32        m_CommandBuffer[6];
};

struct ResDepthOperationData
{
    enum Flag
    {
        FLAG_TEST_ENABLED = 0x1 << 0,
        FLAG_MASK_ENABLED = 0x1 << 1
    };
    nw::ut::ResU32 m_Flags;
    nw::ut::ResU32 m_CommandBuffer[4];
};

struct ResStencilOperationData
{
    nw::ut::ResU32 m_CommandBuffer[4];
};

struct ResFragmentOperationData
{
    ResDepthOperationData   m_DepthOperation;
    ResBlendOperationData   m_BlendOperation;
    ResStencilOperationData m_StencilOperation;
};

struct ResTextureCoordinatorData
{
    nw::ut::ResS32  m_SourceCoordinate;
    nw::ut::ResS32  m_MappingMethod;
    nw::ut::ResS32  m_ReferenceCamera;
    nw::ut::ResS32  m_MatrixMode;
    nw::math::VEC2 m_Scale;
    nw::ut::ResF32  m_Rotate;
    nw::math::VEC2 m_Translate;
    nw::ut::ResBool m_IsDirty;
    nw::ut::ResBool m_IsEnabled;
    u8              _padding_0[2];
    nw::ut::ResMtx34 m_TextureMatrix;
};

struct ResMaterialData : public ResSceneObjectData
{
    enum Flag
    {
        FLAG_FRAGMENTLIGHT_ENABLED           = 0x1 << 0,
        FLAG_VERTEXLIGHT_ENABLED             = 0x1 << 1,
        FLAG_HEMISPHERELIGHT_ENABLED         = 0x1 << 2,
        FLAG_HEMISPHERE_OCCLUSION_ENABLED    = 0x1 << 3,
        FLAG_FOG_ENABLED                     = 0x1 << 4,
        FLAG_PARTICLE_MATERIAL_ENABLED       = 0x1 << 5,
        FLAG_HAS_BEEN_SETUP                  = 0x1 << 6,
        FLAG_HAS_BEEN_SETUP_SHADER           = 0x1 << 7,
        FLAG_HAS_BEEN_SETUP_TEXTURE          = 0x1 << 8,
        FLAG_HAS_BEEN_SETUP_FRAGMENTSHADER   = 0x1 << 9
    };

    nw::ut::ResU32 m_Flags;
    nw::ut::ResS32 m_TextureCoordinateConfig;
    nw::ut::ResS32 m_TranslucencyKind;
    ResMaterialColorData     m_MaterialColor;
    ResRasterizationData     m_Rasterization;
    ResFragmentOperationData m_FragmentOperation;
    nw::ut::ResS32 m_ActiveTextureCoordinatorsCount;
    ResTextureCoordinatorData m_TextureCoordinators[3];
    nw::ut::Offset toTextureMappers[3];
    nw::ut::Offset toProceduralTextureMapper;
    nw::ut::Offset toShader;
    nw::ut::Offset toFragmentShader;
    nw::ut::ResS32 m_ShaderProgramDescriptionIndex;
    nw::ut::ResS32 m_ShaderParametersTableCount;
    nw::ut::Offset toShaderParametersTable;
    nw::ut::ResS32 m_LightSetIndex;
    nw::ut::ResS32 m_FogIndex;
    nw::ut::ResU32 m_ShadingParameterHash;
    nw::ut::ResU32 m_ShaderParametersHash;
    nw::ut::ResU32 m_TextureCoordinatorsHash;
    nw::ut::ResU32 m_TextureSamplersHash;
    nw::ut::ResU32 m_TextureMappersHash;
    nw::ut::ResU32 m_MaterialColorHash;
    nw::ut::ResU32 m_RasterizationHash;
    nw::ut::ResU32 m_FragmentLightingHash;
    nw::ut::ResU32 m_FragmentLightingTableHash;
    nw::ut::ResU32 m_FragmentLightingTableParametersHash;
    nw::ut::ResU32 m_TextureCombinersHash;
    nw::ut::ResU32 m_AlphaTestHash;
    nw::ut::ResU32 m_FragmentOperationHash;
    nw::ut::ResU32 m_MaterialId;
};

class ResMaterialColor : public nw::ut::ResCommon<ResMaterialColorData>
{
public:
    NW_RES_CTOR(ResMaterialColor)

    const nw::ut::FloatColor& GetAmbient() const { return ref().m_AmbientAndVertexColorScale; }
    f32  GetVertexColorScale() const { return ref().m_AmbientAndVertexColorScale.a; }
    void SetVertexColorScale(f32 scale) { ref().m_AmbientAndVertexColorScale.a = scale; }
    u32  GetAmbientU32() const { return ref().m_AmbientU32; }

    void SetAmbient(f32 r, f32 g, f32 b)
    {
        ref().m_AmbientAndVertexColorScale.SetColor(r, g, b);
        ref().m_AmbientU32 = ref().m_AmbientAndVertexColorScale.ToPicaU32();
    }
    void SetAmbient(const nw::ut::FloatColor& value) { this->SetAmbient(value.r, value.g, value.b); }


    u32 GetEmissionU32() const { return ref().m_EmissionU32; }
    void SetEmission(f32 r, f32 g, f32 b) { ref().m_Emission.Set(r, g, b); ref().m_EmissionU32 = ref().m_Emission.ToPicaU32(); }
    void SetEmission(f32 r, f32 g, f32 b, f32 a) { ref().m_Emission.Set(r, g, b, a); ref().m_EmissionU32 = ref().m_Emission.ToPicaU32(); }
    const nw::ut::FloatColor& GetEmission() const { return ref().m_Emission; }
    void SetEmission(const nw::ut::FloatColor& value) { ref().m_Emission = value; ref().m_EmissionU32 = ref().m_Emission.ToPicaU32(); }

    u32 GetDiffuseU32() const { return ref().m_DiffuseU32; }
    void SetDiffuse(f32 r, f32 g, f32 b) { ref().m_Diffuse.Set(r, g, b); ref().m_DiffuseU32 = ref().m_Diffuse.ToPicaU32(); }
    void SetDiffuse(f32 r, f32 g, f32 b, f32 a) { ref().m_Diffuse.Set(r, g, b, a); ref().m_DiffuseU32 = ref().m_Diffuse.ToPicaU32(); }
    const nw::ut::FloatColor& GetDiffuse() const { return ref().m_Diffuse; }
    void SetDiffuse(const nw::ut::FloatColor& value) { ref().m_Diffuse = value; ref().m_DiffuseU32 = ref().m_Diffuse.ToPicaU32(); }

    u32 GetSpecular0U32() const { return ref().m_Specular0U32; }
    void SetSpecular0(f32 r, f32 g, f32 b) { ref().m_Specular0.Set(r, g, b); ref().m_Specular0U32 = ref().m_Specular0.ToPicaU32(); }
    void SetSpecular0(f32 r, f32 g, f32 b, f32 a) { ref().m_Specular0.Set(r, g, b, a); ref().m_Specular0U32 = ref().m_Specular0.ToPicaU32(); }
    const nw::ut::FloatColor& GetSpecular0() const { return ref().m_Specular0; }
    void SetSpecular0(const nw::ut::FloatColor& value) { ref().m_Specular0 = value; ref().m_Specular0U32 = ref().m_Specular0.ToPicaU32(); }

    u32 GetSpecular1U32() const { return ref().m_Specular1U32; }
    void SetSpecular1(f32 r, f32 g, f32 b) { ref().m_Specular1.Set(r, g, b); ref().m_Specular1U32 = ref().m_Specular1.ToPicaU32(); }
    void SetSpecular1(f32 r, f32 g, f32 b, f32 a) { ref().m_Specular1.Set(r, g, b, a); ref().m_Specular1U32 = ref().m_Specular1.ToPicaU32(); }
    const nw::ut::FloatColor& GetSpecular1() const { return ref().m_Specular1; }
    void SetSpecular1(const nw::ut::FloatColor& value) { ref().m_Specular1 = value; ref().m_Specular1U32 = ref().m_Specular1.ToPicaU32(); }

    u32 GetConstant0U32() const { return ref().m_Constant0U32; }
    void SetConstant0(f32 r, f32 g, f32 b) { ref().m_Constant0.Set(r, g, b); ref().m_Constant0U32 = ref().m_Constant0.ToPicaU32(); }
    void SetConstant0(f32 r, f32 g, f32 b, f32 a) { ref().m_Constant0.Set(r, g, b, a); ref().m_Constant0U32 = ref().m_Constant0.ToPicaU32(); }
    const nw::ut::FloatColor& GetConstant0() const { return ref().m_Constant0; }
    void SetConstant0(const nw::ut::FloatColor& value) { ref().m_Constant0 = value; ref().m_Constant0U32 = ref().m_Constant0.ToPicaU32(); }

    u32 GetConstant1U32() const { return ref().m_Constant1U32; }
    void SetConstant1(f32 r, f32 g, f32 b) { ref().m_Constant1.Set(r, g, b); ref().m_Constant1U32 = ref().m_Constant1.ToPicaU32(); }
    void SetConstant1(f32 r, f32 g, f32 b, f32 a) { ref().m_Constant1.Set(r, g, b, a); ref().m_Constant1U32 = ref().m_Constant1.ToPicaU32(); }
    const nw::ut::FloatColor& GetConstant1() const { return ref().m_Constant1; }
    void SetConstant1(const nw::ut::FloatColor& value) { ref().m_Constant1 = value; ref().m_Constant1U32 = ref().m_Constant1.ToPicaU32(); }

    u32 GetConstant2U32() const { return ref().m_Constant2U32; }
    void SetConstant2(f32 r, f32 g, f32 b) { ref().m_Constant2.Set(r, g, b); ref().m_Constant2U32 = ref().m_Constant2.ToPicaU32(); }
    void SetConstant2(f32 r, f32 g, f32 b, f32 a) { ref().m_Constant2.Set(r, g, b, a); ref().m_Constant2U32 = ref().m_Constant2.ToPicaU32(); }
    const nw::ut::FloatColor& GetConstant2() const { return ref().m_Constant2; }
    void SetConstant2(const nw::ut::FloatColor& value) { ref().m_Constant2 = value; ref().m_Constant2U32 = ref().m_Constant2.ToPicaU32(); }

    u32 GetConstant3U32() const { return ref().m_Constant3U32; }
    void SetConstant3(f32 r, f32 g, f32 b) { ref().m_Constant3.Set(r, g, b); ref().m_Constant3U32 = ref().m_Constant3.ToPicaU32(); }
    void SetConstant3(f32 r, f32 g, f32 b, f32 a) { ref().m_Constant3.Set(r, g, b, a); ref().m_Constant3U32 = ref().m_Constant3.ToPicaU32(); }
    const nw::ut::FloatColor& GetConstant3() const { return ref().m_Constant3; }
    void SetConstant3(const nw::ut::FloatColor& value) { ref().m_Constant3 = value; ref().m_Constant3U32 = ref().m_Constant3.ToPicaU32(); }

    u32 GetConstant4U32() const { return ref().m_Constant4U32; }
    void SetConstant4(f32 r, f32 g, f32 b) { ref().m_Constant4.Set(r, g, b); ref().m_Constant4U32 = ref().m_Constant4.ToPicaU32(); }
    void SetConstant4(f32 r, f32 g, f32 b, f32 a) { ref().m_Constant4.Set(r, g, b, a); ref().m_Constant4U32 = ref().m_Constant4.ToPicaU32(); }
    const nw::ut::FloatColor& GetConstant4() const { return ref().m_Constant4; }
    void SetConstant4(const nw::ut::FloatColor& value) { ref().m_Constant4 = value; ref().m_Constant4U32 = ref().m_Constant4.ToPicaU32(); }

    u32 GetConstant5U32() const { return ref().m_Constant5U32; }
    void SetConstant5(f32 r, f32 g, f32 b) { ref().m_Constant5.Set(r, g, b); ref().m_Constant5U32 = ref().m_Constant5.ToPicaU32(); }
    void SetConstant5(f32 r, f32 g, f32 b, f32 a) { ref().m_Constant5.Set(r, g, b, a); ref().m_Constant5U32 = ref().m_Constant5.ToPicaU32(); }
    const nw::ut::FloatColor& GetConstant5() const { return ref().m_Constant5; }
    void SetConstant5(const nw::ut::FloatColor& value) { ref().m_Constant5 = value; ref().m_Constant5U32 = ref().m_Constant5.ToPicaU32(); }
};

class ResRasterization : public nw::ut::ResCommon<ResRasterizationData>
{
public:
    enum CullingMode
    {
        CULLINGMODE_FRONT = 0,
        CULLINGMODE_BACK  = 1,
        CULLINGMODE_NONE  = 3
    };

    NW_RES_CTOR(ResRasterization)

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    f32 GetPolygonOffsetUnit() const { return static_cast<f32>(ref().m_PolygonOffsetUnit); }
    void SetPolygonOffsetUnit(f32 value) { ref().m_PolygonOffsetUnit = value; }

    u32* GetCommandBuffer()
        { return &ref().m_CommandBuffer[0]; }
    const u32* GetCommandBuffer() const
        { return &ref().m_CommandBuffer[0]; }

    CullingMode GetCullingMode() const { return static_cast<CullingMode>(this->ref().m_CullingMode); }
    void SetCullingMode(CullingMode value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x3, CMD_CULLING_MODE_NONE = 0, CMD_CULLING_MODE_OTHER = 1, CMD_CULLING_MODE_BACK = 2 };
        this->ref().m_CullingMode = static_cast<u32>(value);
        static const u32 table[] = { CMD_CULLING_MODE_OTHER, CMD_CULLING_MODE_BACK, CMD_CULLING_MODE_OTHER, CMD_CULLING_MODE_NONE };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], table[value], CMD_MASK, CMD_SHIFT);
    }
};

class ResBlendOperation : public nw::ut::ResCommon<ResBlendOperationData>
{
public:
    enum Mode
    {
        MODE_NOT_USE,
        MODE_BLEND,
        MODE_SEPARATE_BLEND,
        MODE_LOGIC
    };

    enum LogicOp
    {
        LOGICOP_CLEAR           = 0,
        LOGICOP_AND             = 1,
        LOGICOP_AND_REVERSE     = 2,
        LOGICOP_COPY            = 3,
        LOGICOP_SET             = 4,
        LOGICOP_COPY_INVERTED   = 5,
        LOGICOP_NOOP            = 6,
        LOGICOP_INVERT          = 7,
        LOGICOP_NAND            = 8,
        LOGICOP_OR              = 9,
        LOGICOP_NOR             = 10,
        LOGICOP_XOR             = 11,
        LOGICOP_EQUIV           = 12,
        LOGICOP_AND_INVERTED    = 13,
        LOGICOP_OR_REVERSE      = 14,
        LOGICOP_OR_INVERTED     = 15
    };

    enum BlendFactor
    {
        BLENDFACTOR_ZERO                     = 0,
        BLENDFACTOR_ONE                      = 1,
        BLENDFACTOR_SRC_COLOR                = 2,
        BLENDFACTOR_ONE_MINUS_SRC_COLOR      = 3,
        BLENDFACTOR_DST_COLOR                = 4,
        BLENDFACTOR_ONE_MINUS_DST_COLOR      = 5,
        BLENDFACTOR_SRC_ALPHA                = 6,
        BLENDFACTOR_ONE_MINUS_SRC_ALPHA      = 7,
        BLENDFACTOR_DST_ALPHA                = 8,
        BLENDFACTOR_ONE_MINUS_DST_ALPHA      = 9,
        BLENDFACTOR_CONSTANT_COLOR           = 10,
        BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR = 11,
        BLENDFACTOR_CONSTANT_ALPHA           = 12,
        BLENDFACTOR_ONE_MINUS_CONSTANT_ALPHA = 13,
        BLENDFACTOR_SRC_ALPHA_SATURATE       = 14
    };

    enum BlendEquation
    {
        BLENDEQ_FUNC_ADD              = 0,
        BLENDEQ_FUNC_SUBTRACT         = 1,
        BLENDEQ_FUNC_REVERSE_SUBTRACT = 2,
        BLENDEQ_MIN                   = 3,
        BLENDEQ_MAX                   = 4
    };

    NW_RES_CTOR(ResBlendOperation)

    Mode GetMode() const { return static_cast<Mode>(ref().m_Mode); }
    void SetMode(Mode value)
    {
        enum
{ CMD_SHIFT = 8, CMD_MASK = 0x1, CMD_BLEND_MODE_LOGIC = 0, CMD_BLEND_MODE_OTHER = 1 };
        ref().m_Mode = value;
        const u32 table[] = { CMD_BLEND_MODE_OTHER, CMD_BLEND_MODE_OTHER, CMD_BLEND_MODE_OTHER, CMD_BLEND_MODE_LOGIC };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], table[value], CMD_MASK, CMD_SHIFT);
    }

    void SetBlendColor(f32 r, f32 g, f32 b)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0xffffffff };
        ref().m_BlendColor.Set(r, g, b);
        internal::SetCmdValue(&ref().m_CommandBuffer[4], ref().m_BlendColor.ToPicaU32(), CMD_MASK, CMD_SHIFT);
    }
    void SetBlendColor(f32 r, f32 g, f32 b, f32 a)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0xffffffff };
        ref().m_BlendColor.Set(r, g, b, a);
        internal::SetCmdValue(&ref().m_CommandBuffer[4], ref().m_BlendColor.ToPicaU32(), CMD_MASK, CMD_SHIFT);
    }
    const nw::ut::FloatColor& GetBlendColor() const { return ref().m_BlendColor; }
    void SetBlendColor(const nw::ut::FloatColor& value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0xffffffff };
        ref().m_BlendColor = value;
        internal::SetCmdValue(&ref().m_CommandBuffer[4], ref().m_BlendColor.ToPicaU32(), CMD_MASK, CMD_SHIFT);
    }

    LogicOp GetLogicOperation() const
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0xf };
        return static_cast<LogicOp>(internal::GetCmdValue(ref().m_CommandBuffer[3], CMD_MASK, CMD_SHIFT));
    }
    void SetLogicOperation(LogicOp value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0xf };
        internal::SetCmdValue(&ref().m_CommandBuffer[3], value, CMD_MASK, CMD_SHIFT);
    }

    BlendFactor GetBlendFuncSrcRgb() const
    {
        enum
{ CMD_SHIFT = 16, CMD_MASK = 0xf };
        return static_cast<BlendFactor>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetBlendFuncSrcRgb(BlendFactor value)
    {
        enum
{ CMD_SHIFT = 16, CMD_MASK = 0xf };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    BlendFactor GetBlendFuncSrcAlpha() const
    {
        enum
{ CMD_SHIFT = 24, CMD_MASK = 0xf };
        return static_cast<BlendFactor>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetBlendFuncSrcAlpha(BlendFactor value)
    {
        enum
{ CMD_SHIFT = 24, CMD_MASK = 0xf };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    BlendFactor GetBlendFuncDstRgb() const
    {
        enum
{ CMD_SHIFT = 20, CMD_MASK = 0xf };
        return static_cast<BlendFactor>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetBlendFuncDstRgb(BlendFactor value)
    {
        enum
{ CMD_SHIFT = 20, CMD_MASK = 0xf };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    BlendFactor GetBlendFuncDstAlpha() const
    {
        enum
{ CMD_SHIFT = 28, CMD_MASK = 0xf };
        return static_cast<BlendFactor>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetBlendFuncDstAlpha(BlendFactor value)
    {
        enum
{ CMD_SHIFT = 28, CMD_MASK = 0xf };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    BlendEquation GetBlendEquationRgb() const
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x7 };
        return static_cast<BlendEquation>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetBlendEquationRgb(BlendEquation value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x7 };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    BlendEquation GetBlendEquationAlpha() const
    {
        enum
{ CMD_SHIFT = 8, CMD_MASK = 0x7 };
        return static_cast<BlendEquation>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetBlendEquationAlpha(BlendEquation value)
    {
        enum
{ CMD_SHIFT = 8, CMD_MASK = 0x7 };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }


    u32* GetCommandBuffer()
        { return &ref().m_CommandBuffer[0]; }
    const u32* GetCommandBuffer() const
        { return &ref().m_CommandBuffer[0]; };
};

class ResDepthOperation : public nw::ut::ResCommon<ResDepthOperationData>
{
public:
    enum TestFunc
    {
        TESTFUNC_NEVER    = 0,
        TESTFUNC_ALWAYS   = 1,
        TESTFUNC_EQUAL    = 2,
        TESTFUNC_NOTEQUAL = 3,
        TESTFUNC_LESS     = 4,
        TESTFUNC_LEQUAL   = 5,
        TESTFUNC_GREATER  = 6,
        TESTFUNC_GEQUAL   = 7
    };

    NW_RES_CTOR(ResDepthOperation)

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x1 };
        ref().m_Flags = value;
        internal::SetCmdValue(&ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
    }

    TestFunc GetTestFunc() const
    {
        enum
{ CMD_SHIFT = 4, CMD_MASK = 0x7 };
        return static_cast<TestFunc>(internal::GetCmdValue(ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT));
    }
    void SetTestFunc(TestFunc value)
    {
        enum
{ CMD_SHIFT = 4, CMD_MASK = 0x7 };
        enum
{ CMD_SHIFT126 = 24, CMD_MASK126 = 0x3, CMD_NEVER = 0, CMD_ALWAYS = 1, CMD_GEATERORGEQUAL = 2, CMD_OTHERS = 3 };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
        static const uint table[] = { CMD_NEVER, CMD_ALWAYS, CMD_OTHERS, CMD_OTHERS, CMD_OTHERS, CMD_OTHERS, CMD_GEATERORGEQUAL, CMD_GEATERORGEQUAL };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], table[value], CMD_MASK126, CMD_SHIFT126);
    }


    u32* GetCommandBuffer()
        { return &ref().m_CommandBuffer[0]; }
    const u32* GetCommandBuffer() const
        { return &ref().m_CommandBuffer[0]; }
};

class ResStencilOperation : public nw::ut::ResCommon<ResStencilOperationData>
{
public:
    enum TestFunc
    {
        TESTFUNC_NEVER,
        TESTFUNC_ALWAYS,
        TESTFUNC_EQUAL,
        TESTFUNC_NOTEQUAL,
        TESTFUNC_LESS,
        TESTFUNC_LEQUAL,
        TESTFUNC_GREATER,
        TESTFUNC_GEQUAL
    };

    enum StencilOp
    {
        STENCILOP_KEEP,
        STENCILOP_ZERO,
        STENCILOP_REPLACE,
        STENCILOP_INCREASE,
        STENCILOP_DECREASE,
        STENCILOP_INVERT,
        STENCILOP_INCREASE_WRAP,
        STENCILOP_DECREASE_WRAP
    };

    NW_RES_CTOR(ResStencilOperation)


    u32* GetCommandBuffer()
        { return &ref().m_CommandBuffer[0]; }
    const u32* GetCommandBuffer() const
        { return &ref().m_CommandBuffer[0]; };

    bool IsTestEnabled() const
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x1 };
        return (internal::GetCmdValue(ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT)) ? true : false;
    }
    void SetTestEnabled(bool value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x1 };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
    }

    TestFunc GetTestFunction() const
    {
        enum
{ CMD_SHIFT = 4, CMD_MASK = 0x7 };
        return static_cast<TestFunc>(internal::GetCmdValue(ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT));
    }
    void SetTestFunction(TestFunc value)
    {
        enum
{ CMD_SHIFT = 4, CMD_MASK = 0x7 };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
    }

    s32 GetTestReference() const
    {
        enum
{ CMD_SHIFT = 16, CMD_MASK = 0xff };
        return static_cast<s32>(internal::GetCmdValue(ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT));
    }
    void SetTestReference(s32 value)
    {
        enum
{ CMD_SHIFT = 16, CMD_MASK = 0xff };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
    }

    u32 GetTestMask() const
    {
        enum
{ CMD_SHIFT = 24, CMD_MASK = 0xff };
        return static_cast<u32>(internal::GetCmdValue(ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT));
    }
    void SetTestMask(u32 value)
    {
        enum
{ CMD_SHIFT = 24, CMD_MASK = 0xff };
        internal::SetCmdValue(&ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
    }

    StencilOp GetFailOperation() const
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x7 };
        return static_cast<StencilOp>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetFailOperation(StencilOp value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x7 };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    StencilOp GetZFailOperation() const
    {
        enum
{ CMD_SHIFT = 4, CMD_MASK = 0x7 };
        return static_cast<StencilOp>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetZFailOperation(StencilOp value)
    {
        enum
{ CMD_SHIFT = 4, CMD_MASK = 0x7 };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }

    StencilOp GetPassOperation() const
    {
        enum
{ CMD_SHIFT = 8, CMD_MASK = 0x7 };
        return static_cast<StencilOp>(internal::GetCmdValue(ref().m_CommandBuffer[2], CMD_MASK, CMD_SHIFT));
    }
    void SetPassOperation(StencilOp value)
    {
        enum
{ CMD_SHIFT = 8, CMD_MASK = 0x7 };
        internal::SetCmdValue(&ref().m_CommandBuffer[2], value, CMD_MASK, CMD_SHIFT);
    }
};

class ResFragmentOperation : public nw::ut::ResCommon<ResFragmentOperationData>
{
public:
    NW_RES_CTOR(ResFragmentOperation)

    enum FragmentOperationMode
    {
        FRAGMENT_OPERATION_MODE_GL     = 0,
        FRAGMENT_OPERATION_MODE_GAS    = 1,
        FRAGMENT_OPERATION_MODE_SHADOW = 3
    };

    void SetFragmentOperationMode(FragmentOperationMode value)
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x3 };
        ResBlendOperation blendOperation = GetBlendOperation();
        NW_ASSERT(blendOperation.IsValid());
        internal::SetCmdValue(&blendOperation.ref().m_CommandBuffer[0], value, CMD_MASK, CMD_SHIFT);
    }
    FragmentOperationMode GetFragmentOperationMode() const
    {
        enum
{ CMD_SHIFT = 0, CMD_MASK = 0x3 };
        ResBlendOperation blendOperation = GetBlendOperation();
        NW_ASSERT(blendOperation.IsValid());
        return static_cast<FragmentOperationMode>(internal::GetCmdValue(blendOperation.ref().m_CommandBuffer[0], CMD_MASK, CMD_SHIFT));
    }


    ResDepthOperation GetDepthOperation() { return ResDepthOperation(&(ref().m_DepthOperation)); }
    const ResDepthOperation GetDepthOperation() const { return ResDepthOperation(&(ref().m_DepthOperation)); }
    ResDepthOperationData& GetDepthOperationData() { return ref().m_DepthOperation; }
    const ResDepthOperationData& GetDepthOperationData() const { return ref().m_DepthOperation; }

    ResBlendOperation GetBlendOperation() { return ResBlendOperation(&(ref().m_BlendOperation)); }
    const ResBlendOperation GetBlendOperation() const { return ResBlendOperation(&(ref().m_BlendOperation)); }
    ResBlendOperationData& GetBlendOperationData() { return ref().m_BlendOperation; }
    const ResBlendOperationData& GetBlendOperationData() const { return ref().m_BlendOperation; }

    ResStencilOperation GetStencilOperation() { return ResStencilOperation(&(ref().m_StencilOperation)); }
    const ResStencilOperation GetStencilOperation() const { return ResStencilOperation(&(ref().m_StencilOperation)); }
    ResStencilOperationData& GetStencilOperationData() { return ref().m_StencilOperation; }
    const ResStencilOperationData& GetStencilOperationData() const { return ref().m_StencilOperation; }
};

class ResTextureCoordinator : public nw::ut::ResCommon<ResTextureCoordinatorData>
{
public:
    enum MappingMethod
    {
        MAPPINGMETHOD_UV_COORDINATE,
        MAPPINGMETHOD_CAMERA_CUBE_ENV,
        MAPPINGMETHOD_CAMERA_SPHERE_ENV,
        MAPPINGMETHOD_PROJECTION,
        MAPPINGMETHOD_SHADOW,
        MAPPINGMETHOD_SHADOW_CUBE
    };

    enum MappingMatrixMode
    {
        MAPPINGMATRIXMODE_MAYA,
        MAPPINGMATRIXMODE_SOFTIMAGE,
        MAPPINGMATRIXMODE_3DSMAX
    };

    NW_RES_CTOR(ResTextureCoordinator)

    const nw::math::VEC2& GetScale() const { return this->ref().m_Scale; }
    void SetScale(const nw::math::VEC2& scale)
    {
        if (this->ref().m_Scale != scale)
        {
            this->ref().m_Scale.Set(scale.x, scale.y);
            this->SetDirty(true);
        }
    }

    f32 GetRotate() { return this->ref().m_Rotate; }
    void SetRotate(f32 rotate)
    {
        if (this->ref().m_Rotate != rotate)
        {
            this->ref().m_Rotate = rotate;
            this->SetDirty(true);
        }
    }

    const nw::math::VEC2& GetTranslate() const { return this->ref().m_Translate; }
    void SetTranslate(const nw::math::VEC2& translate)
    {
        if (this->ref().m_Translate != translate)
        {
            this->ref().m_Translate.Set(translate.x, translate.y);
            this->SetDirty(true);
        }
    }


    s32 GetSourceCoordinate() const { return static_cast<s32>(ref().m_SourceCoordinate); }
    void SetSourceCoordinate(s32 value) { ref().m_SourceCoordinate = value; }

    MappingMethod GetMappingMethod() const { return static_cast<MappingMethod>(ref().m_MappingMethod); }
    void SetMappingMethod(MappingMethod value) { ref().m_MappingMethod = value; }

    s32 GetReferenceCamera() const { return static_cast<s32>(ref().m_ReferenceCamera); }
    void SetReferenceCamera(s32 value) { ref().m_ReferenceCamera = value; }

    MappingMatrixMode GetMatrixMode() const { return static_cast<MappingMatrixMode>(ref().m_MatrixMode); }
    void SetMatrixMode(MappingMatrixMode value) { ref().m_MatrixMode = value; }

    bool IsDirty() const { return static_cast<bool>(ref().m_IsDirty); }
    void SetDirty(bool value) { ref().m_IsDirty = value; }

    bool IsEnabled() const { return static_cast<bool>(ref().m_IsEnabled); }
    void SetEnabled(bool value) { ref().m_IsEnabled = value; }

    nw::math::MTX34& GetTextureMatrix() { return ref().m_TextureMatrix; }
    const nw::math::MTX34& GetTextureMatrix() const { return ref().m_TextureMatrix; }
    void SetTextureMatrix(const nw::math::MTX34& value) { ref().m_TextureMatrix = value; }
};

class ResMaterial : public ResSceneObject
{
public:
    enum
{ TYPE_INFO       = NW_GFX_RES_TYPE_INFO(ResMaterial) };
    enum
{ SIGNATURE       = NW_RES_SIGNATURE32('MTOB') };
    enum
{ BINARY_REVISION = REVISION_RES_MATERIAL };

    enum TextureCoordinateConfig
    {
        CONFIG_0120,
        CONFIG_0110,
        CONFIG_0111,
        CONFIG_0112,
        CONFIG_0121,
        CONFIG_0122,
        CONFIG_NUM
    };

    enum TranslucencyKind
    {
        TRANSLUCENCY_KIND_LAYER0,
        TRANSLUCENCY_KIND_LAYER1,
        TRANSLUCENCY_KIND_LAYER2,
        TRANSLUCENCY_KIND_LAYER3,
        TRANSLUCENCY_KIND_OPAQUE      = TRANSLUCENCY_KIND_LAYER0,
        TRANSLUCENCY_KIND_TRANSLUCENT = TRANSLUCENCY_KIND_LAYER1,
        TRANSLUCENCY_KIND_SUBTRACTIVE = TRANSLUCENCY_KIND_LAYER2,
        TRANSLUCENCY_KIND_ADDITIVE    = TRANSLUCENCY_KIND_LAYER3,
        TRANSLUCENCY_KIND_END         = TRANSLUCENCY_KIND_LAYER3
    };

    NW_RES_CTOR_INHERIT(ResMaterial, ResSceneObject)

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    TextureCoordinateConfig GetTextureCoordinateConfig() const { return static_cast<TextureCoordinateConfig>(ref().m_TextureCoordinateConfig); }
    void SetTextureCoordinateConfig(TextureCoordinateConfig value) { ref().m_TextureCoordinateConfig = value; }

    TranslucencyKind GetTranslucencyKind() const { return static_cast<TranslucencyKind>(ref().m_TranslucencyKind); }
    void SetTranslucencyKind(TranslucencyKind value) { ref().m_TranslucencyKind = value; }

    ResMaterialColor GetMaterialColor() { return ResMaterialColor(&(ref().m_MaterialColor)); }
    const ResMaterialColor GetMaterialColor() const { return ResMaterialColor(&(ref().m_MaterialColor)); }
    ResMaterialColorData& GetMaterialColorData() { return ref().m_MaterialColor; }
    const ResMaterialColorData& GetMaterialColorData() const { return ref().m_MaterialColor; }

    ResRasterization GetRasterization() { return ResRasterization(&(ref().m_Rasterization)); }
    const ResRasterization GetRasterization() const { return ResRasterization(&(ref().m_Rasterization)); }
    ResRasterizationData& GetRasterizationData() { return ref().m_Rasterization; }
    const ResRasterizationData& GetRasterizationData() const { return ref().m_Rasterization; }

    ResFragmentOperation GetFragmentOperation() { return ResFragmentOperation(&(ref().m_FragmentOperation)); }
    const ResFragmentOperation GetFragmentOperation() const { return ResFragmentOperation(&(ref().m_FragmentOperation)); }
    ResFragmentOperationData& GetFragmentOperationData() { return ref().m_FragmentOperation; }
    const ResFragmentOperationData& GetFragmentOperationData() const { return ref().m_FragmentOperation; }

    s32 GetActiveTextureCoordinatorsCount() const { return static_cast<s32>(ref().m_ActiveTextureCoordinatorsCount); }
    void SetActiveTextureCoordinatorsCount(s32 value) { ref().m_ActiveTextureCoordinatorsCount = value; }

    s32 GetTextureCoordinatorsCount() const
        { return sizeof(ref().m_TextureCoordinators) / sizeof(ResTextureCoordinatorData); }

    ResTextureCoordinator GetTextureCoordinators(int idx)
        { return ResTextureCoordinator(&ref().m_TextureCoordinators[idx]); }

    const ResTextureCoordinator GetTextureCoordinators(int idx) const
        { return ResTextureCoordinator(&ref().m_TextureCoordinators[idx]); }

    ResTextureCoordinatorData& GetTextureCoordinatorsData(int idx)
        { return ref().m_TextureCoordinators[idx]; }

    const ResTextureCoordinatorData& GetTextureCoordinatorsData(int idx) const
        { return ref().m_TextureCoordinators[idx]; }

    ResTextureCoordinatorData* GetTextureCoordinatorsData()
        { return ref().m_TextureCoordinators; }

    const ResTextureCoordinatorData* GetTextureCoordinatorsData() const
        { return ref().m_TextureCoordinators; }

    s32 GetTextureMappersCount() const
        { return sizeof(ref().toTextureMappers) / sizeof(nw::ut::Offset); }
    ResPixelBasedTextureMapper GetTextureMappers(int idx)
        { NW_INDEX_ASSERT(TextureMappers, idx); return ResPixelBasedTextureMapper(ref().toTextureMappers[idx].to_ptr()); }
    const ResPixelBasedTextureMapper GetTextureMappers(int idx) const
        { return ResPixelBasedTextureMapper(ref().toTextureMappers[idx].to_ptr()); }
    nw::ut::internal::ResArray<ResPixelBasedTextureMapper, nw::ut::internal::ResArrayClassTraits> GetTextureMappers()
        { return nw::ut::internal::ResArray<ResPixelBasedTextureMapper, nw::ut::internal::ResArrayClassTraits>(&ref().toTextureMappers[0], GetTextureMappersCount()); }
    const nw::ut::internal::ResArray<const ResPixelBasedTextureMapper, nw::ut::internal::ResArrayClassTraits> GetTextureMappers() const
        { return nw::ut::internal::ResArray<const ResPixelBasedTextureMapper, nw::ut::internal::ResArrayClassTraits>(&ref().toTextureMappers[0], GetTextureMappersCount()); }

    ResProceduralTextureMapper GetProceduralTextureMapper() { return ResProceduralTextureMapper(ref().toProceduralTextureMapper.to_ptr()); }
    const ResProceduralTextureMapper GetProceduralTextureMapper() const { return ResProceduralTextureMapper(ref().toProceduralTextureMapper.to_ptr()); }

    ResShader GetShader() { return ResShader(ref().toShader.to_ptr()); }
    const ResShader GetShader() const { return ResShader(ref().toShader.to_ptr()); }

    ResFragmentShader GetFragmentShader() { return ResFragmentShader(ref().toFragmentShader.to_ptr()); }
    const ResFragmentShader GetFragmentShader() const { return ResFragmentShader(ref().toFragmentShader.to_ptr()); }

    s32 GetShaderProgramDescriptionIndex() const { return static_cast<s32>(ref().m_ShaderProgramDescriptionIndex); }
    void SetShaderProgramDescriptionIndex(s32 value) { ref().m_ShaderProgramDescriptionIndex = value; }

    s32 GetShaderParametersCount() const
        { return ref().m_ShaderParametersTableCount; }
    ResShaderParameter GetShaderParameters(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toShaderParametersTable.to_table_ptr());
            return ResShaderParameter(tbl_ptr[idx].to_ptr());
        }
    const ResShaderParameter GetShaderParameters(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toShaderParametersTable.to_table_ptr());
            return ResShaderParameter(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResShaderParameter, nw::ut::internal::ResArrayClassTraits> GetShaderParameters()
        {
            return nw::ut::internal::ResArray<ResShaderParameter, nw::ut::internal::ResArrayClassTraits>(ref().toShaderParametersTable.to_table_ptr(), GetShaderParametersCount());
        }
    const nw::ut::internal::ResArray<const ResShaderParameter, nw::ut::internal::ResArrayClassTraits> GetShaderParameters() const
        {
            return nw::ut::internal::ResArray<const ResShaderParameter, nw::ut::internal::ResArrayClassTraits>(ref().toShaderParametersTable.to_table_ptr(), GetShaderParametersCount());
        }

    s32 GetLightSetIndex() const { return static_cast<s32>(ref().m_LightSetIndex); }
    void SetLightSetIndex(s32 value) { ref().m_LightSetIndex = value; }

    s32 GetFogIndex() const { return static_cast<s32>(ref().m_FogIndex); }
    void SetFogIndex(s32 value) { ref().m_FogIndex = value; }

    u32 GetShadingParameterHash() const { return static_cast<u32>(ref().m_ShadingParameterHash); }
    void SetShadingParameterHash(u32 value) { ref().m_ShadingParameterHash = value; }

    u32 GetShaderParametersHash() const { return static_cast<u32>(ref().m_ShaderParametersHash); }
    void SetShaderParametersHash(u32 value) { ref().m_ShaderParametersHash = value; }

    u32 GetTextureCoordinatorsHash() const { return static_cast<u32>(ref().m_TextureCoordinatorsHash); }
    void SetTextureCoordinatorsHash(u32 value) { ref().m_TextureCoordinatorsHash = value; }

    u32 GetTextureSamplersHash() const { return static_cast<u32>(ref().m_TextureSamplersHash); }
    void SetTextureSamplersHash(u32 value) { ref().m_TextureSamplersHash = value; }

    u32 GetTextureMappersHash() const { return static_cast<u32>(ref().m_TextureMappersHash); }
    void SetTextureMappersHash(u32 value) { ref().m_TextureMappersHash = value; }

    u32 GetMaterialColorHash() const { return static_cast<u32>(ref().m_MaterialColorHash); }
    void SetMaterialColorHash(u32 value) { ref().m_MaterialColorHash = value; }

    u32 GetRasterizationHash() const { return static_cast<u32>(ref().m_RasterizationHash); }
    void SetRasterizationHash(u32 value) { ref().m_RasterizationHash = value; }

    u32 GetFragmentLightingHash() const { return static_cast<u32>(ref().m_FragmentLightingHash); }
    void SetFragmentLightingHash(u32 value) { ref().m_FragmentLightingHash = value; }

    u32 GetFragmentLightingTableHash() const { return static_cast<u32>(ref().m_FragmentLightingTableHash); }
    void SetFragmentLightingTableHash(u32 value) { ref().m_FragmentLightingTableHash = value; }

    u32 GetFragmentLightingTableParametersHash() const { return static_cast<u32>(ref().m_FragmentLightingTableParametersHash); }
    void SetFragmentLightingTableParametersHash(u32 value) { ref().m_FragmentLightingTableParametersHash = value; }

    u32 GetAlphaTestHash() const { return static_cast<u32>(ref().m_AlphaTestHash); }
    void SetAlphaTestHash(u32 value) { ref().m_AlphaTestHash = value; }

    u32 GetTextureCombinersHash() const { return static_cast<u32>(ref().m_TextureCombinersHash); }
    void SetTextureCombinersHash(u32 value) { ref().m_TextureCombinersHash = value; }

    u32 GetFragmentOperationHash() const { return static_cast<u32>(ref().m_FragmentOperationHash); }
    void SetFragmentOperationHash(u32 value) { ref().m_FragmentOperationHash = value; }

    u32 GetMaterialId() const { return static_cast<u32>(ref().m_MaterialId); }
    void SetMaterialId(u32 value) { ref().m_MaterialId = value; }

    u32 GetRevision() const { return this->GetHeader().revision; }

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);
    void   Cleanup();
    void   SetShader(ResShader resShader);
    void   ForceSetupTexture(const char* targetName, ResTexture texture);
    void   ForceSetupShader(const char* targetName, ResShader resShader);
    void   ForceSetupLookupTable(const char* targetName, ResLookupTable lut);

private:
    Result SetupShader(nw::os::IAllocator* allocator, ResShader resShader, ResGraphicsFile graphicsFile);
    Result SetupTextures(nw::os::IAllocator* allocator, ResMaterial resMaterial, ResGraphicsFile graphicsFile);
    void   CacheUserUniformIndex(ResShaderSymbolArray symbols);
    Result SetupFragmentLightingTable(nw::os::IAllocator* allocator, ResFragmentLightingTable resLightingTable, ResGraphicsFile graphicsFile);
    void   CalcFragmentLightingTableHash();
};

typedef nw::ut::ResArrayPatricia<ResMaterial>::type      ResMaterialArray;
typedef nw::ut::ResArrayPatricia<const ResMaterial>::type ResMaterialArrayConst;

}
}
}

