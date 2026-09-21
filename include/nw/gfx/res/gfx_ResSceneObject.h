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
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneObject) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('SOBJ') };
    
    NW_RES_CTOR(ResSceneObject)
    
    NW_RES_FIELD_STRUCT_DECL(nw::ut::BinaryRevisionHeader, Header)
    NW_RES_FIELD_STRING_DECL(Name)
    NW_RES_FIELD_CLASS_DIC_DECL(nw::ut::ResMetaData, UserData, nw::ut::ResDicPatricia)

    nw::ut::ResTypeInfo GetTypeInfo() const { return ref().typeInfo; }
};

class ResSceneNode : public ResSceneObject
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResSceneNode) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CNOD') };
    
    NW_RES_CTOR_INHERIT(ResSceneNode, ResSceneObject)

    NW_RES_FIELD_FLAGS_DECL(u32, Flags)
    NW_RES_FIELD_CLASS_LIST_DECL(ResSceneObject, Children)
    NW_RES_FIELD_CLASS_DIC_DECL(nw::anim::res::ResAnimGroup, AnimGroups, nw::ut::ResDicPatricia)
    
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
    
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResTransformNode) };
    enum { SIGNATURE = NW_RES_SIGNATURE32('CTFN') };
    
    NW_RES_CTOR_INHERIT( ResTransformNode, ResSceneNode )

    NW_RES_FIELD_STRUCT_DECL(nw::math::Transform3, Transform)
    NW_RES_FIELD_STRUCT_DECL(nw::math::MTX34, WorldMatrix)
};

}
}
}