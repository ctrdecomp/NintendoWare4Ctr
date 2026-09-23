#pragma once

#include <nw/ut/ut_Flag.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/types.h>
#include <nw/assert.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

namespace nw {
namespace gfx {
#define NN_MATH_USE_ANONYMOUS_STRUCT

class CalculatedTransform
{
public:
    static const f32 s_VecSquareLenTol;
    enum Flag
    {
        FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED_SHIFT           = 0,
        FLAG_IS_VALID_SHIFT                                     = 1,
        FLAG_IS_IGNORE_TRANSLATE_SHIFT                          = 2,
        FLAG_IS_IGNORE_SCALE_SHIFT                              = 3,
        FLAG_IS_IGNORE_ROTATE_SHIFT                             = 4,
        FLAG_IS_IDENTITY_SHIFT                                  = 5,
        FLAG_IS_ROTATE_TRANSLATE_ZERO_SHIFT                     = 6,
        FLAG_IS_ROTATE_ZERO_SHIFT                               = 7,
        FLAG_IS_TRANSLATE_ZERO_SHIFT                            = 8,
        FLAG_IS_SCALE_ONE_SHIFT                                 = 9,
        FLAG_IS_UNIFORM_SCALE_SHIFT                             = 10,
        FLAG_IS_DIRTY_SHIFT                                     = 11,
        FLAG_FORCE_VIEW_CALCULATION_ENABLED_SHIFT               = 12,
        FLAG_CONVERTED_FOR_BLEND_SHIFT                          = 30,
        FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED             = 0x1 << FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED_SHIFT,
        FLAG_IS_VALID                                       = 0x1 << FLAG_IS_VALID_SHIFT,
        FLAG_IS_IGNORE_TRANSLATE                            = 0x1 << FLAG_IS_IGNORE_TRANSLATE_SHIFT,
        FLAG_IS_IGNORE_SCALE                                = 0x1 << FLAG_IS_IGNORE_SCALE_SHIFT, 
        FLAG_IS_IGNORE_ROTATE                               = 0x1 << FLAG_IS_IGNORE_ROTATE_SHIFT, 
        FLAG_IS_IDENTITY                                    = 0x1 << FLAG_IS_IDENTITY_SHIFT,
        FLAG_IS_ROTATE_TRANSLATE_ZERO                       = 0x1 << FLAG_IS_ROTATE_TRANSLATE_ZERO_SHIFT,
        FLAG_IS_ROTATE_ZERO                                 = 0x1 << FLAG_IS_ROTATE_ZERO_SHIFT,
        FLAG_IS_TRANSLATE_ZERO                              = 0x1 << FLAG_IS_TRANSLATE_ZERO_SHIFT, 
        FLAG_IS_SCALE_ONE                                   = 0x1 << FLAG_IS_SCALE_ONE_SHIFT,
        FLAG_IS_UNIFORM_SCALE                               = 0x1 << FLAG_IS_UNIFORM_SCALE_SHIFT,
        FLAG_IS_DIRTY                                       = 0x1 << FLAG_IS_DIRTY_SHIFT, 
        FLAG_FORCE_VIEW_CALCULATION_ENABLED                 = 0x1 << FLAG_FORCE_VIEW_CALCULATION_ENABLED_SHIFT, 
        FLAG_CONVERTED_FOR_BLEND                            = 0x1 << FLAG_CONVERTED_FOR_BLEND_SHIFT,
        FLAG_IS_IGNORE_ALL = FLAG_IS_IGNORE_SCALE | FLAG_IS_IGNORE_ROTATE | FLAG_IS_IGNORE_TRANSLATE,
        FLAG_DEFAULT = FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED | FLAG_IS_DIRTY
    };

    static const CalculatedTransform& Identity()
    {
        static const CalculatedTransform identity(
            FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED |
            FLAG_IS_IDENTITY |
            FLAG_IS_ROTATE_TRANSLATE_ZERO |
            FLAG_IS_ROTATE_ZERO |
            FLAG_IS_TRANSLATE_ZERO |
            FLAG_IS_SCALE_ONE |
            FLAG_IS_UNIFORM_SCALE);
        
        return identity;
    }
    
public:
    CalculatedTransform(bit32 flags): 
        m_TransformMatrix(nw::math::MTX34::Identity()),
        m_Scale(1.0f, 1.0f, 1.0f),
        m_Flags(flags) {}

    CalculatedTransform():
        m_TransformMatrix(nw::math::MTX34::Identity()),
        m_Scale(1.0f, 1.0f, 1.0f),
        m_Flags(FLAG_DEFAULT) {}

    CalculatedTransform(const ResBone bone):
        m_Flags(FLAG_DEFAULT)
        {
        Setup(bone);
    }

    CalculatedTransform(const CalculatedTransform& transform):
        m_TransformMatrix(transform.m_TransformMatrix),
        m_Scale(transform.m_Scale),
        m_Flags(transform.m_Flags) {}

    void Setup(const ResBone bone);

    nw::math::MTX34& DirectTransformMatrix() { return this->m_TransformMatrix; }

    const nw::math::MTX34& TransformMatrix() const { return this->m_TransformMatrix; }

    void SetTransformMatrix(const nw::math::MTX34& transformMatrix)
    {
        nw::math::MTX34Copy(&this->m_TransformMatrix, transformMatrix);

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    void SetTransformMatrix(f32 x00, f32 x01, f32 x02, f32 x03,f32 x10, f32 x11, f32 x12, f32 x13,f32 x20, f32 x21, f32 x22, f32 x23)
    {
        this->m_TransformMatrix.f._00 = x00;
        this->m_TransformMatrix.f._10 = x10;
        this->m_TransformMatrix.f._20 = x20;
        this->m_TransformMatrix.f._01 = x01;
        this->m_TransformMatrix.f._11 = x11;
        this->m_TransformMatrix.f._21 = x21;
        this->m_TransformMatrix.f._02 = x02;
        this->m_TransformMatrix.f._12 = x12;
        this->m_TransformMatrix.f._22 = x22;
        this->m_TransformMatrix.f._03 = x03;
        this->m_TransformMatrix.f._13 = x13;
        this->m_TransformMatrix.f._23 = x23;

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    template<typename TMatrix>
    void SetRotateMatrix(const TMatrix& rotateMatrix)
    {
        this->m_TransformMatrix.f._00 = rotateMatrix.f._00;
        this->m_TransformMatrix.f._10 = rotateMatrix.f._10;
        this->m_TransformMatrix.f._20 = rotateMatrix.f._20;
        this->m_TransformMatrix.f._01 = rotateMatrix.f._01;
        this->m_TransformMatrix.f._11 = rotateMatrix.f._11;
        this->m_TransformMatrix.f._21 = rotateMatrix.f._21;
        this->m_TransformMatrix.f._02 = rotateMatrix.f._02;
        this->m_TransformMatrix.f._12 = rotateMatrix.f._12;
        this->m_TransformMatrix.f._22 = rotateMatrix.f._22;

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    void SetRotateMatrix(f32 x00, f32 x01, f32 x02,f32 x10, f32 x11, f32 x12,f32 x20, f32 x21, f32 x22)
    {
        this->m_TransformMatrix.f._00 = x00;
        this->m_TransformMatrix.f._10 = x10;
        this->m_TransformMatrix.f._20 = x20;
        this->m_TransformMatrix.f._01 = x01;
        this->m_TransformMatrix.f._11 = x11;
        this->m_TransformMatrix.f._21 = x21;
        this->m_TransformMatrix.f._02 = x02;
        this->m_TransformMatrix.f._12 = x12;
        this->m_TransformMatrix.f._22 = x22;

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    nw::math::VEC3& DirectScale() { return m_Scale; }

    const nw::math::VEC3& Scale() const { return m_Scale; }

    void SetScale(const nw::math::VEC3& scale)
    {
        m_Scale = scale;

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    void SetScale(f32 fx, f32 fy, f32 fz)
    {
        m_Scale.Set(fx, fy, fz);

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    void SetTranslate(f32 x, f32 y, f32 z)
    {
        this->m_TransformMatrix.f._03 = x;
        this->m_TransformMatrix.f._13 = y;
        this->m_TransformMatrix.f._23 = z;

        this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    void SetTranslate(const math::VEC3& translate)
    {
        SetTranslate(translate.x, translate.y, translate.z);
    }

    void GetTranslate(math::VEC3* translate) const
    {
        NW_NULL_ASSERT(translate);
        translate->x = this->m_TransformMatrix.f._03;
        translate->y = this->m_TransformMatrix.f._13;
        translate->z = this->m_TransformMatrix.f._23;
    }

    math::VEC3 GetTranslate() const
    {
        return math::VEC3(
            this->m_TransformMatrix.f._03,
            this->m_TransformMatrix.f._13,
            this->m_TransformMatrix.f._23);
    }

    void SetRotateXYZ(f32 x, f32 y, f32 z)
    {
        nw::math::VEC3 translate;
        this->GetTranslate(&translate);
        nw::math::MTX34RotXYZRad(&this->m_TransformMatrix,x,y,z);
        this->SetTranslate(translate);
    }

    void SetRotateAndTranslate(const math::VEC3& rotate, const math::VEC3& translate)
    {
        nw::math::MTX34RotXYZRad(&this->m_TransformMatrix,rotate.x,rotate.y,rotate.z);
        this->SetTranslate(translate);
    }

    void SetTransform(const math::Transform3& transform)
    {
        this->m_Scale = transform.scale;
        SetRotateAndTranslate(transform.rotate, transform.translate);
    }

    bool NormalizeRotateMatrix()
    {
        if (!this->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
            math::VEC3* v0 = reinterpret_cast<math::VEC3*>(&this->m_TransformMatrix.m[0]);
            math::VEC3* v1 = reinterpret_cast<math::VEC3*>(&this->m_TransformMatrix.m[1]);
            math::VEC3* v2 = reinterpret_cast<math::VEC3*>(&this->m_TransformMatrix.m[2]);

            (void)math::VEC3Cross(v2, v0, v1);

            float lengthSquareV0 = v0->LengthSquare();
            float lengthSquareV2 = v2->LengthSquare();

            if (lengthSquareV0 < s_VecSquareLenTol || lengthSquareV2 < s_VecSquareLenTol)
            {
                return false;
            }
            else
            {
                *v0 *= 1.0f / nn::math::FSqrt(lengthSquareV0);
                *v2 *= 1.0f / nn::math::FSqrt(lengthSquareV2);

                (void)math::VEC3Cross(v1, v2, v0);
            }

            this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        }
        return true;
    }

    bool RotateMatrixToQuaternion()
    {
        if (!this->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {

            math::QUAT q;
            math::MTX34ToQUAT(&q, &this->m_TransformMatrix);
            this->m_TransformMatrix.f._00 = q.x;
            this->m_TransformMatrix.f._01 = q.y;
            this->m_TransformMatrix.f._02 = q.z;
            this->m_TransformMatrix.f._10 = q.w;

            this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        }
        return true;
    }

    bool QuaternionToRotateMatrix()
    {
        if (!this->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
            math::VEC3 t = this->m_TransformMatrix.GetColumn(3);
            math::QUAT q(this->m_TransformMatrix.f._00, this->m_TransformMatrix.f._01, this->m_TransformMatrix.f._02, this->m_TransformMatrix.f._10);
            math::QUATToMTX34(&this->m_TransformMatrix, &q);
            this->m_TransformMatrix.SetColumn(3, t);

            math::VEC3* v0 = reinterpret_cast<math::VEC3*>(&this->m_TransformMatrix.m[0]);
            math::VEC3* v1 = reinterpret_cast<math::VEC3*>(&this->m_TransformMatrix.m[1]);
            math::VEC3* v2 = reinterpret_cast<math::VEC3*>(&this->m_TransformMatrix.m[2]);
            if (math::VEC3SquareLen(v0) < s_VecSquareLenTol ||
                math::VEC3SquareLen(v1) < s_VecSquareLenTol ||
                math::VEC3SquareLen(v2) < s_VecSquareLenTol)
            {
                return false;
            }

            this->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        }
        return true;
    }

    void AdjustZeroRotateMatrix(const bool useDefaultMtx, const math::MTX34& defaultMtx)
    {
        if (!this->IsEnabledFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE))
        {
            const math::MTX34& m = this->TransformMatrix();
            const math::VEC3* v0 = reinterpret_cast<const math::VEC3*>(&m.m[0]);
            const math::VEC3* v1 = reinterpret_cast<const math::VEC3*>(&m.m[1]);
            const math::VEC3* v2 = reinterpret_cast<const math::VEC3*>(&m.m[2]);

            if (math::VEC3SquareLen(v0) < s_VecSquareLenTol ||
                math::VEC3SquareLen(v1) < s_VecSquareLenTol ||
                math::VEC3SquareLen(v2) < s_VecSquareLenTol)
                {
                if (useDefaultMtx)
                {
                    this->SetRotateMatrix(defaultMtx);
                }
                else
                {
                    this->SetRotateMatrix(1.0f, 0.0f, 0.0f,0.0f, 1.0f, 0.0f,0.0f, 0.0f, 1.0f);
                }
            }
        }
    }

    bit32 GetFlags() const { return m_Flags; }

    void SetFlags(bit32 flags) { m_Flags = flags; }

    void RestoreFlags(bit32 flags, bit32 values) { m_Flags = (m_Flags & ~flags) | (values & flags); }

    bool IsEnabledFlags(bit32 flags) const { return ut::CheckFlag(m_Flags, flags); }

    bool IsEnabledFlagsOr(bit32 flags) const { return ut::CheckFlagOr(m_Flags, flags); }

    void EnableFlags(bit32 flags) { m_Flags = ut::EnableFlag(m_Flags, flags); }

    void DisableFlags(bit32 flags) { m_Flags = ut::DisableFlag(m_Flags, flags);  }

    void EnableFlags(bit32 flags, bool enable)
    {
        if (enable)
        {
            EnableFlags(flags);
        }
        else
        {
            DisableFlags(flags);
        }
    }

    void ResetFlags() { m_Flags = 0x0; }

    void ResetTransformFlags()
    {
        m_Flags &=
            (~(FLAG_IS_IDENTITY |
            FLAG_IS_ROTATE_TRANSLATE_ZERO |
            FLAG_IS_ROTATE_ZERO |
            FLAG_IS_TRANSLATE_ZERO |
            FLAG_IS_SCALE_ONE |
            FLAG_IS_UNIFORM_SCALE));
    }

    void UpdateFlagsStrictly();

    void UpdateFlags();

    void UpdateScaleFlags()
    {
        if (this->IsEnabledFlags(FLAG_IS_IGNORE_SCALE))
        {
            return;
        }

        this->DisableFlags(FLAG_IS_UNIFORM_SCALE | FLAG_IS_SCALE_ONE);

        const math::VEC3& scale = this->m_Scale;
        if (scale.x == scale.y && scale.x == scale.z)
        {
            this->EnableFlags(FLAG_IS_UNIFORM_SCALE);

            if (scale.x == 1.0f)
            {
                this->EnableFlags(FLAG_IS_SCALE_ONE);
            }
        }
    }

    void UpdateRotateFlagsStrictly();

    void UpdateRotateFlags();

    void UpdateTranslateFlags();

    void UpdateCompositeFlags();

private:
    math::MTX34 m_TransformMatrix;
    math::VEC3  m_Scale;
    bit32 m_Flags;

    friend class WorldMatrixUpdater;
    friend class TransformAnimEvaluator;
    friend class TransformAnimBlendOp;
};

}
}