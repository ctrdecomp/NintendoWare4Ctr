// Filename: gfx_CalculatedTransform.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

namespace nw{
namespace gfx{

const f32 CalculatedTransform::s_VecSquareLenTol = 1.0e-6f;

void CalculatedTransform::Setup(const ResBone bone)
{
    bool translateZeroFlag = false;
    bool rotateZeroFlag = false;

    this->SetTransform(bone.GetTransform());
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_IDENTITY) )
    {
        this->EnableFlags(CalculatedTransform::FLAG_IS_IDENTITY);
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_TRANSLATE_ZERO) )
    {
        this->EnableFlags(CalculatedTransform::FLAG_IS_TRANSLATE_ZERO);
        translateZeroFlag = true;
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_ROTATE_ZERO) )
    {
        this->EnableFlags(CalculatedTransform::FLAG_IS_ROTATE_ZERO);
        rotateZeroFlag = true;
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_SCALE_ONE) )
    {
        this->EnableFlags(CalculatedTransform::FLAG_IS_SCALE_ONE);
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_UNIFORM_SCALE) )
    {
        this->EnableFlags(CalculatedTransform::FLAG_IS_UNIFORM_SCALE);
    }
    if (translateZeroFlag && rotateZeroFlag)
    {
        this->EnableFlags(CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO);
    }
}

void CalculatedTransform::UpdateFlagsStrictly()
{
    this->UpdateScaleFlags();
    this->UpdateRotateFlagsStrictly();
    this->UpdateTranslateFlags();
    this->UpdateCompositeFlags();
}

void CalculatedTransform::UpdateFlags()
{
    this->UpdateScaleFlags();
    this->UpdateRotateFlags();
    this->UpdateTranslateFlags();
    this->UpdateCompositeFlags();
}

void CalculatedTransform::UpdateRotateFlagsStrictly()
{
    if (this->IsEnabledFlags(FLAG_IS_IGNORE_ROTATE))
    {
        return;
    }

    const math::MTX34& mtx = this->m_TransformMatrix;
    if (mtx.f._00 == 1.0f && mtx.f._01 == 0.0f && mtx.f._02 == 0.0f &&
        mtx.f._10 == 0.0f && mtx.f._11 == 1.0f && mtx.f._12 == 0.0f &&
        mtx.f._20 == 0.0f && mtx.f._21 == 0.0f && mtx.f._22 == 1.0f)
        {
        this->EnableFlags(FLAG_IS_ROTATE_ZERO);
    }
    else
    {
        this->DisableFlags(FLAG_IS_ROTATE_ZERO);
    }
}

void CalculatedTransform::UpdateRotateFlags()
{
    if (this->IsEnabledFlags(FLAG_IS_IGNORE_ROTATE))
    {
        return;
    }

    const math::MTX34& mtx = this->m_TransformMatrix;
    if (mtx.f._00 == 1.0f && mtx.f._11 == 1.0f)
    {
#if 0   
        NW_WARNING(
            ut::FloatEqualsWeak(mtx.f._00, 1.0f) && ut::FloatEqualsWeak(mtx.f._01, 0.0f) && ut::FloatEqualsWeak(mtx.f._02, 0.0f) &&
            ut::FloatEqualsWeak(mtx.f._10, 0.0f) && ut::FloatEqualsWeak(mtx.f._11, 1.0f) && ut::FloatEqualsWeak(mtx.f._12, 0.0f) &&
            ut::FloatEqualsWeak(mtx.f._20, 0.0f) && ut::FloatEqualsWeak(mtx.f._21, 0.0f) && ut::FloatEqualsWeak(mtx.f._22, 1.0f),
            "The rotation matrix of CalculatedTransform must not contain scale element.\n");
#endif
        this->EnableFlags(FLAG_IS_ROTATE_ZERO);
    }
    else
    {
        this->DisableFlags(FLAG_IS_ROTATE_ZERO);
    }
}

void CalculatedTransform::UpdateTranslateFlags()
{
    if (this->IsEnabledFlags(FLAG_IS_IGNORE_TRANSLATE))
    {
        return;
    }

    const math::MTX34& mtx = this->m_TransformMatrix;
    if (mtx.f._03 == 0.0f && mtx.f._13 == 0.0f && mtx.f._23 == 0.0f)
    {
        this->EnableFlags(FLAG_IS_TRANSLATE_ZERO);
    }
    else
    {
        this->DisableFlags(FLAG_IS_TRANSLATE_ZERO);
    }
}

void CalculatedTransform::UpdateCompositeFlags()
{
    this->DisableFlags(FLAG_IS_ROTATE_TRANSLATE_ZERO | FLAG_IS_IDENTITY);

    if (this->IsEnabledFlags(FLAG_IS_ROTATE_ZERO | FLAG_IS_TRANSLATE_ZERO))
    {
        this->EnableFlags(FLAG_IS_ROTATE_TRANSLATE_ZERO);

        if (this->IsEnabledFlags(FLAG_IS_SCALE_ONE))
        {
            this->EnableFlags(FLAG_IS_IDENTITY);
        }
    }
}

}
}