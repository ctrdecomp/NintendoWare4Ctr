#pragma once

#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>
#include "nw/ut/ut_Color.h"
#include "nw/ut/ut_ResUtil.h"
#include "nw/ut/ut_ResDictionary.h"
#include "nw/gfx/res/gfx_ResSceneObject.h"
#include "nw/gfx/res/gfx_ResRevision.h"
#include "nw/gfx/res/gfx_ResTypeInfo.h"

namespace nw {
namespace os {
    class IAllocator;
}
namespace gfx {
namespace res {

class ResGraphicsFile;

struct ResTextureData : public ResSceneObjectData
{};

struct ResPixelBasedTextureData : public ResTextureData
{
    nw::ut::ResS32 m_Height;
    nw::ut::ResS32 m_Width;
    nw::ut::ResU32 m_Format;
    nw::ut::ResU32 m_FormatType;
    nw::ut::ResS32 m_MipmapSize;
    nw::ut::ResU32 m_TextureObject;
    nw::ut::ResU32 m_LocationFlag;
    nw::ut::ResU32 m_FormatHW;
};

struct ResPixelBasedImageData
{
    enum
    {
        AREA_NO_MALLOC = 0,
        AREA_VRAMA     = NN_GX_MEM_VRAMA,
        AREA_VRAMB     = NN_GX_MEM_VRAMB
    };

    nw::ut::ResS32 m_Height;
    nw::ut::ResS32 m_Width;
    nw::ut::ResS32 m_ImageDataTableCount;
    nw::ut::Offset toImageDataTable;
    nw::os::IAllocator*  m_DynamicAllocator;
    u32 m_BitsPerPixel;
    u32 m_LocationAddress;
    u32                  m_MemoryArea;
};

struct ResImageTextureData : public ResPixelBasedTextureData
{
    nw::ut::Offset toImage;
};

struct ResCubeTextureData : public ResPixelBasedTextureData
{
    enum { MAX_CUBE_FACE = 6 };

    nw::ut::Offset toPositiveXImage;
    nw::ut::Offset toNegativeXImage;
    nw::ut::Offset toPositiveYImage;
    nw::ut::Offset toNegativeYImage;
    nw::ut::Offset toPositiveZImage;
    nw::ut::Offset toNegativeZImage;
    nw::ut::Offset toImagesTables[MAX_CUBE_FACE];
};

struct ResShadowTextureData : public ResPixelBasedTextureData
{
    nw::ut::Offset  toImage;
    nw::ut::ResBool m_IsPerspectiveShadow;
    nw::ut::ResF32  m_ShadowZBias;
    nw::ut::ResF32  m_ShadowZScale;
};

struct ResReferenceTextureData : public ResTextureData
{
    nw::ut::BinString toPath;
    nw::ut::Offset    toTargetTexture;
};

class ResTexture : public ResSceneObject
{
public:
    enum { TYPE_INFO       = NW_GFX_RES_TYPE_INFO(ResTexture) };
    enum { SIGNATURE       = NW_RES_SIGNATURE32('TXOB') };
    enum { BINARY_REVISION = REVISION_RES_TEXTURE };

    NW_RES_CTOR_INHERIT(ResTexture, ResSceneObject)

    u32 GetRevision() const { return this->GetHeader().revision; }

    inline ResTexture       Dereference();
    inline const ResTexture Dereference() const;

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);
    Result Setup(nw::os::IAllocator* allocator);
    void   Cleanup();
};

typedef nw::ut::ResArrayPatricia<ResTexture>::type ResTextureArray;

class ResPixelBasedTexture : public ResTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResPixelBasedTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXPB') };

    NW_RES_CTOR_INHERIT(ResPixelBasedTexture, ResTexture)

    NW_RES_FIELD_PRIMITIVE_DECL(s32, Height)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Width)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, FormatType)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, MipmapSize)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, TextureObject)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, LocationFlag)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, FormatHW)

    enum FormatHW
    {
        FORMAT_HW_RGBA8    = 0,
        FORMAT_HW_RGB8     = 1,
        FORMAT_HW_RGBA5551 = 2,
        FORMAT_HW_RGB565   = 3,
        FORMAT_HW_RGBA4    = 4,
        FORMAT_HW_LA8      = 5,
        FORMAT_HW_HILO8    = 6,
        FORMAT_HW_L8       = 7,
        FORMAT_HW_A8       = 8,
        FORMAT_HW_LA4      = 9,
        FORMAT_HW_L4       = 10,
        FORMAT_HW_A4       = 11,
        FORMAT_HW_ETC1     = 12,
        FORMAT_HW_ETC1A4   = 13
    };
};

class ResPixelBasedImage : public nw::ut::ResCommon<ResPixelBasedImageData>
{
public:
    NW_RES_CTOR(ResPixelBasedImage)

    NW_RES_FIELD_PRIMITIVE_DECL(s32, Height)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Width)
    NW_RES_FIELD_PRIMITIVE_LIST_DECL(u8, ImageData)

    u32  GetLocationAddress() const { return ref().m_LocationAddress; }
    void SetLocationAddress(u32 address) { ref().m_LocationAddress = address; }
    void SetLocationAddress(const void* address) { ref().m_LocationAddress = reinterpret_cast<u32>(address); }
    u32  GetBitsPerPixel() const { return ref().m_BitsPerPixel; }

    u32 GetImageAddress() const
    {
        u32 locationAddress = this->GetLocationAddress();
        if (locationAddress)
            return locationAddress;
        else
            return reinterpret_cast<u32>(this->GetImageData());
    }

    bool CheckMemoryLocation() const;
};

typedef nw::ut::ResArrayClass<ResPixelBasedImage>::type       ResPixelBasedImageArray;
typedef nw::ut::ResArrayClass<const ResPixelBasedImage>::type ResPixelBasedImageArrayConst;

class ResImageTexture : public ResPixelBasedTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResImageTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXIM') };

    NW_RES_CTOR_INHERIT(ResImageTexture, ResPixelBasedTexture)

    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, Image)

    u32  GetLocationAddress() const { return this->GetImage().GetLocationAddress(); }
    void SetLocationAddress(u32 address) { this->GetImage().SetLocationAddress(address); }
    void SetLocationAddress(const void* address) { this->GetImage().SetLocationAddress(address); }

    struct Description
    {
        s32  height;
        s32  width;
        s32  mipmapSize;
        u32  locationFlag;
        u32  locationAddress;
        u32  formatHW;
        bool executingMemoryFill;
        bool isDynamicAllocation;

        Description():
            height(0),
            width(0),
            mipmapSize(1),
            locationFlag(NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP),
            locationAddress(0),
            formatHW(ResPixelBasedTexture::FORMAT_HW_RGBA8),
            executingMemoryFill(false),
            isDynamicAllocation(true) {}
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& Height(s32 height) { m_Description.height = height; return *this; }
        DynamicBuilder& Width(s32 width) { m_Description.width = width; return *this; }
        DynamicBuilder& MipmapSize(s32 mipmapSize) { m_Description.mipmapSize = mipmapSize; return *this; }
        DynamicBuilder& LocationFlag(u32 locationFlag) { m_Description.locationFlag = locationFlag; return *this; }
        DynamicBuilder& LocationAddress(u32 locationAddress) { m_Description.locationAddress = locationAddress; return *this; }
        DynamicBuilder& Format(ResPixelBasedTexture::FormatHW fmt) { m_Description.formatHW = static_cast<u32>(fmt); return *this; }
        DynamicBuilder& ExecutingMemoryFill(bool v) { m_Description.executingMemoryFill = v; return *this; }
        DynamicBuilder& DynamicAllocation(bool v) { m_Description.isDynamicAllocation = v; return *this; }

        ResImageTexture Create(nw::os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    void DynamicDestroy();
};

class ResCubeTexture : public ResPixelBasedTexture
{
public:
    enum CubeFace
    {
        CUBE_FACE_POSITIVE_X = 0,
        CUBE_FACE_NEGATIVE_X,
        CUBE_FACE_POSITIVE_Y,
        CUBE_FACE_NEGATIVE_Y,
        CUBE_FACE_POSITIVE_Z,
        CUBE_FACE_NEGATIVE_Z,
        MAX_CUBE_FACE
    };

    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCubeTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXCB') };

    NW_RES_CTOR_INHERIT(ResCubeTexture, ResPixelBasedTexture)

    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, PositiveXImage)
    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, NegativeXImage)
    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, PositiveYImage)
    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, NegativeYImage)
    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, PositiveZImage)
    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, NegativeZImage)

    s32 GetCubeFaceCount() const { return MAX_CUBE_FACE; }

    ResPixelBasedImage GetImage(CubeFace face)
    {
        NW_ASSERT(0 <= face && face < MAX_CUBE_FACE);
        return ResPixelBasedImage(ref().toImagesTables[face].to_ptr());
    }

    const ResPixelBasedImage GetImage(CubeFace face) const
    {
        NW_ASSERT(0 <= face && face < MAX_CUBE_FACE);
        return ResPixelBasedImage(ref().toImagesTables[face].to_ptr());
    }

    u32  GetLocationAddress(CubeFace face) const { return this->GetImage(face).GetLocationAddress(); }
    void SetLocationAddress(CubeFace face, u32 address) { this->GetImage(face).SetLocationAddress(address); }
    void SetLocationAddress(CubeFace face, const void* address) { this->GetImage(face).SetLocationAddress(address); }

    struct Description
    {
        s32  width;
        s32  mipmapSize;
        u32  locationFlag;
        u32  locationAddress[MAX_CUBE_FACE];
        u32  formatHW;
        bool executingMemoryFill;
        bool isDynamicAllocation;

        Description():
            width(0),
            mipmapSize(1),
            locationFlag(NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP),
            formatHW(ResPixelBasedTexture::FORMAT_HW_RGBA8),
            executingMemoryFill(false),
            isDynamicAllocation(true)
            {
            for (int i = 0; i < MAX_CUBE_FACE; ++i)
                locationAddress[i] = 0;
        }
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& Width(s32 width) { m_Description.width = width; return *this; }
        DynamicBuilder& MipmapSize(s32 mipmapSize) { m_Description.mipmapSize = mipmapSize; return *this; }
        DynamicBuilder& LocationFlag(u32 locationFlag) { m_Description.locationFlag = locationFlag; return *this; }
        DynamicBuilder& LocationAddress(CubeFace face, u32 locationAddress)
        {
            m_Description.locationAddress[face] = locationAddress;
            return *this;
        }
        DynamicBuilder& Format(ResPixelBasedTexture::FormatHW fmt) { m_Description.formatHW = static_cast<u32>(fmt); return *this; }
        DynamicBuilder& ExecutingMemoryFill(bool v) { m_Description.executingMemoryFill = v; return *this; }
        DynamicBuilder& DynamicAllocation(bool v) { m_Description.isDynamicAllocation = v; return *this; }

        ResCubeTexture Create(nw::os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    void DynamicDestroy();
};

class ResShadowTexture : public ResPixelBasedTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShadowTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXSH') };

    NW_RES_CTOR_INHERIT(ResShadowTexture, ResPixelBasedTexture)

    NW_RES_FIELD_CLASS_DECL(ResPixelBasedImage, Image)
    NW_RES_FIELD_BOOL_PRIMITIVE_DECL(PerspectiveShadow)
    NW_RES_FIELD_PRIMITIVE_DECL(f32, ShadowZBias)
    NW_RES_FIELD_PRIMITIVE_DECL(f32, ShadowZScale)

    u32  GetLocationAddress() const { return this->GetImage().GetLocationAddress(); }
    void SetLocationAddress(u32 address) { this->GetImage().SetLocationAddress(address); }
    void SetLocationAddress(const void* address) { this->GetImage().SetLocationAddress(address); }

    struct Description
    {
        s32  height;
        s32  width;
        u32  locationFlag;
        u32  locationAddress;
        bool executingMemoryFill;
        bool isPerspectiveShadow;
        f32  shadowZBias;
        f32  shadowZScale;
        bool isDynamicAllocation;

        Description():
            height(0),
            width(0),
            locationFlag(NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP),
            locationAddress(0),
            executingMemoryFill(false),
            isPerspectiveShadow(true),
            shadowZBias(0.0f),
            shadowZScale(1.0f),
            isDynamicAllocation(false) {}
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& Height(s32 height) { m_Description.height = height; return *this; }
        DynamicBuilder& Width(s32 width) { m_Description.width = width; return *this; }
        DynamicBuilder& LocationFlag(u32 locationFlag) { m_Description.locationFlag = locationFlag; return *this; }
        DynamicBuilder& LocationAddress(u32 locationAddress) { m_Description.locationAddress = locationAddress; return *this; }
        DynamicBuilder& ExecutingMemoryFill(bool v) { m_Description.executingMemoryFill = v; return *this; }
        DynamicBuilder& PerspectiveShadow(bool v) { m_Description.isPerspectiveShadow = v; return *this; }
        DynamicBuilder& ShadowZBias(f32 v) { m_Description.shadowZBias = v; return *this; }
        DynamicBuilder& ShadowZScale(f32 v) { m_Description.shadowZScale = v; return *this; }
        DynamicBuilder& DynamicAllocation(bool v) { m_Description.isDynamicAllocation = v; return *this; }

        ResShadowTexture Create(nw::os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    void DynamicDestroy();
};

class ResReferenceTexture : public ResTexture
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResReferenceTexture) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TXRF') };

    NW_RES_CTOR_INHERIT(ResReferenceTexture, ResTexture)

    NW_RES_FIELD_STRING_DECL(Path)
    NW_RES_FIELD_CLASS_DECL(ResTexture, TargetTexture)
};

inline ResTexture ResTexture::Dereference()
{
    NW_ASSERT(this->IsValid());
    if (this->ref().typeInfo == ResReferenceTexture::TYPE_INFO)
        return nw::ut::ResStaticCast<ResReferenceTexture>(*this).GetTargetTexture();
    else
        return *this;
}

inline const ResTexture ResTexture::Dereference() const
{
    NW_ASSERT(this->IsValid());
    if (this->ref().typeInfo == ResReferenceTexture::TYPE_INFO)
        return nw::ut::ResStaticCast<ResReferenceTexture>(*this).GetTargetTexture();
    else
        return *this;
}

}
}
}