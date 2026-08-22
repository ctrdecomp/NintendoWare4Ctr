#pragma once

#include <nw/types.h>

#include <gles2/gl2.h>
#include <gles2/gl2extern.h>
#include <nn/gx.h>
#include <string.h>
#include <nn/gx/CTR/gx_CommandAccess.h>

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

namespace nw { /* nw */
namespace gfx {
namespace internal {

void nwgfxAddVramDmaCommand(void* srcaddr, void* dstaddr, GLsizei size);
void nwgfxClear( u32 colorAddr, u32 colorSize, u32 clearColor, u32 colorWidth,u32 depthAddr, u32 depthSize, u32 clearDepth, u32 depthWidth );

void GetFrameBufferState(GLuint fboID, u32* pColorAddr, u32* pDepthAddr);

}
}
}