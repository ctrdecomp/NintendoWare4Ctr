#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/gfx_CommandUtil.h>

namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

struct ResFragmentLightingData{
    enum Flag{
        FLAG_CLAMP_HIGH_LIGHT_SHIFT,
        FLAG_DISTRIBUTION0_ENABLED_SHIFT,
        FLAG_DISTRIBUTION1_ENABLED_SHIFT,
        FLAG_GEOMETRIC_FACTOR0_ENABLED_SHIFT,
        FLAG_GEOMETRIC_FACTOR1_ENABLED_SHIFT,
        FLAG_REFLECTION_ENABLED_SHIFT,

        FLAG_CLAMP_HIGH_LIGHT         = 0x1 << FLAG_CLAMP_HIGH_LIGHT_SHIFT,
        FLAG_DISTRIBUTION0_ENABLED     = 0x1 << FLAG_DISTRIBUTION0_ENABLED_SHIFT,
        FLAG_DISTRIBUTION1_ENABLED     = 0x1 << FLAG_DISTRIBUTION1_ENABLED_SHIFT,
        FLAG_GEOMETRIC_FACTOR0_ENABLED = 0x1 << FLAG_GEOMETRIC_FACTOR0_ENABLED_SHIFT,
        FLAG_GEOMETRIC_FACTOR1_ENABLED = 0x1 << FLAG_GEOMETRIC_FACTOR1_ENABLED_SHIFT,
        FLAG_REFLECTION_ENABLED        = 0x1 << FLAG_REFLECTION_ENABLED_SHIFT
    };
    
    nw::ut::ResU32 mFlags;
    nw::ut::ResU32 mLayerConfig;
    nw::ut::ResS32 mFresnelConfig;
    nw::ut::ResU32 mBumpTextureIndex;
    nw::ut::ResU32 mBumpMode;
    nw::ut::ResBool mIsBumpRenormalize;
    u8 _padding_0[3];
};

struct ResLightingLookupTableData{
    nw::ut::ResU32 mInput;
    nw::ut::ResU32 mScale;
    nw::ut::Offset toSampler;
};

struct ResFragmentLightingTableData{
    nw::ut::Offset toReflectanceRSampler;
    nw::ut::Offset toReflectanceGSampler;
    nw::ut::Offset toReflectanceBSampler;
    nw::ut::Offset toDistribution0Sampler;
    nw::ut::Offset toDistribution1Sampler;
    nw::ut::Offset toFresnelSampler;
};

struct ResAlphaTestData{
    nw::ut::ResU32 mCommandBuffer[2];
};

struct ResTextureCombinerSourceData{
    nw::ut::ResU32 mSources[3];
};

struct ResTextureCombinerOperandData{
    nw::ut::ResU32 mOperands[3];
};

struct ResTextureCombinerData{
    nw::ut::ResU32 mConstant;
    
    union{
        struct{
            nw::ut::ResU32  mCmdSource;
            nw::ut::ResU32  mCmdHeader;
            nw::ut::ResU32  mCmdOperand;
            nw::ut::ResU32  mCmdCombine;
            nw::ut::ResU32  mCmdConstant;
            nw::ut::ResU32  mCmdScale;
        };
        nw::ut::ResU32 mCommandBuffer[6];
    };
};

struct ResFragmentShaderData{
    nw::ut::ResFloatColor     mBufferColor;
    ResFragmentLightingData   mFragmentLighting;
    nw::ut::Offset            toFragmentLightingTable;
    ResTextureCombinerData    mTextureCombiners[COMBINER_COUNT];
    ResAlphaTestData          mAlphaTest;
    nw::ut::ResU32            mBufferCommand[6];
};

class ResFragmentLighting : public nw::ut::ResCommon< ResFragmentLightingData >{
public:
    enum LutType{
        LUT_RR_SHIFT,
        LUT_RG_SHIFT,
        LUT_RB_SHIFT,
        LUT_D0_SHIFT,
        LUT_D1_SHIFT,
        LUT_FR_SHIFT,
        LUT_SP_SHIFT,
        LUT_DA_SHIFT,

        LUT_RR      = 0x1 << LUT_RR_SHIFT,
        LUT_RG      = 0x1 << LUT_RG_SHIFT,
        LUT_RB      = 0x1 << LUT_RB_SHIFT,
        LUT_D0      = 0x1 << LUT_D0_SHIFT,
        LUT_D1      = 0x1 << LUT_D1_SHIFT,
        LUT_FR      = 0x1 << LUT_FR_SHIFT,
        LUT_SP      = 0x1 << LUT_SP_SHIFT,
        LUT_DA      = 0x1 << LUT_DA_SHIFT
    };

    enum ConfigDetail{
        CONFIG_DETAIL0        = LUT_RR | LUT_D0 | LUT_SP | LUT_DA,
        CONFIG_DETAIL1        = LUT_RR | LUT_FR | LUT_SP | LUT_DA,
        CONFIG_DETAIL2        = LUT_RR | LUT_D0 | LUT_D1 | LUT_DA,
        CONFIG_DETAIL3        = LUT_D0 | LUT_D1 | LUT_SP | LUT_DA,
        CONFIG_DETAIL4        = LUT_RR | LUT_RG | LUT_RB | LUT_D0 | LUT_D1 | LUT_SP | LUT_DA,
        CONFIG_DETAIL5        = LUT_RR | LUT_RG | LUT_RB | LUT_D0 | LUT_FR | LUT_SP | LUT_DA,
        CONFIG_DETAIL6        = LUT_RR | LUT_D0 | LUT_D1 | LUT_FR | LUT_SP | LUT_DA,
        CONFIG_DETAIL7        = LUT_RR | LUT_RG | LUT_RB | LUT_D0 | LUT_D1 | LUT_FR | LUT_SP | LUT_DA,
    #ifdef NW_TARGET_CTR_GL_FINAL
        CONFIG_DETAIL8        = LUT_RR | LUT_D1 | LUT_SP | LUT_DA,
        CONFIG_DETAIL9        = LUT_D0 | LUT_D1 | LUT_SP | LUT_DA,
        CONFIG_DETAIL10       = LUT_RR | LUT_RG | LUT_RB | LUT_DA
    #else
        CONFIG_DETAIL8        = LUT_RR | LUT_RG | LUT_RB | LUT_D0 | LUT_D1 | LUT_FR | LUT_SP | LUT_DA,
        CONFIG_DETAIL9        = LUT_RR | LUT_RG | LUT_RB | LUT_D0 | LUT_D1 | LUT_FR | LUT_SP | LUT_DA,
        CONFIG_DETAIL10       = LUT_RR | LUT_RG | LUT_RB | LUT_D0 | LUT_D1 | LUT_FR | LUT_SP | LUT_DA
    #endif
    };

    enum Config{
        CONFIG_TYPE0        = 0,
        CONFIG_TYPE1        = 1,
        CONFIG_TYPE2        = 2,
        CONFIG_TYPE3        = 3,
        CONFIG_TYPE4        = 4,
        CONFIG_TYPE5        = 5,
        CONFIG_TYPE6        = 6,
        CONFIG_TYPE7        = 8,
    #if defined(NW_TARGET_CTR_GL_FINAL)
        CONFIG_TYPE8        = 12,
        CONFIG_TYPE9        = 13,
        CONFIG_TYPE10       = 14
    #else
        CONFIG_TYPE8        = 8,
        CONFIG_TYPE9        = 8,
        CONFIG_TYPE10       = 8
    #endif
    };

    enum FresnelConfig{
        CONFIG_NO_FRESNEL            = 0,
        CONFIG_PRI_ALPHA_FRESNEL     = 1,
        CONFIG_SEC_ALPHA_FRESNEL     = 2,
        CONFIG_PRI_SEC_ALPHA_FRESNEL = 3
    };
    
    enum BumpMode{
        BUMPMODE_NOT_USED = 0,
        BUMPMODE_AS_BUMP  = 1,
        BUMPMODE_AS_TANG  = 2
    };

    enum BumpTexture{
        BUMP_TEXTURE0 = 0,
        BUMP_TEXTURE1 = 1,
        BUMP_TEXTURE2 = 2,
        BUMP_TEXTURE3 = 3
    };
    
    NW_RES_CTOR( ResFragmentLighting )

    NW_RES_FIELD_FLAGS_DECL( u32, Flags )
    NW_RES_FIELD_PRIMITIVE_DECL( Config, LayerConfig )
    NW_RES_FIELD_PRIMITIVE_DECL( FresnelConfig, FresnelConfig )
    NW_RES_FIELD_PRIMITIVE_DECL( BumpTexture, BumpTextureIndex )
    NW_RES_FIELD_PRIMITIVE_DECL( BumpMode, BumpMode )
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( BumpRenormalize )
    
    bool IsReflectionEnabled() const { return nw::ut::CheckFlag(GetFlags(), ResFragmentLightingData::FLAG_REFLECTION_ENABLED); }

    static inline ConfigDetail ToConfigDetail( Config config ){
        s32 configNumber = static_cast<s32>(config) - static_cast<s32>(CONFIG_TYPE0);
        static const ConfigDetail configDetailTable[] ={
            CONFIG_DETAIL0,
            CONFIG_DETAIL1,
            CONFIG_DETAIL2,
            CONFIG_DETAIL3,
            CONFIG_DETAIL4,
            CONFIG_DETAIL5,
            CONFIG_DETAIL6,
            CONFIG_DETAIL7,
            CONFIG_DETAIL8,
            CONFIG_DETAIL9,
            CONFIG_DETAIL10
        };

        return configDetailTable[configNumber];
    }
};

class ResLightingLookupTable : public nw::ut::ResCommon<ResLightingLookupTableData>{
public:
    enum { SIGNATURE = NW_RES_SIGNATURE32('LLUT') };

    enum Input{
        INPUT_NH,
        INPUT_VH,
        INPUT_NV,
        INPUT_LN,
        INPUT_SP,
        INPUT_CP,
        INPUT_NUM
    };
    
    NW_RES_CTOR( ResLightingLookupTable )

    NW_RES_FIELD_PRIMITIVE_DECL( Input, Input )

    enum Scale{
        SCALE_1 = 0,
        SCALE_2 = 1,
        SCALE_4 = 2,
        SCALE_8 = 3,
        SCALE_0_25 = 6,
        SCALE_0_5 = 7,
        SCALE_MAX = 8
    };

    NW_RES_FIELD_PRIMITIVE_DECL( Scale, Scale )

    f32 GetScaleF32() const{
        const f32 TABLE[] = {
            1.0f, 2.0f, 4.0f, 8.0f, 0.0f, 0.0f, 0.25f, 0.5f
        };
        
        return TABLE[ref().mScale];
    }

    void SetScaleF32(f32 scale){
        if (scale == 1.0f)       { ref().mScale = SCALE_1; }
        else if (scale == 2.0f)  { ref().mScale = SCALE_2; }
        else if (scale == 4.0f)  { ref().mScale = SCALE_4; }
        else if (scale == 8.0f)  { ref().mScale = SCALE_8; }
        else if (scale == 0.25f) { ref().mScale = SCALE_0_25; }
        else if (scale == 0.5f)  { ref().mScale = SCALE_0_5; }
        else { NW_FATAL_ERROR("Illegal scale value"); }
    }

    NW_RES_FIELD_CLASS_DECL( ResLookupTable, Sampler )

    bool IsAbs() const {
        return this->GetSampler().Dereference().IsAbs();
    }

    void Cleanup() {
        NW_ASSERT( IsValid() );
        nw::ut::SafeCleanup(this->GetSampler());
    }

    /*void SetSampler(ResLookupTable lookupTable){
        NW_ASSERT(lookupTable.IsValid());
        ResImageLookupTable resImageLookupTable = lookupTable.Dereference();
        ResReferenceLookupTable referenceLut = nw::ut::ResStaticCast<res::ResReferenceLookupTable>(GetSampler());
        referenceLut.ref().toTargetLut.set_ptr(resImageLookupTable.ptr());
    }*/

    void ForceSetupSampler(const char* targetName, ResLookupTable lookupTable){
        NW_ASSERT(lookupTable.IsValid());
        NW_NULL_ASSERT(targetName);
        ResImageLookupTable resImageLookupTable = lookupTable.Dereference();
        ResReferenceLookupTable referenceLut = nw::ut::ResStaticCast<res::ResReferenceLookupTable>(GetSampler());
        referenceLut.ForceSetup(targetName, resImageLookupTable);
    }
};

class ResFragmentLightingTable : public nw::ut::ResCommon< ResFragmentLightingTableData >{
public:
    NW_RES_CTOR( ResFragmentLightingTable )

    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, ReflectanceRSampler )
    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, ReflectanceGSampler )
    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, ReflectanceBSampler )
    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, Distribution0Sampler )
    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, Distribution1Sampler )
    NW_RES_FIELD_CLASS_DECL( ResLightingLookupTable, FresnelSampler )

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();

    void ForceSetupLightingLut(const char* targetName, ResLookupTable lookupTable){
        NW_ASSERT(lookupTable.IsValid());
        NW_NULL_ASSERT(targetName);

        if (GetReflectanceRSampler().IsValid()){
            GetReflectanceRSampler().ForceSetupSampler(targetName, lookupTable);
        }

        if (GetReflectanceGSampler().IsValid()){
            GetReflectanceGSampler().ForceSetupSampler(targetName, lookupTable);
        }

        if (GetReflectanceBSampler().IsValid()){
            GetReflectanceBSampler().ForceSetupSampler(targetName, lookupTable);
        }

        if (GetDistribution0Sampler().IsValid()){
            GetDistribution0Sampler().ForceSetupSampler(targetName, lookupTable);
        }

        if (GetDistribution1Sampler().IsValid()){
            GetDistribution1Sampler().ForceSetupSampler(targetName, lookupTable);
        }

        if (GetFresnelSampler().IsValid()){
            GetFresnelSampler().ForceSetupSampler(targetName, lookupTable);
        }
    }
};

class ResTextureCombinerSource : public nw::ut::ResCommon< ResTextureCombinerSourceData >{
public:
    enum Kind{
        KIND_TEXTURE0                     = GL_TEXTURE0,
        KIND_TEXTURE1                     = GL_TEXTURE1,
        KIND_TEXTURE2                     = GL_TEXTURE2,
        KIND_TEXTURE3                     = GL_TEXTURE3,
        KIND_CONSTANT                     = GL_CONSTANT,
        KIND_PRIMARY_COLOR                = GL_PRIMARY_COLOR,
        KIND_PREVIOUS                     = GL_PREVIOUS,
        KIND_FRAGMENT_PRIMARY_COLOR_DMP   = GL_FRAGMENT_PRIMARY_COLOR_DMP,
        KIND_FRAGMENT_SECONDARY_COLOR_DMP = GL_FRAGMENT_SECONDARY_COLOR_DMP
    };
    
    NW_RES_CTOR( ResTextureCombinerSource )

    NW_RES_FIELD_PRIMITIVE_FIXED_LIST_DECL( u32, Sources )
};

class ResTextureCombinerOperand : public nw::ut::ResCommon< ResTextureCombinerOperandData >{
public:
    enum Kind{
        KIND_SRC_COLOR           = GL_SRC_COLOR,
        KIND_ONE_MINUS_SRC_COLOR = GL_ONE_MINUS_SRC_COLOR,
        KIND_SRC_ALPHA           = GL_SRC_ALPHA,
        KIND_ONE_MINUS_SRC_ALPHA = GL_ONE_MINUS_SRC_ALPHA,

    #ifdef NW_TARGET_CTR_GL_FINAL
        KIND_SRC_R               = GL_SRC_R_DMP,
        KIND_SRC_G               = GL_SRC_G_DMP,
        KIND_SRC_B               = GL_SRC_B_DMP,
        KIND_ONE_MINUS_SRC_R     = GL_ONE_MINUS_SRC_R_DMP,
        KIND_ONE_MINUS_SRC_G     = GL_ONE_MINUS_SRC_G_DMP,
        KIND_ONE_MINUS_SRC_B     = GL_ONE_MINUS_SRC_B_DMP
    #else
        KIND_SRC_R               = GL_SRC_COLOR,
        KIND_SRC_G               = GL_SRC_COLOR,
        KIND_SRC_B               = GL_SRC_COLOR,
        KIND_ONE_MINUS_SRC_R     = GL_ONE_MINUS_SRC_COLOR,
        KIND_ONE_MINUS_SRC_G     = GL_ONE_MINUS_SRC_COLOR,
        KIND_ONE_MINUS_SRC_B     = GL_ONE_MINUS_SRC_COLOR
    #endif
    };

    NW_RES_CTOR( ResTextureCombinerOperand )

    NW_RES_FIELD_PRIMITIVE_FIXED_LIST_DECL( u32, Operands )
};

class ResAlphaTest : public nw::ut::ResCommon< ResAlphaTestData >{
public:
    
    NW_RES_CTOR( ResAlphaTest )

    enum TestFunc{
        TESTFUNC_NEVER      = 0,
        TESTFUNC_ALWAYS     = 1,
        TESTFUNC_EQUAL      = 2,
        TESTFUNC_NOTEQUAL   = 3,
        TESTFUNC_LESS       = 4,
        TESTFUNC_LEQUAL     = 5,
        TESTFUNC_GREATER    = 6,
        TESTFUNC_GEQUAL     = 7
    };

    bool IsTestEnabled() const{
        enum { CMD_SHIFT = 0, CMD_MASK = 0x1 };
        
        return internal::GetCmdValue( ref().mCommandBuffer[0], CMD_MASK, CMD_SHIFT ) ? true : false;
    }

    void SetTestEnabled(bool value){
        enum { CMD_SHIFT = 0, CMD_MASK = 0x1 };
        
        internal::SetCmdValue( &ref().mCommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    TestFunc GetTestFunc() const{
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };
        
        return (TestFunc)internal::GetCmdValue( ref().mCommandBuffer[0], CMD_MASK, CMD_SHIFT );
    }

    void SetTestFunc(TestFunc value){
        enum { CMD_SHIFT = 4, CMD_MASK = 0x7 };

        internal::SetCmdValue( &ref().mCommandBuffer[0], value, CMD_MASK, CMD_SHIFT );
    }

    f32 GetTestReference() const{
        enum { CMD_SHIFT = 8, CMD_MASK = 0xff };
        
        return static_cast<f32>(internal::GetCmdValue( ref().mCommandBuffer[0], CMD_MASK, CMD_SHIFT )) / 255.0f;
    }

    void SetTestReference(f32 value){
        enum { CMD_SHIFT = 8, CMD_MASK = 0xff };

        internal::SetCmdValue( &ref().mCommandBuffer[0], (0.5f + (value * 255.0f)), CMD_MASK, CMD_SHIFT );
    }

    NW_RES_FIELD_COMMAND_BUFFER()
};

class ResTextureCombiner : public nw::ut::ResCommon< ResTextureCombinerData >{
public:
    enum Constant{
        CONSTANT0,
        CONSTANT1,
        CONSTANT2,
        CONSTANT3,
        CONSTANT4,
        CONSTANT5,
        EMISSION,
        AMBIENT,
        DIFFUSE,
        SPECULAR0,
        SPECULAR1
    };

    NW_RES_CTOR( ResTextureCombiner )

    NW_RES_FIELD_PRIMITIVE_DECL( Constant, Constant )
    
    NW_RES_FIELD_COMMAND_BUFFER();

    void SetConstantCmd(u32 rgba) { ref().mCmdConstant = rgba; }
    
    typedef enum {
        SOURCE_PRIMARY_COLOR = 0,
        SOURCE_FRAGMENT_PRIMARY_COLOR,
        SOURCE_FRAGMENT_SECONDARY_COLOR,
        SOURCE_TEXTURE0,
        SOURCE_TEXTURE1,
        SOURCE_TEXTURE2,
        SOURCE_TEXTURE3,
        SOURCE_CONSTANT = 0xE,
        SOURCE_PREVIOUS = 0xF,
        SOURCE_PREVIOUS_BUFFER = 0xD
    } Source;

    typedef enum{
        OPERANDRGB_SRC_COLOR = 0,
        OPERANDRGB_ONE_MINUS_SRC_COLOR = 0x1,
        OPERANDRGB_SRC_ALPHA = 0x2,
        OPERANDRGB_ONE_MINUS_SRC_ALPHA = 0x3,
        
        OPERANDRGB_RED = 0x4,
        OPERANDRGB_ONE_MINUS_RED   = 0x5,
        OPERANDRGB_GREEN = 0x8,
        OPERANDRGB_ONE_MINUS_GREEN = 0x9,
        OPERANDRGB_BLUE = 0xC,
        OPERANDRGB_ONE_MINUS_BLUE  = 0xD
    } OperandRgb;
    
    typedef enum{
        OPERANDALPHA_SRC_ALPHA = 0,
        OPERANDALPHA_SRC_ONE_MINUS_ALPHA = 0x1,
        
        OPERANDALPHA_RED = 0x2,
        OPERANDALPHA_ONE_MINUS_RED = 0x3,
        OPERANDALPHA_GREEN = 0x4,
        OPERANDALPHA_ONE_MINUS_GREEN = 0x5,
        OPERANDALPHA_BLUE = 0x6,
        OPERANDALPHA_ONE_MINUS_BLUE = 0x7
        
    } OperandAlpha;
    
    typedef enum{
        COMBINE_REPLACE = 0,
        COMBINE_MODULATE,
        COMBINE_ADD,
        COMBINE_ADDSIGNED,
        COMBINE_INTERPOLATE,
        COMBINE_SUBTRACT,
        COMBINE_DOT3_RGB,
        COMBINE_DOT3_RGBA,
        COMBINE_MULT_ADD,
        COMBINE_ADD_MULT
    } Combine;

    typedef enum{
        SCALE_ONE = 0,
        SCALE_TWO,
        SCALE_FOUR
    } Scale;

    Source GetSourceRgb(s32 index) const {
        enum { CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        return (Source)internal::GetCmdValue( ref().mCmdSource, CMD_MASK, CMD_WIDTH * index );
    }

    void SetSourceRgb(s32 index, Source value){
        enum { CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        internal::SetCmdValue( &ref().mCmdSource, value, CMD_MASK, CMD_WIDTH * index );
    }

    Source GetSourceAlpha(s32 index) const{
        enum { CMD_SHIFT = 16, CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        return (Source)internal::GetCmdValue( ref().mCmdSource, CMD_MASK, CMD_WIDTH * index + CMD_SHIFT );
    }
    
    void SetSourceAlpha(s32 index, Source value){
        enum { CMD_SHIFT = 16, CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        internal::SetCmdValue( &ref().mCmdSource, value, CMD_MASK, CMD_WIDTH * index + CMD_SHIFT );
    }
    
    OperandRgb GetOperandRgb(s32 index) const{
        enum { CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        return (OperandRgb)internal::GetCmdValue( ref().mCmdOperand, CMD_MASK, CMD_WIDTH * index );
    }

    void SetOperandRgb(s32 index, OperandRgb value){
        enum { CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        internal::SetCmdValue( &ref().mCmdOperand, value, CMD_MASK, CMD_WIDTH * index );
    }

    OperandAlpha GetOperandAlpha(s32 index) const{
        enum { CMD_SHIFT = 12, CMD_WIDTH = 4, CMD_MASK = 0xF };
        
        return (OperandAlpha)internal::GetCmdValue( ref().mCmdOperand, CMD_MASK, CMD_WIDTH * index + CMD_SHIFT );
    }

    void SetOperandAlpha(s32 index, OperandAlpha value){
        enum { CMD_SHIFT = 12, CMD_WIDTH = 4, CMD_MASK = 0xF };
        internal::SetCmdValue( &ref().mCmdOperand, value, CMD_MASK, CMD_WIDTH * index + CMD_SHIFT );
    }

    Combine GetCombineRgb() const{
        enum { CMD_SHIFT = 0, CMD_MASK = 0xF };
        return (Combine)internal::GetCmdValue( ref().mCmdCombine, CMD_MASK, CMD_SHIFT );
    }

    void SetCombineRgb(Combine value){
        enum { CMD_SHIFT = 0, CMD_MASK = 0xF };
        internal::SetCmdValue( &ref().mCmdCombine, value, CMD_MASK, CMD_SHIFT );
    }

    Combine GetCombineAlpha() const{
        enum { CMD_SHIFT = 16, CMD_MASK = 0xF };
        return (Combine)internal::GetCmdValue( ref().mCmdCombine, CMD_MASK, CMD_SHIFT );
    }

    void SetCombineAlpha(Combine value){
        enum { CMD_SHIFT = 16, CMD_MASK = 0xF };
        internal::SetCmdValue( &ref().mCmdCombine, value, CMD_MASK, CMD_SHIFT );
    }

    Scale GetScaleRgb() const{
        enum { CMD_SHIFT = 0, CMD_MASK = 0x3 };
        return (Scale)internal::GetCmdValue( ref().mCmdScale, CMD_MASK, CMD_SHIFT );
    }

    void SetScaleRgb(Scale value){
        enum { CMD_SHIFT = 0, CMD_MASK = 0x3 };
        internal::SetCmdValue( &ref().mCmdScale, value, CMD_MASK, CMD_SHIFT );
    }

    Scale GetScaleAlpha() const{
        enum { CMD_SHIFT = 16, CMD_MASK = 0x3 };
        return (Scale)internal::GetCmdValue( ref().mCmdScale, CMD_MASK, CMD_SHIFT );
    }

    void SetScaleAlpha(Scale value){
        enum { CMD_SHIFT = 16, CMD_MASK = 0x3 };
        internal::SetCmdValue( &ref().mCmdScale, value, CMD_MASK, CMD_SHIFT );
    }
};

class ResFragmentShader : public nw::ut::ResCommon< ResFragmentShaderData >{
public:
    enum BufferInput{
        BUFFERINPUT_PREVIOUS_BUFFER  = 0,
        BUFFERINPUT_PREVIOUS = 1
    };

    NW_RES_CTOR( ResFragmentShader )


    NW_RES_FIELD_RESSTRUCT_DECL( ResFragmentLighting, FragmentLighting )
    NW_RES_FIELD_CLASS_DECL( ResFragmentLightingTable, FragmentLightingTable )
    NW_RES_FIELD_STRUCT_FIXED_LIST_DECL( ResTextureCombiner, TextureCombiners )
    NW_RES_FIELD_RESSTRUCT_DECL( ResAlphaTest, AlphaTest )


    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);

    void Cleanup();

    bool CheckFragmentShader();

    u32 GetBufferColorU32() const{
        return ref().mBufferCommand[0];
    }

    void SetBufferColor( f32 r, f32 g, f32 b ){
        ref().mBufferColor.Set(r, g, b);
        ref().mBufferCommand[0] = ref().mBufferColor.ToPicaU32();
    }

    void SetBufferColor( f32 r, f32 g, f32 b, f32 a ){
        ref().mBufferColor.Set(r, g, b, a);
        ref().mBufferCommand[0] = ref().mBufferColor.ToPicaU32();
    }

    const nw::ut::FloatColor& GetBufferColor() const{
        return ref().mBufferColor;
    }

    void SetBufferColor(const nw::ut::FloatColor& value){
        ref().mBufferColor = value;
        ref().mBufferCommand[0] = ref().mBufferColor.ToPicaU32();
    }

    BufferInput GetBufferInputRgb(s32 index) const{
        enum { CMD_SHIFT = 7, CMD_MASK = 0x1 };
        return static_cast<BufferInput>(internal::GetCmdValue( ref().mBufferCommand[2], CMD_MASK, CMD_SHIFT+index ));
    }

    void SetBufferInputRgb(s32 index, BufferInput value){
        enum { CMD_SHIFT = 7, CMD_MASK = 0x1 };
        internal::SetCmdValue( &ref().mBufferCommand[2], value, CMD_MASK, CMD_SHIFT+index );
    }

    BufferInput GetBufferInputAlpha(s32 index) const{
        enum { CMD_SHIFT = 11, CMD_MASK = 0x1 };
        return static_cast<BufferInput>(internal::GetCmdValue( ref().mBufferCommand[2], CMD_MASK, CMD_SHIFT+index ));
    }

    void SetBufferInputAlpha(s32 index, BufferInput value){
        enum { CMD_SHIFT = 11, CMD_MASK = 0x1 };
        internal::SetCmdValue(&ref().mBufferCommand[2], value, CMD_MASK, CMD_SHIFT+index );
    }

    void ForceSetupLightingLut(const char* targetName, ResLookupTable lookupTable){
        NW_ASSERT(lookupTable.IsValid());
        NW_NULL_ASSERT(targetName);
        if (GetFragmentLightingTable().IsValid()){
            GetFragmentLightingTable().ForceSetupLightingLut(targetName, lookupTable);
        }
    }
};

}
}
}