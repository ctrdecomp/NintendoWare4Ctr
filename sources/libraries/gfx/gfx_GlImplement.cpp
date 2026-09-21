#include <nw/gfx/gfx_GlImplement.h>

namespace nw{
namespace gfx {
namespace internal {

void nwgfxAddVramDmaCommand(void* srcaddr, void* dstaddr, GLsizei size)
{
    nngxAddVramDmaCommand( srcaddr, dstaddr, size);
}

void nwgfxClear( 
    u32 colorAddr, u32 colorSize, u32 clearColor, u32 colorWidth,
    u32 depthAddr, u32 depthSize, u32 clearDepth, u32 depthWidth )
{
    nngxAddMemoryFillCommand(reinterpret_cast<GLvoid*>(colorAddr), colorSize, clearColor, colorWidth,reinterpret_cast<GLvoid*>(depthAddr), depthSize, clearDepth, depthWidth);
}

void GetFrameBufferState( GLuint fboID, u32* pColorAddr, u32* pDepthAddr)
{
    glBindFramebuffer( GL_FRAMEBUFFER, fboID );

    if (pColorAddr)
    {
        s32 renderID;
        s32 addr;
        
        glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
            GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
            &renderID);
        
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,
            GL_RENDERBUFFER_DATA_ADDR_DMP,
            &addr);
        
        *pColorAddr = static_cast<u32>(addr);
    }

    if (pDepthAddr)
    {
        s32 renderID;
        s32 addr;
        
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, 
            GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
            &renderID );
        
        glBindRenderbuffer(GL_RENDERBUFFER, renderID);
        
        glGetRenderbufferParameteriv(GL_RENDERBUFFER,GL_RENDERBUFFER_DATA_ADDR_DMP,&addr);
        
        *pDepthAddr = static_cast<u32>(addr);
    }
}

}
}
}