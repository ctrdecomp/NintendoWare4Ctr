#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/math/math_ResTypes.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResBoneData
{
    enum Flag
    {
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
    nw::ut::ResU32 m_Flags;
    nw::ut::ResS32 m_Index;
    nw::ut::ResS32 m_ParentBoneIndex;
    nw::ut::Offset toParentBone;
    nw::ut::Offset toChildBone;
    nw::ut::Offset toPrevSibling;
    nw::ut::Offset toNextSibling;
    nw::math::Transform3 m_Transform;
    nw::ut::ResMtx34 m_LocalMatrix;
    nw::ut::ResMtx34 m_WorldMatrix;
    nw::ut::ResMtx34 m_InverseBaseMatrix;
    nw::ut::ResS32 m_BillboardMode;
    nw::ut::ResS32 m_UserDataDicCount;
    nw::ut::Offset toUserDataDic;
};

struct ResSkeletonData : public ResSceneObjectData
{
    enum Flag
    {
        FLAG_MODEL_COORDINATE                       = 0x1 << 0,
        FLAG_TRANSLATE_ANIMATION_ENABLED            = 0x1 << 1
    };
    
    nw::ut::ResS32 m_BonesDicCount;
    nw::ut::Offset toBonesDic;
    nw::ut::Offset toRootBone;
    nw::ut::ResS32 m_ScalingRule;
    nw::ut::ResU32 m_Flags;
};

class ResBone : public nw::ut::ResCommon< ResBoneData >
{
public:

    enum BillboardMode
    {
        BILLBOARD_MODE_OFF,
        BILLBOARD_MODE_WORLD,
        BILLBOARD_MODE_WORLD_VIEWPOINT,
        BILLBOARD_MODE_SCREEN,
        BILLBOARD_MODE_SCREEN_VIEWPOINT,
        BILLBOARD_MODE_Y_AXIAL,
        BILLBOARD_MODE_Y_AXIAL_VIEWPOINT
    };

    NW_RES_CTOR(ResBone)


    const char* GetName() const { return ref().toName.to_ptr(); }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    s32 GetIndex() const { return static_cast<s32>(ref().m_Index); }
    void SetIndex(s32 value) { ref().m_Index = value; }

    s32 GetParentBoneIndex() const { return static_cast<s32>(ref().m_ParentBoneIndex); }
    void SetParentBoneIndex(s32 value) { ref().m_ParentBoneIndex = value; }

    ResBone GetParentBone() { return ResBone(ref().toParentBone.to_ptr()); }
    const ResBone GetParentBone() const { return ResBone(ref().toParentBone.to_ptr()); }

    ResBone GetChildBone() { return ResBone(ref().toChildBone.to_ptr()); }
    const ResBone GetChildBone() const { return ResBone(ref().toChildBone.to_ptr()); }

    ResBone GetPrevSibling() { return ResBone(ref().toPrevSibling.to_ptr()); }
    const ResBone GetPrevSibling() const { return ResBone(ref().toPrevSibling.to_ptr()); }

    ResBone GetNextSibling() { return ResBone(ref().toNextSibling.to_ptr()); }
    const ResBone GetNextSibling() const { return ResBone(ref().toNextSibling.to_ptr()); }

    nw::math::Transform3& GetTransform() { return ref().m_Transform; }
    const nw::math::Transform3& GetTransform() const { return ref().m_Transform; }
    void SetTransform(const nw::math::Transform3& value) { ref().m_Transform = value; }

    nw::math::MTX34& GetLocalMatrix() { return ref().m_LocalMatrix; }
    const nw::math::MTX34& GetLocalMatrix() const { return ref().m_LocalMatrix; }
    void SetLocalMatrix(const nw::math::MTX34& value) { ref().m_LocalMatrix = value; }

    nw::math::MTX34& GetWorldMatrix() { return ref().m_WorldMatrix; }
    const nw::math::MTX34& GetWorldMatrix() const { return ref().m_WorldMatrix; }
    void SetWorldMatrix(const nw::math::MTX34& value) { ref().m_WorldMatrix = value; }

    nw::math::MTX34& GetInverseBaseMatrix() { return ref().m_InverseBaseMatrix; }
    const nw::math::MTX34& GetInverseBaseMatrix() const { return ref().m_InverseBaseMatrix; }
    void SetInverseBaseMatrix(const nw::math::MTX34& value) { ref().m_InverseBaseMatrix = value; }

    BillboardMode GetBillboardMode() const { return static_cast<BillboardMode>(ref().m_BillboardMode); }
    void SetBillboardMode(BillboardMode value) { ref().m_BillboardMode = value; }


    s32 GetUserDataCount() const
        { return ref().m_UserDataDicCount; }
    nw::ut::ResMetaData GetUserData(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return nw::ut::ResMetaData(dic[idx]);
        }
    nw::ut::ResMetaData GetUserData(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return nw::ut::ResMetaData(dic[key]);
        }
    const nw::ut::ResMetaData GetUserData(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return nw::ut::ResMetaData(dic[idx]);
        }
    const nw::ut::ResMetaData GetUserData(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return nw::ut::ResMetaData(dic[key]);
        }
    s32 GetUserDataIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<nw::ut::ResMetaData, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetUserData()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return nw::ut::internal::ResArray<nw::ut::ResMetaData, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetUserDataCount());
        }
    const nw::ut::internal::ResArray<const nw::ut::ResMetaData, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetUserData() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toUserDataDic.to_ptr());
            return nw::ut::internal::ResArray<const nw::ut::ResMetaData, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetUserDataCount());
        }
};
typedef nw::ut::ResArrayPatricia<ResBone>::type  ResBoneArray;

class ResSkeleton : public ResSceneObject
{
public:
    enum Flag
    {
        FLAG_MODEL_COORDINATE                       = 0x1 << 0,
        FLAG_TRANSLATE_ANIMATION_ENABLED            = 0x1 << 1
    };

    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSkeleton)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CSKL')
    };
    
    NW_RES_CTOR_INHERIT(ResSkeleton, ResSceneObject)


    s32 GetBonesCount() const
        { return ref().m_BonesDicCount; }
    ResBone GetBones(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return ResBone(dic[idx]);
        }
    ResBone GetBones(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return ResBone(dic[key]);
        }
    const ResBone GetBones(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return ResBone(dic[idx]);
        }
    const ResBone GetBones(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return ResBone(dic[key]);
        }
    s32 GetBonesIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<ResBone, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetBones()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return nw::ut::internal::ResArray<ResBone, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetBonesCount());
        }
    const nw::ut::internal::ResArray<const ResBone, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetBones() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toBonesDic.to_ptr());
            return nw::ut::internal::ResArray<const ResBone, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetBonesCount());
        }

    ResBone GetRootBone() { return ResBone(ref().toRootBone.to_ptr()); }
    const ResBone GetRootBone() const { return ResBone(ref().toRootBone.to_ptr()); }

    s32 GetScalingRule() const { return static_cast<s32>(ref().m_ScalingRule); }
    void SetScalingRule(s32 value) { ref().m_ScalingRule = value; }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }
};

}
}
}

