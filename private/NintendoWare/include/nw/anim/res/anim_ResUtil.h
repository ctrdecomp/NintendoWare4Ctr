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

#ifndef NW_ANIM_RESUTIL_H_
#define NW_ANIM_RESUTIL_H_

#include <nw/anim/res/anim_ResAnim.h>

namespace nw {
namespace anim {
namespace res {

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
ResAnim CloneTextureAnim(os::IAllocator* allocator, const ResAnim& src);

//
//
//
//
//
void DestroyClonedTextureAnim(os::IAllocator* allocator, ResAnim anim);

} // namespace res
} // namespace anim
} // namespace nw

#endif // NW_ANIM_RESUTIL_H_
