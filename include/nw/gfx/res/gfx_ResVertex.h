#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_ResDeclMacros.h>
#include <nw/ut/ut_ResArrayTypes.h>
#include <nw/ut/ut_ResPrimitive.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw  {
namespace gfx {
namespace res {

struct ResVertexAttributeData
{
    enum VertexType
    {
        FLAG_VERTEX_PARAM = 0x1,
        FLAG_INTERLEAVE = 0x2
    };
    
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResS32 m_Usage;
    nw::ut::ResU32 m_Flags;
};

class ResVertexStream;
class ResVertexParamAttribute;

class ResVertexAttribute : public nw::ut::ResCommon< ResVertexAttributeData >
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexAttribute) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('VATR') };
    
    enum VertexType
    {
        FLAG_VERTEX_PARAM = 0x1,
        FLAG_INTERLEAVE = 0x2
    };
    
    enum VertexAttributeUsage
    {
        USAGE_POSITION,
        USAGE_NORMAL,
        USAGE_TANGENT,
        USAGE_COLOR,
        USAGE_TEXTURECOODINATE0,
        USAGE_TEXTURECOODINATE1,
        USAGE_TEXTURECOODINATE2,
        USAGE_BONEINDEX,
        USAGE_BONEWEIGHT,
        USAGE_USERATTRIBUTE0,
        USAGE_USERATTRIBUTE1,
        USAGE_USERATTRIBUTE2,
        USAGE_USERATTRIBUTE3,
        USAGE_USERATTRIBUTE4,
        USAGE_USERATTRIBUTE5,
        USAGE_USERATTRIBUTE6,
        USAGE_USERATTRIBUTE7,
        USAGE_USERATTRIBUTE8,
        USAGE_USERATTRIBUTE9,
        USAGE_USERATTRIBUTE10,
        USAGE_USERATTRIBUTE11,
        USAGE_INTERLEAVE,
        USAGE_QUANTITY,
        USAGE_NONE
    };
    
    NW_RES_CTOR( ResVertexAttribute )


    s32 GetUsage() const { return static_cast<s32>(ref().m_Usage); }
    void SetUsage(s32 value) { ref().m_Usage = value; }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    void    Setup();

    void    Cleanup();

    u32     GetVertexCount();

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};
typedef nw::ut::ResArrayClass<ResVertexAttribute>::type  ResVertexAttributeArray;

struct ResVertexParamAttributeData : public ResVertexAttributeData
{
    nw::ut::ResU32 m_FormatType;
    nw::ut::ResU8 m_Dimension;
    u8 _padding_0[3];
    nw::ut::ResF32 m_Scale;
    nw::ut::Offset m_AttributeTableCount;
    nw::ut::Offset toAttributeTable;
};

class ResVertexParamAttribute : public ResVertexAttribute
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexParamAttribute) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('VAPM') };
    
    NW_RES_CTOR_INHERIT( ResVertexParamAttribute, ResVertexAttribute )


    u32 GetFormatType() const { return static_cast<u32>(ref().m_FormatType); }
    void SetFormatType(u32 value) { ref().m_FormatType = value; }

    u8 GetDimension() const { return static_cast<u8>(ref().m_Dimension); }
    void SetDimension(u8 value) { ref().m_Dimension = value; }

    f32 GetScale() const { return static_cast<f32>(ref().m_Scale); }
    void SetScale(f32 value) { ref().m_Scale = value; }

    s32 GetAttributeCount() const
        { return ref().m_AttributeTableCount; }
    f32 GetAttribute(int idx) const
        {
            const f32* tbl_ptr = static_cast<const f32*>(ref().toAttributeTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetAttribute(int idx, f32 value)
        {
            f32* tbl_ptr = GetAttribute();
            tbl_ptr[idx] = value;
        }
    f32* GetAttribute()
        { return static_cast<f32*>(ref().toAttributeTable.to_table_ptr()); }
    const f32* GetAttribute() const
        { return static_cast<const f32*>(ref().toAttributeTable.to_table_ptr()); }

    void    Setup();

    void    Cleanup();
};
typedef nw::ut::ResArrayClass<ResVertexParamAttribute>::type ResVertexParamAttributeArray;


struct ResVertexStreamBaseData : public ResVertexAttributeData
{
    enum
    {
        AREA_NO_MALLOC = 0,
        AREA_VRAMA = NN_GX_MEM_VRAMA,
        AREA_VRAMB = NN_GX_MEM_VRAMB
    };
    
    nw::ut::ResU32 m_BufferObject;
    nw::ut::ResU32 m_LocationFlag;
    nw::ut::ResS32 m_StreamTableCount;
    nw::ut::Offset toStreamTable;
    u32            m_LocationAddress;
    u32            m_MemoryArea;
};

struct ResVertexStreamData : public ResVertexStreamBaseData
{
    nw::ut::ResU32 m_FormatType;
    nw::ut::ResU8 m_Dimension;
    u8 _padding_0[3];
    nw::ut::ResF32 m_Scale;
    nw::ut::ResU32 m_Offset;
};

struct ResInterleavedVertexStreamData : public ResVertexStreamBaseData
{
    nw::ut::ResU32 m_Stride;
    nw::ut::ResS32 m_VertexStreamsTableCount;
    nw::ut::Offset toVertexStreamsTable;
};

class ResVertexStreamBase : public ResVertexAttribute
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexStreamBase) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('VSTB') };
    
    NW_RES_CTOR_INHERIT( ResVertexStreamBase, ResVertexAttribute )


    u32 GetBufferObject() const { return static_cast<u32>(ref().m_BufferObject); }
    void SetBufferObject(u32 value) { ref().m_BufferObject = value; }

    u32 GetLocationFlag() const { return static_cast<u32>(ref().m_LocationFlag); }
    void SetLocationFlag(u32 value) { ref().m_LocationFlag = value; }

    s32 GetStreamCount() const
        { return ref().m_StreamTableCount; }
    u8 GetStream(int idx) const
        {
            const u8* tbl_ptr = static_cast<const u8*>(ref().toStreamTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetStream(int idx, u8 value)
        {
            u8* tbl_ptr = GetStream();
            tbl_ptr[idx] = value;
        }
    u8* GetStream()
        { return static_cast<u8*>(ref().toStreamTable.to_table_ptr()); }
    const u8* GetStream() const
        { return static_cast<const u8*>(ref().toStreamTable.to_table_ptr()); }

    u32 GetLocationAddress() const { return ref().m_LocationAddress; }

    void SetLocationAddress(u32 address) { ref().m_LocationAddress = address; }
    void SetLocationAddress(const void* address) { ref().m_LocationAddress = reinterpret_cast<u32>(address); }

    u32 GetImageAddress() const
    {
        u32 locationAddress = this->GetLocationAddress();

        if (locationAddress)
        {
            return locationAddress;
        }
        else
        {
            return reinterpret_cast<u32>( this->GetStream() );
        }
    }
};

class ResVertexStream : public ResVertexStreamBase
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexStream) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('VSTM') };
    
    NW_RES_CTOR_INHERIT(ResVertexStream, ResVertexStreamBase)

    u32 GetFormatType() const { return static_cast<u32>(ref().m_FormatType); }
    void SetFormatType(u32 value) { ref().m_FormatType = value; }

    u8 GetDimension() const { return static_cast<u8>(ref().m_Dimension); }
    void SetDimension(u8 value) { ref().m_Dimension = value; }

    f32 GetScale() const { return static_cast<f32>(ref().m_Scale); }
    void SetScale(f32 value) { ref().m_Scale = value; }

    u32 GetOffset() const { return static_cast<u32>(ref().m_Offset); }
    void SetOffset(u32 value) { ref().m_Offset = value; }

    void    Setup();

    void    Cleanup();
};
typedef nw::ut::ResArrayClass<ResVertexStream>::type  ResVertexStreamArray;

class ResInterleavedVertexStream : public ResVertexStreamBase
{
public:
    NW_RES_CTOR_INHERIT(ResInterleavedVertexStream, ResVertexStreamBase)


    u32 GetStride() const { return static_cast<u32>(ref().m_Stride); }
    void SetStride(u32 value) { ref().m_Stride = value; }

    s32 GetVertexStreamsCount() const
        { return ref().m_VertexStreamsTableCount; }
    ResVertexStream GetVertexStreams(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toVertexStreamsTable.to_table_ptr());
            return ResVertexStream(tbl_ptr[idx].to_ptr());
        }
    const ResVertexStream GetVertexStreams(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toVertexStreamsTable.to_table_ptr());
            return ResVertexStream(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResVertexStream, nw::ut::internal::ResArrayClassTraits> GetVertexStreams()
        {
            return nw::ut::internal::ResArray<ResVertexStream, nw::ut::internal::ResArrayClassTraits>(ref().toVertexStreamsTable.to_table_ptr(), GetVertexStreamsCount());
        }
    const nw::ut::internal::ResArray<const ResVertexStream, nw::ut::internal::ResArrayClassTraits> GetVertexStreams() const
        {
            return nw::ut::internal::ResArray<const ResVertexStream, nw::ut::internal::ResArrayClassTraits>(ref().toVertexStreamsTable.to_table_ptr(), GetVertexStreamsCount());
        }

    void    Setup();

    void    Cleanup();
};

struct ResIndexStreamData
{
    enum
    {
        AREA_NO_MALLOC = 0,
        AREA_VRAMA = NN_GX_MEM_VRAMA,
        AREA_VRAMB = NN_GX_MEM_VRAMB
    };
    
    nw::ut::ResU32 m_FormatType;
    nw::ut::ResU8  m_PrimitiveMode;
    bool  m_IsVisible;
    u8 _padding_0[2];
    nw::ut::ResS32 m_StreamTableCount;
    nw::ut::Offset toStreamTable;
    nw::ut::ResU32 m_BufferObject;
    nw::ut::ResU32 m_LocationFlag;
    
    void* m_CommandCache;
    s32   m_CommandCacheSize;
    u32   m_LocationAddress;
    u32   m_MemoryArea;
    
    nw::ut::Offset toBoundingVolume;
};

class ResIndexStream : public nw::ut::ResCommon<ResIndexStreamData>
{
public:
    NW_RES_CTOR( ResIndexStream )


    u32 GetFormatType() const { return static_cast<u32>(ref().m_FormatType); }
    void SetFormatType(u32 value) { ref().m_FormatType = value; }

    u8 GetPrimitiveMode() const { return static_cast<u8>(ref().m_PrimitiveMode); }
    void SetPrimitiveMode(u8 value) { ref().m_PrimitiveMode = value; }

    s32 GetStreamCount() const
        { return ref().m_StreamTableCount; }
    u8 GetStream(int idx) const
        {
            const u8* tbl_ptr = static_cast<const u8*>(ref().toStreamTable.to_table_ptr());
            return tbl_ptr[idx];
        }
    void SetStream(int idx, u8 value)
        {
            u8* tbl_ptr = GetStream();
            tbl_ptr[idx] = value;
        }
    u8* GetStream()
        { return static_cast<u8*>(ref().toStreamTable.to_table_ptr()); }
    const u8* GetStream() const
        { return static_cast<const u8*>(ref().toStreamTable.to_table_ptr()); }

    u32 GetBufferObject() const { return static_cast<u32>(ref().m_BufferObject); }
    void SetBufferObject(u32 value) { ref().m_BufferObject = value; }

    u32 GetLocationFlag() const { return static_cast<u32>(ref().m_LocationFlag); }
    void SetLocationFlag(u32 value) { ref().m_LocationFlag = value; }


    nw::ut::ResBoundingVolume GetBoundingVolume() { return nw::ut::ResBoundingVolume(ref().toBoundingVolume.to_ptr()); }
    const nw::ut::ResBoundingVolume GetBoundingVolume() const { return nw::ut::ResBoundingVolume(ref().toBoundingVolume.to_ptr()); }

    u32 GetLocationAddress() const { return ref().m_LocationAddress; }

    void SetLocationAddress(u32 address) { ref().m_LocationAddress = address; }
    void SetLocationAddress(const void* address) { ref().m_LocationAddress = reinterpret_cast<u32>(address); }

    u32 GetImageAddress() const
    {
        u32 locationAddress = this->GetLocationAddress();

        if (locationAddress)
        {
            return locationAddress;
        }
        else
        {
            return reinterpret_cast<u32>(this->GetStream());
        }
    }


    bool IsVisible() const { return static_cast<bool>(ref().m_IsVisible); }
    void SetVisible(bool value) { ref().m_IsVisible = value; }

    u32     GetVertexCount();
};
typedef nw::ut::ResArrayClass<ResIndexStream>::type  ResIndexStreamArray;

}
}
}

