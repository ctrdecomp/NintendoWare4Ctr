

#include <nw/gfx/gfx_OnScreenBuffer.h>

#include <nw/os/os_Memory.h>

#include <nn/gx.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(OnScreenBuffer, IRenderTarget);

OnScreenBuffer::OnScreenBuffer(os::IAllocator* pAllocator, const Description& description): 
    IRenderTarget(pAllocator),
    mColorBuffer(0),
    mDepthBuffer(0),
    mDescription(description){
    
    GLuint fboID;
    
    glGenFramebuffers(1, &fboID);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glGenRenderbuffers(1, &this->mColorBuffer);
    glGenRenderbuffers(1, &this->mDepthBuffer);

    NW_ASSERT(MEMORY_AREA_FCRAM < mDescription.colorArea && mDescription.colorArea < GRAPHICS_MEMORY_AREA_COUNT);
    NW_ASSERT(MEMORY_AREA_FCRAM < mDescription.depthArea && mDescription.depthArea < GRAPHICS_MEMORY_AREA_COUNT);

    glBindRenderbuffer(GL_RENDERBUFFER, this->mColorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER | this->mDescription.colorArea,this->mDescription.colorFormat,this->mDescription.width,this->mDescription.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,this->mColorBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, this->mDepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER | this->mDescription.depthArea,this->mDescription.depthFormat,this->mDescription.width,this->mDescription.height);

    if (mDescription.depthFormat == RENDER_DEPTH_FORMAT_24_STENCIL8){
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,this->mDepthBuffer);
    }
    else{
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,this->mDepthBuffer);
    }
    
    this->mBackBufferObject.SetFboID(fboID);
    this->mBackBufferObject.SetWidth(this->mDescription.width );
    this->mBackBufferObject.SetHeight(this->mDescription.height );
    this->mBackBufferObject.SetColorFormat(this->mDescription.colorFormat );
    this->mBackBufferObject.SetDepthFormat(this->mDescription.depthFormat );
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OnScreenBuffer::~OnScreenBuffer(){
    if (mDepthBuffer != 0){
        glDeleteRenderbuffers(1, &this->mDepthBuffer);
    }

    if (mColorBuffer != 0){
        glDeleteRenderbuffers(1, &this->mColorBuffer);
    }

    if (this->mBackBufferObject.GetFboID() != 0){
        GLuint fboID = this->mBackBufferObject.GetFboID();
        glDeleteFramebuffers(1, &fboID);
    }
}

}
}