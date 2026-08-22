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

#ifndef NW_GFX_RESSHADER_H_
#define NW_GFX_RESSHADER_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/gfx/gfx_Config.h>
#include <nw/gfx/gfx_ShaderBinaryInfo.h>

namespace nw {

namespace os {
class IAllocator;
}

namespace gfx {

namespace res {

class ResGraphicsFile;
class ResShaderSymbol;
class ResBinaryShader;

//
typedef ut::ResArrayClass<ResShaderSymbol>::type::iterator ResShaderSymbolArrayIterator;
//
typedef ut::ResArrayClass<const ResShaderSymbol>::type::const_iterator ResShaderSymbolArrayConstIterator;

//
typedef ut::ResArrayClass<ResShaderSymbol>::type ResShaderSymbolArray;

//
typedef ut::ResArrayClass<const ResShaderSymbol>::type ResShaderSymbolArrayConst;

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShaderParameterValueData
{
    nw::ut::ResS32 m_UniformType;   //
    nw::ut::ResF32 m_Value[1];      //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShaderSymbolData
{
    nw::ut::BinString toName;                   //
    nw::ut::ResBool m_IsEnabled;                //
    nw::ut::ResBool m_IsGeometryUniform;        //
    u8 _padding_0[2];                           //
    nw::ut::ResS32 m_Location;                  //
    ResShaderParameterValueData m_DefaultValue; //
    // Since ResShaderParameterValueData is of variable length, do not insert data after it.
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShaderParameterData
{
    nw::ut::BinString toName;                   //
    nw::ut::ResS32 m_SymbolIndex;               //
    ResShaderParameterValueData m_Parameter;    //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShaderProgramDescriptionData
{
    //
    enum { VERTEX_ATTRIBUTE_USAGE_COUNT = 22 };

    nw::ut::ResU32 m_Flags;                 //
    nw::ut::ResU32 m_VertexShaderObject;    //
    nw::ut::ResU32 m_GeometryShaderObject;  //
    nw::ut::ResS32 m_VertexShaderIndex;     //
    nw::ut::ResS32 m_GeometryShaderIndex;   //
    nw::ut::ResS32 m_SymbolsTableCount;     //
    nw::ut::Offset toSymbolsTable;          //
    nw::ut::BinString toAttributeSymbols[VERTEX_ATTRIBUTE_COUNT];   //
    nw::ut::ResS8 m_AttributeIndices[VERTEX_ATTRIBUTE_USAGE_COUNT]; //
    u8 _padding_0[2];                       //
    nw::ut::ResS32 m_MaxBoneCount;          //
    nw::ut::ResS32 m_MaxVertexLightCount;   //
    nw::ut::ResS32 m_VertexLightEndUniform; //
    nw::ut::ResS32 m_GeometryShaderMode;    //
    nw::ut::ResU32 m_ProgramObject;         //
    void* m_UniformLocation;                //

    void* m_CommandCache;                   //
    u32   m_CommandCacheSize;               //

    nw::ut::Offset toOwnerShader;           //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShaderData : public ResSceneObjectData
{
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResBinaryShaderData : public ResShaderData
{
    nw::ut::ResS32 m_BinaryDataTableCount;      //
    nw::ut::Offset toBinaryDataTable;           //
    nw::ut::ResS32 m_ShaderKindsTableCount;     //
    nw::ut::Offset toShaderKindsTable;          //
    nw::ut::ResS32 m_DescriptionsTableCount;    //
    nw::ut::Offset toDescriptionsTable;         //
    nw::ut::ResS32 m_ShaderObjectsTableCount;   //
    nw::ut::Offset toShaderObjectsTable;        //
    void*          m_CommandCache;              //
    s32            m_CommandCacheSize;          //
    void*          m_ShaderBinaryInfo;          //
    nw::os::IAllocator* m_CommandAllocator;     //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResReferenceShaderData : public ResShaderData
{
    nw::ut::BinString toPath;                   //
    nw::ut::Offset toTargetShader;              //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShaderParameterValue : public nw::ut::ResCommon< ResShaderParameterValueData >
{
public:
    //
    enum UniformType
    {
        TYPE_BOOL1,     //
        TYPE_FLOAT1,    //
        TYPE_FLOAT2,    //
        TYPE_FLOAT3,    //
        TYPE_FLOAT4     //
    };

    NW_RES_CTOR( ResShaderParameterValue )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( UniformType, UniformType ) // GetUniformType(), SetUniformType()

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const f32* GetValueF32() const { return &ref().m_Value[0]; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    f32*       GetValueF32()       { return &ref().m_Value[0]; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const s32* GetValueS32() const { return reinterpret_cast<const s32*>( &ref().m_Value[0] ); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32*       GetValueS32()       { return reinterpret_cast<s32*>( &ref().m_Value[0] ); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool       GetValueBool() const { return *reinterpret_cast<const s32*>( &ref().m_Value[0]) != 0; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(bool value)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_BOOL1 );
        s32* pValue = this->GetValueS32();
        *pValue = value ? 1 : 0;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(f32 x)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT1 );
        f32* pValue = this->GetValueF32();
        *pValue = x;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(f32 x, f32 y)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT2 );
        f32* pValue = this->GetValueF32();
        *pValue++ = x;
        *pValue   = y;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(const math::VEC2& value)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT2 );
        f32* pDstValue = this->GetValueF32();
        const f32* pSrcValue = value;

        *pDstValue++ = *pSrcValue++;
        *pDstValue   = *pSrcValue;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(f32 x, f32 y, f32 z)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT3 );
        f32* pValue = this->GetValueF32();
        *pValue = x; ++pValue;
        *pValue = y; ++pValue;
        *pValue = z;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(const math::VEC3& value)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT3 );
        f32* pDstValue = this->GetValueF32();
        const f32* pSrcValue = value;

        *pDstValue++ = *pSrcValue++;
        *pDstValue++ = *pSrcValue++;
        *pDstValue   = *pSrcValue;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(f32 x, f32 y, f32 z, f32 w)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT4 );
        f32* pValue = this->GetValueF32();
        *pValue = x; ++pValue;
        *pValue = y; ++pValue;
        *pValue = z; ++pValue;
        *pValue = w;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void       SetValue(const math::VEC4& value)
    {
        NW_ASSERT( this->GetUniformType() == TYPE_FLOAT4 );
        f32* pDstValue = this->GetValueF32();
        const f32* pSrcValue = value;

        *pDstValue++ = *pSrcValue++;
        *pDstValue++ = *pSrcValue++;
        *pDstValue++ = *pSrcValue++;
        *pDstValue   = *pSrcValue;
    }
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShaderParameter : public nw::ut::ResCommon< ResShaderParameterData >
{
public:
    NW_RES_CTOR( ResShaderParameter )

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
    NW_RES_FIELD_STRING_DECL( Name )                // GetName()
    NW_RES_FIELD_PRIMITIVE_DECL( s32, SymbolIndex ) // GetSymbolIndex(), SetSymbolIndex()
    NW_RES_FIELD_RESSTRUCT_DECL( ResShaderParameterValue, Parameter ) // GetParamter()

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    s32 GetParameterLength()
    {
        switch (GetParameter().GetUniformType())
        {
        case ResShaderParameterValue::TYPE_BOOL1:
            return 1;

        case ResShaderParameterValue::TYPE_FLOAT1:
            return 1;

        case ResShaderParameterValue::TYPE_FLOAT2:
            return 2;

        case ResShaderParameterValue::TYPE_FLOAT3:
            return 3;

        case ResShaderParameterValue::TYPE_FLOAT4:
            return 4;

        default:
            NW_FATAL_ERROR("Unsupported ShaderParameterValue type.");
            return 0;
        }
    }
};

//
typedef nw::ut::ResArrayClass<ResShaderParameter>::type  ResShaderParameterArray;
//
typedef nw::ut::ResArrayClass<const ResShaderParameter>::type  ResShaderParameterArrayConst;


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShaderSymbol : public nw::ut::ResCommon< ResShaderSymbolData >
{
public:
    NW_RES_CTOR( ResShaderSymbol )

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
    NW_RES_FIELD_STRING_DECL( Name )             // GetName()
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Enabled )  // IsEnabled(), SetEnabled()
    NW_RES_FIELD_RESSTRUCT_DECL( ResShaderParameterValue, DefaultValue ) // GetDefaultValue()
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( GeometryUniform ) // IsGeometryUniform(), SetGeometryUniform()
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Location) // GetLocation(), SetLocation()
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShaderProgramDescription : public nw::ut::ResCommon< ResShaderProgramDescriptionData >
{
public:

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum Flag
    {
        FLAG_IS_SUPPORTING_RIGID_SKINNING      = 0x1 << 0,      //
        FLAG_IS_SUPPORTING_SMOOTH_SKINNING     = 0x1 << 1,      //
        FLAG_IS_SUPPORTING_HEMISPHERE_LIGHTING = 0x1 << 2,      //
        FLAG_IS_SUPPORTING_VERTEX_MORPH_SHADER = 0x1 << 3       //
    };

    NW_RES_CTOR( ResShaderProgramDescription )

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
    NW_RES_FIELD_PRIMITIVE_DECL( u32, VertexShaderObject )          // GetVertexShaderObject(), SetVertexShaderObject()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, GeometryShaderObject )        // GetGeometryShaderObject(), SetGeometryShaderObject()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, VertexShaderIndex )           // GetVertexShaderIndex(), SetVertexShaderIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, GeometryShaderIndex )         // GetGeometryShaderIndex(), SetGeometryShaderIndex()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResShaderSymbol, Symbols )        // GetSymbols(int idx), GetSymbolsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_FIXED_LIST_DECL( AttributeSymbols )         // GetAttributeSymbols(int idx), GetAttributeSymbolsCount()

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
    NW_RES_FIELD_PRIMITIVE_FIXED_LIST_DECL( s8, AttributeIndices )  // GetAttributeIndices(int idx), GetAttributeIndices(), GetAttributeIndicesCount(), SetAttributeIndices(int,s8)

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, MaxBoneCount )                // GetMaxBoneCount(), SetMaxBoneCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, MaxVertexLightCount )         // GetMaxVertexLightCount(), SetMaxVertexLightCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, VertexLightEndUniform )       // GetVertexLightEndUniform(), SetVertexLightEndUniform()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, GeometryShaderMode )          // GetGeometryShaderMode(), SetGeometryShaderMode()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, ProgramObject )               // GetProgramObject(), SetProgramObject()

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    void*       GetUniformLocation() { return ref().m_UniformLocation; }
    const void* GetUniformLocation() const { return ref().m_UniformLocation; }
    void        SetUniformLocation(void* uniformLocation) { ref().m_UniformLocation = uniformLocation; }
#endif

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ResBinaryShaderData* GetOwnerShaderData()
    {
        return static_cast<ResBinaryShaderData*>( ref().toOwnerShader.to_ptr() );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ResBinaryShaderData* GetOwnerShaderData() const
    {
        return static_cast<const ResBinaryShaderData*>( ref().toOwnerShader.to_ptr() );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ShaderBinaryInfo* GetShaderBinaryInfo() const
    {
        return static_cast<const ShaderBinaryInfo*>( this->GetOwnerShaderData()->m_ShaderBinaryInfo );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetVertexUniformIndex( const char* name, ShaderBinaryInfo::SymbolType* pSymbolType ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    s32 GetGeometryUniformIndex( const char* name, ShaderBinaryInfo::SymbolType* pSymbolType ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    Result Setup(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
private:
    GLuint CreateProgramObject();
    void AttachProgram(GLuint programObject);
    void LinkProgram(GLuint programObject);
#endif
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShader : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShader) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SHDR') };
    enum { BINARY_REVISION = REVISION_RES_SHADER };

    NW_RES_CTOR_INHERIT( ResShader, ResSceneObject )

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetRevision() const { return this->GetHeader().revision; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Result Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile);
    Result Setup(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void Cleanup();

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    ResBinaryShader        Dereference();

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    const ResBinaryShader  Dereference() const;
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResBinaryShader : public ResShader
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResBinaryShader) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('BSHD') };

    NW_RES_CTOR_INHERIT( ResBinaryShader, ResShader )

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, BinaryData )         // GetBinaryData(), GetBinaryData(int idx), GetBinaryDataCount()

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u32, ShaderKinds )       // GetShaderKinds(), GetShaderKinds(int idx), GetShaderKindsCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResShaderProgramDescription, Descriptions ) // GetDescriptions(int idx), GetDescriptionsCount()

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u32, ShaderObjects )     // GetShaderObjects(), GetShaderObjects(int idx), GetShaderObjectsCount()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const void* GetCommandCache() const { return ref().m_CommandCache; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    int GetCommandCacheSize() const { return ref().m_CommandCacheSize; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ShaderBinaryInfo* GetShaderBinaryInfo()
    {
        return static_cast<ShaderBinaryInfo*>( ref().m_ShaderBinaryInfo );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ShaderBinaryInfo* GetShaderBinaryInfo() const
    {
        return static_cast<const ShaderBinaryInfo*>( ref().m_ShaderBinaryInfo );
    }
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResReferenceShader : public ResShader
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceShader) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SDRF') };

    NW_RES_CTOR_INHERIT( ResReferenceShader, ResShader )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( Path )                     // GetPath()
    NW_RES_FIELD_CLASS_DECL( ResShader, TargetShader )   // GetTargetShader()
};

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESSHADER_H_
