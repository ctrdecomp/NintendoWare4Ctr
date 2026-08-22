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

#ifndef NW_UT_RES_TYPE_INFO_H_
#define NW_UT_RES_TYPE_INFO_H_

#include <nw/types.h>

namespace nw
{
namespace ut
{
namespace internal
{

#define NW_UT_RES_TYPE_INFO(name)  internal::name##_TYPE_INFO

//! @details :private
enum ResTypeInfoDefinition
{
    ResMetaData_TYPE_INFO               = 0x00000000,
    ResFloatArrayMetaData_TYPE_INFO     = 0x80000000,
    ResColorArrayMetaData_TYPE_INFO     = 0x40000000,
    ResIntArrayMetaData_TYPE_INFO       = 0x20000000,
    ResStringArrayMetaData_TYPE_INFO    = 0x10000000,
    ResVector3ArrayMetaData_TYPE_INFO   = 0x08000000,
    ResBoundingVolume_TYPE_INFO         = 0x00000000,
    ResOrientedBoundingBox_TYPE_INFO    = 0x80000000,
    ResAxisAlignedBoundingBox_TYPE_INFO = 0x40000000
};

} // namespace internal
} // namespace ut
} // namespace nw

#endif /* NW_UT_RES_TYPE_INFO_H_ */


