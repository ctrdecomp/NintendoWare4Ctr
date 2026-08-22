#include <cmath>
#include <nw/math/math_Types.h>
#include <nw/Assert.h>

namespace nw { 
namespace math {
using namespace nn::math;

inline MTX44* MTX44TextureMatrixForMaya(MTX44* pOut,float scaleS, float scaleT,float rotate,float translateS, float translateT){
    NW_NULL_ASSERT(pOut);

    float rotateSin;
    float rotateCos;
    SinCosFIdx(&rotateSin, &rotateCos, NN_MATH_RAD_TO_FIDX(rotate));

    pOut->matrix[0][0] =  scaleS * rotateCos;
    pOut->matrix[0][1] = -scaleS * rotateSin;
    pOut->matrix[0][3] =  scaleS * ( 0.5f * rotateSin - 0.5f * rotateCos + 0.5f - translateS);

    pOut->matrix[1][0] = scaleT * rotateSin;
    pOut->matrix[1][1] = scaleT * rotateCos;
    pOut->matrix[1][3] = scaleT * (-0.5f * rotateSin - 0.5f * rotateCos + 0.5f - translateT);

    pOut->matrix[0][2] = pOut->matrix[1][2] =
    pOut->matrix[2][0] = pOut->matrix[2][1] = pOut->matrix[2][3] =
    pOut->matrix[3][0] = pOut->matrix[3][1] = pOut->matrix[3][2] = 0.0f;
    pOut->matrix[2][2] = pOut->matrix[3][3] = 1.0f;

    return pOut;
}

inline MTX44* MTX44TextureMatrixForSoftimage(MTX44* pOut,float scaleS, float scaleT,float rotate,float translateS, float translateT){
    NW_NULL_ASSERT(pOut);

    float rotateSin;
    float rotateCos;
    SinCosFIdx(&rotateSin, &rotateCos, NN_MATH_RAD_TO_FIDX(rotate));

    pOut->matrix[0][0] = scaleS * rotateCos;
    pOut->matrix[0][1] = scaleS * rotateSin;
    pOut->matrix[0][3] = scaleS * (-rotateCos * translateS - rotateSin * translateT);

    pOut->matrix[1][0] = -scaleT * rotateSin;
    pOut->matrix[1][1] =  scaleT * rotateCos;
    pOut->matrix[1][3] =  scaleT * ( rotateSin * translateS - rotateCos * translateT);

    pOut->matrix[0][2] = pOut->matrix[1][2] =
    pOut->matrix[2][0] = pOut->matrix[2][1] = pOut->matrix[2][3] =
    pOut->matrix[3][0] = pOut->matrix[3][1] = pOut->matrix[3][2] = 0.0f;
    pOut->matrix[2][2] = pOut->matrix[3][3] = 1.0f;

    return pOut;
}

inline MTX44* MTX44TextureMatrixForMax(MTX44* pOut,float scaleS, float scaleT,float rotate,float translateS, float translateT){
    NW_NULL_ASSERT(pOut);

    float rotateSin;
    float rotateCos;
    SinCosFIdx(&rotateSin, &rotateCos, NN_MATH_RAD_TO_FIDX(rotate));

    float scaleSSin = scaleS * rotateSin;
    float scaleSCos = scaleS * rotateCos;
    float scaleTSin = scaleT * rotateSin;
    float scaleTCos = scaleT * rotateCos;
    float ts = -translateS - 0.5f;
    float tt =  translateT - 0.5f;

    pOut->matrix[0][0] =  scaleSCos;
    pOut->matrix[0][1] = -scaleSSin;
    pOut->matrix[0][3] =  (scaleSCos * ts) - (scaleSSin * tt) + 0.5f;

    pOut->matrix[1][0] =  scaleTSin;
    pOut->matrix[1][1] =  scaleTCos;
    pOut->matrix[1][3] =  (scaleTSin * ts) + (scaleTCos * tt) + 0.5f;

    pOut->matrix[0][2] = pOut->matrix[1][2] =
    pOut->matrix[2][0] = pOut->matrix[2][1] = pOut->matrix[2][3] =
    pOut->matrix[3][0] = pOut->matrix[3][1] = pOut->matrix[3][2] = 0.0f;
    pOut->matrix[2][2] = pOut->matrix[3][3] = 1.0f;

    return pOut;
}

}
}