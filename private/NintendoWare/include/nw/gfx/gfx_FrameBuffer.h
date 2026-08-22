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

#ifndef NW_GFX_FRAME_BUFFER_H_
#define NW_GFX_FRAME_BUFFER_H_

#include <gles2/gl2.h>
#include <nn/gx/CTR/gx_CTR.h>
#include <nw/ut/ut_Color.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class FrameBufferObject
{
public:

    //
    struct Description
    {
        GLuint fboID;     //
        u32 height;       //
        u32 width;        //
        u32 colorFormat;  //
        u32 depthFormat;  //
        u32 colorAddress; //
        u32 depthAddress; //

        bool useBlock32;  //

        //
        Description()
        {
            fboID = 0;
            useBlock32 = false;
            colorAddress = NULL;
            depthAddress = NULL;
        }
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    operator GLuint() const
    {
        return m_Description.fboID;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    const Description& GetDescription() const { return m_Description; }
    Description& GetDescription() { return m_Description; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void  SetDescription(const Description& description);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    GLuint GetFboID() const { return m_Description.fboID; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetFboID(GLuint fboID);

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32    GetHeight() const { return m_Description.height; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetHeight(u32 height) { m_Description.height = height; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32    GetWidth() const { return m_Description.width; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetWidth(u32 width) { m_Description.width = width; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32    GetColorFormat() const { return m_Description.colorFormat; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetColorFormat(u32 colorFormat) { m_Description.colorFormat = colorFormat; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32    GetDepthFormat() const { return m_Description.depthFormat; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetDepthFormat(u32 depthFormat) { m_Description.depthFormat = depthFormat; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32    GetColorAddress() const { return m_Description.colorAddress; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetColorAddress(u32 colorAddress) { m_Description.colorAddress = colorAddress; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    u32    GetDepthAddress() const { return m_Description.depthAddress; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void   SetDepthAddress(u32 depthAddress) { m_Description.depthAddress = depthAddress; }

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    void ActivateBuffer() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void TransferRenderImage(u32 dstAddress, GLenum dstFormat, GLenum antiAliasMode, bool yFlip) const
    {
        const void* srcAddress = reinterpret_cast<const void*>(this->GetColorAddress());
        const u32 width        = this->GetWidth();
        const u32 height       = this->GetHeight();
        const u32 srcFormat    = this->GetColorFormat();

        nngxAddB2LTransferCommand(
            srcAddress, width, height, srcFormat,
            reinterpret_cast<void*>(dstAddress), width, height, dstFormat,
            antiAliasMode, yFlip, 8);
    }

    enum
    {
        //
        CLEAR_MASK_COLOR = GL_COLOR_BUFFER_BIT,

        //
        //
        CLEAR_MASK_DEPTH = GL_DEPTH_BUFFER_BIT,

        //
        CLEAR_MASK_ALL   = CLEAR_MASK_COLOR | CLEAR_MASK_DEPTH
    };

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void ClearBuffer( u32 mask, const ut::FloatColor& clearColor, f32 clearDepth, u8 clearStencil = 0 ) const;
    void ClearBuffer( const ut::FloatColor& clearColor, f32 clearDepth, u8 clearStencil = 0 ) const
    {
        this->ClearBuffer( CLEAR_MASK_ALL, clearColor, clearDepth, clearStencil );
    }

private:
    Description m_Description;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif


#endif // NW_GFX_ACTIVATE_COMMAND_H_
