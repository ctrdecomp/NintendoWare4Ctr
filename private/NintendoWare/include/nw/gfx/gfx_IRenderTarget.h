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

#ifndef NW_GFX_IRENDERTARGET_H_
#define NW_GFX_IRENDERTARGET_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/gfx/gfx_FrameBuffer.h>
#include <nw/gfx/res/gfx_ResTexture.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <nn/gx/CTR/gx_CTR.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{
namespace res
{
class ResTexture;
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
enum RenderColorFormat
{
    RENDER_COLOR_FORMAT_NONE,
    RENDER_COLOR_FORMAT_RGBA8 = GL_RGBA8_OES,
    RENDER_COLOR_FORMAT_RGB8 = GL_RGB8_OES,
    RENDER_COLOR_FORMAT_RGBA4 = GL_RGBA4,
    RENDER_COLOR_FORMAT_RGB5_A1 = GL_RGB5_A1,
    RENDER_COLOR_FORMAT_RGB565 = GL_RGB565,
    RENDER_COLOR_FORMAT_COUNT
};

enum RenderDepthFormat
{
    RENDER_DEPTH_FORMAT_NONE,
    RENDER_DEPTH_FORMAT_16 = GL_DEPTH_COMPONENT16,
    RENDER_DEPTH_FORMAT_24 = GL_DEPTH_COMPONENT24_OES,
    RENDER_DEPTH_FORMAT_24_STENCIL8 = GL_DEPTH24_STENCIL8_EXT,
    RENDER_DEPTH_FORMAT_COUNT
};

namespace internal
{
    enum
    {
        MEMORY_AREA_FCRAM_INTERNAL = NN_GX_MEM_FCRAM,
        MEMORY_AREA_VRAMA_INTERNAL = NN_GX_MEM_VRAMA,
        MEMORY_AREA_VRAMB_INTERNAL = NN_GX_MEM_VRAMB
    };
}

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
enum GraphicsMemoryArea
{
    MEMORY_AREA_NONE,
    MEMORY_AREA_FCRAM = internal::MEMORY_AREA_FCRAM_INTERNAL, //
    MEMORY_AREA_VRAMA = internal::MEMORY_AREA_VRAMA_INTERNAL, //
    MEMORY_AREA_VRAMB = internal::MEMORY_AREA_VRAMB_INTERNAL, //
    GRAPHICS_MEMORY_AREA_COUNT                                //
};

//
enum ShadowKind
{
    SHADOW_KIND_NONE,
    SHADOW_KIND_TEXTURE,
    SHADOW_KIND_CUBE
};

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class IRenderTarget : public GfxObject
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    struct Description
    {
        s32 width; //
        s32 height; //
        RenderColorFormat colorFormat; //
        RenderDepthFormat depthFormat; //
        GraphicsMemoryArea colorArea;  //
        GraphicsMemoryArea depthArea;  //
#if 0   // This code is commented out because the following members do not affect the program's behavior.
        u32 colorAddress;              //
        u32 depthAddress;              //
#endif
        ShadowKind shadowKind;         //

        //
        Description()
        : width(400),
          height(240),
          colorFormat(RENDER_COLOR_FORMAT_RGBA8),
          depthFormat(RENDER_DEPTH_FORMAT_24_STENCIL8),
          colorArea(MEMORY_AREA_VRAMA),
          depthArea(MEMORY_AREA_VRAMB),
#if 0     // This code is commented out because the following members do not affect the program's behavior.
          colorAddress(NULL),
          depthAddress(NULL),
#endif
          shadowKind(SHADOW_KIND_NONE)
        {}
    };

    //
    class Builder
    {
    public:

        //
        //
        //
        //
        Builder& Described(const Description& description)
        {
            m_Description = description;
            return *this;
        }

        //
        //
        //
        //
        //
        Builder& BufferSize(s32 width, s32 height)
        {
            m_Description.width = width;
            m_Description.height = height;
            return *this;
        }

        //
        Builder& Width(s32 width) { m_Description.width = width; return *this; }

        //
        Builder& Height(s32 height) { m_Description.height = height; return *this; }

        //
        Builder& ColorFormat(RenderColorFormat format)
        {
            m_Description.colorFormat = format;
            return *this;
        }

        //
        Builder& DepthFormat(RenderDepthFormat format)
        {
            m_Description.depthFormat = format;
            return *this;
        }

        //
        //
        //
        Builder& ColorArea(GraphicsMemoryArea memory)
        {
            m_Description.colorArea = memory;
            return *this;
        }

        //
        //
        //
        Builder& DepthArea(GraphicsMemoryArea memory)
        {
            m_Description.depthArea = memory;
            return *this;
        }

#if 0   // This code is commented out because the following members do not affect the program's behavior.
        //
        Builder& ColorAddress(u32 colorAddress)
        {
            m_Description.colorAddress = colorAddress;
            return *this;
        }

        //
        Builder& DepthAddress(u32 depthAddress)
        {
            m_Description.depthAddress = depthAddress;
            return *this;
        }
#endif

        //
        //
        //
        //
        //
        //
        IRenderTarget* Create(os::IAllocator* allocator);

    private:
        Description m_Description;
    };

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    static IRenderTarget* CreateOffScreenBuffer(os::IAllocator* allocator, ResTexture resTexture);

    //

    //----------------------------------------
    //
    //

    //
    virtual const Description& GetDescription() const = 0;

    //
    virtual const FrameBufferObject& GetBufferObject() const = 0;

    //

protected:
    //----------------------------------------
    //
    //

    //
    IRenderTarget(os::IAllocator* allocator) : GfxObject(allocator) {}

    //
    virtual ~IRenderTarget() {}

    //
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_IRENDERTARGET_H_
