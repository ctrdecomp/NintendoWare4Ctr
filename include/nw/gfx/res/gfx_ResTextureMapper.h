#pragma once

#include <nw/types.h>
#include <nw/gfx/res/gfx_ResTexture.h>

namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

struct ResTextureSamplerData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::Offset toOwner;
    nw::ut::ResS32 m_MinFilter;
};

struct ResStandardTextureSamplerData : public ResTextureSamplerData
{
    nw::ut::ResFloatColor m_BorderColor;
    nw::ut::ResF32 m_LodBias;
};

struct ResShadowTextureSamplerData : public ResTextureSamplerData
{};

struct ResTextureMapperData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::os::IAllocator* m_DynamicAllocator;
    nw::ut::Offset toTexture;
};

struct ResPixelBasedTextureMapperData : public ResTextureMapperData
{
    nw::ut::Offset toSampler;
    
    enum
{ ADDRESS_INDEX = 7 };
    
    nw::ut::ResU32 m_CommandCache[14];
    nw::ut::ResU32 m_CommandSizeToSend;
};

struct ResProceduralTextureMapperData : public ResTextureMapperData
{};

class ResTextureSampler : public nw::ut::ResCommon< ResTextureSamplerData >
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTextureSampler) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('BSTS') };

    enum MinFilter
    {
        MINFILTER_NEAREST,
        MINFILTER_LINEAR,
        MINFILTER_NEAREST_MIPMAP_NEAREST,
        MINFILTER_NEAREST_MIPMAP_LINEAR,
        MINFILTER_LINEAR_MIPMAP_NEAREST,
        MINFILTER_LINEAR_MIPMAP_LINEAR
    };

    enum MagFilter
    {
        MAGFILTER_NEAREST,
        MAGFILTER_LINEAR
    };

    enum SamplerType
    {
        SAMPLERTYPE_TEXTURE_2D,
        SAMPLERTYPE_CUBE_MAP,
        SAMPLERTYPE_SHADOW,
        SAMPLERTYPE_PROJECTION,
        SAMPLERTYPE_SHADOW_CUBE
    };
    
    NW_RES_CTOR( ResTextureSampler )
    
    MinFilter GetMinFilter() const {return static_cast<MinFilter>(this->ref().m_MinFilter);}

    ResTextureMapperData* GetOwnerData() {return static_cast<ResTextureMapperData*>( ref().toOwner.to_ptr() ); }
    const ResTextureMapperData* GetOwnerData() const {return static_cast<const ResTextureMapperData*>( ref().toOwner.to_ptr() ); }

    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }

protected:
    ::std::pair<u32, u32*> GetOwnerCommand() const;

    void SetTextureMipmapCommand();
};

class ResStandardTextureSampler : public ResTextureSampler
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResStandardTextureSampler) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('STTS') };

    NW_RES_CTOR_INHERIT( ResStandardTextureSampler, ResTextureSampler )

    const nw::ut::FloatColor& GetBorderColor() const { return ref().m_BorderColor; }

    void SetBorderColor(const nw::ut::FloatColor& value)
    {
        ref().m_BorderColor = value;
        u32 borderColorU32 = ref().m_BorderColor.ToPicaU32();

        enum { CMD_INDEX = 2 };
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        command.second[CMD_INDEX] = borderColorU32;
    }

    void SetBorderColor(f32 r, f32 g, f32 b)
    {
        ref().m_BorderColor.Set(r, g, b);
        u32 borderColorU32 = ref().m_BorderColor.ToPicaU32();

        enum { CMD_INDEX = 2 };
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        command.second[CMD_INDEX] = borderColorU32;
    }

    void SetBorderColor( f32 r, f32 g, f32 b, f32 a )
    {
        ref().m_BorderColor.Set(r, g, b, a);
        u32 borderColorU32 = ref().m_BorderColor.ToPicaU32();

        enum { CMD_INDEX = 2 };
        ::std::pair<u32, u32*> command = GetOwnerCommand();


        command.second[CMD_INDEX] = borderColorU32;
    }

    enum Wrap
    {
        WRAP_CLAMP_TO_EDGE   = 0,
        WRAP_CLAMP_TO_BORDER = 1,
        WRAP_REPEAT          = 2,
        WRAP_MIRRORED_REPEAT = 3
    };

    f32 GetLodBias() const { return this->ref().m_LodBias; }
};

class ResShadowTextureSampler : public ResTextureSampler
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShadowTextureSampler) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SHTS') };

    NW_RES_CTOR_INHERIT(ResShadowTextureSampler, ResTextureSampler)
};

class ResTextureMapper : public nw::ut::ResCommon< ResTextureMapperData >
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTextureMapper) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TMAP') };

    NW_RES_CTOR(ResTextureMapper)

    void SetTexture(ResTexture resTexture);
    const ResTexture GetTexture() const { return ResTexture( ref().toTexture.to_ptr() ); }

    Result Setup(nw::os::IAllocator* allocator, ResGraphicsFile graphicsFile);
    void Cleanup();

    ResTextureMapper CloneDynamic(nw::os::IAllocator* allocator);

    size_t GetMemorySizeForClone(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeForCloneInternal(&size);

        return size.GetSizeWithPadding(alignment);
    }

    void GetMemorySizeForCloneInternal(nw::os::MemorySizeCalculator* pSize) const;

    void DestroyDynamic();

    nw::ut::ResTypeInfo  GetTypeInfo() const { return ref().typeInfo; }
};

typedef nw::ut::ResArrayClass<ResTextureMapper>::type  ResTextureMapperArray;

class ResPixelBasedTextureMapper : public ResTextureMapper
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResPixelBasedTextureMapper)};
    enum { SIGNATURE = NW_RES_SIGNATURE32('PMAP') };
    
    NW_RES_CTOR_INHERIT(ResPixelBasedTextureMapper, ResTextureMapper)

    const ResTextureSampler  GetSampler() const { return ResTextureSampler( ref().toSampler.to_ptr() ); }
    ResTextureSampler  GetSampler() { return ResTextureSampler( ref().toSampler.to_ptr() ); }

    u32* GetCommandCache() { return &ref().m_CommandCache[0]; }
    const u32* GetCommandCache() const { return &ref().m_CommandCache[0]; }
    
    NW_RES_FIELD_PRIMITIVE_DECL(u32, CommandSizeToSend)
    
    bool IsCommandReady() const
    {
        return ref().m_CommandCache[ResPixelBasedTextureMapperData::ADDRESS_INDEX] != 0;
    }
    
    void ResetCommand()
    {
        ref().m_CommandCache[ResPixelBasedTextureMapperData::ADDRESS_INDEX] = 0;
    }

    void ForceSetupTexture(ResTexture texture);
};

typedef nw::ut::ResArrayClass<ResPixelBasedTextureMapper>::type        ResPixelBasedTextureMapperArray;
typedef nw::ut::ResArrayClass<const ResPixelBasedTextureMapper>::type  ResPixelBasedTextureMapperArrayConst;

class ResProceduralTextureMapper : public ResTextureMapper
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResProceduralTextureMapper) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PRCM') };

    NW_RES_CTOR_INHERIT( ResProceduralTextureMapper, ResTextureMapper )
};

}
}
}