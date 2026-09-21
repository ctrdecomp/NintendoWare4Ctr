// Filename: gfx_IRenderTarget.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_IRenderTarget.h>

#include <nw/gfx/gfx_OnScreenBuffer.h>
#include <nw/gfx/gfx_OffScreenBuffer.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(IRenderTarget);

IRenderTarget* IRenderTarget::Builder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    IRenderTarget* renderTarget = NULL;

    const int renderTargetAlignment = 32;
    void* memory = allocator->Alloc(sizeof(OnScreenBuffer), renderTargetAlignment);
    NW_NULL_ASSERT(memory);
    
    renderTarget = new(memory) OnScreenBuffer(allocator, m_Description);
    
    return renderTarget;
}

IRenderTarget* IRenderTarget::CreateOffScreenBuffer(os::IAllocator* allocator, ResTexture resTexture)
{
    NW_NULL_ASSERT(allocator);
    IRenderTarget* renderTarget = NULL;

    const int renderTargetAlignment = 32;
    void* memory = allocator->Alloc(sizeof(OffScreenBuffer), renderTargetAlignment);
    NW_NULL_ASSERT(memory);

    ResPixelBasedTexture texture = ResStaticCast<ResPixelBasedTexture>(resTexture.Dereference());

    const RenderColorFormat formatTable[] ={
        RENDER_COLOR_FORMAT_RGBA8,
        RENDER_COLOR_FORMAT_RGB8,
        RENDER_COLOR_FORMAT_RGB5_A1,
        RENDER_COLOR_FORMAT_RGB565,
        RENDER_COLOR_FORMAT_RGBA4,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE,
        RENDER_COLOR_FORMAT_NONE
    };

    Description description;
    description.width = texture.GetWidth();
    description.height = texture.GetHeight();
    description.colorFormat = formatTable[texture.GetFormatHW()];
    NW_ASSERT(description.colorFormat != RENDER_COLOR_FORMAT_NONE);
    
    u32 locationFlag = texture.GetLocationFlag();
    locationFlag &= ~(GL_COPY_FCRAM_DMP | GL_NO_COPY_FCRAM_DMP);
    
    NW_ASSERT(
        (locationFlag == MEMORY_AREA_VRAMA) ||
        (locationFlag == MEMORY_AREA_VRAMB));
    description.colorArea = static_cast<GraphicsMemoryArea>(locationFlag);

    description.depthFormat = RENDER_DEPTH_FORMAT_NONE;
    description.depthArea = MEMORY_AREA_NONE;

    switch (texture.GetTypeInfo())
    {
    case ResImageTexture::TYPE_INFO:{
            description.shadowKind = SHADOW_KIND_NONE;
        }
        break;
    case ResCubeTexture::TYPE_INFO:{
            description.shadowKind = SHADOW_KIND_NONE;
        }
        break;
    case ResShadowTexture::TYPE_INFO:{
            description.shadowKind = SHADOW_KIND_TEXTURE;
        }
        break;
    default:{
            NW_FATAL_ERROR("Unsupported texture type.\n");
        }
        break;
    }
    
    renderTarget = new(memory) OffScreenBuffer(allocator, description, texture);
    
    return renderTarget;
}

}
}