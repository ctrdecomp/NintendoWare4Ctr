#pragma once

#include <nw/math/math_Types.h>

namespace nw { 
namespace math {

MTX44* MTX44TextureMatrixForMax(MTX44* pOut,float scaleS, float scaleT,float rotate,float translateS, float translateT);
MTX44* MTX44TextureMatrixForMaya(MTX44* pOut,float scaleS, float scaleT,float rotate,float translateS, float translateT);
MTX44* MTX44TextureMatrixForSoftimage(MTX44* pOut,float scaleS, float scaleT,float rotate,float translateS, float translateT);

}
}