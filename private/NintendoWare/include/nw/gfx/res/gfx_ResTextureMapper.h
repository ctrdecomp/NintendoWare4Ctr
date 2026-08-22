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

#ifndef NW_GFX_RESTEXTUREMAPPER_H_
#define NW_GFX_RESTEXTUREMAPPER_H_

#include <nw/types.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/gfx_CommandUtil.h>
#include <nw/ut/ut_Float.h>

#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
#endif
namespace nw {
namespace gfx {
namespace res {

class ResGraphicsFile;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResTextureSamplerData
{
    nw::ut::ResTypeInfo typeInfo;   //
    nw::ut::Offset toOwner;         //
    nw::ut::ResS32 m_MinFilter;     //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResStandardTextureSamplerData : public ResTextureSamplerData
{
    nw::ut::ResFloatColor m_BorderColor;    //
    nw::ut::ResF32 m_LodBias;               //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResShadowTextureSamplerData : public ResTextureSamplerData
{
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResTextureMapperData
{
    nw::ut::ResTypeInfo typeInfo;           //
    nw::os::IAllocator* m_DynamicAllocator; //
    nw::ut::Offset toTexture;               //
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
struct ResPixelBasedTextureMapperData : public ResTextureMapperData
{
    nw::ut::Offset toSampler;           //

    //
    enum { ADDRESS_INDEX = 7 };

    nw::ut::ResU32 m_CommandCache[14];  //
    nw::ut::ResU32 m_CommandSizeToSend; //
};


//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
//
struct ResProceduralTextureMapperData : public ResTextureMapperData
{
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResTextureSampler : public nw::ut::ResCommon< ResTextureSamplerData >
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTextureSampler) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('BSTS') };

    //
    enum MinFilter
    {
        MINFILTER_NEAREST,                //
        MINFILTER_LINEAR,                 //
        MINFILTER_NEAREST_MIPMAP_NEAREST, //
        MINFILTER_NEAREST_MIPMAP_LINEAR,  //
        MINFILTER_LINEAR_MIPMAP_NEAREST,  //
        MINFILTER_LINEAR_MIPMAP_LINEAR    //
    };

    //
    enum MagFilter
    {
        MAGFILTER_NEAREST, //
        MAGFILTER_LINEAR   //
    };

    //
    enum SamplerType
    {
        SAMPLERTYPE_TEXTURE_2D,    //
        SAMPLERTYPE_CUBE_MAP,      //
        SAMPLERTYPE_SHADOW,        //
        SAMPLERTYPE_PROJECTION,    //
        SAMPLERTYPE_SHADOW_CUBE    //
    };

    NW_RES_CTOR( ResTextureSampler )

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    MinFilter GetMinFilter() const
    {
        return static_cast<MinFilter>(this->ref().m_MinFilter);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetMinFilter(MinFilter value)
    {
        this->ref().m_MinFilter = static_cast<MinFilter>(value);

        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum {
            CMD_SHIFT2 = 2,
            CMD_MASK2 = 0x1,
            CMD_NEAR = 0,
            CMD_LINEAR = 1,
            CMD_SHIFT24 = 24,
            CMD_MASK24 = 0x1,
            CMD_OTHER = 0,
            CMD_MIPMAP_LINER = 1,
            CMD_INDEX = 5
        };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        const u32 table2[] =
        {
            CMD_NEAR,
            CMD_LINEAR,
            CMD_NEAR,
            CMD_NEAR,
            CMD_LINEAR,
            CMD_LINEAR
        };
        u32 value2 = table2[value];
        internal::SetCmdValue( &command.second[CMD_INDEX], value2, CMD_MASK2, CMD_SHIFT2 );

        const u32 table24[] =
        {
            CMD_OTHER,
            CMD_OTHER,
            CMD_OTHER,
            CMD_MIPMAP_LINER,
            CMD_OTHER,
            CMD_MIPMAP_LINER
        };
        u32 value24 = table24[value];

        internal::SetCmdValue( &command.second[CMD_INDEX], value24, CMD_MASK24, CMD_SHIFT24 );

        // Change 0x84 [27:24]min lod|[19:16]mipmap size.
        SetTextureMipmapCommand();
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    MagFilter GetMagFilter() const
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 1, CMD_MASK = 0x1, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        return static_cast<MagFilter>(internal::GetCmdValue( command.second[CMD_INDEX], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetMagFilter(MagFilter value)
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 1, CMD_MASK = 0x1, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        internal::SetCmdValue( &command.second[CMD_INDEX], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetSamplerType(SamplerType value)
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 28, CMD_MASK = 0x7, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        internal::SetCmdValue( &command.second[CMD_INDEX], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    SamplerType GetSamplerType() const
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 28, CMD_MASK = 0x7, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        return static_cast<SamplerType>(internal::GetCmdValue( command.second[CMD_INDEX], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ResTextureMapperData* GetOwnerData()
    {
        return static_cast<ResTextureMapperData*>( ref().toOwner.to_ptr() );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ResTextureMapperData* GetOwnerData() const
    {
        return static_cast<const ResTextureMapperData*>( ref().toOwner.to_ptr() );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }

protected:
    //
    ::std::pair<u32, u32*> GetOwnerCommand() const;

    //
    void SetTextureMipmapCommand();
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResStandardTextureSampler : public ResTextureSampler
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResStandardTextureSampler) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('STTS') };

    NW_RES_CTOR_INHERIT( ResStandardTextureSampler, ResTextureSampler )

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    const ut::FloatColor& GetBorderColor() const { return ref().m_BorderColor; }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    void SetBorderColor(const ut::FloatColor& value) {
        ref().m_BorderColor = value;
        u32 borderColorU32 = ref().m_BorderColor.ToPicaU32();

        enum { CMD_INDEX = 2 };
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        command.second[CMD_INDEX] = borderColorU32;
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    void SetBorderColor( f32 r, f32 g, f32 b )
    {
        ref().m_BorderColor.Set(r, g, b);
        u32 borderColorU32 = ref().m_BorderColor.ToPicaU32();

        enum { CMD_INDEX = 2 };
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        command.second[CMD_INDEX] = borderColorU32;
    }


        //---------------------------------------------------------------------------
        //
        //
        //---------------------------------------------------------------------------
    void SetBorderColor( f32 r, f32 g, f32 b, f32 a )
    {
            ref().m_BorderColor.Set(r, g, b, a);
            u32 borderColorU32 = ref().m_BorderColor.ToPicaU32();

            enum { CMD_INDEX = 2 };
            ::std::pair<u32, u32*> command = GetOwnerCommand();

            NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
            NW_NULL_ASSERT(command.second);

            command.second[CMD_INDEX] = borderColorU32;
    }

    enum Wrap
    {
        WRAP_CLAMP_TO_EDGE   = 0,     //
        WRAP_CLAMP_TO_BORDER = 1,     //
        WRAP_REPEAT          = 2,     //
        WRAP_MIRRORED_REPEAT = 3      //
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Wrap GetWrapS() const
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 12, CMD_MASK = 0x7, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        return static_cast<Wrap>(internal::GetCmdValue( command.second[CMD_INDEX], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetWrapS(Wrap value)
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 12, CMD_MASK = 0x7, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        internal::SetCmdValue( &command.second[CMD_INDEX], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    Wrap GetWrapT() const
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 8, CMD_MASK = 0x7, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        return static_cast<Wrap>(internal::GetCmdValue( command.second[CMD_INDEX], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetWrapT(Wrap value)
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 8, CMD_MASK = 0x7, CMD_INDEX = 5 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        internal::SetCmdValue( &command.second[CMD_INDEX], value, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    u32 GetMinLod() const
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 24, CMD_MASK = 0xf, CMD_INDEX = 6 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        return static_cast<Wrap>(internal::GetCmdValue( command.second[CMD_INDEX], CMD_MASK, CMD_SHIFT ));
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    void SetMinLod(u32 value)
    {
        ::std::pair<u32, u32*> command = GetOwnerCommand();

        enum { CMD_SHIFT = 24, CMD_MASK = 0xf, CMD_INDEX = 6 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);
        NW_MINMAX_ASSERT(value, 0, 15);

        u32 minLod = value;
        if (this->ref().m_MinFilter == ResTextureSampler::MINFILTER_NEAREST ||
            this->ref().m_MinFilter == ResTextureSampler::MINFILTER_LINEAR)
        {
            minLod = 0;
        }

        internal::SetCmdValue( &command.second[CMD_INDEX], minLod, CMD_MASK, CMD_SHIFT );
    }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    f32 GetLodBias() const { return this->ref().m_LodBias; }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    void SetLodBias(f32 value)
    {
        this->ref().m_LodBias = value;
        ::std::pair<u32, u32*> command = GetOwnerCommand();
        enum { CMD_SHIFT = 0, CMD_MASK = 0x1fff, CMD_INDEX = 6 };

        NW_MIN_ASSERT(command.first, (CMD_INDEX + 1) * sizeof(u32));
        NW_NULL_ASSERT(command.second);

        u32 lodBias = ut::FixedS13Fraction8::Float32ToFixed13(value);
        internal::SetCmdValue( &command.second[CMD_INDEX], lodBias, CMD_MASK, CMD_SHIFT );
    }
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResShadowTextureSampler : public ResTextureSampler
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResShadowTextureSampler) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SHTS') };

    NW_RES_CTOR_INHERIT( ResShadowTextureSampler, ResTextureSampler )
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResTextureMapper : public nw::ut::ResCommon< ResTextureMapperData >
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTextureMapper) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('TMAP') };

    NW_RES_CTOR( ResTextureMapper )

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetTexture(ResTexture resTexture);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ResTexture GetTexture() const { return ResTexture( ref().toTexture.to_ptr() ); }

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

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    ResTextureMapper CloneDynamic(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    size_t GetMemorySizeForClone(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeForCloneInternal(&size);

        return size.GetSizeWithPadding(alignment);
    }

    //
    void GetMemorySizeForCloneInternal(os::MemorySizeCalculator* pSize) const;

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void DestroyDynamic();

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

//
typedef nw::ut::ResArrayClass<ResTextureMapper>::type  ResTextureMapperArray;

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResPixelBasedTextureMapper : public ResTextureMapper
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResPixelBasedTextureMapper) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PMAP') };

    NW_RES_CTOR_INHERIT( ResPixelBasedTextureMapper, ResTextureMapper )

    //
    //
    //
    struct Description
    {
        ut::FloatColor                  borderColor;    //
        ResStandardTextureSampler::Wrap wrapS;          //
        ResStandardTextureSampler::Wrap wrapT;          //
        ResTextureSampler::MagFilter    magFilter;      //
        ResTextureSampler::MinFilter    minFilter;      //
        ResTextureSampler::SamplerType  samplerType;    //
        u32                             minLod;         //
        f32                             lodBias;        //
        ResTexture                      targetTexture;  //

        //
        Description()
        : borderColor(0.0f, 0.0f, 0.0f, 0.0f),
          wrapS(ResStandardTextureSampler::WRAP_REPEAT),
          wrapT(ResStandardTextureSampler::WRAP_REPEAT),
          magFilter(ResTextureSampler::MAGFILTER_NEAREST),
          minFilter(ResTextureSampler::MINFILTER_NEAREST),
          samplerType(ResTextureSampler::SAMPLERTYPE_TEXTURE_2D),
          minLod(0),
          lodBias(0.0f),
          targetTexture(NULL)
        {
        }
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void DestroyDynamic();

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const ResTextureSampler  GetSampler() const { return ResTextureSampler( ref().toSampler.to_ptr() ); }
    ResTextureSampler  GetSampler() { return ResTextureSampler( ref().toSampler.to_ptr() ); }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32* GetCommandCache() { return &ref().m_CommandCache[0]; }
    const u32* GetCommandCache() const { return &ref().m_CommandCache[0]; }

    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    //---------------------------------------------------------------------------
    //
    //
    //---------------------------------------------------------------------------
    NW_RES_FIELD_PRIMITIVE_DECL( u32, CommandSizeToSend )       // GetCommandSizeToSend(), SetCommandSizeToSend()

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    bool IsCommandReady() const
    {
        return ref().m_CommandCache[ResPixelBasedTextureMapperData::ADDRESS_INDEX] != 0;
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void ResetCommand()
    {
        ref().m_CommandCache[ResPixelBasedTextureMapperData::ADDRESS_INDEX] = 0;
    }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void ForceSetupTexture(ResTexture texture);
};

//
typedef nw::ut::ResArrayClass<ResPixelBasedTextureMapper>::type        ResPixelBasedTextureMapperArray;
//
typedef nw::ut::ResArrayClass<const ResPixelBasedTextureMapper>::type  ResPixelBasedTextureMapperArrayConst;


//--------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class ResProceduralTextureMapper : public ResTextureMapper
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResProceduralTextureMapper) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('PRCM') };

    NW_RES_CTOR_INHERIT( ResProceduralTextureMapper, ResTextureMapper )
};

} // namespace res
} // namespace gfx
} // namespace nw
#ifdef NW_PLATFORM_CTR
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif
#endif

#endif // NW_GFX_RESTEXTUREMAPPER_H_
