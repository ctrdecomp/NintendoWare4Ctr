

#include <nw/gfx/gfx_OffScreenBuffer.h>

#include <nw/os/os_Memory.h>

#include <nn/gx.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(OffScreenBuffer, IRenderTarget);

OffScreenBuffer::OffScreenBuffer(os::IAllocator* pAllocator, const Description& description, ResPixelBasedTexture resTexture): 
    IRenderTarget(pAllocator),
    mActivateCommand(NULL),
    mDescription(description),
    mTexture(resTexture){
    ResPixelBasedTexture resPixelBasedTexture = ResStaticCast<ResPixelBasedTexture>(resTexture);

    NW_ASSERT(resPixelBasedTexture.GetTextureObject() != 0);

    this->mBackBufferObject.SetHeight(resTexture.GetHeight());
    this->mBackBufferObject.SetWidth(resTexture.GetWidth());
    
    GLuint format;
    
    switch (resTexture.GetFormatHW()){
    case ResPixelBasedTexture::FORMAT_HW_RGBA8:    format = GL_RGBA8_OES; break;
    case ResPixelBasedTexture::FORMAT_HW_RGBA5551: format = GL_RGB5_A1; break;
    case ResPixelBasedTexture::FORMAT_HW_RGBA4:    format = GL_RGBA4; break;
    case ResPixelBasedTexture::FORMAT_HW_RGB565:   format = GL_RGB565; break;
    default:
        NW_FATAL_ERROR("illegal texture format for OffScreenBuffer");
    }
    
    this->mBackBufferObject.SetColorFormat( format );
    this->mBackBufferObject.SetDepthFormat( 0 );

    switch (resTexture.GetTypeInfo()){
    case ResImageTexture::TYPE_INFO:{
            u32 address = ResStaticCast<ResImageTexture>(resTexture).GetImage().GetImageAddress();
            this->mBackBufferObject.SetColorAddress(address);
        }
        break;
    case ResCubeTexture::TYPE_INFO:{
            ResCubeTexture::CubeFace face = ResCubeTexture::CUBE_FACE_POSITIVE_X;
            u32 address = ResStaticCast<ResCubeTexture>(resTexture).GetImage(face).GetImageAddress();
            
            this->mBackBufferObject.SetColorAddress(address);
        }
        break;
    case ResShadowTexture::TYPE_INFO:{
            u32 address = ResStaticCast<ResShadowTexture>(resTexture).GetImage().GetImageAddress();
            this->mBackBufferObject.SetColorAddress(address);
        }
        break;
    default:{
            NW_FATAL_ERROR("Unsupported texture type.\n");
        }
        break;
    }
}

OffScreenBuffer::~OffScreenBuffer(){ }

}
}