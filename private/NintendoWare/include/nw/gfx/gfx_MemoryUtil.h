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
#ifndef NW_GFX_MEMORY_UTIL_H_
#define NW_GFX_MEMORY_UTIL_H_

#include <nw/types.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/math.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {
namespace internal {

#define DECL_SUB(size) \
asm u32* FastWordCopyAsm_##size(u32* /*dst*/, u32* /*src*/);

DECL_SUB(2)
DECL_SUB(4)
DECL_SUB(6)
DECL_SUB(8)
DECL_SUB(10)
DECL_SUB(12)
DECL_SUB(14)
DECL_SUB(16)
DECL_SUB(18)
DECL_SUB(20)
DECL_SUB(22)
DECL_SUB(24)
DECL_SUB(26)
DECL_SUB(28)
DECL_SUB(30)
DECL_SUB(32)
DECL_SUB(34)
DECL_SUB(36)
DECL_SUB(38)
DECL_SUB(78)

#undef DECL_SUB

NW_FORCE_INLINE u32*
FastWordCopy(u32* dst, u32* src, u32 size)
{
    size /= 4;
    NW_ASSERT(size % 2 == 0);

#define INVOKE_SUB(count) \
    if (size == count) \
    { \
        return FastWordCopyAsm_##count(dst, src); \
    }

    INVOKE_SUB(2)
    INVOKE_SUB(4)
    INVOKE_SUB(6)
    INVOKE_SUB(8)
    INVOKE_SUB(10)
    INVOKE_SUB(12)
    INVOKE_SUB(14)
    INVOKE_SUB(16)
    INVOKE_SUB(18)
    INVOKE_SUB(20)
    INVOKE_SUB(22)
    INVOKE_SUB(24)
    INVOKE_SUB(26)
    INVOKE_SUB(28)
    INVOKE_SUB(30)
    INVOKE_SUB(32)
    INVOKE_SUB(34)
    INVOKE_SUB(36)
    INVOKE_SUB(38)
    INVOKE_SUB(78)

    NW_LOG("Unsupported size : %d\n", size);
    NW_ASSERT(false);
    return NULL;
#undef INVOKE_SUB
}

} // namespace internal
} // namespace gfx
} // namespace nw

#endif // NW_GFX_MEMORY_UTIL_H_
