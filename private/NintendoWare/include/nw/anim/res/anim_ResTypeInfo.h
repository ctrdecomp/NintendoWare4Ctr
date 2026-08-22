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

#ifndef NW_ANIM_RES_TYPE_INFO_H_
#define NW_ANIM_RES_TYPE_INFO_H_

#include <nw/types.h>

namespace nw
{
namespace anim
{
namespace res
{

#define NW_ANIM_RES_TYPE_INFO(name)  res::name##_TYPE_INFO

//! @details :private
enum ResTypeInfoDefinition
{
    ResAnimGroup_TYPE_INFO              = 0x00000000,
    ResGraphicsAnimGroup_TYPE_INFO      = 0x80000000,
    ResAnimGroupMember_TYPE_INFO        = 0x00000000,
    ResTextureCoordinatorMember_TYPE_INFO = 0x80000000,
    ResBoneMember_TYPE_INFO             = 0x40000000,
    ResTextureMapperMember_TYPE_INFO    = 0x20000000,
    ResModelMember_TYPE_INFO            = 0x10000000,
    ResMaterialColorMember_TYPE_INFO    = 0x08000000,
    ResBlendOperationMember_TYPE_INFO   = 0x04000000,
    ResTextureSamplerMember_TYPE_INFO   = 0x02000000,
    ResMeshMember_TYPE_INFO             = 0x01000000,
    ResTransformMember_TYPE_INFO        = 0x00800000,
    ResViewUpdaterMember_TYPE_INFO      = 0x00400000,
    ResProjectionUpdaterMember_TYPE_INFO = 0x00200000,
    ResLightMember_TYPE_INFO            = 0x00100000,
    ResMeshNodeVisibilityMember_TYPE_INFO = 0x00080000,
    ResFogMember_TYPE_INFO              = 0x00040000,
    ResAnim_TYPE_INFO                   = 0x00000000,
    ResLightAnim_TYPE_INFO              = 0x80000000,
    ResCameraAnim_TYPE_INFO             = 0x40000000
};

} // namespace res
} // namespace anim
} // namespace nw

#endif /* NW_ANIM_RES_TYPE_INFO_H_ */


