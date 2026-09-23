// Filename: anim_ResAnim.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/res/anim_ResAnim.h>
#include <nw/ut/ut_Color.h>
#include <nw/gfx/res/gfx_ResUtil.h>
#include <nw/gfx/res/gfx_ResGraphicsFile.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw {
namespace anim {
namespace res {

int ResMemberAnim::GetPrimitiveSize() const
{
    switch (GetPrimitiveType())
    {
    case ResMemberAnim::PRIMITIVETYPE_FLOAT:
        return sizeof(float);
    case ResMemberAnim::PRIMITIVETYPE_INT:
        return sizeof(int);
    case ResMemberAnim::PRIMITIVETYPE_BOOL:
        return sizeof(bool);
    case ResMemberAnim::PRIMITIVETYPE_VECTOR2:
        return sizeof(math::VEC2);
    case ResMemberAnim::PRIMITIVETYPE_VECTOR3:
        return sizeof(math::VEC3);
    case ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
        return sizeof(gfx::CalculatedTransform);
    case ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
        return sizeof(ut::ResFloatColor);
    case ResMemberAnim::PRIMITIVETYPE_TEXTURE:
        return sizeof(ut::Offset);
    default:
        NW_ASSERT(false);
    }
    return 0;
}

bit32 ResVec2Anim::EvaluateResult(void* dstBuf, 
    bit32 dstFlags, float frame) const
{
    math::VEC2* dst = reinterpret_cast<math::VEC2*>(dstBuf);
    const bit32 animFlags = GetFlags();
    if (!(animFlags & ResVec2AnimData::FLAG_X_NOT_EXIST))
    {
        dst->x = EvaluateCurveX(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
    }
    if (!(animFlags & ResVec2AnimData::FLAG_Y_NOT_EXIST))
    {
        dst->y = EvaluateCurveY(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT1;
    }
    return dstFlags;
}

void ResVec2Anim::ApplyCache(void* target, const void* cacheBuf) const
{
    math::VEC2* dst = reinterpret_cast<math::VEC2*>(target);
    const math::VEC2* src = reinterpret_cast<const math::VEC2*>(cacheBuf);
    const bit32 animFlags = GetFlags();
    if (!(animFlags & ResVec2AnimData::FLAG_X_NOT_EXIST))
    {
        dst->x = src->x;
    }
    if (!(animFlags & ResVec2AnimData::FLAG_Y_NOT_EXIST))
    {
        dst->y = src->y;
    }
}

bit32 ResVec3Anim::EvaluateResult(void* dstBuf, bit32 dstFlags, float frame) const
{
    math::VEC3* dst = reinterpret_cast<math::VEC3*>(dstBuf);
    const bit32 animFlags = GetFlags();
    if (!(animFlags & ResVec3AnimData::FLAG_X_NOT_EXIST))
    {
        dst->x = EvaluateCurveX(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
    }
    if (!(animFlags & ResVec3AnimData::FLAG_Y_NOT_EXIST))
    {
        dst->y = EvaluateCurveY(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT1;
    }
    if (!(animFlags & ResVec3AnimData::FLAG_Z_NOT_EXIST))
    {
        dst->z = EvaluateCurveZ(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT2;
    }
    return dstFlags;
}

void ResVec3Anim::ApplyCache(void* target, const void* cacheBuf) const
{
    math::VEC3* dst = reinterpret_cast<math::VEC3*>(target);
    const math::VEC3* src = reinterpret_cast<const math::VEC3*>(cacheBuf);
    const bit32 animFlags = GetFlags();
    if (!(animFlags & ResVec3AnimData::FLAG_X_NOT_EXIST))
    {
        dst->x = src->x;
    }
    if (!(animFlags & ResVec3AnimData::FLAG_Y_NOT_EXIST))
    {
        dst->y = src->y;
    }
    if (!(animFlags & ResVec3AnimData::FLAG_Z_NOT_EXIST))
    {
        dst->z = src->z;
    }
}

bit32 ResRgbaColorAnim::EvaluateResult(void* dstBuf, bit32 dstFlags, float frame) const
{
    ut::ResFloatColor* dst = reinterpret_cast<ut::ResFloatColor*>(dstBuf);
    const bit32 animFlags = GetFlags();
    if (!(animFlags & ResRgbaColorAnimData::FLAG_R_NOT_EXIST))
    {
        dst->r = EvaluateCurveR(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
    }
    if (!(animFlags & ResRgbaColorAnimData::FLAG_G_NOT_EXIST))
    {
        dst->g = EvaluateCurveG(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT1;
    }
    if (!(animFlags & ResRgbaColorAnimData::FLAG_B_NOT_EXIST))
    {
        dst->b = EvaluateCurveB(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT2;
    }
    if (!(animFlags & ResRgbaColorAnimData::FLAG_A_NOT_EXIST))
    {
        dst->a = EvaluateCurveA(frame);
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT3;
    }
    return dstFlags;
}

void ResRgbaColorAnim::ApplyCache(void* target, const void* cacheBuf) const
{
    ut::ResFloatColor* dst = reinterpret_cast<ut::ResFloatColor*>(target);
    const ut::ResFloatColor* src = reinterpret_cast<const ut::ResFloatColor*>(cacheBuf);
    const bit32 animFlags = GetFlags();
    if (!(animFlags & ResRgbaColorAnimData::FLAG_R_NOT_EXIST))
    {
        dst->r = src->r;
    }
    if (!(animFlags & ResRgbaColorAnimData::FLAG_G_NOT_EXIST))
    {
        dst->g = src->g;
    }
    if (!(animFlags & ResRgbaColorAnimData::FLAG_B_NOT_EXIST))
    {
        dst->b = src->b;
    }
    if (!(animFlags & ResRgbaColorAnimData::FLAG_A_NOT_EXIST))
    {
        dst->a = src->a;
    }
}

gfx::Result ResTextureAnim::Setup(os::IAllocator* allocator, gfx::ResGraphicsFile graphicsFile)
{
    NW_UNUSED_VARIABLE(allocator);

    gfx::Result result = gfx::RESOURCE_RESULT_OK;

    for (int i = 0; i < GetTexturesCount(); ++i)
    {
        if (!GetTextures(i).GetTargetTexture().IsValid())
        {
            ::std::pair<gfx::ResTexture, bool> target = gfx::GetReferenceTextureTarget(GetTextures(i), graphicsFile);

            if (!target.second)
            {
                result |= gfx::Result::MASK_FAIL_BIT;
                result |= gfx::RESOURCE_RESULT_NOT_FOUND_TEXTURE;
            }
        }
    }

    return result;
}

void ResTextureAnim::Cleanup()
{
}

bit32 ResTextureAnim::EvaluateResult(void* dstBuf, bit32 dstFlags, float frame) const
{
    ut::Offset* dst = reinterpret_cast<ut::Offset*>(dstBuf);
    s32 textureIndex = (s32)EvaluateCurve(frame);

    if (0 <= textureIndex && textureIndex < GetTexturesCount())
    {
        dst->set_ptr(GetTextures(textureIndex).ptr());
        dstFlags |= AnimResult::FLAG_VALID_COMPONENT0;
    }

    return dstFlags;
}

void ResTextureAnim::ApplyCache(void* target, const void* cacheBuf) const
{
    ut::Offset* dst = reinterpret_cast<ut::Offset*>(target);
    const ut::Offset* src = reinterpret_cast<const ut::Offset*>(cacheBuf);

    dst->set_ptr(src->to_ptr());
}

bit32 ResTransformAnim::EvaluateResult(void* dstBuf, bit32 dstFlags, float frame,
    const void* originalValue) const
{
    const math::Transform3& originalTransform =
        *static_cast<const math::Transform3*>(originalValue);

    const u32 flags = GetFlags();
    math::Transform3 transform;

    transform.scale.x = (flags & ResTransformAnimData::FLAG_SCALE_X_NOT_EXIST) ?
        originalTransform.scale.x : EvaluateScaleX(frame);
    transform.scale.y = (flags & ResTransformAnimData::FLAG_SCALE_Y_NOT_EXIST) ?
        originalTransform.scale.y : EvaluateScaleY(frame);
    transform.scale.z = (flags & ResTransformAnimData::FLAG_SCALE_Z_NOT_EXIST) ?
        originalTransform.scale.z : EvaluateScaleZ(frame);

    transform.rotate.x = (flags & ResTransformAnimData::FLAG_ROTATE_X_NOT_EXIST) ?
        originalTransform.rotate.x : EvaluateRotateX(frame);
    transform.rotate.y = (flags & ResTransformAnimData::FLAG_ROTATE_Y_NOT_EXIST) ?
        originalTransform.rotate.y : EvaluateRotateY(frame);
    transform.rotate.z = (flags & ResTransformAnimData::FLAG_ROTATE_Z_NOT_EXIST) ?
        originalTransform.rotate.z : EvaluateRotateZ(frame);

    transform.translate.x = (flags & ResTransformAnimData::FLAG_TRANSLATE_X_NOT_EXIST) ?
        originalTransform.translate.x : EvaluateTranslateX(frame);
    transform.translate.y = (flags & ResTransformAnimData::FLAG_TRANSLATE_Y_NOT_EXIST) ?
        originalTransform.translate.y : EvaluateTranslateY(frame);
    transform.translate.z = (flags & ResTransformAnimData::FLAG_TRANSLATE_Z_NOT_EXIST) ?
        originalTransform.translate.z : EvaluateTranslateZ(frame);

    gfx::CalculatedTransform* dst = new (dstBuf) gfx::CalculatedTransform();
    dst->SetTransform(transform);
    dst->UpdateScaleFlags();
    dst->UpdateRotateFlags();
    dst->UpdateTranslateFlags();
    dst->UpdateCompositeFlags();

    return dstFlags;
}

void ResTransformAnim::ApplyCache(void* target, const void* cacheBuf) const
{
    gfx::CalculatedTransform* dst = reinterpret_cast<gfx::CalculatedTransform*>(target);
    const gfx::CalculatedTransform* src = reinterpret_cast<const gfx::CalculatedTransform*>(cacheBuf);

    *dst = *src;
}

void ResBakedTransformAnim::ApplyBakedFlags(gfx::CalculatedTransform* transform, const bit32 flags)
{
    static const bit32 CLEAR_FLAGS = (0 |
        gfx::CalculatedTransform::FLAG_IS_IDENTITY |
        gfx::CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO |
        gfx::CalculatedTransform::FLAG_IS_ROTATE_ZERO |
        gfx::CalculatedTransform::FLAG_IS_TRANSLATE_ZERO |
        gfx::CalculatedTransform::FLAG_IS_SCALE_ONE |
        gfx::CalculatedTransform::FLAG_IS_UNIFORM_SCALE
    );

    static const bit32 CLEAR_COMPOSITE_FLAGS = (0 |
        gfx::CalculatedTransform::FLAG_IS_IDENTITY |
        gfx::CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO
        );

    bit32 originalFlags = transform->GetFlags() & ~CLEAR_FLAGS;

    bit32 f = flags & ~CLEAR_COMPOSITE_FLAGS;

    if (ut::CheckFlag(flags,
        gfx::CalculatedTransform::FLAG_IS_ROTATE_ZERO | gfx::CalculatedTransform::FLAG_IS_TRANSLATE_ZERO))
    {
        f |= gfx::CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO;
    }
    
    if (ut::CheckFlag(flags,
        gfx::CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO | gfx::CalculatedTransform::FLAG_IS_SCALE_ONE))
    {
        f |= gfx::CalculatedTransform::FLAG_IS_IDENTITY;
    }

    transform->SetFlags(originalFlags | f);
}

gfx::Result ResMemberAnim::Setup(os::IAllocator* allocator, gfx::ResGraphicsFile graphicsFile)
{
    switch (GetPrimitiveType())
    {
    case ResMemberAnim::PRIMITIVETYPE_TEXTURE:
        return static_cast<ResTextureAnim&>(*this).Setup(allocator, graphicsFile);
    }

    return gfx::RESOURCE_RESULT_OK;
}

void ResMemberAnim::Cleanup()
{
    switch (GetPrimitiveType())
    {
    case ResMemberAnim::PRIMITIVETYPE_TEXTURE:
        static_cast<ResTextureAnim&>(*this).Cleanup();
        break;
    }
}

bit32 ResMemberAnim::EvaluateResultForType(void* dstBuf, bit32 dstFlags,
    float frame, const void* originalValue) const
{
    const int primType = GetPrimitiveType();
    switch (primType)
    {
    case ResMemberAnim::PRIMITIVETYPE_FLOAT:
        dstFlags = static_cast<const ResFloatAnim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    case ResMemberAnim::PRIMITIVETYPE_INT:
        dstFlags = static_cast<const ResIntAnim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    case ResMemberAnim::PRIMITIVETYPE_BOOL:
        dstFlags = static_cast<const ResBoolAnim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    case ResMemberAnim::PRIMITIVETYPE_VECTOR2:
        dstFlags = static_cast<const ResVec2Anim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    case ResMemberAnim::PRIMITIVETYPE_VECTOR3:
        dstFlags = static_cast<const ResVec3Anim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    case ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
        dstFlags = static_cast<const ResTransformAnim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame, originalValue);
        break;
    case ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
        dstFlags = static_cast<const ResRgbaColorAnim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    case ResMemberAnim::PRIMITIVETYPE_TEXTURE:
        dstFlags = static_cast<const ResTextureAnim&>(*this)
            .EvaluateResult(dstBuf, dstFlags, frame);
        break;
    default:
        NW_ASSERT(false);
        break;
    }
    return dstFlags;
}

void ResMemberAnim::ApplyCacheForType(void* target,const void* cacheBuf) const
{
    const int primType = GetPrimitiveType();
    switch (primType)
    {
    case ResMemberAnim::PRIMITIVETYPE_FLOAT:
        static_cast<const ResFloatAnim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_INT:
        static_cast<const ResIntAnim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_BOOL:
        static_cast<const ResBoolAnim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_VECTOR2:
        static_cast<const ResVec2Anim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_VECTOR3:
        static_cast<const ResVec3Anim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_TRANSFORM:
        static_cast<const ResTransformAnim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_RGBA_COLOR:
        static_cast<const ResRgbaColorAnim&>(*this).ApplyCache(target, cacheBuf);
        break;
    case ResMemberAnim::PRIMITIVETYPE_TEXTURE:
        static_cast<const ResTextureAnim&>(*this).ApplyCache(target, cacheBuf);
        break;
    default:
        NW_ASSERT(false);
        break;
    }
}

gfx::Result ResAnim::Setup(os::IAllocator* allocator, gfx::ResGraphicsFile graphicsFile)
{
    gfx::Result result = gfx::RESOURCE_RESULT_OK;

    for(int i = 0; i < GetMemberAnimSetCount(); i++)
    {
        ResMemberAnim member = GetMemberAnimSet(i);
        result |= member.Setup(allocator, graphicsFile);
    }

    return result;
}

void ResAnim::Cleanup()
{
    for(int i = 0; i < GetMemberAnimSetCount(); i++)
    {
        ResMemberAnim member = GetMemberAnimSet(i);
        member.Cleanup();
    }
}

ResAnim ResAnim::CreateEmptyResAnim(nw::os::IAllocator* allocator, const char* targetAnimGroupName)
{
    size_t size = sizeof(ResAnimData);
    size += sizeof(ut::ResDicPatriciaData);
    
    u8* memory = reinterpret_cast<u8*>( allocator->Alloc(size) );
    
    ResAnimData* data = reinterpret_cast<ResAnimData*>(memory);
    memory += sizeof(ResAnimData);
    ut::ResDicPatriciaData* dicData = reinterpret_cast<ut::ResDicPatriciaData*>(memory);

    data->m_Header.revision = ResAnim::BINARY_REVISION;
    data->m_Header.signature = ResAnim::SIGNATURE;
    data->toName.set_ptr("");
    data->toTargetAnimGroupName.set_ptr(targetAnimGroupName);
    data->m_LoopMode = ResAnimData::LOOP_MODE_ONETIME;
    data->m_FrameSize = 1;
    data->m_MemberAnimSetDicCount = 0;
    data->toMemberAnimSetDic.set_ptr(dicData);
    data->m_UserDataDicCount = 0;
    data->toUserDataDic.set_ptr(dicData);

    ut::internal::InitializeResDicPatricia(dicData);

    return ResAnim(data);
}

bool ResAnim::IsFullBakedAnim() const
{
    if (GetMemberAnimSetCount() == 0)
    {
        return false;
    }

    return GetMemberAnimSet(0).GetPrimitiveType() == ResMemberAnim::PRIMITIVETYPE_FULL_BAKED;
}

} // namespace res
} // namespace anim
} // namespace nw