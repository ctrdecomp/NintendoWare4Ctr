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

#ifndef NW_ANIM_RESREVISION_H_
#define NW_ANIM_RESREVISION_H_

#include <nw/types.h>

namespace nw {
namespace anim {
namespace res {

//
enum AnimationBinaryRevisions
{
    REVISION_RES_ANIM_DATA = 0x07000001 // The binary revision of ResAnim.
};

} /* namespcae res */
} /* namespace anim */
} /* namespace nw */

#endif /* NW_ANIM_RESREVISION_H_ */
