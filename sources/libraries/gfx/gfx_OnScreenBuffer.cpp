// Filename: gfx_OnScreenBuffer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_OnScreenBuffer.h>

#include <nw/os/os_Memory.h>

#include <nn/gx.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(OnScreenBuffer, IRenderTarget);

OnScreenBuffer::OnScreenBuffer(os::IAllocator* pAllocator, const Description& description): 
    IRenderTarget(pAllocator),
    m_ColorBuffer(0),
    m_DepthBuffer(0),
    m_Description(description)
    {
    
    GLuint fboID;
    
    glGenFramebuffers(1, &fboID);
    
    glBindFramebuffer(GL_FRAMEBUFFER, fboID);

    glGenRenderbuffers(1, &this->m_ColorBuffer);
    glGenRenderbuffers(1, &this->m_DepthBuffer);

    NW_ASSERT(MEMORY_AREA_FCRAM < m_Description.colorArea && m_Description.colorArea < GRAPHICS_MEMORY_AREA_COUNT);
    NW_ASSERT(MEMORY_AREA_FCRAM < m_Description.depthArea && m_Description.depthArea < GRAPHICS_MEMORY_AREA_COUNT);

    glBindRenderbuffer(GL_RENDERBUFFER, this->m_ColorBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER | this->m_Description.colorArea,this->m_Description.colorFormat,this->m_Description.width,this->m_Description.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0,GL_RENDERBUFFER,this->m_ColorBuffer);

    glBindRenderbuffer(GL_RENDERBUFFER, this->m_DepthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER | this->m_Description.depthArea,this->m_Description.depthFormat,this->m_Description.width,this->m_Description.height);

    if (m_Description.depthFormat == RENDER_DEPTH_FORMAT_24_STENCIL8)
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_STENCIL_ATTACHMENT,GL_RENDERBUFFER,this->m_DepthBuffer);
    }
    else
    {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_RENDERBUFFER,this->m_DepthBuffer);
    }
    
    this->m_BackBufferObject.SetFboID(fboID);
    this->m_BackBufferObject.SetWidth(this->m_Description.width );
    this->m_BackBufferObject.SetHeight(this->m_Description.height );
    this->m_BackBufferObject.SetColorFormat(this->m_Description.colorFormat );
    this->m_BackBufferObject.SetDepthFormat(this->m_Description.depthFormat );
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

OnScreenBuffer::~OnScreenBuffer()
{
    if (m_DepthBuffer != 0)
    {
        glDeleteRenderbuffers(1, &this->m_DepthBuffer);
    }

    if (m_ColorBuffer != 0)
    {
        glDeleteRenderbuffers(1, &this->m_ColorBuffer);
    }

    if (this->m_BackBufferObject.GetFboID() != 0)
    {
        GLuint fboID = this->m_BackBufferObject.GetFboID();
        glDeleteFramebuffers(1, &fboID);
    }
}

}
}