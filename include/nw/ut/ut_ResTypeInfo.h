#pragma once

#include <nw/types.h>

namespace nw {
namespace ut {
namespace internal {

#define NW_UT_RES_TYPE_INFO(name)  internal::name##_TYPE_INFO

enum ResTypeInfoDefinition{
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

}
}
}