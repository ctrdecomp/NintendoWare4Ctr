// Filename: gfx_TransformAnimEvalulator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/anim_AnimBlend.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/gfx/gfx_TransformAnim.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(TransformAnimEvaluator, BaseAnimEvaluator);

void TransformAnimEvaluator::EvaluateMemberAnim(CalculatedTransform* result,anim::ResTransformAnim transformAnim,float frame,const math::Transform3* originalTransform,bool writeNoAnimMember) const
{
    const u32 flags = transformAnim.GetFlags();
    
    const u32 FLAG_TRANSLATE_NOT_EXIST = anim::ResTransformAnimData::FLAG_TRANSLATE_NOT_EXIST;
    const u32 FLAG_ROTATE_NOT_EXIST = anim::ResTransformAnimData::FLAG_ROTATE_NOT_EXIST;
    const u32 FLAG_SCALE_NOT_EXIST = anim::ResTransformAnimData::FLAG_SCALE_NOT_EXIST;

    if (writeNoAnimMember)
    {
        math::Transform3 transform;

        if (m_IsScaleDisabled)
        {
            transform.scale = originalTransform->scale;
        }
        else{
            transform.scale.x = (flags & anim::ResTransformAnimData::FLAG_SCALE_X_NOT_EXIST) ? originalTransform->scale.x : transformAnim.EvaluateScaleX(frame);
            transform.scale.y = (flags & anim::ResTransformAnimData::FLAG_SCALE_Y_NOT_EXIST) ? originalTransform->scale.y : transformAnim.EvaluateScaleY(frame);
            transform.scale.z = (flags & anim::ResTransformAnimData::FLAG_SCALE_Z_NOT_EXIST) ? originalTransform->scale.z : transformAnim.EvaluateScaleZ(frame);
        }

        if (m_IsRotateDisabled)
        {
            transform.rotate = originalTransform->rotate;
        }
        else{
            transform.rotate.x = (flags & anim::ResTransformAnimData::FLAG_ROTATE_X_NOT_EXIST) ? originalTransform->rotate.x : transformAnim.EvaluateRotateX(frame);
            transform.rotate.y = (flags & anim::ResTransformAnimData::FLAG_ROTATE_Y_NOT_EXIST) ? originalTransform->rotate.y : transformAnim.EvaluateRotateY(frame);
            transform.rotate.z = (flags & anim::ResTransformAnimData::FLAG_ROTATE_Z_NOT_EXIST) ? originalTransform->rotate.z : transformAnim.EvaluateRotateZ(frame);
        }

        if (m_IsTranslateDisabled)
        {
            transform.translate = originalTransform->translate;
        }
        else{
            transform.translate.x = (flags & anim::ResTransformAnimData::FLAG_TRANSLATE_X_NOT_EXIST) ? originalTransform->translate.x : transformAnim.EvaluateTranslateX(frame);
            transform.translate.y = (flags & anim::ResTransformAnimData::FLAG_TRANSLATE_Y_NOT_EXIST) ? originalTransform->translate.y : transformAnim.EvaluateTranslateY(frame);
            transform.translate.z = (flags & anim::ResTransformAnimData::FLAG_TRANSLATE_Z_NOT_EXIST) ? originalTransform->translate.z : transformAnim.EvaluateTranslateZ(frame);
        }

        result->SetTransform(transform);
    }
    else{
        if (!m_IsScaleDisabled)
        {
            if (!(flags & anim::ResTransformAnimData::FLAG_SCALE_X_NOT_EXIST))
            {
                result->DirectScale().x = transformAnim.EvaluateScaleX(frame);
            }
            if (!(flags & anim::ResTransformAnimData::FLAG_SCALE_Y_NOT_EXIST))
            {
                result->DirectScale().y = transformAnim.EvaluateScaleY(frame);
            }
            if (!(flags & anim::ResTransformAnimData::FLAG_SCALE_Z_NOT_EXIST))
            {
                result->DirectScale().z = transformAnim.EvaluateScaleZ(frame);
            }
        }

        if (!m_IsRotateDisabled)
        {

            if ((flags & FLAG_ROTATE_NOT_EXIST) != FLAG_ROTATE_NOT_EXIST)
            {
                const f32 rotX = (flags & anim::ResTransformAnimData::FLAG_ROTATE_X_NOT_EXIST) ? originalTransform->rotate.x : transformAnim.EvaluateRotateX(frame);
                const f32 rotY = (flags & anim::ResTransformAnimData::FLAG_ROTATE_Y_NOT_EXIST) ? originalTransform->rotate.y : transformAnim.EvaluateRotateY(frame);
                const f32 rotZ = (flags & anim::ResTransformAnimData::FLAG_ROTATE_Z_NOT_EXIST) ? originalTransform->rotate.z : transformAnim.EvaluateRotateZ(frame);
                result->SetRotateXYZ(rotX, rotY, rotZ);
            }
        }

        if (!m_IsTranslateDisabled)
        {
            if (!(flags & anim::ResTransformAnimData::FLAG_TRANSLATE_X_NOT_EXIST))
            {
                result->DirectTransformMatrix().f._03 = transformAnim.EvaluateTranslateX(frame);
            }
            if (!(flags & anim::ResTransformAnimData::FLAG_TRANSLATE_Y_NOT_EXIST))
            {
                result->DirectTransformMatrix().f._13 = transformAnim.EvaluateTranslateY(frame);
            }
            if (!(flags & anim::ResTransformAnimData::FLAG_TRANSLATE_Z_NOT_EXIST))
            {
                result->DirectTransformMatrix().f._23 = transformAnim.EvaluateTranslateZ(frame);
            }
        }

        result->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }
    UpdateFlagsCommon(result);

    if ((flags & FLAG_ROTATE_NOT_EXIST) != FLAG_ROTATE_NOT_EXIST)
    {
        result->DisableFlags(CalculatedTransform::FLAG_IS_ROTATE_ZERO);
    }

    if ((flags & FLAG_TRANSLATE_NOT_EXIST) != FLAG_TRANSLATE_NOT_EXIST)
    {
        result->DisableFlags(CalculatedTransform::FLAG_IS_TRANSLATE_ZERO);
    }

    if ((flags & FLAG_SCALE_NOT_EXIST) != FLAG_SCALE_NOT_EXIST)
    {
        result->DisableFlags(CalculatedTransform::FLAG_IS_SCALE_ONE);
    }
    result->UpdateCompositeFlags();
}

void TransformAnimEvaluator::EvaluateMemberBakedAnim(CalculatedTransform* result,anim::ResBakedTransformAnim transformAnim,float frame,const math::Transform3* originalTransform,bool writeNoAnimMember) const
{
    const u32 flags = transformAnim.GetFlags();
    bit32 frameFlags = 0;

    if (!writeNoAnimMember)
    {
        frameFlags = result->GetFlags();
    }

    if (!(flags & anim::ResBakedTransformAnimData::FLAG_ROTATE_NOT_EXIST))
    {
        frameFlags = ut::DisableFlag(frameFlags, gfx::CalculatedTransform::FLAG_IS_ROTATE_ZERO);

        transformAnim.EvaluateRotate(&result->m_TransformMatrix, &frameFlags, frame);
        result->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    if (!(flags & anim::ResBakedTransformAnimData::FLAG_TRANSLATE_NOT_EXIST))
    {
        frameFlags = ut::DisableFlag(frameFlags,
            gfx::CalculatedTransform::FLAG_IS_TRANSLATE_ZERO);

        transformAnim.EvaluateTranslate(&result->m_TransformMatrix, &frameFlags, frame);
        result->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    if (!(flags & anim::ResBakedTransformAnimData::FLAG_SCALE_NOT_EXIST))
    {
        frameFlags = ut::DisableFlag(frameFlags, gfx::CalculatedTransform::FLAG_IS_UNIFORM_SCALE | gfx::CalculatedTransform::FLAG_IS_SCALE_ONE);

        transformAnim.EvaluateScale(&result->m_Scale, &frameFlags, frame);
        result->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    }

    if (writeNoAnimMember)
    {
        if (flags & anim::ResBakedTransformAnimData::FLAG_ROTATE_NOT_EXIST)
        {
            result->SetRotateXYZ(originalTransform->rotate.x, originalTransform->rotate.y, originalTransform->rotate.z);

            if (originalTransform->rotate.IsZero())
            {
                frameFlags |= gfx::CalculatedTransform::FLAG_IS_ROTATE_ZERO;
            }
        }

        if (flags & anim::ResBakedTransformAnimData::FLAG_TRANSLATE_NOT_EXIST)
        {
            result->SetTranslate(originalTransform->translate);

            if (originalTransform->translate.IsZero())
            {
                frameFlags |= gfx::CalculatedTransform::FLAG_IS_TRANSLATE_ZERO;
            }
        }

        if (flags & anim::ResBakedTransformAnimData::FLAG_SCALE_NOT_EXIST)
        {
            result->SetScale(originalTransform->scale);

            const math::VEC3& scale = originalTransform->scale;
            if (scale.x == scale.y && scale.x == scale.z)
            {
                frameFlags |= gfx::CalculatedTransform::FLAG_IS_UNIFORM_SCALE;

                if (scale.x == 1.f)
                {
                    frameFlags |= gfx::CalculatedTransform::FLAG_IS_SCALE_ONE;
                }
            }
        }
    }

    ApplyBakedFlags(result, frameFlags);
}

void TransformAnimEvaluator::UpdateFlagsCommon(CalculatedTransform* transform) const
{
    transform->DisableFlags(CalculatedTransform::FLAG_CONVERTED_FOR_BLEND);

    transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_SCALE, this->m_IsScaleDisabled);
    transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ROTATE, this->m_IsRotateDisabled);
    transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_TRANSLATE, this->m_IsTranslateDisabled);
}

void TransformAnimEvaluator::UpdateFlags(CalculatedTransform* transform) const
{
    UpdateFlagsCommon(transform);
    transform->UpdateScaleFlags();
    transform->UpdateRotateFlags();
    transform->UpdateTranslateFlags();
    transform->UpdateCompositeFlags();
}

void TransformAnimEvaluator::ApplyBakedFlags(CalculatedTransform* transform, bit32 flags) const
{
    UpdateFlagsCommon(transform);
    anim::ResBakedTransformAnim::ApplyBakedFlags(transform, flags);
}

const anim::AnimResult* TransformAnimEvaluator::GetResult(void* target,int memberIdx) const
{
    return this->GetResultCommon(target, memberIdx, true);
}

void TransformAnimEvaluator::ResetNoAnimMember(AnimGroup* animGroup, anim::ResAnim animData)
{
    using namespace anim;

    for (int memberIdx = 0; memberIdx < animGroup->GetMemberCount(); ++memberIdx)
    {
        const int animIdx = m_BindIndexTable[memberIdx];
        if (animIdx == NotFoundIndex) { continue; }

        ResMemberAnim memberAnim = animData.GetMemberAnimSet(animIdx);
        switch (memberAnim.GetPrimitiveType())
        {
        case ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
        case ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM:{
                u32 rotateAndTranslateMask;
                u32 scaleMask;
                if (memberAnim.GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_TRANSFORM)
                {
                    rotateAndTranslateMask = ResTransformAnimData::FLAG_ROTATE_NOT_EXIST | ResTransformAnimData::FLAG_TRANSLATE_NOT_EXIST;
                    scaleMask = ResTransformAnimData::FLAG_SCALE_NOT_EXIST;
                }
                else{
                    rotateAndTranslateMask = ResBakedTransformAnimData::FLAG_ROTATE_NOT_EXIST | ResBakedTransformAnimData::FLAG_TRANSLATE_NOT_EXIST;
                    scaleMask = ResBakedTransformAnimData::FLAG_SCALE_NOT_EXIST;
                }
                
                CalculatedTransform* target = static_cast<CalculatedTransform*>(animGroup->GetTargetPtr(memberIdx));

                const math::Transform3* originalValue = static_cast<const math::Transform3*>(animGroup->GetOriginalValue(memberIdx));

                const u32 flags = memberAnim.GetFlags();
                bool modified = false;

                if (flags & rotateAndTranslateMask)
                {
                    target->SetRotateAndTranslate(originalValue->rotate, originalValue->translate);
                    modified = true;
                }

                if (flags & scaleMask)
                {
                    target->SetScale(originalValue->scale);
                    modified = true;
                }

                if (modified)
                {
                    UpdateFlags(target);

                    if (!m_CacheTransforms.Empty())
                    {
                        m_CacheTransforms[animIdx] = *target;
                    }
                }
            }
            break;
        case ResMemberAnim::PRIMITIVETYPE_FULL_BAKED:
            break;
        default:
            NW_ASSERT(false);
        }
    }
}

const anim::AnimResult* TransformAnimEvaluator::GetResultFast(void* target, int memberIdx) const
{
    return this->GetResultCommon(target, memberIdx, false);
}

const anim::AnimResult* TransformAnimEvaluator::GetResultCommon(void* target, int memberIdx, bool writeNoAnimMember) const
{
    using namespace anim;

    if (!HasMemberAnim(memberIdx))
    {
        return NULL;
    }

    CalculatedTransform* transform = reinterpret_cast<CalculatedTransform*>(target);
    if (!this->m_CacheTransforms.Empty() && !m_IsCacheDirty && this->m_AnimData.ptr() != NULL)
    {
        const int animIdx = m_BindIndexTable[memberIdx];
        *transform = m_CacheTransforms[animIdx];
    }
    else{
        const math::Transform3* originalValue =
            static_cast<const math::Transform3*>(this->m_AnimGroup->GetOriginalValue(memberIdx));
        if (this->m_AnimData.ptr() != NULL)
        {
            const int animIdx = m_BindIndexTable[memberIdx];
            ResMemberAnim memberAnim = this->m_AnimData.GetMemberAnimSet(animIdx);

            switch (memberAnim.GetPrimitiveType())
            {
            case ResMemberAnim::PRIMITIVETYPE_TRANSFORM:{
                    ResTransformAnim transformAnim = static_cast<ResTransformAnim>(memberAnim);
                    EvaluateMemberAnim(transform, transformAnim, this->m_AnimFrameController.GetFrame(), originalValue, writeNoAnimMember);
                }
                break;
            case ResMemberAnim::PRIMITIVETYPE_BAKED_TRANSFORM:{
                    ResBakedTransformAnim transformAnim = static_cast<ResBakedTransformAnim>(memberAnim);
                    EvaluateMemberBakedAnim(transform, transformAnim, this->m_AnimFrameController.GetFrame(), originalValue, writeNoAnimMember);
                }
                break;
            case ResMemberAnim::PRIMITIVETYPE_FULL_BAKED:{
                    ResFullBakedAnim transformAnim = static_cast<ResFullBakedAnim>(memberAnim);
                    math::MTX34* worldMatrix = reinterpret_cast<math::MTX34*>(target);

                    transformAnim.EvaluateTransform(worldMatrix, this->m_AnimFrameController.GetFrame());
                }
                break;
            default:
                NW_ASSERT(false);
                break;
            }
        }
        else{
            transform->SetTransform(*originalValue);
            UpdateFlags(transform);
        }
    }

    return reinterpret_cast<AnimResult*>(target);
}

Result TransformAnimEvaluator::TryBind(AnimGroup* animGroup)
{
    Result result;
    NW_NULL_ASSERT(m_AnimData.ptr());

    result = BaseAnimEvaluator::TryBind(animGroup);
    if (result.IsFailure())
    {
        return result;
    }

    ResetNoAnimMember(animGroup, this->m_AnimData);
    m_AnimGroup = animGroup;

    return result;
}

}
}