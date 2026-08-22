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

#ifndef NW_GFX_ONSCREENBUFFER_H_
#define NW_GFX_ONSCREENBUFFER_H_

#include <nw/gfx/gfx_IRenderTarget.h>
#include <nw/ut/ut_Preprocessor.h>

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
class OnScreenBuffer : public IRenderTarget
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(OnScreenBuffer);

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
    //
    OnScreenBuffer(os::IAllocator* pAllocator, const Description& description);

    void InitializeGl();
    void InitizlizeManulally();

    //
    virtual ~OnScreenBuffer();

    u32 m_ColorBuffer;
    u32 m_DepthBuffer;
    FrameBufferObject m_BackBufferObject;

    Description m_Description;

    friend class IRenderTarget;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_ONSCREENBUFFER_H_
