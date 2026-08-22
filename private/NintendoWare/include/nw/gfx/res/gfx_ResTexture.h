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

#ifndef NW_GFX_RESTEXTURE_H_
#define NW_GFX_RESTEXTURE_H_

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <nn/gx/CTR/gx_CTR.h>

#include <nw/ut/ut_Color.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
#endif
namespace nw
{
namespace os
{
class IAllocator;
} // namesapce os

namespace gfx {
namespace res {

class ResGraphicsFile;

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResTextureData : public ResSceneObjectData
{
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResPixelBasedTextureData : public ResTextureData
{
    nw::ut::ResS32 m_Height;            //
    nw::ut::ResS32 m_Width;             //
    nw::ut::ResU32 m_Format;            //
    nw::ut::ResU32 m_FormatType;        //
    nw::ut::ResS32 m_MipmapSize;        //
    nw::ut::ResU32 m_TextureObject;     //
    nw::ut::ResU32 m_LocationFlag;      //
    nw::ut::ResU32 m_FormatHW;          //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResPixelBasedImageData
{
    //
    enum
    {
        AREA_NO_MALLOC = 0,             //
        AREA_FCRAM = NN_GX_MEM_FCRAM,   //
        AREA_VRAMA = NN_GX_MEM_VRAMA,   //
        AREA_VRAMB = NN_GX_MEM_VRAMB    //
    };

    nw::ut::ResS32      m_Height;               //
    nw::ut::ResS32      m_Width;                //
    nw::ut::ResS32      m_ImageDataTableCount;  //
    nw::ut::Offset      toImageDataTable;       //
    nw::os::IAllocator* m_DynamicAllocator;     //
    u32                 m_BitsPerPixel;         //
    u32                 m_LocationAddress;      //
    u32                 m_MemoryArea;           //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResImageTextureData : public ResPixelBasedTextureData
{
    nw::ut::Offset toImage;
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResCubeTextureData : public ResPixelBasedTextureData
{
    //
    enum { MAX_CUBE_FACE = 6 };

    union
    {
        struct
        {
            nw::ut::Offset toPositiveXImage;    //
            nw::ut::Offset toNegativeXImage;    //
            nw::ut::Offset toPositiveYImage;    //
            nw::ut::Offset toNegativeYImage;    //
            nw::ut::Offset toPositiveZImage;    //
            nw::ut::Offset toNegativeZImage;    //
        };

        nw::ut::Offset toImagesTables[ MAX_CUBE_FACE ]; //
    };
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResShadowTextureData : public ResPixelBasedTextureData
{
    nw::ut::Offset toImage;                     //
    nw::ut::ResBool m_IsPerspectiveShadow;      //
    nw::ut::ResF32 m_ShadowZBias;               //
    nw::ut::ResF32 m_ShadowZScale;              //
};

//
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResReferenceTextureData : public ResTextureData
{
    nw::ut::BinString toPath;
    nw::ut::Offset toTargetTexture;
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResTexture : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXOB') };
    enum { BINARY_REVISION = REVISION_RES_TEXTURE };

    NW_RES_CTOR_INHERIT( ResTexture, ResSceneObject )

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
    NW_INLINE ResTexture        Dereference();
    NW_INLINE const ResTexture  Dereference() const;

    //---------------------------------------------------------------------------
    //
    //
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
    void    Cleanup();
};

//
typedef nw::ut::ResArrayPatricia<ResTexture>::type  ResTextureArray;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResPixelBasedTexture : public ResTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResPixelBasedTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXPB') };

    NW_RES_CTOR_INHERIT( ResPixelBasedTexture, ResTexture )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Height )        // GetHeight(), SetHeight()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Width )         // GetWidth(), SetWidth()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FormatType )    // GetFormatType(), SetFormatType()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, MipmapSize )    // GetMipmapSize(), SetMipmapSize()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, TextureObject ) // GetTextureObject(), SetTextureObject()

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
    NW_RES_FIELD_PRIMITIVE_DECL( u32, FormatHW )      // GetFormatHW(), SetFormatHW()

    enum FormatHW
    {
        FORMAT_HW_RGBA8    = 0,     //
        FORMAT_HW_RGB8     = 1,     //
        FORMAT_HW_RGBA5551 = 2,     //
        FORMAT_HW_RGB565   = 3,     //
        FORMAT_HW_RGBA4    = 4,     //
        FORMAT_HW_LA8      = 5,     //
        FORMAT_HW_HILO8    = 6,     //
        FORMAT_HW_L8       = 7,     //
        FORMAT_HW_A8       = 8,     //
        FORMAT_HW_LA4      = 9,     //
        FORMAT_HW_L4       = 10,    //
        FORMAT_HW_A4       = 11,    //
        FORMAT_HW_ETC1     = 12,    //
        FORMAT_HW_ETC1A4   = 13     //
    };
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResPixelBasedImage : public nw::ut::ResCommon< ResPixelBasedImageData >
{
public:
    NW_RES_CTOR( ResPixelBasedImage )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Height )        // GetHeight(), SetHeight()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( s32, Width )         // GetWidth(), SetWidth()

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
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( u8, ImageData ) // GetImageData(), GetImageData(int idx), GetImageDataCount()

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
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetLocationAddress(u32 address)
    {
        const int ALIGNMENT_TEXTURE = 128;
        NW_ALIGN_ASSERT(address, ALIGNMENT_TEXTURE);
        ref().m_LocationAddress = address;
    }
    void SetLocationAddress(const void* address)
    {
        const int ALIGNMENT_TEXTURE = 128;
        NW_ALIGN_ASSERT(address, ALIGNMENT_TEXTURE);
        ref().m_LocationAddress = reinterpret_cast<u32>(address);
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    u32 GetBitsPerPixel() const { return ref().m_BitsPerPixel; }

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
            return reinterpret_cast<u32>( this->GetImageData() );
        }
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool CheckMemoryLocation() const;
};

//
typedef nw::ut::ResArrayClass<ResPixelBasedImage>::type        ResPixelBasedImageArray;
//
typedef nw::ut::ResArrayClass<const ResPixelBasedImage>::type  ResPixelBasedImageArrayConst;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResImageTexture : public ResPixelBasedTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResImageTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXIM') };

    NW_RES_CTOR_INHERIT( ResImageTexture, ResPixelBasedTexture )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, Image ) // GetImage()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetLocationAddress() const { return this->GetImage().GetLocationAddress(); }

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
    //---------------------------------------------------------------------------
    void SetLocationAddress(u32 address) { this->GetImage().SetLocationAddress( address ); }
    void SetLocationAddress(const void* address) { this->GetImage().SetLocationAddress( address ); }

    //
    struct Description
    {
        s32 height;                 //
        s32 width;                  //
        s32 mipmapSize;             //
        u32 locationFlag;           //
        u32 locationAddress;        //
        u32 formatHW;               //
        bool executingMemoryFill;   //
        bool isDynamicAllocation;   //

        //
        Description()
         : height(0),
           width(0),
           mipmapSize(1),
           locationFlag(NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP),
           locationAddress(0),
           formatHW(ResPixelBasedTexture::FORMAT_HW_RGBA8),
           executingMemoryFill(false),
           isDynamicAllocation(true)
        {}
    };

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
        DynamicBuilder& Height(s32 height)
        {
            m_Description.height = height;
            return *this;
        }

        //
        DynamicBuilder& Width(s32 width)
        {
            m_Description.width = width;
            return *this;
        }

        //
        //
        //
        DynamicBuilder& MipmapSize(s32 mipmapSize)
        {
            m_Description.mipmapSize = mipmapSize;
            return *this;
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
        //---------------------------------------------------------------------------
        DynamicBuilder& LocationFlag(u32 locationFlag)
        {
            m_Description.locationFlag = locationFlag;
            return *this;
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
        DynamicBuilder& LocationAddress(u32 locationAddress)
        {
            m_Description.locationAddress = locationAddress;
            return *this;
        }

        //
        DynamicBuilder& Format(ResPixelBasedTexture::FormatHW format)
        {
            m_Description.formatHW = static_cast<u32>(format);
            return *this;
        }

        //
        DynamicBuilder& ExecutingMemoryFill(bool executingMemoryFill)
        {
            m_Description.executingMemoryFill = executingMemoryFill;
            return *this;
        }

        //
        //
        //
        DynamicBuilder& DynamicAllocation(bool isDynamicAllocation)
        {
            m_Description.isDynamicAllocation = isDynamicAllocation;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        ResImageTexture Create(os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //
    //
    void DynamicDestroy();

    //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResCubeTexture : public ResPixelBasedTexture
{
public:

    //
    enum CubeFace
    {
        CUBE_FACE_POSITIVE_X = 0,   //
        CUBE_FACE_NEGATIVE_X,       //
        CUBE_FACE_POSITIVE_Y,       //
        CUBE_FACE_NEGATIVE_Y,       //
        CUBE_FACE_POSITIVE_Z,       //
        CUBE_FACE_NEGATIVE_Z,       //
        MAX_CUBE_FACE               //
    };

    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCubeTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXCB') };

    NW_RES_CTOR_INHERIT( ResCubeTexture, ResPixelBasedTexture )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, PositiveXImage ) // GetPositiveXImage()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, NegativeXImage ) // GetNegativeXImage()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, PositiveYImage ) // GetPositiveYImage()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, NegativeYImage ) // GetNegativeYImage()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, PositiveZImage ) // GetPositiveZImage()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, NegativeZImage ) // GetNegativeZImage()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    s32     GetCubeFaceCount() const { return MAX_CUBE_FACE; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    ResPixelBasedImage  GetImage( CubeFace face )
    {
        NW_ASSERT( 0 <= face && face < MAX_CUBE_FACE );

        return ResPixelBasedImage( ref().toImagesTables[ face ].to_ptr() );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const ResPixelBasedImage    GetImage( CubeFace face ) const
    {
        NW_ASSERT( 0 <= face && face < MAX_CUBE_FACE );

        return ResPixelBasedImage( ref().toImagesTables[ face ].to_ptr() );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetLocationAddress( CubeFace face ) const { return this->GetImage( face ).GetLocationAddress(); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetLocationAddress( CubeFace face, u32 address) { this->GetImage( face ).SetLocationAddress( address ); }
    void SetLocationAddress( CubeFace face, const void* address) { this->GetImage( face ).SetLocationAddress( address ); }

    //
    struct Description
    {
        s32 width;                          //
        s32 mipmapSize;                     //
        u32 locationFlag;                   //
        u32 locationAddress[MAX_CUBE_FACE]; //
        u32 formatHW;                       //
        bool executingMemoryFill;           //
        bool isDynamicAllocation;           //

        //
        Description()
        : width(0),
          mipmapSize(1),
          locationFlag(NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP),
          formatHW(ResPixelBasedTexture::FORMAT_HW_RGBA8),
          executingMemoryFill(false),
          isDynamicAllocation(true)
        {
            for (int i = 0 ; i < MAX_CUBE_FACE ; ++i)
            {
                locationAddress[i] = 0;
            }
        }
    };

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
        DynamicBuilder& Width(s32 width)
        {
            m_Description.width = width;
            return *this;
        }

        //
        //
        //
        DynamicBuilder& MipmapSize(s32 mipmapSize)
        {
            m_Description.mipmapSize = mipmapSize;
            return *this;
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
        //---------------------------------------------------------------------------
        DynamicBuilder& LocationFlag(u32 locationFlag)
        {
            m_Description.locationFlag = locationFlag;
            return *this;
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
        //---------------------------------------------------------------------------
        DynamicBuilder& LocationAddress(CubeFace face, u32 locationAddress)
        {
            NW_MINMAXLT_ASSERT(face, 0, MAX_CUBE_FACE);
            m_Description.locationAddress[face] = locationAddress;
            return *this;
        }

        //
        DynamicBuilder& Format(ResPixelBasedTexture::FormatHW format)
        {
            m_Description.formatHW = static_cast<u32>(format);
            return *this;
        }

        //
        DynamicBuilder& ExecutingMemoryFill(bool executingMemoryFill)
        {
            m_Description.executingMemoryFill = executingMemoryFill;
            return *this;
        }

        //
        //
        //
        DynamicBuilder& DynamicAllocation(bool isDynamicAllocation)
        {
            m_Description.isDynamicAllocation = isDynamicAllocation;
            return *this;
        }

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
        ResCubeTexture Create(os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //
    //
    void DynamicDestroy();

    //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShadowTexture : public ResPixelBasedTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShadowTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXSH') };

    NW_RES_CTOR_INHERIT( ResShadowTexture, ResPixelBasedTexture )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResPixelBasedImage, Image ) // GetImage()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL( PerspectiveShadow )      // IsPerspectiveShadow(), SetPerspectiveShadow()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( f32, ShadowZBias )    // GetShadowZBias(), SetShadowZBias()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32 GetLocationAddress() const { return this->GetImage().GetLocationAddress(); }

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
    //---------------------------------------------------------------------------
    void SetLocationAddress(u32 address) { this->GetImage().SetLocationAddress( address ); }
    void SetLocationAddress(const void* address) { this->GetImage().SetLocationAddress( address ); }


    //
    struct Description
    {
        s32 height;                 //
        s32 width;                  //
        u32 locationFlag;           //
        u32 locationAddress;        //
        bool executingMemoryFill;   //
        bool isPerspectiveShadow;   //
        f32 shadowZBias;            //
        bool isDynamicAllocation;   //

        //
        Description()
         : height(0),
           width(0),
           locationFlag(NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP),
           locationAddress(0),
           executingMemoryFill(false),
           isPerspectiveShadow(true),
           shadowZBias(0.0f),
           isDynamicAllocation(false)
        {}
    };

    //----------------------------------------
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
        DynamicBuilder& Height(s32 height)
        {
            m_Description.height = height;
            return *this;
        }

        //
        DynamicBuilder& Width(s32 width)
        {
            m_Description.width = width;
            return *this;
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
        //---------------------------------------------------------------------------
        DynamicBuilder& LocationFlag(u32 locationFlag)
        {
            m_Description.locationFlag = locationFlag;
            return *this;
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
        DynamicBuilder& LocationAddress(u32 locationAddress)
        {
            m_Description.locationAddress = locationAddress;
            return *this;
        }

        //
        DynamicBuilder& ExecutingMemoryFill(bool executingMemoryFill)
        {
            m_Description.executingMemoryFill = executingMemoryFill;
            return *this;
        }

        //
        DynamicBuilder& PerspectiveShadow(bool isPerspectiveShadow)
        {
            m_Description.isPerspectiveShadow = isPerspectiveShadow;
            return *this;
        }

        //
        DynamicBuilder& ShadowZBias(f32 shadowZBias)
        {
            m_Description.shadowZBias = shadowZBias;
            return *this;
        }

        //
        //
        DynamicBuilder& DynamicAllocation(bool isDynamicAllocation)
        {
            m_Description.isDynamicAllocation = isDynamicAllocation;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        ResShadowTexture Create(os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //
    //
    void DynamicDestroy();

    //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResReferenceTexture : public ResTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXRF') };

    NW_RES_CTOR_INHERIT( ResReferenceTexture, ResTexture )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_STRING_DECL( Path )                     // GetPath()

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_CLASS_DECL( ResTexture, TargetTexture ) // GetTargetTexture()
};




//----------------------------------------
NW_INLINE ResTexture
ResTexture::Dereference()
{
    NW_ASSERT( this->IsValid() );

    if ( this->ref().typeInfo == ResReferenceTexture::TYPE_INFO )
    {
        return ResStaticCast<ResReferenceTexture>(*this).GetTargetTexture();
    }
    else
    {
        return *this;
    }
}

//----------------------------------------
NW_INLINE const ResTexture
ResTexture::Dereference() const
{
    NW_ASSERT( this->IsValid() );

    if ( this->ref().typeInfo == ResReferenceTexture::TYPE_INFO )
    {
        return ResStaticCast<ResReferenceTexture>(*this).GetTargetTexture();
    }
    else
    {
        return *this;
    }
}

} // namespace res
} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif
#endif

#endif // NW_GFX_RESTEXTURE_H_
