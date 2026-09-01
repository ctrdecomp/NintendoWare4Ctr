#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/math/math_ResTypes.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResBoneData{
    enum Flag{
        FLAG_IS_IDENTITY                       = 0x1 << 0,
        FLAG_IS_TRANSLATE_ZERO                 = 0x1 << 1,
        FLAG_IS_ROTATE_ZERO                    = 0x1 << 2,
        FLAG_IS_SCALE_ONE                      = 0x1 << 3,
        FLAG_IS_UNIFORM_SCALE                  = 0x1 << 4,
        FLAG_IS_SEGMENTSCALE_COMPENSATE     = 0x1 << 5,
        FLAG_IS_NEED_RENDERING              = 0x1 << 6,
        FLAG_IS_LOCAL_MATRIX_CALCULATE      = 0x1 << 7,
        FLAG_IS_WORLD_MATRIX_CALCULATE      = 0x1 << 8,
        FLAG_HAS_SKINNING_MATRIX            = 0x1 << 9
    };
    
    nw::ut::BinString toName;
    nw::ut::ResU32 mFlags;
    nw::ut::ResS32 mIndex;
    nw::ut::ResS32 mParentBoneIndex;
    nw::ut::Offset toParentBone;
    nw::ut::Offset toChildBone;
    nw::ut::Offset toPrevSibling;
    nw::ut::Offset toNextSibling;
    nw::math::Transform3 mTransform;
    nw::ut::ResMtx34 mLocalMatrix;
    nw::ut::ResMtx34 mWorldMatrix;
    nw::ut::ResMtx34 mInverseBaseMatrix;
    nw::ut::ResS32 mBillboardMode;
    nw::ut::ResS32 mUserDataDicCount;
    nw::ut::Offset toUserDataDic;
};

struct ResSkeletonData : public ResSceneObjectData{
    enum Flag{
        FLAG_MODEL_COORDINATE                       = 0x1 << 0,
        FLAG_TRANSLATE_ANIMATION_ENABLED            = 0x1 << 1
    };
    
    nw::ut::ResS32 mBonesDicCount;
    nw::ut::Offset toBonesDic;
    nw::ut::Offset toRootBone;
    nw::ut::ResS32 mScalingRule;
    nw::ut::ResU32 mFlags;
};

class ResBone : public nw::ut::ResCommon< ResBoneData >{
public:

    enum BillboardMode{
        BILLBOARD_MODE_OFF,
        BILLBOARD_MODE_WORLD,
        BILLBOARD_MODE_WORLD_VIEWPOINT,
        BILLBOARD_MODE_SCREEN,
        BILLBOARD_MODE_SCREEN_VIEWPOINT,
        BILLBOARD_MODE_Y_AXIAL,
        BILLBOARD_MODE_Y_AXIAL_VIEWPOINT
    };

    NW_RES_CTOR(ResBone)

    NW_RES_FIELD_STRING_DECL(Name)
    NW_RES_FIELD_FLAGS_DECL(u32, Flags)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, Index)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, ParentBoneIndex)
    NW_RES_FIELD_CLASS_DECL(ResBone, ParentBone)
    NW_RES_FIELD_CLASS_DECL(ResBone, ChildBone)
    NW_RES_FIELD_CLASS_DECL(ResBone, PrevSibling)
    NW_RES_FIELD_CLASS_DECL(ResBone, NextSibling)
    NW_RES_FIELD_STRUCT_DECL(nw::math::Transform3, Transform)
    NW_RES_FIELD_STRUCT_DECL(nw::math::MTX34, LocalMatrix)
    NW_RES_FIELD_STRUCT_DECL(nw::math::MTX34, WorldMatrix)
    NW_RES_FIELD_STRUCT_DECL(nw::math::MTX34, InverseBaseMatrix)
    NW_RES_FIELD_PRIMITIVE_DECL(BillboardMode, BillboardMode)

    NW_RES_FIELD_CLASS_DIC_DECL(nw::ut::ResMetaData, UserData, nw::ut::ResDicPatricia)
};
typedef nw::ut::ResArrayPatricia<ResBone>::type  ResBoneArray;

class ResSkeleton : public ResSceneObject{
public:
    enum Flag{
        FLAG_MODEL_COORDINATE                       = 0x1 << 0,
        FLAG_TRANSLATE_ANIMATION_ENABLED            = 0x1 << 1
    };

    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSkeleton) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CSKL') };
    
    NW_RES_CTOR_INHERIT(ResSkeleton, ResSceneObject)

    NW_RES_FIELD_CLASS_DIC_DECL(ResBone, Bones, nw::ut::ResDicPatricia)
    NW_RES_FIELD_CLASS_DECL(ResBone, RootBone)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, ScalingRule)
    NW_RES_FIELD_FLAGS_DECL(u32, Flags)
};

}
}
}