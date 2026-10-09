#pragma once

#include <nw/math/math_Types.h>

namespace nw { 
namespace math {

VEC3*  MTX34DecomposeToColumnScale(VEC3* pOut, const MTX34* pM);
MTX34* MTX34CameraRotateRad(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamRotate);

}
}
