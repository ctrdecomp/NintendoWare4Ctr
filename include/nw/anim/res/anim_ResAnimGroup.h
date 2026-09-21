#pragma once

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDeclMacros.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_ResArrayTypes.h>
#include <nw/anim/res/anim_ResTypeInfo.h>

namespace nw {
namespace gfx {
    class SkeletalModel;
    class Model;
    class Camera;
    class Light;
namespace res {
    class ResModel;
}
}
}

namespace nw {
namespace anim {
namespace res {

struct ResAnimGroupMemberData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::BinString   toPath;
    nw::ut::ResS32      m_MemberOffset;
    nw::ut::ResS32      m_BlendOperationIndex;
    nw::ut::ResU32      m_ObjectType;
    nw::ut::ResU32      m_MemberType;
    nw::ut::ResU32      m_ResMaterialPtr;
};

struct ResAnimGroupMemberData;

struct ResBoneMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toBoneName;
};

struct ResMaterialColorMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toMaterialName;
};

struct ResTextureSamplerMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toMaterialName;
    nw::ut::ResU32      m_TextureMapperIndex;
};

struct ResTextureMapperMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toMaterialName;
    nw::ut::ResU32      m_TextureMapperIndex;
};

struct ResBlendOperationMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toMaterialName;
};

struct ResTextureCoordinatorMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toMaterialName;
    nw::ut::ResU32      m_TextureCoordinatorIndex;
};

struct ResModelMemberData : public ResAnimGroupMemberData
{
};

struct ResMeshMemberData : public ResAnimGroupMemberData
{
    nw::ut::ResU32      m_MeshIndex;
};

struct ResMeshNodeVisibilityMemberData : public ResAnimGroupMemberData
{
    nw::ut::BinString   toNodeName;
};

struct ResTransformMemberData : public ResAnimGroupMemberData
{
};

struct ResViewUpdaterMemberData : public ResAnimGroupMemberData
{
};

struct ResProjectionUpdaterMemberData : public ResAnimGroupMemberData
{
};

struct ResLightMemberData : public ResAnimGroupMemberData
{
};

struct ResFragmentLightMemberData : public ResAnimGroupMemberData
{
};

struct ResAmbientLightMemberData : public ResAnimGroupMemberData
{
};

struct ResVertexLightMemberData : public ResAnimGroupMemberData
{
};

struct ResHemiSphereLightMemberData : public ResAnimGroupMemberData
{
};

struct ResFogMemberData : public ResAnimGroupMemberData
{
};

struct ResAnimGroupData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResU32      m_Flags;
    nw::ut::BinString   toName;
    nw::ut::ResS32      m_TargetType;
    nw::ut::ResS32      m_MemberInfoSetDicCount;
    nw::ut::Offset      toMemberInfoSetDic;
    nw::ut::ResS32      m_BlendOperationsTableCount;
    nw::ut::Offset      toBlendOperationsTable;
};

struct ResGraphicsAnimGroupData : public ResAnimGroupData
{
    nw::ut::ResS32      m_EvaluationTiming;
};

class ResAnimGroupMember : public nw::ut::ResCommon<ResAnimGroupMemberData>
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResAnimGroupMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGMB') };
    
    enum ObjectType
    {
        OBJECT_TYPE_BONE,

        OBJECT_TYPE_MATERIAL_COLOR,
        OBJECT_TYPE_TEXTURE_SAMPLER,
        OBJECT_TYPE_TEXTURE_MAPPER,
        OBJECT_TYPE_BLEND_OPERATION,
        OBJECT_TYPE_TEXTURE_COORDINATOR,

        OBJECT_TYPE_MODEL,
        OBJECT_TYPE_MESH,
        OBJECT_TYPE_MESH_NODE_VISIBILITY,

        OBJECT_TYPE_TRANSFORM,
        
        OBJECT_TYPE_VIEW_UPDATER,
        OBJECT_TYPE_PROJECTION_UPDATER,

        OBJECT_TYPE_LIGHT,
        OBJECT_TYPE_FRAGMENT_LIGHT,
        OBJECT_TYPE_VERTEX_LIGHT,
        OBJECT_TYPE_HEMISPHERE_LIGHT,
        OBJECT_TYPE_AMBIENT_LIGHT,

        OBJECT_TYPE_FOG
    };

    enum MemberType
    {
        MEMBER_TYPE_INVALID = 0xFFFFFFFF
    };

    NW_RES_CTOR(ResAnimGroupMember)
    
    NW_RES_FIELD_STRING_DECL(Path)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, MemberOffset)
    NW_RES_FIELD_PRIMITIVE_DECL(s32, BlendOperationIndex)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, ObjectType)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, MemberType)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, ResMaterialPtr)

    void SetValueForType(void* object, const void* value) const;

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};
typedef nw::ut::ResArrayPatricia<ResAnimGroupMember>::type        ResAnimGroupMemberArray;
typedef nw::ut::ResArrayPatricia<const ResAnimGroupMember>::type  ResAnimGroupMemberArrayConst;

class ResBoneMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResBoneMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGBM') };

    enum MemberType
    {
        MEMBER_TYPE_TRANSFORM
    };

    NW_RES_CTOR_INHERIT(ResBoneMember, ResAnimGroupMember)

    NW_RES_FIELD_STRING_DECL(BoneName)

    void SetValue(void* object, const void* value) const;
};

class ResMaterialColorMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResMaterialColorMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGMC') };

    enum MemberType
    {
        MEMBER_TYPE_EMISSION,
        MEMBER_TYPE_AMBIENT,
        MEMBER_TYPE_DIFFUSE,
        MEMBER_TYPE_SPECULAR0,
        MEMBER_TYPE_SPECULAR1,
        MEMBER_TYPE_CONSTANT0,
        MEMBER_TYPE_CONSTANT1,
        MEMBER_TYPE_CONSTANT2,
        MEMBER_TYPE_CONSTANT3,
        MEMBER_TYPE_CONSTANT4,
        MEMBER_TYPE_CONSTANT5
    };

    NW_RES_CTOR_INHERIT(ResMaterialColorMember, ResAnimGroupMember)

    NW_RES_FIELD_STRING_DECL(MaterialName)

    void SetValue(void* object, const void* value) const;
};

class ResTextureSamplerMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResTextureSamplerMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGTS') };

    enum MemberType
    {
        MEMBER_TYPE_BORDER_COLOR
    };

    NW_RES_CTOR_INHERIT(ResTextureSamplerMember, ResAnimGroupMember)

    NW_RES_FIELD_STRING_DECL(MaterialName)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, TextureMapperIndex)

    void SetValue(void* object, const void* value) const;
};

class ResTextureMapperMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResTextureMapperMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGTM') };

    enum MemberType
    {
        MEMBER_TYPE_TEXTURE
    };

    NW_RES_CTOR_INHERIT(ResTextureMapperMember, ResAnimGroupMember)

    NW_RES_FIELD_STRING_DECL(MaterialName)
    NW_RES_FIELD_PRIMITIVE_DECL(u32, TextureMapperIndex)

    void SetValue(void* object, const void* value) const;
};

class ResBlendOperationMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResBlendOperationMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGBO') };

    enum MemberType
    {
        MEMBER_TYPE_BLEND_COLOR
    };

    NW_RES_CTOR_INHERIT( ResBlendOperationMember, ResAnimGroupMember )

    NW_RES_FIELD_STRING_DECL( MaterialName )

    void SetValue(void* object, const void* value) const;
};

class ResTextureCoordinatorMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResTextureCoordinatorMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGTC') };

    enum MemberType
    {
        MEMBER_TYPE_SCALE,
        MEMBER_TYPE_ROTATE,
        MEMBER_TYPE_TRANSLATE
    };

    NW_RES_CTOR_INHERIT( ResTextureCoordinatorMember, ResAnimGroupMember )

    NW_RES_FIELD_STRING_DECL( MaterialName )
    NW_RES_FIELD_PRIMITIVE_DECL( u32, TextureCoordinatorIndex )

    void SetValue(void* object, const void* value) const;
};

class ResModelMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResModelMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGMO') };

    enum MemberType
    {
        MEMBER_TYPE_BRANCH_VISIBLE,
        MEMBER_TYPE_VISIBLE
    };

    NW_RES_CTOR_INHERIT( ResModelMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResMeshMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResMeshMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGME') };

    enum MemberType
    {
        MEMBER_TYPE_VISIBLE
    };

    NW_RES_CTOR_INHERIT( ResMeshMember, ResAnimGroupMember )

    NW_RES_FIELD_PRIMITIVE_DECL( u32, MeshIndex )

    void SetValue(void* object, const void* value) const;
};

class ResMeshNodeVisibilityMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResMeshNodeVisibilityMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGND') };

    enum MemberType
    {
        MEMBER_TYPE_VISIBLE
    };

    NW_RES_CTOR_INHERIT( ResMeshNodeVisibilityMember, ResAnimGroupMember )

    NW_RES_FIELD_STRING_DECL( NodeName )

    void SetValue(void* object, const void* value) const;
};

class ResTransformMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResTransformMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGTF') };

    enum MemberType
    {
        MEMBER_TYPE_TRANSFORM
    };

    NW_RES_CTOR_INHERIT( ResTransformMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResViewUpdaterMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResViewUpdaterMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGVU') };

    enum MemberType
    {
        MEMBER_TYPE_TARGET_POSITION,
        MEMBER_TYPE_TWIST,
        MEMBER_TYPE_UPWARD_VECTOR,
        MEMBER_TYPE_VIEW_ROTATE,
        MEMBER_TYPE_TRANSFORM
    };

    NW_RES_CTOR_INHERIT( ResViewUpdaterMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResProjectionUpdaterMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResProjectionUpdaterMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGPU') };

    enum MemberType
    {
        MEMBER_TYPE_NEAR,
        MEMBER_TYPE_FAR,
        MEMBER_TYPE_FOVY,
        MEMBER_TYPE_ASPECT_RATIO,
        MEMBER_TYPE_HEIGHT,
        MEMBER_TYPE_CENTER
    };

    NW_RES_CTOR_INHERIT( ResProjectionUpdaterMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResLightMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResLightMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGLM') };

    enum MemberType
    {
        MEMBER_TYPE_IS_LIGHT_ENABLED
    };

    NW_RES_CTOR_INHERIT( ResLightMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResFragmentLightMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResLightMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGLM') };

    enum MemberType
    {
        MEMBER_TYPE_AMBIENT,
        MEMBER_TYPE_DIFFUSE,
        MEMBER_TYPE_SPECULAR0,
        MEMBER_TYPE_SPECULAR1,
        MEMBER_TYPE_DIRECTION,
        MEMBER_TYPE_DISTANCE_ATTENUATION_START,
        MEMBER_TYPE_DISTANCE_ATTENUATION_END
    };

    NW_RES_CTOR_INHERIT( ResFragmentLightMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResAmbientLightMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResLightMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGLM') };

    enum MemberType
    {
        MEMBER_TYPE_AMBIENT
    };

    NW_RES_CTOR_INHERIT( ResAmbientLightMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResVertexLightMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResLightMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGLM') };

    enum MemberType
    {
        MEMBER_TYPE_AMBIENT,
        MEMBER_TYPE_DIFFUSE,
        MEMBER_TYPE_DIRECTION,
        MEMBER_TYPE_DISTANCE_ATTENUATION_CONSTANT,
        MEMBER_TYPE_DISTANCE_ATTENUATION_LINEAR,
        MEMBER_TYPE_DISTANCE_ATTENUATION_QUADRATIC,
        MEMBER_TYPE_SPOT_EXPONENT,
        MEMBER_TYPE_SPOT_CUTOFF_ANGLE
    };

    NW_RES_CTOR_INHERIT( ResVertexLightMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResHemiSphereLightMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResLightMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGLM') };

    enum MemberType
    {
        MEMBER_TYPE_GROUND_COLOR,
        MEMBER_TYPE_SKY_COLOR,
        MEMBER_TYPE_DIRECTION,
        MEMBER_TYPE_LERP_FACTOR
    };

    NW_RES_CTOR_INHERIT( ResHemiSphereLightMember, ResAnimGroupMember )

    void SetValue(void* object, const void* value) const;
};

class ResFogMember : public ResAnimGroupMember
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResFogMember) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('AGFM') };

    enum MemberType
    {
        MEMBER_TYPE_COLOR
    };

    NW_RES_CTOR_INHERIT( ResFogMember, ResAnimGroupMember )
    
    void SetValue(void* object, const void* value) const;
};

class ResAnimGroup : public nw::ut::ResCommon<ResAnimGroupData>
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResAnimGroup) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('ANMG') };

    enum BlendOperationType
    {
        BLENDOP_BOOL,
        BLENDOP_INT,
        BLENDOP_FLOAT,
        BLENDOP_RGBA_COLOR,
        BLENDOP_TRANSFORM,
        BLENDOP_VECTOR2,
        BLENDOP_VECTOR3,
        BLENDOP_TEXTURE,
        BLENDOP_CALCULATED_TRANSFORM,
        BLENDOP_CALCULATED_TRANSFORM_QUAT,
        BLENDOP_CALCULATED_TRANSFORM_ACCURATE_SCALE,
        BLENDOP_CALCULATED_TRANSFORM_ACCURATE_SCALE_QUAT
    };

    enum TargetType
    {
        TARGET_TYPE_NONE    = 0,
        TARGET_TYPE_UNKNOWN = -1
    };

    enum Flag
    {
        FLAG_IS_CALCULATED_TRANSFORM = 0x1 << 0
    };
    
    NW_RES_CTOR( ResAnimGroup )
    
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )

    NW_RES_FIELD_STRING_DECL( Name )

    NW_RES_FIELD_PRIMITIVE_DECL( s32, TargetType )

    NW_RES_FIELD_CLASS_DIC_DECL( ResAnimGroupMember, MemberInfoSet, nw::ut::ResDicPatricia )

    NW_RES_FIELD_PRIMITIVE_LIST_DECL( s32, BlendOperations )
};

class ResGraphicsAnimGroup : public ResAnimGroup
{
public:
    enum
{ TYPE_INFO = NW_ANIM_RES_TYPE_INFO(ResGraphicsAnimGroup) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('GXAG') };
    
    enum TargetType
    {
        TARGET_TYPE_BONE       = 1,
        TARGET_TYPE_MATERIAL   = 2,
        TARGET_TYPE_VISIBILITY = 3,
        TARGET_TYPE_LIGHT      = 4,
        TARGET_TYPE_CAMERA     = 5,
        TARGET_TYPE_FOG        = 6
    };

    enum EvaluationTiming
    {
        EVALUATION_BEFORE_WORLD_UPDATE,
        EVALUATION_AFTER_SCENE_CULLING
    };
    
    NW_RES_CTOR_INHERIT( ResGraphicsAnimGroup, ResAnimGroup )
    
    NW_RES_FIELD_PRIMITIVE_DECL( EvaluationTiming, EvaluationTiming )
};

}
}
}