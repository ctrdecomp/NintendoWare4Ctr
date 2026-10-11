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

struct ResCameraData : public ResTransformNodeData
{
    nw::ut::ResS32 m_ViewType;
    nw::ut::ResS32 m_ProjectionType;
    nw::ut::Offset toViewUpdater;
    nw::ut::Offset toProjectionUpdater;
    nw::ut::ResF32 m_WScale;
};

struct ResCameraViewUpdaterData
{
    nw::ut::ResTypeInfo typeInfo;
};

struct ResCameraProjectionUpdaterData
{
    nw::ut::ResTypeInfo typeInfo;
    nw::ut::ResF32 m_Near;
    nw::ut::ResF32 m_Far;
};

struct ResAimTargetViewUpdaterData : public ResCameraViewUpdaterData
{
    enum Flag
    {
        FLAG_INHERITING_TARGET_ROTATE    = 0x1 << 0,
        FLAG_INHERITING_TARGET_TRANSLATE = 0x1 << 1
    };
    
    nw::ut::ResU32 m_Flags;
    nw::ut::ResVec3 m_TargetPosition;
    nw::ut::ResF32 m_Twist;
};

struct ResLookAtTargetViewUpdaterData : public ResCameraViewUpdaterData
{
    enum Flag
    {
        FLAG_INHERITING_UP_ROTATE        = 0x1 << 0,
        FLAG_INHERITING_TARGET_ROTATE    = 0x1 << 1,
        FLAG_INHERITING_TARGET_TRANSLATE = 0x1 << 2
    };
    
    nw::ut::ResU32 m_Flags;
    nw::ut::ResVec3 m_TargetPosition;
    nw::ut::ResVec3 m_UpwardVector;
};

struct ResRotateViewUpdaterData : public ResCameraViewUpdaterData
{
    enum Flag
    {
        FLAG_INHERITING_ROTATE = 0x1 << 0
    };
    
    nw::ut::ResU32 m_Flags;
    nw::ut::ResVec3 m_ViewRotate;
};

struct ResProjectionRect
{
    nw::ut::ResF32 m_AspectRatio;
    nw::ut::ResF32 m_Height;
    nw::math::VEC2 m_Center;
    
    operator nw::ut::Rect() const
    {
        f32 width = m_Height * m_AspectRatio;
        
        return nw::ut::Rect( m_Center.x - width / 2, m_Center.y - m_Height / 2, m_Center.x + width / 2, m_Center.y + m_Height / 2 );
    }

    f32 GetHeight() const { return m_Height; }

    f32 GetWidth() const { return m_Height * m_AspectRatio; }

    f32 GetAspectRatio() const { return m_AspectRatio; }

    nw::math::VEC2 GetCenter() const { return m_Center; }

    void SetWithoutFlip(f32 l, f32 r, f32 b, f32 t)
    {
        m_Height      = t - b;
        m_AspectRatio = (r - l) / m_Height;
        m_Center = nw::math::VEC2( (r + l) / 2, (t + b) / 2 );
    }

    void SetWithYFlip(f32 l, f32 r, f32 b, f32 t)
    {
        m_Height      = b - t;
        m_AspectRatio = (r - l) / m_Height;
        m_Center = nw::math::VEC2( (r + l) / 2, (b + t) / 2 );
    }

    void Set(f32 aspect, f32 height, const nw::math::VEC2& center)
    {
        m_Height = height;
        m_AspectRatio = aspect;
        m_Center = center;
    }
};

struct ResFrustumProjectionUpdaterData : public ResCameraProjectionUpdaterData
{
    ResProjectionRect m_Rect;
};

struct ResOrthoProjectionUpdaterData : public ResCameraProjectionUpdaterData
{
    ResProjectionRect m_Rect;
};

struct ResPerspectiveProjectionUpdaterData : public ResCameraProjectionUpdaterData
{
    nw::ut::ResF32 m_AspectRatio;
    nw::ut::ResF32 m_Fovy;
};

class ResCameraViewUpdater : public nw::ut::ResCommon< ResCameraViewUpdaterData >
{
public:
    enum
    {
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCameraViewUpdater)
    };
    
    NW_RES_CTOR( ResCameraViewUpdater )

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResCameraProjectionUpdater : public nw::ut::ResCommon< ResCameraProjectionUpdaterData >
{
public:

    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCameraProjectionUpdater) 
    };
    
    NW_RES_CTOR(ResCameraProjectionUpdater)


    f32 GetNear() const { return static_cast<f32>(ref().m_Near); }           
    void SetNear(f32 value) { ref().m_Near = value; }


    f32 GetFar() const { return static_cast<f32>(ref().m_Far); }           
    void SetFar(f32 value) { ref().m_Far = value; }

    nw::ut::ResTypeInfo     GetTypeInfo() const { return ref().typeInfo; }
};

class ResAimTargetViewUpdater : public ResCameraViewUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResAimTargetViewUpdater)
    };
    
    NW_RES_CTOR_INHERIT(ResAimTargetViewUpdater, ResCameraViewUpdater)



    void SetTargetPosition(f32 x, f32 y, f32 z) { ref().m_TargetPosition.Set(x, y, z); }            
    const nw::math::VEC3& GetTargetPosition() const { return ref().m_TargetPosition; }                       
    void SetTargetPosition(const nw::math::VEC3& value) { ref().m_TargetPosition = value; }

    f32 GetTwist() const { return static_cast<f32>(ref().m_Twist); }           
    void SetTwist(f32 value) { ref().m_Twist = value; }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }
};

class ResLookAtTargetViewUpdater : public ResCameraViewUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResLookAtTargetViewUpdater) 
    };
    
    NW_RES_CTOR_INHERIT(ResLookAtTargetViewUpdater, ResCameraViewUpdater)

    void SetTargetPosition(f32 x, f32 y, f32 z) { ref().m_TargetPosition.Set(x, y, z); }            
    const nw::math::VEC3& GetTargetPosition() const { return ref().m_TargetPosition; }                       
    void SetTargetPosition(const nw::math::VEC3& value) { ref().m_TargetPosition = value; }

    void SetUpwardVector(f32 x, f32 y, f32 z) { ref().m_UpwardVector.Set(x, y, z); }            
    const nw::math::VEC3& GetUpwardVector() const { return ref().m_UpwardVector; }                       
    void SetUpwardVector(const nw::math::VEC3& value) { ref().m_UpwardVector = value; }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }
};

class ResRotateViewUpdater : public ResCameraViewUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResRotateViewUpdater) 
    };
    
    NW_RES_CTOR_INHERIT(ResRotateViewUpdater, ResCameraViewUpdater)


    void SetViewRotate(f32 x, f32 y, f32 z) { ref().m_ViewRotate.Set(x, y, z); }            
    const nw::math::VEC3& GetViewRotate() const { return ref().m_ViewRotate; }                       
    void SetViewRotate(const nw::math::VEC3& value) { ref().m_ViewRotate = value; }
    u32 GetFlags() const { return static_cast<u32>(ref().m_Flags); }
    void SetFlags(u32 value) { ref().m_Flags = value; }
    void EnableFlags(u32 value) { ref().m_Flags = nw::ut::EnableFlag(ref().m_Flags, value); }
    void DisableFlags(u32 value) { ref().m_Flags = nw::ut::DisableFlag(ref().m_Flags, value); }
};

class ResFrustumProjectionUpdater : public ResCameraProjectionUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResFrustumProjectionUpdater) 
    };
    
    NW_RES_CTOR_INHERIT( ResFrustumProjectionUpdater, ResCameraProjectionUpdater )


    void SetRectWithoutFlip(f32 l, f32 r, f32 b, f32 t) { ref().m_Rect.SetWithoutFlip(l, r, b, t); } 
    void SetRectWithYFlip(f32 l, f32 r, f32 b, f32 t) { ref().m_Rect.SetWithYFlip(l, r, b, t); } 
    const ResProjectionRect& GetRect() const { return ref().m_Rect; }                       
    void SetRect(const ResProjectionRect& value) { ref().m_Rect = value; }
};

class ResOrthoProjectionUpdater : public ResCameraProjectionUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResOrthoProjectionUpdater) 
    };
    
    NW_RES_CTOR_INHERIT(ResOrthoProjectionUpdater, ResCameraProjectionUpdater)


    void SetRectWithoutFlip(f32 l, f32 r, f32 b, f32 t) { ref().m_Rect.SetWithoutFlip(l, r, b, t); } 
    void SetRectWithYFlip(f32 l, f32 r, f32 b, f32 t) { ref().m_Rect.SetWithYFlip(l, r, b, t); } 
    const ResProjectionRect& GetRect() const { return ref().m_Rect; }                       
    void SetRect(const ResProjectionRect& value) { ref().m_Rect = value; }
};


class ResPerspectiveProjectionUpdater : public ResCameraProjectionUpdater
{
public:
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResPerspectiveProjectionUpdater) 
    };
    
    NW_RES_CTOR_INHERIT(ResPerspectiveProjectionUpdater, ResCameraProjectionUpdater)


    f32 GetAspectRatio() const { return static_cast<f32>(ref().m_AspectRatio); }           
    void SetAspectRatio(f32 value) { ref().m_AspectRatio = value; }

    f32 GetFovy() const { return static_cast<f32>(ref().m_Fovy); }           
    void SetFovy(f32 value) { ref().m_Fovy = value; }
};

class ResCamera : public ResTransformNode
{
public:
    enum
    { 
        BINARY_REVISION = REVISION_RES_CAMERA 
    };
    enum
    { 
        TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResCamera) 
    };

    enum ViewType
    {
        VIEWTYPE_AIM,
        VIEWTYPE_LOOKAT,
        VIEWTYPE_ROTATE,
        VIEWTYPE_COUNT
    };

    enum ProjType
    {
        PROJTYPE_PERSPECTIVE,
        PROJTYPE_FRUSTUM,
        PROJTYPE_ORTHO,
        PROJTYPE_COUNT
    };
    
    NW_RES_CTOR_INHERIT(ResCamera, ResTransformNode)
    
    ViewType GetViewType() const { return static_cast<ViewType>(ref().m_ViewType); }           
    void SetViewType(ViewType value) { ref().m_ViewType = value; }

    ProjType GetProjectionType() const { return static_cast<ProjType>(ref().m_ProjectionType); }           
    void SetProjectionType(ProjType value) { ref().m_ProjectionType = value; }


    ResCameraViewUpdater GetViewUpdater() { return ResCameraViewUpdater(ref().toViewUpdater.to_ptr()); }                     
    const ResCameraViewUpdater GetViewUpdater() const { return ResCameraViewUpdater(ref().toViewUpdater.to_ptr()); }

    ResCameraProjectionUpdater GetProjectionUpdater() { return ResCameraProjectionUpdater(ref().toProjectionUpdater.to_ptr()); }                     
    const ResCameraProjectionUpdater GetProjectionUpdater() const { return ResCameraProjectionUpdater(ref().toProjectionUpdater.to_ptr()); }


    f32 GetWScale() const { return static_cast<f32>(ref().m_WScale); }           
    void SetWScale(f32 value) { ref().m_WScale = value; }

    u32 GetRevision() const { return this->GetHeader().revision; }
};

typedef nw::ut::ResArrayPatricia<ResCamera>::type  ResCameraArray;

}
}
}

