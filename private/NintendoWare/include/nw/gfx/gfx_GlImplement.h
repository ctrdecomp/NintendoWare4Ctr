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
/*------------------------------------------------------------------------------*/
/*                                                                              */
/*  Copyright (c) 2009, Digital Media Professionals Inc. All rights reserved.   */
/*                                                                              */
/*------------------------------------------------------------------------------*/
#ifndef NW_GFX_GL_IMPLEMENT_H_
#define NW_GFX_GL_IMPLEMENT_H_

#include <nw/types.h>

#include <gles2/gl2.h>
#include <gles2/gl2ext.h>
#include <nn/gx.h>
#include <string.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

//===============================================================
//
// WARNING: Do not directly reference the API or structure contained in this header.
//
//===============================================================

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PER_PIXEL_LIGHTS            8

typedef void* (*dmpgl_allocator_t)(GLenum, GLenum, GLuint, GLsizei);
typedef void (*dmpgl_deallocator_t)(GLenum, GLenum ,GLuint, void*);

extern dmpgl_allocator_t __dmpgl_allocator;
extern dmpgl_deallocator_t  __dmpgl_deallocator;

extern unsigned* __cb_current_command_buffer;
extern unsigned* __cb_current_max_command_buffer;

#ifdef __cplusplus
}
#endif

namespace nw {
namespace gfx {
namespace internal {

void nwgfxAddVramDmaCommand(void* srcaddr, void* dstaddr, GLsizei size);
void nwgfxClear(
    u32 colorAddr, u32 colorSize, u32 clearColor, u32 colorWidth,
    u32 depthAddr, u32 depthSize, u32 clearDepth, u32 depthWidth
);

void
GetFrameBufferState(
    GLuint fboID,
    u32* pColorAddr,
    u32* pDepthAddr
);

} // namespace internal
} // namespace gfx
} // namespace nw




#endif // NW_GFX_GL_IMPLEMENT_H_
