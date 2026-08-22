#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResLookupTable.h>
#include <nw/gfx/res/gfx_ResFragmentShader.h>
#include <nw/ut/ut_Rect.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/anim/res/anim_ResAnim.h>

namespace nw {
namespace gfx {
namespace res {

struct ResCameraData : public ResTransformNodeData{
    nw::ut::ResS32 mViewType;
    nw::ut::ResS32 mProjectionType;
    nw::ut::Offset toViewUpdater;
    nw::ut::Offset toProjectionUpdater;
    nw::ut::ResF32 mWScale;
};

struct ResCameraViewUpdaterData{
    nw::ut::ResTypeInfo typeInfo;
};

struct ResCameraProjectionUpdaterData{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResF32 mNear;
    nw::ut::ResF32 mFar;
};

struct ResAimTargetViewUpdaterData : public ResCameraViewUpdaterData{
    enum Flag{
        FLAG_INHERITING_TARGET_ROTATE    = 0x1 << 0,
        FLAG_INHERITING_TARGET_TRANSLATE = 0x1 << 1
    };
    
    nw::ut::ResU32 mFlags;
    nw::ut::ResVec3 mTargetPosition;
    nw::ut::ResF32 mTwist;
};

struct ResLookAtTargetViewUpdaterData : public ResCameraViewUpdaterData{
    enum Flag{
        FLAG_INHERITING_UP_ROTATE        = 0x1 << 0,
        FLAG_INHERITING_TARGET_ROTATE    = 0x1 << 1,
        FLAG_INHERITING_TARGET_TRANSLATE = 0x1 << 2
    };
    
    nw::ut::ResU32 mFlags;
    nw::ut::ResVec3 mTargetPosition;
    nw::ut::ResVec3 mUpwardVector;
};

struct ResRotateViewUpdaterData : public ResCameraViewUpdaterData{
    enum Flag{
        FLAG_INHERITING_ROTATE = 0x1 << 0
    };
    
    nw::ut::ResU32 mFlags;
    nw::ut::ResVec3 mViewRotate;
};

struct ResProjectionRect{
    nw::ut::ResF32 mAspectRatio;
    nw::ut::ResF32 mHeight;
    nw::ut::VEC2 mCenter;
    
    operator nw::ut::Rect() const{ 
        f32 width = mHeight * mAspectRatio;
        
        return nw::ut::Rect( mCenter.x - width / 2, mCenter.y - mHeight / 2, mCenter.x + width / 2, mCenter.y + mHeight / 2 ); 
    }

    f32 GetHeight() const { return mHeight; }

    f32 GetWidth() const { return mHeight * mAspectRatio; }

    f32 GetAspectRatio() const { return mAspectRatio; }

    nw::math::VEC2 GetCenter() const { return mCenter; }

    void SetWithoutFlip( f32 l, f32 r, f32 b, f32 t ){
        mHeight      = t - b;
        mAspectRatio = (r - l) / mHeight;
        mCenter = nw::math::VEC2( (r + l) / 2, (t + b) / 2 );
    }

    void SetWithYFlip( f32 l, f32 r, f32 b, f32 t ){
        mHeight      = b - t;
        mAspectRatio = (r - l) / mHeight;
        mCenter = nw::math::VEC2( (r + l) / 2, (b + t) / 2 );
    }

    void Set( f32 aspect, f32 height, const nw::math::VEC2& center ){
        mHeight = height;
        mAspectRatio = aspect;
        mCenter = center;
    }
};

struct ResFrustumProjectionUpdaterData : public ResCameraProjectionUpdaterData{
    ResProjectionRect mRect;
};

struct ResOrthoProjectionUpdaterData : public ResCameraProjectionUpdaterData{
    ResProjectionRect mRect;
};

struct ResPerspectiveProjectionUpdaterData : public ResCameraProjectionUpdaterData{
    nw::ut::ResF32 mAspectRatio;
    nw::ut::ResF32 mFovy;
};

class ResCameraViewUpdater : public nw::ut::ResCommon< ResCameraViewUpdaterData >{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCameraViewUpdater) };
    
    NW_RES_CTOR( ResCameraViewUpdater )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResCameraProjectionUpdater : public nw::ut::ResCommon< ResCameraProjectionUpdaterData >{
public:

    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCameraProjectionUpdater) };
    
    NW_RES_CTOR( ResCameraProjectionUpdater )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Near )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, Far )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResAimTargetViewUpdater : public ResCameraViewUpdater
{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResAimTargetViewUpdater) };
    
    NW_RES_CTOR_INHERIT( ResAimTargetViewUpdater, ResCameraViewUpdater )


    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, TargetPosition )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Twist )
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )
};

class ResLookAtTargetViewUpdater : public ResCameraViewUpdater{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookAtTargetViewUpdater) };
    
    NW_RES_CTOR_INHERIT( ResLookAtTargetViewUpdater, ResCameraViewUpdater )

    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, TargetPosition )
    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, UpwardVector )
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )
};

class ResRotateViewUpdater : public ResCameraViewUpdater{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResRotateViewUpdater) };
    
    NW_RES_CTOR_INHERIT( ResRotateViewUpdater, ResCameraViewUpdater )

    NW_RES_FIELD_VECTOR3_DECL( nw::math::VEC3, ViewRotate )
    NW_RES_FIELD_FLAGS_DECL( u32, Flags )
};

class ResFrustumProjectionUpdater : public ResCameraProjectionUpdater{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResFrustumProjectionUpdater) };
    
    NW_RES_CTOR_INHERIT( ResFrustumProjectionUpdater, ResCameraProjectionUpdater )

    NW_RES_FIELD_RECT_DECL( ResProjectionRect, Rect )
};

class ResOrthoProjectionUpdater : public ResCameraProjectionUpdater{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResOrthoProjectionUpdater) };
    
    NW_RES_CTOR_INHERIT( ResOrthoProjectionUpdater, ResCameraProjectionUpdater )

    NW_RES_FIELD_RECT_DECL( ResProjectionRect, Rect )
};


class ResPerspectiveProjectionUpdater : public ResCameraProjectionUpdater{
public:
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResPerspectiveProjectionUpdater) };
    
    NW_RES_CTOR_INHERIT( ResPerspectiveProjectionUpdater, ResCameraProjectionUpdater )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, AspectRatio )
    NW_RES_FIELD_PRIMITIVE_DECL( f32, Fovy )
};

class ResCamera : public ResTransformNode{
public:
    enum { BINARY_REVISION = REVISION_RES_CAMERA };
    enum { TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCamera) };

    enum ViewType{
        VIEWTYPE_AIM,
        VIEWTYPE_LOOKAT,
        VIEWTYPE_ROTATE,
        VIEWTYPE_COUNT
    };

    enum ProjType{
        PROJTYPE_PERSPECTIVE,
        PROJTYPE_FRUSTUM,
        PROJTYPE_ORTHO,
        PROJTYPE_COUNT
    };
    
    NW_RES_CTOR_INHERIT( ResCamera, ResTransformNode )
    
    NW_RES_FIELD_PRIMITIVE_DECL( ViewType, ViewType )
    NW_RES_FIELD_PRIMITIVE_DECL( ProjType, ProjectionType )

    NW_RES_FIELD_CLASS_DECL( ResCameraViewUpdater, ViewUpdater )
    NW_RES_FIELD_CLASS_DECL( ResCameraProjectionUpdater, ProjectionUpdater )

    NW_RES_FIELD_PRIMITIVE_DECL( f32, WScale )

    u32 GetRevision() const { return this->GetHeader().revision; }
};

typedef nw::ut::ResArrayPatricia<ResCamera>::type  ResCameraArray;

}
}
}