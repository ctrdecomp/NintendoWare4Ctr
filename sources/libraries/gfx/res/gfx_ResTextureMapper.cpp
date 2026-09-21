// Filename: gfx_ResTextureMapper.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResTextureMapper.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/res/gfx_ResProceduralTexture.h>
#include <nw/gfx/gfx_CommandUtil.h>

namespace nw {
namespace gfx {
namespace res {

typedef Result (*SetupFunc)(ResTextureMapper resTextureMapper, os::IAllocator* allocator, ResGraphicsFile graphicsFile);
typedef void (*CleanupFunc)(ResTextureMapper resTextureMapper);
typedef ResTextureMapper (*CloneDynamicFunc)(ResTextureMapper resTextureMapper, os::IAllocator* allocator);
typedef void (*DestroyDynamicFunc)(ResTextureMapper resTextureMapper, os::IAllocator* allocator);

static Result ResPixelBasedTextureMapper_Setup(ResTextureMapper resTextureMapper, os::IAllocator* allocator, ResGraphicsFile graphicsFile);
static Result ResProceduralTextureMapper_Setup(ResTextureMapper resTextureMapper, os::IAllocator* allocator, ResGraphicsFile graphicsFile);
static void ResPixelBasedTextureMapper_Cleanup(ResTextureMapper resTextureMapper);
static void ResProceduralTextureMapper_Cleanup(ResTextureMapper resTextureMapper);
static ResTextureMapper ResPixelBasedTextureMapper_CloneDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator);
static ResTextureMapper ResProceduralTextureMapper_CloneDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator);
static void ResPixelBasedTextureMapper_DestroyDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator);
static void ResProceduralTextureMapper_DestroyDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator);

static ResTexture ReferResTexture(os::IAllocator* allocator, ResTexture resTexture);

static Result SetupTexture(
    os::IAllocator* allocator,
    ResTexture resTexture,
    ResGraphicsFile graphicsFile
);

static void SetupTextureMapperCommand(ResPixelBasedTextureMapper mapper, ResImageTexture texture);
static void SetupTextureMapperCommand(ResPixelBasedTextureMapper mapper, ResCubeTexture texture);
static void SetupTextureMapperCommand(ResPixelBasedTextureMapper mapper, ResShadowTexture texture);

static SetupFunc s_TextureMapperSetupTable[] = {
    ResPixelBasedTextureMapper_Setup,
    ResProceduralTextureMapper_Setup
};

static CleanupFunc s_TextureMapperCleanupTable[] = {
    ResPixelBasedTextureMapper_Cleanup,
    ResProceduralTextureMapper_Cleanup
};

static CloneDynamicFunc s_TextureMapperCloneDynamicTable[] = {
    ResPixelBasedTextureMapper_CloneDynamic,
    ResProceduralTextureMapper_CloneDynamic
};

static DestroyDynamicFunc s_TextureMapperDestroyDynamicTable[] = {
    ResPixelBasedTextureMapper_DestroyDynamic,
    ResProceduralTextureMapper_DestroyDynamic
};

/* ResTextureSampler */

::std::pair<u32, u32*> ResTextureSampler::GetOwnerCommand() const
{
    ResPixelBasedTextureMapper textureMapper =
        ResStaticCast<ResPixelBasedTextureMapper>(ResTextureMapper(this->GetOwnerData()));
    
    return ::std::make_pair(textureMapper.GetCommandSizeToSend(), textureMapper.GetCommandCache());
}

void ResTextureSampler::SetTextureMipmapCommand()
{
    ResPixelBasedTextureMapper textureMapper =
        ResStaticCast<ResPixelBasedTextureMapper>(ResTextureMapper(this->GetOwnerData()));

    ResTexture texture = textureMapper.GetTexture();
    
    MinFilter filter = this->GetMinFilter();

    int mipmapSize = 0;
    if (texture.IsValid() &&
        filter != MINFILTER_LINEAR &&
        filter != MINFILTER_NEAREST)
        {
        ResPixelBasedTexture pixelBasedTexture =
            ResStaticCast<ResPixelBasedTexture>(texture.Dereference());
        mipmapSize = pixelBasedTexture.GetMipmapSize() - 1;
    }

    u32* command = textureMapper.GetCommandCache();

    enum
    {
        CMD_SHIFT = 16,
        CMD_MASK = 0xF,
        CMD_INDEX = 6
    };

    internal::SetCmdValue( &command[CMD_INDEX], mipmapSize, CMD_MASK, CMD_SHIFT );
}

/* Statics */
static Result ResPixelBasedTextureMapper_Setup(ResTextureMapper resTextureMapper, os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    ResPixelBasedTextureMapper resPixelBasedTextureMapper =
        ResStaticCast<ResPixelBasedTextureMapper>(resTextureMapper);
    ResTexture resTexture = resTextureMapper.GetTexture();
    
    Result result = SetupTexture(allocator, resTexture, graphicsFile);

    if (result.IsFailure())
    {
        return result;
    }
    
    ResTexture resImageTexture = resTexture.Dereference();
    
    switch (resImageTexture.ref().typeInfo)
    {
    case ResImageTexture::TYPE_INFO:
        SetupTextureMapperCommand(resPixelBasedTextureMapper, ResStaticCast<ResImageTexture>(resImageTexture));
        break;
    case ResCubeTexture::TYPE_INFO:
        SetupTextureMapperCommand(resPixelBasedTextureMapper, ResStaticCast<ResCubeTexture>(resImageTexture));
        break;
    case ResShadowTexture::TYPE_INFO:
        SetupTextureMapperCommand(resPixelBasedTextureMapper, ResStaticCast<ResShadowTexture>(resImageTexture));
        break;
    default:
        NW_FATAL_ERROR("Unsupported texture type.\n");
    }

    return result;
}


static Result ResProceduralTextureMapper_Setup(ResTextureMapper resTextureMapper, os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    return SetupTexture(allocator, resTextureMapper.GetTexture(), graphicsFile);
}

static void ResPixelBasedTextureMapper_Cleanup(ResTextureMapper resTextureMapper)
{
    ResPixelBasedTextureMapper resPixelBasedTextureMapper = ResStaticCast<ResPixelBasedTextureMapper>(resTextureMapper);

    resPixelBasedTextureMapper.ResetCommand();
    
    ResTexture resTexture = resTextureMapper.GetTexture();
    ut::SafeCleanup(resTexture);
}

static void ResProceduralTextureMapper_Cleanup(ResTextureMapper resTextureMapper)
{
    ResTexture resTexture = resTextureMapper.GetTexture();
    ut::SafeCleanup(resTexture);
}

static ResTextureMapper ResPixelBasedTextureMapper_CloneDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator)
{

    ResPixelBasedTextureMapper resPixelBasedTextureMapper = ResStaticCast<ResPixelBasedTextureMapper>(resTextureMapper);

    void* mapperMemory = allocator->Alloc(sizeof(ResPixelBasedTextureMapperData));
    if (mapperMemory == NULL)
    {
        return ResTextureMapper(NULL);
    }

    ResPixelBasedTextureMapperData* textureMapper =
        new(mapperMemory) ResPixelBasedTextureMapperData(resPixelBasedTextureMapper.ref());

    ResPixelBasedTextureMapper cloneTextureMapper = ResPixelBasedTextureMapper(textureMapper);

    ResTexture resTexture = resPixelBasedTextureMapper.GetTexture();
    if (resTexture.IsValid())
    {

        ResTexture referenceTexture = ReferResTexture(allocator, resTexture);
        if (referenceTexture.IsValid())
        {
            textureMapper->toTexture.set_ptr(referenceTexture.ptr());
        }
        else{
            ResPixelBasedTextureMapper_DestroyDynamic(cloneTextureMapper, allocator);
            return ResTextureMapper(NULL);
        }
    }

    ResStandardTextureSampler resStandardTextureSampler =
        ResDynamicCast<ResStandardTextureSampler>(resPixelBasedTextureMapper.GetSampler());
    
    if (resStandardTextureSampler.IsValid())
    {

        void* samplerMemory = allocator->Alloc(sizeof(ResStandardTextureSamplerData));
        if (samplerMemory == NULL)
        {
            ResPixelBasedTextureMapper_DestroyDynamic(cloneTextureMapper, allocator);
            return ResTextureMapper(NULL);
        }

        ResStandardTextureSamplerData* textureSampler =
            new(samplerMemory) ResStandardTextureSamplerData(resStandardTextureSampler.ref());
        
        textureMapper->toSampler.set_ptr(textureSampler);
        textureSampler->toOwner.set_ptr(textureMapper);
    }

    return cloneTextureMapper;
}

static ResTextureMapper ResProceduralTextureMapper_CloneDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator)
{
    ResProceduralTextureMapper resProceduralTextureMapper = ResStaticCast<ResProceduralTextureMapper>(resTextureMapper);

    void* mapperMemory = allocator->Alloc(sizeof(ResProceduralTextureMapperData));
    if (mapperMemory == NULL)
    {
        return ResTextureMapper(NULL);
    }

    ResProceduralTextureMapperData* textureMapper =
        new(mapperMemory) ResProceduralTextureMapperData(resProceduralTextureMapper.ref());

    ResProceduralTextureMapper cloneTextureMapper = ResProceduralTextureMapper(textureMapper);
    
    ResTexture resTexture = resProceduralTextureMapper.GetTexture();
    if (resTexture.IsValid())
    {

        ResTexture referenceTexture = ReferResTexture(allocator, resTexture);
        if (referenceTexture.IsValid())
        {
            textureMapper->toTexture.set_ptr(referenceTexture.ptr());
        }
        else{
            ResProceduralTextureMapper_DestroyDynamic(cloneTextureMapper, allocator);
            return ResTextureMapper(NULL);
        }
    }

    return cloneTextureMapper;
}

static void ResPixelBasedTextureMapper_DestroyDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator)
{
    ResPixelBasedTextureMapper resPixelBasedTextureMapper =
        ResStaticCast<ResPixelBasedTextureMapper>(resTextureMapper);

    if (resPixelBasedTextureMapper.GetSampler().IsValid())
    {
        allocator->Free(resPixelBasedTextureMapper.ref().toSampler.to_ptr());
    }

    if (resPixelBasedTextureMapper.GetTexture().IsValid())
    {
        allocator->Free(resPixelBasedTextureMapper.ref().toTexture.to_ptr());
    }

    allocator->Free(resPixelBasedTextureMapper.ptr());
}

static void ResProceduralTextureMapper_DestroyDynamic(ResTextureMapper resTextureMapper, os::IAllocator* allocator)
{
    ResProceduralTextureMapper resProceduralTextureMapper =
        ResStaticCast<ResProceduralTextureMapper>(resTextureMapper);

    if (resProceduralTextureMapper.GetTexture().IsValid())
    {
        allocator->Free(resProceduralTextureMapper.ref().toTexture.to_ptr());
    }
    
    allocator->Free(resProceduralTextureMapper.ptr());
}

/* ResTextureMapper */

ResTextureMapper ResTextureMapper::CloneDynamic(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    ResTextureMapper resTextureMapper;
    switch ( this->ref().typeInfo )
    {
    case ResPixelBasedTextureMapper::TYPE_INFO:{
            resTextureMapper = s_TextureMapperCloneDynamicTable[0]( *this, allocator );
        }
        break;
    case ResProceduralTextureMapper::TYPE_INFO:{
            resTextureMapper = s_TextureMapperCloneDynamicTable[1]( *this, allocator );
        }
        break;
    default:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
    }

    if (resTextureMapper.IsValid())
    {
        resTextureMapper.ref().m_DynamicAllocator = allocator;
    }

    return resTextureMapper;
}

void ResTextureMapper::GetMemorySizeForCloneInternal(os::MemorySizeCalculator* pSize) const
{
    os::MemorySizeCalculator& size = *pSize;

    switch (this->ref().typeInfo)
    {
    case res::ResPixelBasedTextureMapper::TYPE_INFO:{

            NW_ASSERT(ResStaticCast<ResPixelBasedTextureMapper>(*this).GetTexture().IsValid());
            NW_ASSERT(ResDynamicCast<ResStandardTextureSampler>(ResStaticCast<ResPixelBasedTextureMapper>(*this).GetSampler()).IsValid());

            size += sizeof(ResPixelBasedTextureMapperData);
            size += sizeof(ResReferenceTextureData);
            size += sizeof(ResStandardTextureSamplerData);

            return;
        }
    case res::ResProceduralTextureMapper::TYPE_INFO:
    default:
        NW_FATAL_ERROR("Unsupported texture mapper type.\n");
    }
}

void ResTextureMapper::DestroyDynamic()
{
    if (this->ref().m_DynamicAllocator == NULL) { return; }
    
    switch (this->ref().typeInfo)
    {
    case ResPixelBasedTextureMapper::TYPE_INFO:{
            s_TextureMapperDestroyDynamicTable[0](*this, this->ref().m_DynamicAllocator);
        }
        break;

    case ResProceduralTextureMapper::TYPE_INFO:{
            s_TextureMapperDestroyDynamicTable[1](*this, this->ref().m_DynamicAllocator);
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
    }
}


Result  ResTextureMapper::Setup(os::IAllocator* allocator,ResGraphicsFile graphicsFile)
{
    NW_NULL_ASSERT(allocator);
    Result result = RESOURCE_RESULT_OK;

    switch ( this->ref().typeInfo )
    {
    case ResPixelBasedTextureMapper::TYPE_INFO:{
            result |= s_TextureMapperSetupTable[0](*this, allocator, graphicsFile);
        }
        break;

    case ResProceduralTextureMapper::TYPE_INFO:{
            result |= s_TextureMapperSetupTable[1](*this, allocator, graphicsFile);
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
    }

    return result;
}

void ResTextureMapper::Cleanup()
{
    switch (this->ref().typeInfo)
    {
    case ResPixelBasedTextureMapper::TYPE_INFO:{
            s_TextureMapperCleanupTable[0](*this);
        }
        break;

    case ResProceduralTextureMapper::TYPE_INFO:{
            s_TextureMapperCleanupTable[1](*this);
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
    }
}

void ResTextureMapper::SetTexture(ResTexture resTexture)
{
    NW_ASSERT(resTexture.IsValid());

    ResReferenceTexture referenceTexture = ResStaticCast<ResReferenceTexture>(this->GetTexture());
    referenceTexture.ref().toTargetTexture.set_ptr(resTexture.Dereference().ptr());

    switch (this->ref().typeInfo)
    {
    case ResPixelBasedTextureMapper::TYPE_INFO:{

            ResPixelBasedTextureMapper resPixelBasedTextureMapper = ResStaticCast<ResPixelBasedTextureMapper>(*this);
            ResTexture resImageTexture = resTexture.Dereference();
            
            switch (resImageTexture.ref().typeInfo)
            {
            case ResImageTexture::TYPE_INFO:
                SetupTextureMapperCommand(resPixelBasedTextureMapper, ResStaticCast<ResImageTexture>(resImageTexture));
                break;
            case ResCubeTexture::TYPE_INFO:
                SetupTextureMapperCommand(resPixelBasedTextureMapper, ResStaticCast<ResCubeTexture>(resImageTexture));
                break;
            case ResShadowTexture::TYPE_INFO:
                SetupTextureMapperCommand(resPixelBasedTextureMapper, ResStaticCast<ResShadowTexture>(resImageTexture));
                break;
            default:
                NW_FATAL_ERROR("Unsupported texture type.\n");
            }
        }
        break;
    case ResProceduralTextureMapper::TYPE_INFO:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
    }
}

static ResTexture ReferResTexture(os::IAllocator* allocator,ResTexture resTexture)
{

    void* textureMemory = allocator->Alloc(sizeof(ResReferenceTextureData));
    if (textureMemory == NULL)
    {
        return ResTexture(NULL);
    }

    ResReferenceTextureData* texture = new(textureMemory) ResReferenceTextureData();
    texture->typeInfo = ResReferenceTexture::TYPE_INFO;
    texture->toName.set_ptr(NULL);
    texture->toPath.set_ptr(NULL);
    texture->toTargetTexture.set_ptr(NULL);

    switch ( resTexture.ref().typeInfo )
    {
    case ResImageTexture::TYPE_INFO:{
            texture->toTargetTexture.set_ptr(resTexture.ptr());
        }
        break;

    case ResCubeTexture::TYPE_INFO:{
            texture->toTargetTexture.set_ptr(resTexture.ptr());
        }
        break;

    case ResReferenceTexture::TYPE_INFO:{
            ResReferenceTexture referenceTexture = ResDynamicCast<ResReferenceTexture>(resTexture);
            texture->toTargetTexture.set_ptr(referenceTexture.GetTargetTexture().ptr());
        }
        break;

    case ResProceduralTexture::TYPE_INFO:{
            texture->toTargetTexture.set_ptr(resTexture.ptr());
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture mapper type.\n");
        }
    }

    return ResTexture(texture);
}

static Result SetupTexture(os::IAllocator* allocator,ResTexture resTexture,ResGraphicsFile graphicsFile)
{
    Result result = RESOURCE_RESULT_OK;
    ResTexture setupTexture;

    bool existTexture = false;
    if (resTexture.IsValid())
    {

        ResReferenceTexture refer = ResDynamicCast<ResReferenceTexture>(resTexture);
        if (refer.IsValid())
        {
            if (refer.GetTargetTexture().IsValid())
            {
                setupTexture = refer.GetTargetTexture();
                existTexture = true;
            }
            else{
                ::std::pair<ResTexture, bool> referenceResult;
                referenceResult = GetReferenceTextureTarget(refer, graphicsFile);
                if (referenceResult.second)
                {
                    setupTexture = referenceResult.first;
                    existTexture = true;
                }
                else{
                    result |= Result::MASK_FAIL_BIT;
                    result |= RESOURCE_RESULT_NOT_FOUND_TEXTURE;
                }
            }
        }
        else{
            setupTexture = resTexture;
            existTexture = true;
        }
    }

    if (existTexture)
    {
        result |= setupTexture.Setup(allocator, graphicsFile);
    }

    return result;
}

enum
{
    HEIGHT_SHIFT = 0,
    WIDTH_SHIFT = 16,
    HEIGHT_MASK = 0x7FF << HEIGHT_SHIFT,
    WIDTH_MASK = 0x7FF << WIDTH_SHIFT,
    ADDRESS_MASK = 0x0FFFFFFF,
    CUBE_ADDRESS_MASK = 0x003FFFFF,
    FORMAT_SHIFT = 0,
    FORMAT_MASK = 0xF,
    FORMAT_ETC_SHIFT = 4,
    FORMAT_ETC_MASK = 0x3 << FORMAT_ETC_SHIFT,
    MIPMAP_SIZE_SHIFT = 16,
    MIPMAP_SIZE_MASK = 0xF << MIPMAP_SIZE_SHIFT,
    SAMPLER_TYPE_SHADOW_2D = 2,
    SAMPLER_TYPE_SHIFT = 28,
    SAMPLER_TYPE_MASK = 0x7 << SAMPLER_TYPE_SHIFT
};

static void SetupTextureMapperCommand(ResPixelBasedTextureMapper mapper, ResImageTexture texture)
{
    u32* command = mapper.GetCommandCache();
    mapper.SetCommandSizeToSend(8 * sizeof(u32));

    command[0] &= ~FORMAT_MASK;
    command[3] &= ~0x0FF00000;
    command[4] &= ~(HEIGHT_MASK | WIDTH_MASK);
    command[5] &= ~(FORMAT_ETC_MASK);
    command[6] &= ~(MIPMAP_SIZE_MASK);
    command[7] &= ~ADDRESS_MASK;
    
    uint texId = texture.GetTextureObject();
    ResPixelBasedImage resImage = texture.GetImage();
    u32 addr = resImage.GetImageAddress();
    
    uint format = texture.GetFormatHW();
    
    command[0] |= format & FORMAT_MASK;

    command[3] |= 4 << 20;
    command[1] |= 0x000F0000;
        
    command[4] |= (texture.GetHeight() & 0x7FF) << 0;
    command[4] |= (texture.GetWidth() & 0x7FF) << WIDTH_SHIFT;

    if (format == ResImageTexture::FORMAT_HW_ETC1)
    {
        command[5] |= 2 << FORMAT_ETC_SHIFT;
    }

    ResTextureSampler sampler = mapper.GetSampler();
    NW_ASSERT(sampler.IsValid());
    if ((sampler.GetMinFilter() != ResTextureSampler::MINFILTER_NEAREST) &&
        (sampler.GetMinFilter() != ResTextureSampler::MINFILTER_LINEAR))
        {
        command[6] |= ((texture.GetMipmapSize() - 1) & 0xF) << MIPMAP_SIZE_SHIFT;
    }
    
    command[7] |= (nngxGetPhysicalAddr( addr ) >> 3) & ADDRESS_MASK;
}

static bool VerifyCubeTextureAddress_( ResCubeTexture texture )
{
    u32 base_addr = 0;
    uint texId = texture.GetTextureObject();
    
    for (int face = 0; face < ResCubeTexture::MAX_CUBE_FACE; ++face)
    {
        ResPixelBasedImage resImage = texture.GetImage( static_cast<ResCubeTexture::CubeFace>( face ) );
        u32 addr = nngxGetPhysicalAddr( resImage.GetImageAddress() );

        if (face == ResCubeTexture::CUBE_FACE_POSITIVE_X)
        {
            base_addr = addr;
        }
        else{
            if (addr < base_addr)
            {
                return false;
            }
            
            if ((base_addr & 0xFE000000) != (addr & 0xFE000000))
            {
                return false;
            }
        }
    }
    
    return true;
}

static void SetupTextureMapperCommand(ResPixelBasedTextureMapper mapper, ResCubeTexture texture)
{
    u32* command = mapper.GetCommandCache();
    mapper.SetCommandSizeToSend(14 * sizeof(u32));
    
    uint texId = texture.GetTextureObject();
    
    uint format = texture.GetFormatHW();

    command[0] &= ~FORMAT_MASK;
    command[3] &= ~0x0FF00000;
    command[4] &= ~(HEIGHT_MASK | WIDTH_MASK);
    command[5] &= ~(FORMAT_ETC_MASK);
    command[6] &= ~(MIPMAP_SIZE_MASK);
    command[7] &= ~ADDRESS_MASK;
    command[8] &= ~CUBE_ADDRESS_MASK;
    command[9] &= ~CUBE_ADDRESS_MASK;
    command[10] &= ~CUBE_ADDRESS_MASK;
    command[11] &= ~CUBE_ADDRESS_MASK;
    command[12] &= ~CUBE_ADDRESS_MASK;
    
    command[0] |= format & 0xF;

    command[3] |= 9 << 20;
    
    command[4] |= (texture.GetHeight() & 0x7FF) << 0;
    command[4] |= (texture.GetWidth() & 0x7FF) << WIDTH_SHIFT;

    if (format == ResImageTexture::FORMAT_HW_ETC1)
    {
        command[5] |= 2 << FORMAT_ETC_SHIFT;
    }
    
    NW_ASSERT(VerifyCubeTextureAddress_(texture));

    ResTextureSampler sampler = mapper.GetSampler();
    NW_ASSERT(sampler.IsValid());
    if ((sampler.GetMinFilter() != ResTextureSampler::MINFILTER_NEAREST) &&
        (sampler.GetMinFilter() != ResTextureSampler::MINFILTER_LINEAR))
        {
        command[6] |= ((texture.GetMipmapSize() - 1) & 0xF) << MIPMAP_SIZE_SHIFT;
    }
    
    for (int face = 0; face < ResCubeTexture::MAX_CUBE_FACE; ++face)
    {
        ResPixelBasedImage resImage = texture.GetImage( static_cast<ResCubeTexture::CubeFace>( face ) );
        u32 addr = resImage.GetImageAddress();
        
        if (face == 0)
        {
            command[7] |= (nngxGetPhysicalAddr( addr ) >> 3) & ADDRESS_MASK;
        }
        else{
            command[7 + face] |= (nngxGetPhysicalAddr( addr ) >> 3) & CUBE_ADDRESS_MASK;
        }
    }
}

static void SetupTextureMapperCommand(ResPixelBasedTextureMapper mapper, ResShadowTexture texture)
{
    u32* command = mapper.GetCommandCache();
    mapper.SetCommandSizeToSend(14 * sizeof(u32));

    command[0] &= ~FORMAT_MASK;
    command[3] &= ~0x0FF00000;
    command[4] &= ~(HEIGHT_MASK | WIDTH_MASK);
    command[5] &= ~(FORMAT_ETC_MASK | SAMPLER_TYPE_MASK);
    command[6] &= ~(MIPMAP_SIZE_MASK);
    command[7] &= ~ADDRESS_MASK;
    command[13] = 0x0;
    
    uint texId = texture.GetTextureObject();
    ResPixelBasedImage resImage = texture.GetImage();
    u32 addr = resImage.GetImageAddress();
    
    uint format = texture.GetFormatHW();
    
    command[0] |= format & FORMAT_MASK;

    command[3] |= 10 << 20;
    command[1] |= 0x000F0000;
        
    command[4] |= (texture.GetHeight() & 0x7FF) << 0;
    command[4] |= (texture.GetWidth() & 0x7FF) << WIDTH_SHIFT;

    command[5] |= SAMPLER_TYPE_SHADOW_2D << SAMPLER_TYPE_SHIFT;

    ResTextureSampler sampler = mapper.GetSampler();
    NW_ASSERT(sampler.IsValid());
    if ((sampler.GetMinFilter() != ResTextureSampler::MINFILTER_NEAREST) &&
        (sampler.GetMinFilter() != ResTextureSampler::MINFILTER_LINEAR))
        {
        command[6] |= ((texture.GetMipmapSize() - 1) & 0xF) << MIPMAP_SIZE_SHIFT;
    }
    
    command[7] |= (nngxGetPhysicalAddr( addr ) >> 3) & ADDRESS_MASK;

    command[13] |= ((texture.IsPerspectiveShadow() ? 0 : 1) & 0x1) << 0;
    command[13] |= ((ut::FixedU24::Float32ToFixedU24(texture.GetShadowZBias()) >> 1) & 0x7FFFFF) << 1;

    u32 scaleU32 = ((ut::Float32::Float32ToBits32(texture.GetShadowZScale()) >> 23) & 0xFF);
    command[13] |= ((scaleU32 - 127) & 0xFF) << 24;
}

/* ResPixelBasedTextureMapper */
void ResPixelBasedTextureMapper::ForceSetupTexture(ResTexture texture)
{
    NW_ASSERT(texture.IsValid());
    ResReferenceTexture refer = ResStaticCast<ResReferenceTexture>(this->GetTexture());
    refer.ref().toTargetTexture.set_ptr(texture.ptr());

    ResTexture resImageTexture = refer.Dereference();
    
    switch ( resImageTexture.ref().typeInfo )
    {
    case ResImageTexture::TYPE_INFO:
        SetupTextureMapperCommand(*this, ResStaticCast<ResImageTexture>(resImageTexture));
        break;

    case ResCubeTexture::TYPE_INFO:
        SetupTextureMapperCommand(*this, ResStaticCast<ResCubeTexture>(resImageTexture));
        break;

    case ResShadowTexture::TYPE_INFO:
        SetupTextureMapperCommand(*this, ResStaticCast<ResShadowTexture>(resImageTexture));
        break;

    default:
        NW_FATAL_ERROR("Unsupported texture type.\n");
    }
}

}
}
}