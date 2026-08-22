#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResParticleAnimationOption.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace adsl {
namespace gfx {
namespace res {

struct ResParticleAnimationData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::Offset toParticleAnimationOption;
    nw::ut::ResS32 mAnimationLength;
    nw::ut::ResS32 mAnimationDimension;
    nw::ut::ResS32 mAnimationStride;
    nw::ut::ResS32 mAnimationEnabledTableCount;
    nw::ut::Offset toAnimationEnabledTable;
    nw::ut::ResS32 mAnimationDataTableCount;
    nw::ut::Offset toAnimationDataTable;
};

class ResParticleAnimation : public nw::ut::ResCommon< ResParticleAnimationData >{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleAnimation) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('ANIM') };

    NW_RES_CTOR( ResParticleAnimation )

    NW_RES_FIELD_CLASS_DECL( ResParticleAnimationOption, ParticleAnimationOption )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, AnimationLength )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, AnimationDimension )
    NW_RES_FIELD_PRIMITIVE_DECL( s32, AnimationStride )
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( bool, AnimationEnabled )
    NW_RES_FIELD_PRIMITIVE_LIST_DECL( f32, AnimationData )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

}
}
}