// Filename: anim_AnimBlend.cpp
//
// Project: NintendoWare4Ctr

#include <nw/anim/anim_AnimBlend.h>
#include <nw/ut/ut_Color.h>

namespace nw {
namespace anim {

bool AnimBlendOpBool::Blend(
    AnimResult* dst, float* /*dstWeights*/,
    const AnimResult* src, const float* /*srcWeights*/) const
{
    bool* dstPtr = reinterpret_cast<bool*>(dst->GetValueBuffer());
    const bool* srcPtr = reinterpret_cast<const bool*>(src->GetValueBuffer());
    if (dst->IsEnabledFlags(VALID_SINGLE))
    {
        *dstPtr = *dstPtr | *srcPtr;
    }
    else
    {
        *dstPtr = *srcPtr;
        dst->EnableFlags(VALID_SINGLE);
    }
    return !*dstPtr;
}

bool AnimBlendOpFloat::Blend(
    AnimResult* dst, float* /*dstWeights*/,
    const AnimResult* src, const float* srcWeights) const
{
    float* dstPtr = reinterpret_cast<float*>(dst->GetValueBuffer());
    const float* srcPtr = reinterpret_cast<const float*>(src->GetValueBuffer());
    const float weightedSrc = srcWeights[0] * (*srcPtr);
    if (dst->IsEnabledFlags(VALID_SINGLE))
    {
        *dstPtr += weightedSrc;
    }
    else
    {
        *dstPtr = weightedSrc;
        dst->EnableFlags(VALID_SINGLE);
    }
    return true;
}

bool AnimBlendOp::BlendFloatVector(
    AnimResult* dst, const AnimResult* src,
    float srcWeight, int compCount) const
{
    float* dstValues = reinterpret_cast<float*>(dst->GetValueBuffer());
    const float* srcValues = reinterpret_cast<const float*>(src->GetValueBuffer());

    bit32 compFlag = VALID_X;
    for (int compIdx = 0; compIdx < compCount; ++compIdx)
    {
        if (src->IsEnabledFlags(compFlag))
        {
            const float weightedSrc = srcWeight * srcValues[compIdx];
            if (dst->IsEnabledFlags(compFlag))
            {
                dstValues[compIdx] += weightedSrc;
            }
            else
            {
                dstValues[compIdx] = weightedSrc;
                dst->EnableFlags(compFlag);
            }
        }
        compFlag <<= 1;
    }
    return true;
}

bool AnimBlendOp::OverrideFloatVector(
    AnimResult* dst, const AnimResult* src,
    int compCount, bit32 allCompValidFlag) const
{
    float* dstValues = reinterpret_cast<float*>(dst->GetValueBuffer());
    const float* srcValues = reinterpret_cast<const float*>(src->GetValueBuffer());
    bit32 dstFlags = dst->GetFlags();
    const bit32 srcFlags = src->GetFlags();

    bit32 compFlag = VALID_X;
    for (int compIdx = 0; compIdx < compCount; ++compIdx)
    {
        if (~dstFlags & srcFlags & compFlag)
        {
            dstValues[compIdx] = srcValues[compIdx];
            dstFlags |= compFlag;
        }
        compFlag <<= 1;
    }
    dst->SetFlags(dstFlags);
    return ((dstFlags & allCompValidFlag) == allCompValidFlag);
}

void AnimBlendOp::ApplyFloatVector(void* target, const AnimResult* result, int compCount) const
{
    float* dstValues = reinterpret_cast<float*>(target);
    const float* srcValues = reinterpret_cast<const float*>(result->GetValueBuffer());

    bit32 compFlag = VALID_X;
    for (int compIdx = 0; compIdx < compCount; ++compIdx)
    {
        if (result->IsEnabledFlags(compFlag))
        {
            dstValues[compIdx] = srcValues[compIdx];
        }
        compFlag <<= 1;
    }
}

void AnimBlendOp::ConvertFloatVectorToAnimResult(
    AnimResult* result, const void* source, int compCount) const
{
    float* dstValues = reinterpret_cast<float*>(result->GetValueBuffer());
    const float* srcValues = reinterpret_cast<const float*>(source);

    result->DisableFlags(VALID_XYZW);

    bit32 compFlag = VALID_X;
    for (int compIdx = 0; compIdx < compCount; ++compIdx)
    {
        result->EnableFlags(compFlag);
        dstValues[compIdx] = srcValues[compIdx];
        compFlag <<= 1;
    }
}

bool AnimBlendOpRgbaColor::Blend(
    AnimResult* dst, float* /*dstWeights*/,
    const AnimResult* src, const float* srcWeights) const
{
    NW_ASSERT(src->GetFlags() & VALID_RGBA);
    float* dstColor = reinterpret_cast<ut::ResFloatColor*>(dst->GetValueBuffer())->ToArray();
    const float* srcColor = reinterpret_cast<const ut::ResFloatColor*>(src->GetValueBuffer())->ToArray();

    bit32 compFlag = VALID_R;
    for (int compIdx = 0; compIdx < COMPONENT_COUNT; ++compIdx)
    {
        if (src->IsEnabledFlags(compFlag))
        {
            const float weightedSrc = srcWeights[0] * srcColor[compIdx];
            if (dst->IsEnabledFlags(compFlag))
            {
                dstColor[compIdx] += weightedSrc;
            }
            else
            {
                dstColor[compIdx] = weightedSrc;
                dst->EnableFlags(compFlag);
            }
        }
        compFlag <<= 1;
    }
    return true;
}

bool AnimBlendOpRgbaColor::Override(AnimResult* dst, const AnimResult* src) const
{
    NW_ASSERT(src->GetFlags() & VALID_RGBA);
    float* dstColor = reinterpret_cast<ut::ResFloatColor*>(dst->GetValueBuffer())->ToArray();
    const float* srcColor = reinterpret_cast<const ut::ResFloatColor*>(src->GetValueBuffer())->ToArray();
    bit32 dstFlags = dst->GetFlags();
    const bit32 srcFlags = src->GetFlags();

    bit32 compFlag = VALID_R;
    for (int compIdx = 0; compIdx < COMPONENT_COUNT; ++compIdx)
    {
        if (~dstFlags & srcFlags & compFlag)
        {
            dstColor[compIdx] = srcColor[compIdx];
            dstFlags |= compFlag;
        }
        compFlag <<= 1;
    }
    dst->SetFlags(dstFlags);
    return ((dstFlags & VALID_RGBA) == VALID_RGBA);
}

void AnimBlendOpRgbaColor::Apply(void* target, const AnimResult* result) const
{
    float* dstColor = reinterpret_cast<ut::ResFloatColor*>(target)->ToArray();
    const float* srcColor = reinterpret_cast<const ut::ResFloatColor*>(result->GetValueBuffer())->ToArray();

    bit32 compFlag = VALID_R;
    for (int compIdx = 0; compIdx < COMPONENT_COUNT; ++compIdx)
    {
        if (result->IsEnabledFlags(compFlag))
        {
            dstColor[compIdx] = srcColor[compIdx];
        }
        compFlag <<= 1;
    }
}

void AnimBlendOpRgbaColor::ConvertToAnimResult(AnimResult* result, const void* source) const
{
    float* dstColor = reinterpret_cast<ut::ResFloatColor*>(result->GetValueBuffer())->ToArray();
    const float* srcColor = reinterpret_cast<const ut::ResFloatColor*>(source)->ToArray();

    result->DisableFlags(VALID_RGBA);

    bit32 compFlag = VALID_R;
    for (int compIdx = 0; compIdx < COMPONENT_COUNT; ++compIdx)
    {
        result->EnableFlags(compFlag);
        dstColor[compIdx] = srcColor[compIdx];
        compFlag <<= 1;
    }
}

} // namespace anim
} // namespace nw