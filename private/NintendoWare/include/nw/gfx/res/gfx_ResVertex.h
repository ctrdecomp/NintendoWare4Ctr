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

#ifndef NW_GFX_RESVERTEX_H_
#define NW_GFX_RESVERTEX_H_

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_ResDeclMacros.h>
#include <nw/ut/ut_ResArrayTypes.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/ut/ut_ResPrimitive.h>

namespace nw {
namespace gfx {
namespace res {

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResVertexAttributeData
{
    // TODO: These definitions will be deleted.
    //
    enum VertexType
    {
        FLAG_VERTEX_PARAM = 0x1,    //
        FLAG_INTERLEAVE = 0x2       //
    };

    nw::ut::ResTypeInfo typeInfo;   //
    nw::ut::ResS32 m_Usage;         //
    nw::ut::ResU32 m_Flags;         //
};

class ResVertexStream;
class ResVertexParamAttribute;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResVertexAttribute : public nw::ut::ResCommon< ResVertexAttributeData >
{
public:
    //
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexAttribute) };
    //
    enum { SIGNATURE = NW_RES_SIGNATURE32('VATR') };

    //
    enum VertexType
    {
        FLAG_VERTEX_PARAM = 0x1,    //
        FLAG_INTERLEAVE = 0x2       //
    };

    //
    enum VertexAttributeUsage
    {
        USAGE_POSITION,             //
        USAGE_NORMAL,               //
        USAGE_TANGENT,              //
        USAGE_COLOR,                //
        USAGE_TEXTURECOODINATE0,    //
        USAGE_TEXTURECOODINATE1,    //
        USAGE_TEXTURECOODINATE2,    //
        USAGE_BONEINDEX,            //
        USAGE_BONEWEIGHT,           //
        USAGE_USERATTRIBUTE0,       //
        USAGE_USERATTRIBUTE1,       //
        USAGE_USERATTRIBUTE2,       //
        USAGE_USERATTRIBUTE3,       //
        USAGE_USERATTRIBUTE4,       //
        USAGE_USERATTRIBUTE5,       //
        USAGE_USERATTRIBUTE6,       //
        USAGE_USERATTRIBUTE7,       //
        USAGE_USERATTRIBUTE8,       //
        USAGE_USERATTRIBUTE9,       //
        USAGE_USERATTRIBUTE10,      //
        USAGE_USERATTRIBUTE11,      //
        USAGE_INTERLEAVE,           //
        USAGE_QUANTITY,             //
        USAGE_NONE                  //
    };

    NW_RES_CTOR( ResVertexAttribute )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Usage ) // GetUsage(), SetUsage()

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
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )     // GetFlags(), SetFlags(), EnableFlags(), DisableFlags()

    //
    void    Setup();

    //
    void    Cleanup();

    //
    //
    //
    u32     GetVertexCount();

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

//
typedef nw::ut::ResArrayClass<ResVertexAttribute>::type  ResVertexAttributeArray;


//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResVertexParamAttributeData : public ResVertexAttributeData
{
    nw::ut::ResU32 m_FormatType;            //
    nw::ut::ResU8 m_Dimension;              //
    u8 _padding_0[3];                       //
    nw::ut::ResF32 m_Scale;                 //
    nw::ut::Offset m_AttributeTableCount;   //
    nw::ut::Offset toAttributeTable;        //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResVertexParamAttribute : public ResVertexAttribute
{
public:
    //
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexParamAttribute) };
    //
    enum { SIGNATURE = NW_RES_SIGNATURE32('VAPM') };

    NW_RES_CTOR_INHERIT( ResVertexParamAttribute, ResVertexAttribute )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FormatType )     // GetFormatType(), SetFormatType()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u8, Dimension )       // GetDimension(), SetDimension()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Scale )          // GetScale(), SetScale()

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

    NW_RES_FIELD_PRIMITIVE_LIST_DECL( f32, Attribute ) // GetAttribute(), GetAttribute(int idx), GetAttributeCount()

    //
    void    Setup();

    //
    void    Cleanup();
};

//
typedef nw::ut::ResArrayClass<ResVertexParamAttribute>::type ResVertexParamAttributeArray;


//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResVertexStreamBaseData : public ResVertexAttributeData
{
    //
    enum
    {
        AREA_NO_MALLOC = 0,             //
        AREA_FCRAM = NN_GX_MEM_FCRAM,   //
        AREA_VRAMA = NN_GX_MEM_VRAMA,   //
        AREA_VRAMB = NN_GX_MEM_VRAMB    //
    };

    nw::ut::ResU32 m_BufferObject;      //
    nw::ut::ResU32 m_LocationFlag;      //
    nw::ut::ResS32 m_StreamTableCount;  //
    nw::ut::Offset toStreamTable;       //
    u32            m_LocationAddress;   //
    u32            m_MemoryArea;        //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResVertexStreamData : public ResVertexStreamBaseData
{
    nw::ut::ResU32 m_FormatType;    //
    nw::ut::ResU8 m_Dimension;      //
    u8 _padding_0[3];               //
    nw::ut::ResF32 m_Scale;         //
    nw::ut::ResU32 m_Offset;        //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResInterleavedVertexStreamData : public ResVertexStreamBaseData
{
    nw::ut::ResU32 m_Stride;                    //
    nw::ut::ResS32 m_VertexStreamsTableCount;   //
    nw::ut::Offset toVertexStreamsTable;        //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResVertexStreamBase : public ResVertexAttribute
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexStreamBase) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('VSTB') };

    NW_RES_CTOR_INHERIT( ResVertexStreamBase, ResVertexAttribute )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, BufferObject ) // GetBufferObject(), SetBufferObject()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, LocationFlag )  // GetLocationFlag(), SetLocationFlag()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, Stream )   // GetStream(), GetStream(int idx), GetStreamCount()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetLocationAddress() const { return ref().m_LocationAddress; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetLocationAddress(u32 address) { ref().m_LocationAddress = address; }
    void SetLocationAddress(const void* address) { ref().m_LocationAddress = reinterpret_cast<u32>(address); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetImageAddress() const
    {
        u32 locationAddress = this->GetLocationAddress();

        if ( locationAddress )
        {
            return locationAddress;
        }
        else
        {
            return reinterpret_cast<u32>( this->GetStream() );
        }
    }
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResVertexStream : public ResVertexStreamBase
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResVertexStream) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('VSTM') };

    NW_RES_CTOR_INHERIT( ResVertexStream, ResVertexStreamBase )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FormatType )   // GetFormatType(), SetFormatType()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u8, Dimension )     // GetDimension(), SetDimension()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Scale )        // GetScale(), SetScale()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, Offset )

    //
    void    Setup();

    //
    void    Cleanup();
};

//
typedef nw::ut::ResArrayClass<ResVertexStream>::type  ResVertexStreamArray;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResInterleavedVertexStream : public ResVertexStreamBase
{
public:
    NW_RES_CTOR_INHERIT( ResInterleavedVertexStream, ResVertexStreamBase )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, Stride )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_LIST_DECL( ResVertexStream, VertexStreams )

    //
    void    Setup();

    //
    void    Cleanup();
};


//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResIndexStreamData
{
    //
    enum
    {
        AREA_NO_MALLOC = 0,             //
        AREA_FCRAM = NN_GX_MEM_FCRAM,   //
        AREA_VRAMA = NN_GX_MEM_VRAMA,   //
        AREA_VRAMB = NN_GX_MEM_VRAMB    //
    };

    nw::ut::ResU32 m_FormatType;        //
    nw::ut::ResU8  m_PrimitiveMode;     //
    bool  m_IsVisible;                  //
    u8 _padding_0[2];                   //
    nw::ut::ResS32 m_StreamTableCount;  //
    nw::ut::Offset toStreamTable;       //
    nw::ut::ResU32 m_BufferObject;      //
    nw::ut::ResU32 m_LocationFlag;      //

    void* m_CommandCache;               //
    s32   m_CommandCacheSize;           //
    u32   m_LocationAddress;            //
    u32   m_MemoryArea;                 //

    nw::ut::Offset toBoundingVolume;    //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResIndexStream : public nw::ut::ResCommon< ResIndexStreamData >
{
public:
    NW_RES_CTOR( ResIndexStream )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FormatType )   // GetFormatType(), SetFormatType()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u8, PrimitiveMode ) // GetPrimitiveMode(), SetPrimitiveMode()

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, Stream )   // GetStream(), GetStream(int idx), GetStreamCount()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, BufferObject ) // GetBufferObject(), SetBufferObject()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, LocationFlag )  // GetLocationFlag(), SetLocationFlag()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( nw::ut::ResBoundingVolume, BoundingVolume )  // GetBoundingVolume()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetLocationAddress() const { return ref().m_LocationAddress; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetLocationAddress(u32 address) { ref().m_LocationAddress = address; }
    void SetLocationAddress(const void* address) { ref().m_LocationAddress = reinterpret_cast<u32>(address); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetImageAddress() const
    {
        u32 locationAddress = this->GetLocationAddress();

        if ( locationAddress )
        {
            return locationAddress;
        }
        else
        {
            return reinterpret_cast<u32>( this->GetStream() );
        }
    }

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
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( Visible )      // IsVisible(), SetVisible()

    //
    u32     GetVertexCount();
};

//
typedef nw::ut::ResArrayClass<ResIndexStream>::type  ResIndexStreamArray;

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESVERTEX_H_
