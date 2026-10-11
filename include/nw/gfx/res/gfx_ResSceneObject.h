#pragma once

#include <nw/ut/ut_BinaryFileFormat.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/math/math_ResTypes.h>
#include <nw/math/math_Transform.h>
#include <nw/gfx/gfx_Common.h>
#include <nw/anim/res/anim_ResAnimGroup.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>
#include <nw/ut/ut_ResMetaData.h>

namespace nw {
namespace gfx {
namespace res {

struct ResSceneObjectData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::BinaryRevisionHeader m_Header;
    nw::ut::BinString toName;
    nw::ut::ResS32 m_UserDataDicCount;
    nw::ut::Offset toUserDataDic;
};

struct ResSceneNodeData : public ResSceneObjectData
{
    enum Flag
    {
        FLAG_IS_BRANCH_VISIBLE_SHIFT = 0,
        FLAG_SHIFT_MAX,
        
        FLAG_IS_BRANCH_VISIBLE = 0x1 << FLAG_IS_BRANCH_VISIBLE_SHIFT
    };
    
    nw::ut::ResU32 m_Flags;
    nw::ut::ResBool m_IsBranchVisible;
    u8             padding_0[3];
    nw::ut::ResS32 m_ChildrenTableCount;
    nw::ut::Offset toChildrenTable;
    nw::ut::ResS32 m_AnimGroupsDicCount;
    nw::ut::Offset toAnimGroupsDic;
};

struct ResTransformNodeData : public ResSceneNodeData
{
    nw::math::Transform3 m_Transform;
    nw::ut::ResMtx34 m_LocalMatrix;
    nw::ut::ResMtx34 m_WorldMatrix;
};

class ResSceneObject : public nw::ut::ResCommon< ResSceneObjectData >
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneObject)
    };

    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('SOBJ')
    };
    
    NW_RES_CTOR(ResSceneObject)
    

    nw::ut::BinaryRevisionHeader& GetHeader() { return ref().m_Header; }
    const nw::ut::BinaryRevisionHeader& GetHeader() const { return ref().m_Header; }
    void SetHeader(const nw::ut::BinaryRevisionHeader& value) { ref().m_Header = value; }

    const char* GetName() const { return ref().toName.to_ptr(); }

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

    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }
};

class ResSceneNode : public ResSceneObject
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneNode)
    };
    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CNOD')
    };
    
    NW_RES_CTOR_INHERIT(ResSceneNode, ResSceneObject)

    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }

    s32 GetChildrenCount() const
        { return ref().m_ChildrenTableCount; }
    ResSceneObject GetChildren(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toChildrenTable.to_table_ptr());
            return ResSceneObject(tbl_ptr[idx].to_ptr());
        }
    const ResSceneObject GetChildren(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toChildrenTable.to_table_ptr());
            return ResSceneObject(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResSceneObject, nw::ut::internal::ResArrayClassTraits> GetChildren()
        {
            return nw::ut::internal::ResArray<ResSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toChildrenTable.to_table_ptr(), GetChildrenCount());
        }
    const nw::ut::internal::ResArray<const ResSceneObject, nw::ut::internal::ResArrayClassTraits> GetChildren() const
        {
            return nw::ut::internal::ResArray<const ResSceneObject, nw::ut::internal::ResArrayClassTraits>(ref().toChildrenTable.to_table_ptr(), GetChildrenCount());
        }

    s32 GetAnimGroupsCount() const
        { return ref().m_AnimGroupsDicCount; }
    nw::anim::res::ResAnimGroup GetAnimGroups(int idx)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return nw::anim::res::ResAnimGroup(dic[idx]);
        }
    nw::anim::res::ResAnimGroup GetAnimGroups(const char* key)
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return nw::anim::res::ResAnimGroup(dic[key]);
        }
    const nw::anim::res::ResAnimGroup GetAnimGroups(int idx) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return nw::anim::res::ResAnimGroup(dic[idx]);
        }
    const nw::anim::res::ResAnimGroup GetAnimGroups(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return nw::anim::res::ResAnimGroup(dic[key]);
        }
    s32 GetAnimGroupsIndex(const char* key) const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return dic.GetIndex(key);
        }
    nw::ut::internal::ResArray<nw::anim::res::ResAnimGroup, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetAnimGroups()
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return nw::ut::internal::ResArray<nw::anim::res::ResAnimGroup, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetAnimGroupsCount());
        }
    const nw::ut::internal::ResArray<const nw::anim::res::ResAnimGroup, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits> GetAnimGroups() const
        {
            nw::ut::ResDicPatricia dic = nw::ut::ResDicPatricia(ref().toAnimGroupsDic.to_ptr());
            return nw::ut::internal::ResArray<const nw::anim::res::ResAnimGroup, nw::ut::internal::ResArrayDicTraits<nw::ut::ResDicPatriciaData>::Traits>(
                (dic.IsValid()) ? dic.ref().GetBeginNode() : NULL, GetAnimGroupsCount());
        }
    
    bool IsBranchVisible() const { return ref().m_Flags & ResSceneNodeData::FLAG_IS_BRANCH_VISIBLE; }
    
    void SetBranchVisible(bool flag) 
    {
        if (flag)
        {
            ref().m_Flags |= ResSceneNodeData::FLAG_IS_BRANCH_VISIBLE;
        }
        else
        {
            ref().m_Flags &= ~ResSceneNodeData::FLAG_IS_BRANCH_VISIBLE;
        }
    }
};

class ResTransformNode : public ResSceneNode
{
public:
    enum Flag
    {
        FLAG_SHIFT_MAX = ResSceneNodeData::FLAG_SHIFT_MAX
    };
    
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTransformNode)
    };

    enum
    {
        SIGNATURE = NW_RES_SIGNATURE32('CTFN')
    };
    
    NW_RES_CTOR_INHERIT( ResTransformNode, ResSceneNode )


    nw::math::Transform3& GetTransform() { return ref().m_Transform; }
    const nw::math::Transform3& GetTransform() const { return ref().m_Transform; }
    void SetTransform(const nw::math::Transform3& value) { ref().m_Transform = value; }

    nw::math::MTX34& GetWorldMatrix() { return ref().m_WorldMatrix; }
    const nw::math::MTX34& GetWorldMatrix() const { return ref().m_WorldMatrix; }
    void SetWorldMatrix(const nw::math::MTX34& value) { ref().m_WorldMatrix = value; }
};

}
}
}

