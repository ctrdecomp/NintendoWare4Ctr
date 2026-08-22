#pragma once

#include <gles2/gl2.h>
#include <nw/ut/ut_Color.h>

namespace nw {
namespace gfx {

class FrameBufferObject{
public:
    struct Description{
        GLuint fboID;
        u32 height;
        u32 width;
        u32 colorFormat;
        u32 depthFormat;
        u32 colorAddress;
        u32 depthAddress;
        
        bool useBlock32;

        Description(){
            fboID = 0;
            useBlock32 = false;
            colorAddress = NULL;
            depthAddress = NULL;
        }
    };
    
    operator GLuint() const{
        return mDescription.fboID;
    }
    
    const Description& GetDescription() const { return mDescription; }
    Description& GetDescription() { return mDescription; }

    void  SetDescription(const Description& description);

    GLuint GetFboID() const { return mDescription.fboID; }

    void   SetFboID(GLuint fboID);

    u32    GetHeight() const { return mDescription.height; }

    void   SetHeight(u32 height) { mDescription.height = height; }

    u32    GetWidth() const { return mDescription.width; }

    void   SetWidth(u32 width) { mDescription.width = width; }

    u32    GetColorFormat() const { return mDescription.colorFormat; }

    void   SetColorFormat(u32 colorFormat) { mDescription.colorFormat = colorFormat; }

    u32    GetDepthFormat() const { return mDescription.depthFormat; }

    void SetDepthFormat(u32 depthFormat) { mDescription.depthFormat = depthFormat; }

    u32 GetColorAddress() const { return mDescription.colorAddress; }

    void SetColorAddress(u32 colorAddress) { mDescription.colorAddress = colorAddress; }

    u32 GetDepthAddress() const { return mDescription.depthAddress; }

    void SetDepthAddress(u32 depthAddress) { mDescription.depthAddress = depthAddress; }

    void ActivateBuffer() const;

    void TransferRenderImage(u32 dstAddress, GLenum dstFormat, GLenum antiAliasMode, bool yFlip) const{
        const void* srcAddress = reinterpret_cast<const void*>(this->GetColorAddress());
        const u32 width        = this->GetWidth();
        const u32 height       = this->GetHeight();
        const u32 srcFormat    = this->GetColorFormat();
        
        nngxAddB2LTransferCommand(srcAddress, width, height, srcFormat,reinterpret_cast<void*>(dstAddress), width, height, dstFormat,antiAliasMode, yFlip, 8);
    }
    
    enum{
        CLEAR_MASK_COLOR = GL_COLOR_BUFFER_BIT,
        CLEAR_MASK_DEPTH = GL_DEPTH_BUFFER_BIT, 
        CLEAR_MASK_ALL   = CLEAR_MASK_COLOR | CLEAR_MASK_DEPTH 
    };

    void ClearBuffer(u32 mask, const nw::ut::FloatColor& clearColor, f32 clearDepth, u8 clearStencil = 0) const;
    void ClearBuffer(const nw::ut::FloatColor& clearColor, f32 clearDepth, u8 clearStencil = 0) const{
        this->ClearBuffer(CLEAR_MASK_ALL, clearColor, clearDepth, clearStencil);
    }

private:
    Description mDescription;
};

}
}