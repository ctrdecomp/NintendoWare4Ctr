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

#ifndef NW_GFX_OFFSCREENBUFFER_H_
#define NW_GFX_OFFSCREENBUFFER_H_

#include <nw/gfx/gfx_IRenderTarget.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/gfx/res/gfx_ResTexture.h>
#include <nw/gfx/gfx_FrameBuffer.h>

namespace nw
{
namespace os
{
class IAllocator;
} // namespace os

namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class OffScreenBuffer : public IRenderTarget
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(OffScreenBuffer);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    virtual const Description& GetDescription() const { return m_Description; }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    virtual const FrameBufferObject& GetBufferObject() const { return m_BackBufferObject; }

private:
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    OffScreenBuffer(os::IAllocator* pAllocator, const Description& description, ResPixelBasedTexture resTexture);

    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    virtual ~OffScreenBuffer();

//    GLuint m_BackBufferObject;
    FrameBufferObject m_BackBufferObject;
    void* m_ActivateCommand;

    Description m_Description;

    ResTexture m_Texture;

    friend class IRenderTarget;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_OFFSCREENBUFFER_H_
