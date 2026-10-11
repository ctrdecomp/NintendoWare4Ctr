#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
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

typedef nw::ut::ResArrayClass<ResShaderSymbol>::type::iterator ResShaderSymbolArrayIterator;
typedef nw::ut::ResArrayClass<const ResShaderSymbol>::type::const_iterator ResShaderSymbolArrayConstIterator;
typedef nw::ut::ResArrayClass<ResShaderSymbol>::type ResShaderSymbolArray;
typedef nw::ut::ResArrayClass<const ResShaderSymbol>::type ResShaderSymbolArrayConst;

struct ResShaderParameterValueData
{
    nw::ut::ResS32 m_UniformType;
    nw::ut::ResF32 m_Value[1];
};

struct ResShaderSymbolData
{
    nw::ut::BinString toName;
    nw::ut::ResBool m_IsEnabled;
    nw::ut::ResBool m_IsGeometryUniform;
    u8 _padding_0[2];
    nw::ut::ResS32 m_Location;
    ResShaderParameterValueData m_DefaultValue;
};

struct ResShaderParameterData
{
    nw::ut::BinString toName;
    nw::ut::ResS32 m_SymbolIndex;
    ResShaderParameterValueData m_Parameter;
};

struct ResShaderProgramDescriptionData
{
    enum
    {
        VERTEX_ATTRIBUTE_USAGE_COUNT = 22
    };
    
    nw::ut::ResU32 m_Flags;
    nw::ut::ResU32 m_VertexShaderObject;
    nw::ut::ResU32 m_GeometryShaderObject;
    nw::ut::ResS32 m_VertexShaderIndex;
    nw::ut::ResS32 m_GeometryShaderIndex;
    nw::ut::ResS32 m_SymbolsTableCount;
    nw::ut::Offset toSymbolsTable;
    nw::ut::BinString toAttributeSymbols[VERTEX_ATTRIBUTE_COUNT];
    nw::ut::ResS8 m_AttributeIndices[VERTEX_ATTRIBUTE_USAGE_COUNT];
    u8 _padding_0[2];
    nw::ut::ResS32 m_MaxBoneCount;
    nw::ut::ResS32 m_MaxVertexLightCount;
    nw::ut::ResS32 m_VertexLightEndUniform;
    nw::ut::ResS32 m_GeometryShaderMode;
    nw::ut::ResU32 m_ProgramObject;
    void* m_UniformLocation;

    void* m_CommandCache;
    u32   m_CommandCacheSize;
    
    nw::ut::Offset toOwnerShader;
};

struct ResShaderData : public ResSceneObjectData
{
};

struct ResBinaryShaderData : public ResShaderData
{
    nw::ut::ResS32 m_BinaryDataTableCount;
    nw::ut::Offset toBinaryDataTable;
    nw::ut::ResS32 m_ShaderKindsTableCount;
    nw::ut::Offset toShaderKindsTable;
    nw::ut::ResS32 m_DescriptionsTableCount;
    nw::ut::Offset toDescriptionsTable;
    nw::ut::ResS32 m_ShaderObjectsTableCount;
    nw::ut::Offset toShaderObjectsTable;
    void*          m_CommandCache;
    s32            m_CommandCacheSize;
    void*          m_ShaderBinaryInfo;
    nw::os::IAllocator* m_CommandAllocator;
};

struct ResReferenceShaderData : public ResShaderData
{
    nw::ut::BinString toPath;
    nw::ut::Offset toTargetShader;
};

class ResShaderParameterValue : public nw::ut::ResCommon< ResShaderParameterValueData >
{
public:
    enum UniformType
    {
        TYPE_BOOL1,
        TYPE_FLOAT1,
        TYPE_FLOAT2,
        TYPE_FLOAT3,
        TYPE_FLOAT4
    };
    
    NW_RES_CTOR( ResShaderParameterValue )


    UniformType GetUniformType() const { return static_cast<UniformType>(ref().m_UniformType); }
    void SetUniformType(UniformType value) { ref().m_UniformType = value; }


    const f32* GetValueF32() const { return &ref().m_Value[0]; }
    f32*       GetValueF32() { return &ref().m_Value[0]; }
    const s32* GetValueS32() const { return reinterpret_cast<const s32*>( &ref().m_Value[0] ); }
    s32*       GetValueS32() { return reinterpret_cast<s32*>( &ref().m_Value[0] ); }
    bool       GetValueBool() const { return *reinterpret_cast<const s32*>( &ref().m_Value[0]) != 0; }

    void SetValue(bool value) 
    { 
        NW_ASSERT( this->GetUniformType() == TYPE_BOOL1 ); 
        s32* pValue = this->GetValueS32();
        *pValue = value ? 1 : 0; 
    }

    void SetValue(f32 x)
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT1);
        f32* pValue = this->GetValueF32();
        *pValue = x;
    }
    

    void SetValue(f32 x, f32 y)
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT2);
        f32* pValue = this->GetValueF32();
        *pValue++ = x;
        *pValue   = y;
    }

    void SetValue(const nw::math::VEC2& value) 
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT2);
        f32* pDstValue = this->GetValueF32();
        const f32* pSrcValue = value;
        
        *pDstValue++ = *pSrcValue++;
        *pDstValue   = *pSrcValue;
    }

    void SetValue(f32 x, f32 y, f32 z)
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT3);
        f32* pValue = this->GetValueF32();
        *pValue = x; ++pValue;
        *pValue = y; ++pValue;
        *pValue = z;
    }

    void SetValue(const nw::math::VEC3& value) 
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT3);
        f32* pDstValue = this->GetValueF32();
        const f32* pSrcValue = value;
        
        *pDstValue++ = *pSrcValue++;
        *pDstValue++ = *pSrcValue++;
        *pDstValue   = *pSrcValue;
    }

    void SetValue(f32 x, f32 y, f32 z, f32 w)
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT4);
        f32* pValue = this->GetValueF32();
        *pValue = x; ++pValue;
        *pValue = y; ++pValue;
        *pValue = z; ++pValue;
        *pValue = w;
    }

    void SetValue(const nw::math::VEC4& value) 
    {
        NW_ASSERT(this->GetUniformType() == TYPE_FLOAT4);
        f32* pDstValue = this->GetValueF32();
        const f32* pSrcValue = value;
        
        *pDstValue++ = *pSrcValue++;
        *pDstValue++ = *pSrcValue++;
        *pDstValue++ = *pSrcValue++;
        *pDstValue   = *pSrcValue;
    }
};

class ResShaderParameter : public nw::ut::ResCommon< ResShaderParameterData >
{
public:
    NW_RES_CTOR( ResShaderParameter )


    const char* GetName() const { return ref().toName.to_ptr(); }

    s32 GetSymbolIndex() const { return static_cast<s32>(ref().m_SymbolIndex); }
    void SetSymbolIndex(s32 value) { ref().m_SymbolIndex = value; }

    ResShaderParameterValue GetParameter() { return ResShaderParameterValue(&(ref().m_Parameter)); }
    const ResShaderParameterValue GetParameter() const { return ResShaderParameterValue(&(ref().m_Parameter)); }
    ResShaderParameterValueData& GetParameterData() { return ref().m_Parameter; }
    const ResShaderParameterValueData& GetParameterData() const { return ref().m_Parameter; }

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

typedef nw::ut::ResArrayClass<ResShaderParameter>::type  ResShaderParameterArray;
typedef nw::ut::ResArrayClass<const ResShaderParameter>::type  ResShaderParameterArrayConst;

class ResShaderSymbol : public nw::ut::ResCommon< ResShaderSymbolData >
{
public:
    NW_RES_CTOR( ResShaderSymbol )


    const char* GetName() const { return ref().toName.to_ptr(); }

    bool IsEnabled() const { return static_cast<bool>(ref().m_IsEnabled); }
    void SetEnabled(bool value) { ref().m_IsEnabled = value; }

    ResShaderParameterValue GetDefaultValue() { return ResShaderParameterValue(&(ref().m_DefaultValue)); }
    const ResShaderParameterValue GetDefaultValue() const { return ResShaderParameterValue(&(ref().m_DefaultValue)); }
    ResShaderParameterValueData& GetDefaultValueData() { return ref().m_DefaultValue; }
    const ResShaderParameterValueData& GetDefaultValueData() const { return ref().m_DefaultValue; }

    bool IsGeometryUniform() const { return static_cast<bool>(ref().m_IsGeometryUniform); }
    void SetGeometryUniform(bool value) { ref().m_IsGeometryUniform = value; }

    s32 GetLocation() const { return static_cast<s32>(ref().m_Location); }
    void SetLocation(s32 value) { ref().m_Location = value; }
};

class ResShaderProgramDescription : public nw::ut::ResCommon< ResShaderProgramDescriptionData >
{
public:
    enum Flag
    {
        FLAG_IS_SUPPORTING_RIGID_SKINNING      = 0x1 << 0,
        FLAG_IS_SUPPORTING_SMOOTH_SKINNING     = 0x1 << 1,
        FLAG_IS_SUPPORTING_HEMISPHERE_LIGHTING = 0x1 << 2,
        FLAG_IS_SUPPORTING_VERTEX_MORPH_SHADER = 0x1 << 3
    };
    
    NW_RES_CTOR(ResShaderProgramDescription)

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }


    u32 GetVertexShaderObject() const { return static_cast<u32>(ref().m_VertexShaderObject); }
    void SetVertexShaderObject(u32 value) { ref().m_VertexShaderObject = value; }


    u32 GetGeometryShaderObject() const { return static_cast<u32>(ref().m_GeometryShaderObject); }
    void SetGeometryShaderObject(u32 value) { ref().m_GeometryShaderObject = value; }


    s32 GetVertexShaderIndex() const { return static_cast<s32>(ref().m_VertexShaderIndex); }
    void SetVertexShaderIndex(s32 value) { ref().m_VertexShaderIndex = value; }


    s32 GetGeometryShaderIndex() const { return static_cast<s32>(ref().m_GeometryShaderIndex); }
    void SetGeometryShaderIndex(s32 value) { ref().m_GeometryShaderIndex = value; }


    s32 GetSymbolsCount() const
        { return ref().m_SymbolsTableCount; }
    ResShaderSymbol GetSymbols(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toSymbolsTable.to_table_ptr());
            return ResShaderSymbol(tbl_ptr[idx].to_ptr());
        }
    const ResShaderSymbol GetSymbols(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toSymbolsTable.to_table_ptr());
            return ResShaderSymbol(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResShaderSymbol, nw::ut::internal::ResArrayClassTraits> GetSymbols()
        {
            return nw::ut::internal::ResArray<ResShaderSymbol, nw::ut::internal::ResArrayClassTraits>(ref().toSymbolsTable.to_table_ptr(), GetSymbolsCount());
        }
    const nw::ut::internal::ResArray<const ResShaderSymbol, nw::ut::internal::ResArrayClassTraits> GetSymbols() const
        {
            return nw::ut::internal::ResArray<const ResShaderSymbol, nw::ut::internal::ResArrayClassTraits>(ref().toSymbolsTable.to_table_ptr(), GetSymbolsCount());
        }


    s32 GetAttributeSymbolsCount() const
        { return sizeof(ref().toAttributeSymbols) / sizeof(nw::ut::BinString); }

    const char* GetAttributeSymbols(int idx) const;

    nw::ut::internal::ResArray<
        const nw::ut::BinString,
        nw::ut::internal::ResArrayPrimitiveTraits
    > GetAttributeSymbols() const
    {
        return nw::ut::internal::ResArray<
            const nw::ut::BinString,
            nw::ut::internal::ResArrayPrimitiveTraits
        >(&(ref().toAttributeSymbols[0]), GetAttributeSymbolsCount());
    }


    s32 GetAttributeIndicesCount() const
        { return sizeof(ref().m_AttributeIndices) / sizeof(s8); }

    s8 GetAttributeIndices(int idx) const
        { return ref().m_AttributeIndices[idx]; }

    void SetAttributeIndices(int idx, s8 value)
        { ref().m_AttributeIndices[idx] = value; }

    nw::ut::internal::ResArray<
        s8,
        nw::ut::internal::ResArrayPrimitiveTraits
    > GetAttributeIndices()
    {
        return nw::ut::internal::ResArray<
            s8,
            nw::ut::internal::ResArrayPrimitiveTraits
        >(&(ref().m_AttributeIndices[0]), GetAttributeIndicesCount());
    }

    nw::ut::internal::ResArray<
        const s8,
        nw::ut::internal::ResArrayPrimitiveTraits
    > GetAttributeIndices() const
    {
        return nw::ut::internal::ResArray<
            const s8,
            nw::ut::internal::ResArrayPrimitiveTraits
        >(&(ref().m_AttributeIndices[0]), GetAttributeIndicesCount());
    }


    s32 GetMaxBoneCount() const { return static_cast<s32>(ref().m_MaxBoneCount); }
    void SetMaxBoneCount(s32 value) { ref().m_MaxBoneCount = value; }

    s32 GetMaxVertexLightCount() const { return static_cast<s32>(ref().m_MaxVertexLightCount); }
    void SetMaxVertexLightCount(s32 value) { ref().m_MaxVertexLightCount = value; }

    s32 GetVertexLightEndUniform() const { return static_cast<s32>(ref().m_VertexLightEndUniform); }
    void SetVertexLightEndUniform(s32 value) { ref().m_VertexLightEndUniform = value; }

    s32 GetGeometryShaderMode() const { return static_cast<s32>(ref().m_GeometryShaderMode); }
    void SetGeometryShaderMode(s32 value) { ref().m_GeometryShaderMode = value; }

    u32 GetProgramObject() const { return static_cast<u32>(ref().m_ProgramObject); }
    void SetProgramObject(u32 value) { ref().m_ProgramObject = value; }

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
    void*       GetUniformLocation() { return ref().m_UniformLocation; }
    const void* GetUniformLocation() const { return ref().m_UniformLocation; }
    void        SetUniformLocation(void* uniformLocation) { ref().m_UniformLocation = uniformLocation; }
#endif
    ResBinaryShaderData* GetOwnerShaderData() 
    {
        return static_cast<ResBinaryShaderData*>( ref().toOwnerShader.to_ptr() ); 
    }
    
    const ResBinaryShaderData* GetOwnerShaderData() const 
    {
        return static_cast<const ResBinaryShaderData*>( ref().toOwnerShader.to_ptr() ); 
    }

    const ShaderBinaryInfo* GetShaderBinaryInfo() const
{ 
        return static_cast<const ShaderBinaryInfo*>( this->GetOwnerShaderData()->m_ShaderBinaryInfo ); 
    }

    s32 GetVertexUniformIndex( const char* name, ShaderBinaryInfo::SymbolType* pSymbolType ) const;

    s32 GetGeometryUniformIndex( const char* name, ShaderBinaryInfo::SymbolType* pSymbolType ) const;

    Result Setup(nw::os::IAllocator* allocator);

    void Cleanup();

#if defined(NW_GFX_PROGRAM_OBJECT_ENABLED)
private:
    GLuint CreateProgramObject();
    void AttachProgram(GLuint programObject);
    void LinkProgram(GLuint programObject);
#endif
};

class ResShader : public ResSceneObject
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShader)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('SHDR')
    };
    enum
    {
        BINARY_REVISION = REVISION_RES_SHADER
    };
    
    NW_RES_CTOR_INHERIT( ResShader, ResSceneObject )

    u32 GetRevision() const { return this->GetHeader().revision; }
    
    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);
    Result Setup(nw::os::IAllocator* allocator);

    void Cleanup();

    ResBinaryShader        Dereference();

    const ResBinaryShader  Dereference() const;
};

class ResBinaryShader : public ResShader
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResBinaryShader)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('BSHD')
    };
    
    NW_RES_CTOR_INHERIT( ResBinaryShader, ResShader )


    s32 GetBinaryDataCount() const
        { return ref().m_BinaryDataTableCount; }
    u8 GetBinaryData(int idx) const
        {
            const u8* tbl_ptr = static_cast<const u8*>(ref().toBinaryDataTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetBinaryData(int idx, u8 value)
        {
            u8* tbl_ptr = GetBinaryData();
            tbl_ptr[idx] = value;
        }
    u8* GetBinaryData()
        { return static_cast<u8*>(ref().toBinaryDataTable.to_table_ptr()); }
    const u8* GetBinaryData() const
        { return static_cast<const u8*>(ref().toBinaryDataTable.to_table_ptr()); }


    s32 GetShaderKindsCount() const
        { return ref().m_ShaderKindsTableCount; }
    u32 GetShaderKinds(int idx) const
        {
            const u32* tbl_ptr = static_cast<const u32*>(ref().toShaderKindsTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetShaderKinds(int idx, u32 value)
        {
            u32* tbl_ptr = GetShaderKinds();
            tbl_ptr[idx] = value;
        }
    u32* GetShaderKinds()
        { return static_cast<u32*>(ref().toShaderKindsTable.to_table_ptr()); }
    const u32* GetShaderKinds() const
        { return static_cast<const u32*>(ref().toShaderKindsTable.to_table_ptr()); }

    s32 GetDescriptionsCount() const
        { return ref().m_DescriptionsTableCount; }
    ResShaderProgramDescription GetDescriptions(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toDescriptionsTable.to_table_ptr());
            return ResShaderProgramDescription(tbl_ptr[idx].to_ptr());
        }
    const ResShaderProgramDescription GetDescriptions(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toDescriptionsTable.to_table_ptr());
            return ResShaderProgramDescription(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResShaderProgramDescription, nw::ut::internal::ResArrayClassTraits> GetDescriptions()
        {
            return nw::ut::internal::ResArray<ResShaderProgramDescription, nw::ut::internal::ResArrayClassTraits>(ref().toDescriptionsTable.to_table_ptr(), GetDescriptionsCount());
        }
    const nw::ut::internal::ResArray<const ResShaderProgramDescription, nw::ut::internal::ResArrayClassTraits> GetDescriptions() const
        {
            return nw::ut::internal::ResArray<const ResShaderProgramDescription, nw::ut::internal::ResArrayClassTraits>(ref().toDescriptionsTable.to_table_ptr(), GetDescriptionsCount());
        }


    s32 GetShaderObjectsCount() const
        { return ref().m_ShaderObjectsTableCount; }
    u32 GetShaderObjects(int idx) const
        {
            const u32* tbl_ptr = static_cast<const u32*>(ref().toShaderObjectsTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetShaderObjects(int idx, u32 value)
        {
            u32* tbl_ptr = GetShaderObjects();
            tbl_ptr[idx] = value;
        }
    u32* GetShaderObjects()
        { return static_cast<u32*>(ref().toShaderObjectsTable.to_table_ptr()); }
    const u32* GetShaderObjects() const
        { return static_cast<const u32*>(ref().toShaderObjectsTable.to_table_ptr()); }

    const void* GetCommandCache() const { return ref().m_CommandCache; }

    int GetCommandCacheSize() const { return ref().m_CommandCacheSize; }

    ShaderBinaryInfo* GetShaderBinaryInfo() 
    { 
        return static_cast<ShaderBinaryInfo*>( ref().m_ShaderBinaryInfo ); 
    }

    const ShaderBinaryInfo* GetShaderBinaryInfo() const
    { 
        return static_cast<const ShaderBinaryInfo*>( ref().m_ShaderBinaryInfo ); 
    }
};

class ResReferenceShader : public ResShader
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceShader)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('SDRF')
    };
    
    NW_RES_CTOR_INHERIT( ResReferenceShader, ResShader )


    const char* GetPath() const { return ref().toPath.to_ptr(); }

    ResShader GetTargetShader() { return ResShader(ref().toTargetShader.to_ptr()); }
    const ResShader GetTargetShader() const { return ResShader(ref().toTargetShader.to_ptr()); }
};

}
}
}

