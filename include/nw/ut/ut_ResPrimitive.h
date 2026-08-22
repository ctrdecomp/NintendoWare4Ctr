#pragma once


#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_Inlines.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDeclMacros.h>
#include <nw/ut/ut_ResTypeInfo.h>
#include <nw/math/math_Types.h>

namespace nw {
namespace ut {

struct ResBoundingVolumeData{
    nw::ut::ResTypeInfo typeInfo;
};

struct ResOrientedBoundingBoxData : public ResBoundingVolumeData{
    ResVec3   mCenterPosition;
    ResMtx33  mOrientationMatrix;
    ResVec3   mSize;
};

struct ResAxisAlignedBoundingBoxData : public ResBoundingVolumeData{
    ResVec3   mCenterPosition;
    ResVec3   mSize;
};

class ResBoundingVolume : public nw::ut::ResCommon<ResBoundingVolumeData>{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResBoundingVolume) };
    
    NW_RES_CTOR( ResBoundingVolume )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResOrientedBoundingBox : public ResBoundingVolume{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResOrientedBoundingBox) };
    
    NW_RES_CTOR_INHERIT(ResOrientedBoundingBox, ResBoundingVolume )

    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, CenterPosition)
    
    NW_RES_FIELD_STRUCT_DECL(nw::math::MTX33, OrientationMatrix)

    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Size)
};

class ResAxisAlignedBoundingBox : public ResBoundingVolume{
public:
    enum { TYPE_INFO = NW_UT_RES_TYPE_INFO(ResAxisAlignedBoundingBox) };
    
    NW_RES_CTOR_INHERIT(ResAxisAlignedBoundingBox, ResBoundingVolume)

    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, CenterPosition)
    
    NW_RES_FIELD_VECTOR3_DECL(nw::math::VEC3, Size)
};

}
}