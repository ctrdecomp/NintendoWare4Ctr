#include <cmath>
#include <nw/math/math_Types.h>

namespace nw { 
namespace math {

inline MTX34* MTX34LookAtFIdx(nw::math::MTX34* pOut, const nw::math::VEC3* pCamPos, f32 twist, const nw::math::VEC3* pTarget){
    using namespace nw::math;

    f32 (*const m)[4] = pOut->matrix;

    VEC3 lookReverse(pCamPos->x - pTarget->x, pCamPos->y - pTarget->y, pCamPos->z - pTarget->z);

    if ((lookReverse.x == 0.0f) && (lookReverse.z == 0.0f)){
        m[0][0] = 1.0f;
        m[0][1] = 0.0f;
        m[0][2] = 0.0f;
        m[0][3] = -pCamPos->x;

        m[1][0] = 0.0f;
        m[1][1] = 0.0f;

        m[2][0] = 0.0f;
        m[2][2] = 0.0f;

        if (lookReverse.y <= 0.0f){
            m[1][2] = 1.0f;
            m[1][3] = -pCamPos->z;

            m[2][1] = -1.0f;
            m[2][3] = pCamPos->y;
        }
        else{
            m[1][2] = -1.0f;
            m[1][3] = pCamPos->z;

            m[2][1] = 1.0f;
            m[2][3] = -pCamPos->y;
        }
    }
    else{
        VEC3 r(lookReverse.z, 0.0f, -lookReverse.x);

        VEC3Normalize(&lookReverse, &lookReverse);
        VEC3Normalize(&r, &r);

        VEC3 u;
        VEC3Cross(&u, &lookReverse, &r);

        f32 st, ct;
        SinCosFIdx(&st, &ct, twist);
        VEC3 right, up;

        right.x = st * u.x + ct * r.x;
        right.y = st * u.y;
        right.z = st * u.z + ct * r.z;

        up.x    = ct * u.x - st * r.x;
        up.y    = ct * u.y;
        up.z    = ct * u.z - st * r.z;

        m[0][0] = right.x;
        m[0][1] = right.y;
        m[0][2] = right.z;
        m[0][3] = -VEC3Dot(pCamPos, &right);

        m[1][0] = up.x;
        m[1][1] = up.y;
        m[1][2] = up.z;
        m[1][3] = -VEC3Dot(pCamPos, &up);

        m[2][0] = lookReverse.x;
        m[2][1] = lookReverse.y;
        m[2][2] = lookReverse.z;
        m[2][3] = -VEC3Dot(pCamPos, &lookReverse);
    }

    return pOut;
}

inline MTX34* MTX34LookAtRad(MTX34* pOut, const VEC3* pCamPos, f32 twist, const VEC3* pTarget){
    f32 twistFIdx = NN_MATH_RAD_TO_FIDX(twist);
    return MTX34LookAtFIdx(pOut, pCamPos, twistFIdx, pTarget);
}



inline MTX34* MTX34CameraRotateFIdx(nw::math::MTX34* pOut, const nw::math::VEC3* pCamPos, const nw::math::VEC3* pCamRotate){
    using namespace nw::math;


    f32 (*const m)[4] = pOut->matrix;

    f32 sx, sy, sz, cx, cy, cz;
    SinCosFIdx(&sx, &cx, pCamRotate->x);
    SinCosFIdx(&sy, &cy, pCamRotate->y);
    SinCosFIdx(&sz, &cz, pCamRotate->z);

    VEC3 right, up, back;

    right.x = sx * sy * sz + cy * cz;
    right.y = cx * sz;
    right.z = sx * cy * sz - sy * cz;

    up.x    = sx * sy * cz - cy * sz;
    up.y    = cx * cz;
    up.z    = sx * cy * cz + sy * sz;

    back.x  = cx * sy;
    back.y  = - sx;
    back.z  = cx * cy;

    m[0][0] = right.x;
    m[0][1] = right.y;
    m[0][2] = right.z;
    m[0][3] = -VEC3Dot(pCamPos, &right);

    m[1][0] = up.x;
    m[1][1] = up.y;
    m[1][2] = up.z;
    m[1][3] = -VEC3Dot(pCamPos, &up);

    m[2][0] = back.x;
    m[2][1] = back.y;
    m[2][2] = back.z;
    m[2][3] = -VEC3Dot(pCamPos, &back);

    return pOut;
}

inline MTX34* MTX34CameraRotateRad(MTX34* pOut, const VEC3* pCamPos, const VEC3* pCamRotate){
    VEC3 rotateFIdx;
    rotateFIdx.x = NN_MATH_RAD_TO_FIDX(pCamRotate->x);
    rotateFIdx.y = NN_MATH_RAD_TO_FIDX(pCamRotate->y);
    rotateFIdx.z = NN_MATH_RAD_TO_FIDX(pCamRotate->z);

    return MTX34CameraRotateFIdx(pOut, pCamPos, &rotateFIdx);
}

inline VEC3* MTX34DecomposeToColumnScale(VEC3* pOut, const MTX34* pM){
    pOut->x = FSqrt((pM->matrix[0][0] * pM->matrix[0][0]) + (pM->matrix[1][0] * pM->matrix[1][0]) + (pM->matrix[2][0] * pM->matrix[1][0]));
    pOut->y = FSqrt((pM->matrix[0][1] * pM->matrix[0][1]) + (pM->matrix[1][1] * pM->matrix[1][1]) + (pM->matrix[2][1] * pM->matrix[2][1]));
    pOut->z = FSqrt((pM->matrix[0][2] * pM->matrix[0][2]) + (pM->matrix[1][2] * pM->matrix[1][2]) + (pM->matrix[2][2] * pM->matrix[2][2]));

    return pOut;
}

}
}