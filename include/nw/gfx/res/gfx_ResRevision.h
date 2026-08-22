#pragma once

#include <nw/types.h>

namespace nw {
namespace gfx {
namespace res {

enum GraphicsBinaryRevisions{
    REVISION_RES_GRAPHICS_FILE = 0x05000000,
    REVISION_RES_SHADER        = 0x06000000,
    REVISION_RES_LUT_SET       = 0x04000000,
    REVISION_RES_TEXTURE       = 0x05000000,
    REVISION_RES_MATERIAL      = 0x06000000,
    REVISION_RES_MODEL         = 0x09000000,
    REVISION_RES_CAMERA        = 0x07010000,
    REVISION_RES_LIGHT         = 0x07010000,
    REVISION_RES_FOG           = 0x06000000,
    REVISION_RES_EMITTER       = 0x05000000,
    REVISION_RES_RENDER_ENV_SETTING = 0x01000000
};

}
}
}