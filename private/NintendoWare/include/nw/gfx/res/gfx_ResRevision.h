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

#ifndef NW_GFX_RESREVISION_H_
#define NW_GFX_RESREVISION_H_

#include <nw/types.h>

namespace nw {
namespace gfx {
namespace res {

//
enum GraphicsBinaryRevisions
{
    REVISION_RES_GRAPHICS_FILE = 0x05000000, // The binary revision of ResGraphicsFile.
    REVISION_RES_SHADER        = 0x06000000, // The binary revision of ResShader.
    REVISION_RES_LUT_SET       = 0x04000100, // The binary revision of ResLookupTableSet.
    REVISION_RES_TEXTURE       = 0x05000000, // The binary revision of ResTexture.
    REVISION_RES_MATERIAL      = 0x06000003, // The binary revision of ResMaterial.
    REVISION_RES_MODEL         = 0x09000000, // The binary revision of ResModel.
    REVISION_RES_CAMERA        = 0x07010000, // The binary revision of ResCamera.
    REVISION_RES_LIGHT         = 0x07010000, // The binary revision of ResLight.
    REVISION_RES_FOG           = 0x06000000, // The binary revision of ResFog.
    REVISION_RES_EMITTER       = 0x05000000, // The binary revision of ResEmitter.
    REVISION_RES_RENDER_ENV_SETTING = 0x01000000 // The binary revision of ResSceneEnvironmentSetting.
};

} // namespace res
} // namespace gfx
} // namespace nw

#endif // NW_GFX_RESREVISION_H_
