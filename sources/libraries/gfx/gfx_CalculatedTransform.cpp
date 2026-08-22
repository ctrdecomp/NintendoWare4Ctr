#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

namespace nw{
namespace gfx{

const f32 CalculatedTransform::sVecSquareLenTol = 1.0e-6f;

void CalculatedTransform::Setup(const ResBone bone){
    bool translateZeroFlag = false;
    bool rotateZeroFlag = false;

    this->SetTransform(bone.GetTransform());
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_IDENTITY) ){
        this->EnableFlags(CalculatedTransform::FLAG_IS_IDENTITY);
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_TRANSLATE_ZERO) ){
        this->EnableFlags(CalculatedTransform::FLAG_IS_TRANSLATE_ZERO);
        translateZeroFlag = true;
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_ROTATE_ZERO) ){
        this->EnableFlags(CalculatedTransform::FLAG_IS_ROTATE_ZERO);
        rotateZeroFlag = true;
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_SCALE_ONE) ){
        this->EnableFlags(CalculatedTransform::FLAG_IS_SCALE_ONE);
    }
    if (ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_UNIFORM_SCALE) ){
        this->EnableFlags(CalculatedTransform::FLAG_IS_UNIFORM_SCALE);
    }
    if (translateZeroFlag && rotateZeroFlag){
        this->EnableFlags(CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO);
    }
}

void CalculatedTransform::UpdateFlagsStrictly(){
    this->UpdateScaleFlags();
    this->UpdateRotateFlagsStrictly();
    this->UpdateTranslateFlags();
    this->UpdateCompositeFlags();
}

void CalculatedTransform::UpdateFlags(){
    this->UpdateScaleFlags();
    this->UpdateRotateFlags();
    this->UpdateTranslateFlags();
    this->UpdateCompositeFlags();
}

void CalculatedTransform::UpdateRotateFlagsStrictly(){
    if (this->IsEnabledFlags(FLAG_IS_IGNORE_ROTATE)){
        return;
    }

    const math::MTX34& mtx = this->mTransformMatrix;
    if (mtx.matrix[0][0] == 1.0f && mtx.matrix[0][1] == 0.0f && mtx.matrix[0][2] == 0.0f &&
        mtx.matrix[1][0] == 0.0f && mtx.matrix[1][1] == 1.0f && mtx.matrix[1][2] == 0.0f &&
        mtx.matrix[2][0] == 0.0f && mtx.matrix[2][1] == 0.0f && mtx.matrix[2][2] == 1.0f){
        this->EnableFlags(FLAG_IS_ROTATE_ZERO);
    }
    else{
        this->DisableFlags(FLAG_IS_ROTATE_ZERO);
    }
}

void CalculatedTransform::UpdateRotateFlags(){
    if (this->IsEnabledFlags(FLAG_IS_IGNORE_ROTATE)){
        return;
    }

    const math::MTX34& mtx = this->mTransformMatrix;
    if (mtx.matrix[0][0] == 1.0f && mtx.matrix[1][1] == 1.0f){
#if 0   
        NW_WARNING(
            ut::FloatEqualsWeak(mtx.f._00, 1.0f) && ut::FloatEqualsWeak(mtx.f._01, 0.0f) && ut::FloatEqualsWeak(mtx.f._02, 0.0f) &&
            ut::FloatEqualsWeak(mtx.f._10, 0.0f) && ut::FloatEqualsWeak(mtx.f._11, 1.0f) && ut::FloatEqualsWeak(mtx.f._12, 0.0f) &&
            ut::FloatEqualsWeak(mtx.f._20, 0.0f) && ut::FloatEqualsWeak(mtx.f._21, 0.0f) && ut::FloatEqualsWeak(mtx.f._22, 1.0f),
            "The rotation matrix of CalculatedTransform must not contain scale element.\n");
#endif
        this->EnableFlags(FLAG_IS_ROTATE_ZERO);
    }
    else{
        this->DisableFlags(FLAG_IS_ROTATE_ZERO);
    }
}

void CalculatedTransform::UpdateTranslateFlags(){
    if (this->IsEnabledFlags(FLAG_IS_IGNORE_TRANSLATE)){
        return;
    }

    const math::MTX34& mtx = this->mTransformMatrix;
    if (mtx.matrix[0][3] == 0.0f && mtx.matrix[1][3] == 0.0f && mtx.matrix[2][3] == 0.0f){
        this->EnableFlags(FLAG_IS_TRANSLATE_ZERO);
    }
    else{
        this->DisableFlags(FLAG_IS_TRANSLATE_ZERO);
    }
}

void CalculatedTransform::UpdateCompositeFlags(){
    this->DisableFlags(FLAG_IS_ROTATE_TRANSLATE_ZERO | FLAG_IS_IDENTITY);

    if (this->IsEnabledFlags(FLAG_IS_ROTATE_ZERO | FLAG_IS_TRANSLATE_ZERO)){
        this->EnableFlags(FLAG_IS_ROTATE_TRANSLATE_ZERO);

        if (this->IsEnabledFlags(FLAG_IS_SCALE_ONE)){
            this->EnableFlags(FLAG_IS_IDENTITY);
        }
    }
}

}
}