// Filename: gfx_ResTexture.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/gfx_ShaderProgram.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/res/gfx_ResProceduralTexture.h>
#include <nw/os/os_Memory.h>

#include <nn/gx.h>

namespace nw  {
namespace gfx {
namespace res {

typedef void (*SetupFunc)(ResTexture resTex);

static void ResImageTexture_Setup(ResTexture resTex);
static void ResCubeTexture_Setup(ResTexture resTex);
static void ResReferenceTexture_Setup(ResTexture resTex);
static void ResProceduralTexture_Setup(ResTexture resTex);
static void ResShadowTexture_Setup(ResTexture resTex);

static void ResImageTexture_Cleanup(ResTexture resTex);
static void ResCubeTexture_Cleanup(ResTexture resTex);
static void ResReferenceTexture_Cleanup(ResTexture resTex);
static void ResProceduralTexture_Cleanup(ResTexture resTex);
static void ResShadowTexture_Cleanup(ResTexture resTex);

static SetupFunc s_TextureSetupTable[] = {
    ResImageTexture_Setup,
    ResCubeTexture_Setup,
    ResReferenceTexture_Setup,
    ResProceduralTexture_Setup,
    ResShadowTexture_Setup
};

static SetupFunc s_TextureCleanupTable[] = {
    ResImageTexture_Cleanup,
    ResCubeTexture_Cleanup,
    ResReferenceTexture_Cleanup,
    ResProceduralTexture_Cleanup,
    ResShadowTexture_Cleanup
};

namespace {
    static const uint GL_FORMAT_TABLE[] = {
        GL_RGBA_NATIVE_DMP,
        GL_RGB_NATIVE_DMP,
        GL_RGBA_NATIVE_DMP,
        GL_RGB_NATIVE_DMP,
        GL_RGBA_NATIVE_DMP,
        GL_LUMINANCE_ALPHA_NATIVE_DMP,
        GL_HILO8_DMP_NATIVE_DMP,
        GL_LUMINANCE_NATIVE_DMP,
        GL_ALPHA_NATIVE_DMP,
        GL_LUMINANCE_ALPHA_NATIVE_DMP,
        GL_LUMINANCE_NATIVE_DMP,
        GL_ALPHA_NATIVE_DMP,
        GL_ETC1_RGB8_NATIVE_DMP,
        GL_ETC1_ALPHA_RGB8_A4_NATIVE_DMP
    };

    static const uint GL_FORMAT_TYPE_TABLE[] = {
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_SHORT_5_5_5_1,
        GL_UNSIGNED_SHORT_5_6_5,
        GL_UNSIGNED_SHORT_4_4_4_4,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE,
        GL_UNSIGNED_BYTE_4_4_DMP,
        GL_UNSIGNED_4BITS_DMP,
        GL_UNSIGNED_4BITS_DMP,
        0,
        0
    };

    const f32 BYTESIZE_4 = 4.0f;
    const f32 BYTESIZE_2 = 2.0f;
    const f32 BYTESIZE_1 = 1.0f;
    const f32 BYTESIZE_HALF = 0.5f;
    static const f32 BYTESIZE_PER_PIXEL_TABLE[] = {
        BYTESIZE_4,
        BYTESIZE_4,
        BYTESIZE_2,
        BYTESIZE_2,
        BYTESIZE_2,
        BYTESIZE_2,
        BYTESIZE_2,
        BYTESIZE_1,
        BYTESIZE_1,
        BYTESIZE_1,
        BYTESIZE_1,
        BYTESIZE_1,
        BYTESIZE_HALF,
        BYTESIZE_1
    };
}

static void TexImage2D(u32 texID, ResPixelBasedImage resImage, u32 loadFlag)
{
    u32 size = resImage.GetImageDataCount();
    GLenum transtype = loadFlag & 0xFFFF0000;
    const u32 NN_GX_MEM_MASK = 0x00030000;
    void* address = NULL;
    
    if (size == 0)
    {
        size = resImage.GetWidth() * resImage.GetHeight() * resImage.GetBitsPerPixel() / 8;
    }
    
    switch (transtype)
    {
    case (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP):
        nngxUpdateBuffer(resImage.GetImageData(), size);
        break;
        
    case (NN_GX_MEM_VRAMA | GL_NO_COPY_FCRAM_DMP):
    case (NN_GX_MEM_VRAMB | GL_NO_COPY_FCRAM_DMP):
    {
            GLuint area = (transtype & NN_GX_MEM_MASK);
            address = __dmpgl_allocator(area, NN_GX_MEM_TEXTURE, texID, size);
            if (resImage.GetImageData())
            {
                nngxAddVramDmaCommand( resImage.GetImageData(), address, size );
            }
            
            resImage.SetLocationAddress( address );
            resImage.ref().m_MemoryArea = area;
        }
        break;
        
    case (NN_GX_MEM_FCRAM | GL_COPY_FCRAM_DMP):
    {
            address = __dmpgl_allocator(NN_GX_MEM_FCRAM, NN_GX_MEM_TEXTURE, texID, size);
            if (resImage.GetImageData())
            {
                nw::os::MemCpy( address, resImage.GetImageData(), size );
                nngxUpdateBuffer( address, size );
            }
            
            resImage.SetLocationAddress( address );
            resImage.ref().m_MemoryArea = NN_GX_MEM_FCRAM;
        }
        break;
        
    case (NN_GX_MEM_VRAMA | GL_COPY_FCRAM_DMP):
    case (NN_GX_MEM_VRAMB | GL_COPY_FCRAM_DMP):

        NW_FATAL_ERROR("(GL_COPY_FCRAM_DMP | NN_GX_MEM_VRAM) is not supported!");
        break;
    default:
        break;
    }
}

static void DeleteImage2D( u32 texID, ResPixelBasedImage resImage )
{
    GLuint area = resImage.ref().m_MemoryArea;
    
    if (area != ResPixelBasedImageData::AREA_NO_MALLOC)
    {
        void* address = reinterpret_cast<void*>( resImage.GetLocationAddress() );
        __dmpgl_deallocator( area, NN_GX_MEM_TEXTURE, texID, address );
        resImage.SetLocationAddress( static_cast<u32>(NULL) );
    }
}

static inline void SetTexImage2D( u32 texID, ResPixelBasedImage resImage, u32 loadFlag )
{
    NW_ASSERT(resImage.IsValid());

    if (resImage.GetLocationAddress() != NULL)
    {
        resImage.ref().m_MemoryArea = ResPixelBasedImageData::AREA_NO_MALLOC;
        return;
    }

    if (loadFlag == 0)
    {
        loadFlag = NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP;
    }

    NW_ASSERT((loadFlag != (NN_GX_MEM_FCRAM | GL_NO_COPY_FCRAM_DMP)) || resImage.CheckMemoryLocation());
     
    TexImage2D(texID, resImage, loadFlag);
}

static void ResImageTexture_Setup(ResTexture resTex)
{
    ResImageTexture resImgTex = ResDynamicCast<ResImageTexture>( resTex );
    
    NW_ASSERT( resImgTex.IsValid() );
    
    if (resImgTex.GetTextureObject() != 0)
    {
        return;
    }
    
    u32 texID = reinterpret_cast<u32>(resTex.ptr());
    resImgTex.SetTextureObject(texID);
    
    ResPixelBasedImage resImage = resImgTex.GetImage();
    
    u32 loadFlag = resImgTex.GetLocationFlag();
    
    SetTexImage2D(texID, resImage, loadFlag);
}

static void ResShadowTexture_Setup(ResTexture resTex)
{
    ResShadowTexture resSdwTex = ResDynamicCast<ResShadowTexture>( resTex );
    
    NW_ASSERT( resSdwTex.IsValid() );
    
    if (resSdwTex.GetTextureObject() != 0)
    {
        return;
    }
    
    u32 texID = reinterpret_cast<u32>(resTex.ptr());
    resSdwTex.SetTextureObject(texID);
    
    ResPixelBasedImage resImage = resSdwTex.GetImage();
    
    u32 loadFlag = resSdwTex.GetLocationFlag();
    
    SetTexImage2D(texID, resImage, loadFlag);
}

static void ResCubeTexture_Setup(ResTexture resTex)
{
    ResCubeTexture resCubeTex = ResDynamicCast<ResCubeTexture>( resTex );
    
    NW_ASSERT(resCubeTex.IsValid());
    
    if (resCubeTex.GetTextureObject() != 0)
    {
        return;
    }
    
    u32 texID = reinterpret_cast<u32>(resTex.ptr());
    resCubeTex.SetTextureObject(texID);
    
    u32 loadFlag = resCubeTex.GetLocationFlag();

    for (int face = 0; face < ResCubeTexture::MAX_CUBE_FACE; ++face)
    {
        ResPixelBasedImage resImage = resCubeTex.GetImage(static_cast<ResCubeTexture::CubeFace>(face));
        
        SetTexImage2D(texID, resImage, loadFlag);
    }
}

static void ResReferenceTexture_Setup(ResTexture resTex)
{
    ResReferenceTexture resRefTex = ResStaticCast<ResReferenceTexture>( resTex );
    
    NW_ASSERT(resRefTex.GetTargetTexture().IsValid());
    
    switch (resRefTex.GetTargetTexture().ref().typeInfo)
    {
    case ResImageTexture::TYPE_INFO:{
            ResImageTexture_Setup(resRefTex.GetTargetTexture());
            break;

        }
    case ResCubeTexture::TYPE_INFO:{
            ResCubeTexture_Setup(resRefTex.GetTargetTexture());
            break;

        }
    case ResReferenceTexture::TYPE_INFO:{
            ResReferenceTexture_Setup(resRefTex.GetTargetTexture());
            break;

        }
    case ResProceduralTexture::TYPE_INFO:{
            ResProceduralTexture_Setup(resRefTex.GetTargetTexture());
            break;

        }
    case ResShadowTexture::TYPE_INFO:{
            ResShadowTexture_Setup(resRefTex.GetTargetTexture());
            break;

        }
    default:{
            NW_FATAL_ERROR("Unsupported texture type.\n");
        }
    }
}

static void ResProceduralTexture_Setup(ResTexture resTex)
{
    NW_UNUSED_VARIABLE(resTex);
}

static void ResImageTexture_Cleanup(ResTexture resTex)
{
    ResImageTexture resImgTex = ResDynamicCast<ResImageTexture>(resTex);
    NW_ASSERT(resImgTex.IsValid());
    
    GLuint texID = resImgTex.GetTextureObject();
    if (texID == 0)
    {
        return;
    }
    
    ResPixelBasedImage resImage = resImgTex.GetImage();
    DeleteImage2D(texID, resImage);
    resImgTex.SetTextureObject(0);
}

static void ResShadowTexture_Cleanup(ResTexture resTex)
{
    ResShadowTexture resSdwTex = ResDynamicCast<ResShadowTexture>(resTex);
    NW_ASSERT(resSdwTex.IsValid());
    
    GLuint texID = resSdwTex.GetTextureObject();
    if (texID == 0)
    {
        return;
    }
    
    ResPixelBasedImage resImage = resSdwTex.GetImage();
    DeleteImage2D(texID, resImage);
    resSdwTex.SetTextureObject(0);
}

static void ResCubeTexture_Cleanup(ResTexture resTex)
{
    ResCubeTexture resCubeTex = ResDynamicCast<ResCubeTexture>( resTex );
    NW_ASSERT(resCubeTex.IsValid());
    
    GLuint texID = resCubeTex.GetTextureObject();
    if (texID == 0)
    {
        return;
    }
    
    for (int face = 0; face < ResCubeTexture::MAX_CUBE_FACE; ++face)
    {
        ResPixelBasedImage resImage = resCubeTex.GetImage(static_cast<ResCubeTexture::CubeFace>(face));
        DeleteImage2D(texID, resImage);
    }
    
    resCubeTex.SetTextureObject(0);
}

static void ResReferenceTexture_Cleanup(ResTexture resTex)
{
    ResReferenceTexture resRefTex = ResStaticCast<ResReferenceTexture>(resTex);
    
    resRefTex.ref().toTargetTexture.set_ptr(NULL);
}

static void ResProceduralTexture_Cleanup(ResTexture resTex)
{
    ResProceduralTexture resProcTex = ResDynamicCast<ResProceduralTexture>(resTex);
    NW_ASSERT(resProcTex.IsValid());

    ResProceduralNoise noise = resProcTex.GetNoise();
    ut::SafeCleanup(noise.GetNoiseTable());

    ResProceduralClampShift clamp = resProcTex.GetClampShift();
    ResProceduralMapping mapping = resProcTex.GetMapping();
    ut::SafeCleanup(mapping.GetMappingTable());
    ut::SafeCleanup(mapping.GetAlphaMappingTable());

    ResProceduralColor color = resProcTex.GetColor();
    ut::SafeCleanup(color.GetColorTables(0));
    ut::SafeCleanup(color.GetColorTables(1));
    ut::SafeCleanup(color.GetColorTables(2));
    ut::SafeCleanup(color.GetColorTables(3));
}

/* ResTexture */
Result ResTexture::Setup(os::IAllocator* allocator, ResGraphicsFile graphicsFile)
{
    NW_UNUSED_VARIABLE(allocator);
    NW_UNUSED_VARIABLE(graphicsFile);

    NW_ASSERT( internal::ResCheckRevision( *this ) );
    
    Result result = RESOURCE_RESULT_OK;
    switch (this->ref().typeInfo)
    {
    case ResImageTexture::TYPE_INFO:{
            s_TextureSetupTable[0]( *this );
        }
        break;

    case ResCubeTexture::TYPE_INFO:{
            s_TextureSetupTable[1]( *this );
        }
        break;

    case ResReferenceTexture::TYPE_INFO:{
            s_TextureSetupTable[2]( *this );
        }
        break;

    case ResProceduralTexture::TYPE_INFO:{
            s_TextureSetupTable[3]( *this );
        }
        break;

    case ResShadowTexture::TYPE_INFO:{
            s_TextureSetupTable[4]( *this );
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture type.\n");
        }
    }
    
    return result;
}

Result ResTexture::Setup(os::IAllocator* allocator)
{ 
    return this->Setup(allocator, ResGraphicsFile(NULL)); 
}

void ResTexture::Cleanup()
{
    switch (this->ref().typeInfo)
    {
    case ResImageTexture::TYPE_INFO:{
            s_TextureCleanupTable[0]( *this );
        }
        break;

    case ResCubeTexture::TYPE_INFO:{
            s_TextureCleanupTable[1]( *this );
        }
        break;

    case ResReferenceTexture::TYPE_INFO:{
            s_TextureCleanupTable[2]( *this );
        }
        break;

    case ResProceduralTexture::TYPE_INFO:{
            s_TextureCleanupTable[3]( *this );
        }
        break;

    case ResShadowTexture::TYPE_INFO:{
            s_TextureCleanupTable[4]( *this );
        }
        break;

    default:{
            NW_FATAL_ERROR("Unsupported texture type.\n");
        }
    }
}

/* ResImageTexture */

void ResImageTexture::DynamicDestroy()
{
    os::IAllocator* allocator = this->GetImage().ref().m_DynamicAllocator;

    NW_NULL_ASSERT( allocator );

    if (this->IsValid())
    {
        ResPixelBasedImage resPixelBasedImage = this->GetImage();

        if (resPixelBasedImage.IsValid())
        {

            if (resPixelBasedImage.ptr()->toImageDataTable.to_ptr() != NULL)
            {
                allocator->Free(resPixelBasedImage.ptr()->toImageDataTable.to_ptr());
            }

            allocator->Free(resPixelBasedImage.ptr());
        }

        allocator->Free(this->ptr()); 
        this->invalidate();
    }
}

/* ResCubeTexture */

void ResCubeTexture::DynamicDestroy()
{
    ResPixelBasedImage firstFace = this->GetImage(static_cast<CubeFace>(0));

    os::IAllocator* allocator = firstFace.ref().m_DynamicAllocator;

    NW_NULL_ASSERT( allocator );

    if (this->IsValid())
    {

        if (firstFace.IsValid())
        {
            if (firstFace.ptr()->toImageDataTable.to_ptr() != NULL)
            {
                allocator->Free(firstFace.ptr()->toImageDataTable.to_ptr());
            }

            allocator->Free(firstFace.ptr());
        }

        allocator->Free(this->ptr()); 
        this->invalidate();
    }
}

/* ResShadowTexture */

void ResShadowTexture::DynamicDestroy()
{
    os::IAllocator* allocator = this->GetImage().ref().m_DynamicAllocator;

    NW_NULL_ASSERT(allocator);

    ResPixelBasedImage resPixelBasedImage = this->GetImage();

    if (resPixelBasedImage.IsValid())
    {
        if (resPixelBasedImage.ptr()->toImageDataTable.to_ptr() != NULL)
        {
            allocator->Free(resPixelBasedImage.ptr()->toImageDataTable.to_ptr());
        }

        allocator->Free(resPixelBasedImage.ptr());
    }

    allocator->Free(this->ptr()); 
    this->invalidate();
}

/* ResPixelBasedImage */

bool ResPixelBasedImage::CheckMemoryLocation() const
{
    if (!nw::os::IsDeviceMemory(this->GetImageData()))
    {
        return false;
    }
    
    u32 startAddress = reinterpret_cast<u32>( this->GetImageData() );
    u32 endAddress = reinterpret_cast<u32>( this->GetImageData() ) + static_cast<u32>(this->GetImageDataCount()) - 1;

    if ((startAddress & 0x7F) != 0)
    {
        return false;
    }
    
    return true;
}

}
}
}