/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_CAMERA_H_
#define NW_GFX_CAMERA_H_

#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_Viewport.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class Camera : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Camera);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    struct Description : public TransformNode::Description
    {
        //
        Description()
        {}
    };

    //----------------------------------------
    //
    //

    //
    //
    //
    class DynamicBuilder
    {
    public:
        //
        DynamicBuilder() {}

        //
        ~DynamicBuilder() {}

        //
        //
        //
        //
        //
        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        //
        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        //
        //
        DynamicBuilder& ViewUpdater(CameraViewUpdater* viewUpdater) { m_ViewUpdater.Reset(viewUpdater); return *this; }

        //
        //
        DynamicBuilder& ProjectionUpdater(CameraProjectionUpdater* projectionUpdater) { m_ProjectionUpdater.Reset(projectionUpdater); return *this; }

        //
        //
        //
        //
        //
        //
        Camera* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        Camera::Description m_Description;
        GfxPtr<CameraViewUpdater> m_ViewUpdater;
        GfxPtr<CameraProjectionUpdater> m_ProjectionUpdater;
    };

    //
    //
    //
    //
    //
    //
    //
    //
    //
    static Camera* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const Camera::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResCamera resource,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT
    )
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResCamera resource,
        Description description);

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //

    //----------------------------------------
    //
    //

    //
    ResCamera GetResCamera() { return ResStaticCast<ResCamera>(GetResSceneObject()); }

    //
    const ResCamera GetResCamera() const { return ResStaticCast<ResCamera>(GetResSceneObject()); }

    //

    //----------------------------------------
    //
    //

    //
    void UpdateCameraMatrix();

    //
    math::MTX44& ProjectionMatrix() { return m_ProjectionMatrix; }

    //
    const math::MTX44& ProjectionMatrix() const { return m_ProjectionMatrix; }

    //
    math::MTX44& InverseProjectionMatrix() { return m_InverseProjectionMatrix; }

    //
    const math::MTX44& InverseProjectionMatrix() const { return m_InverseProjectionMatrix; }

    //
    math::MTX34& ViewMatrix() { return m_ViewMatrix; }

    //
    const math::MTX34& ViewMatrix() const { return m_ViewMatrix; }

    //
    math::MTX34& InverseViewMatrix() { return m_InverseViewMatrix; }

    //
    const math::MTX34& InverseViewMatrix() const { return m_InverseViewMatrix; }

    //
    math::MTX34& TextureProjectionMatrix() { return m_TextureProjectionMatrix; }

    //
    const math::MTX34& TextureProjectionMatrix() const { return m_TextureProjectionMatrix; }

    //

    //----------------------------------------
    //
    //

    //
    CameraViewUpdater* GetViewUpdater() { return this->m_ViewUpdater.Get(); }

    //
    const CameraViewUpdater* GetViewUpdater() const { return this->m_ViewUpdater.Get(); }

    //
    void SetViewUpdater(CameraViewUpdater* viewUpdater) { this->m_ViewUpdater.Reset(viewUpdater); }

    //
    //
    //
    //
    //
    //
    //
    CameraViewUpdater* SwapViewUpdater(CameraViewUpdater* viewUpdater)
    {
        NW_NULL_ASSERT(viewUpdater);
        CameraViewUpdater* cameraViewUpdater = this->m_ViewUpdater.Release();
        this->m_ViewUpdater.Reset(viewUpdater);
        return cameraViewUpdater;
    }

    //
    CameraProjectionUpdater* GetProjectionUpdater() { return this->m_ProjectionUpdater.Get(); }

    //
    const CameraProjectionUpdater* GetProjectionUpdater() const { return this->m_ProjectionUpdater.Get(); }

    //
    void SetProjectionUpdater(CameraProjectionUpdater* projectionUpdater) { this->m_ProjectionUpdater.Reset(projectionUpdater); }


    //
    //
    //
    //
    //
    //
    //
    CameraProjectionUpdater* SwapProjectionUpdater(CameraProjectionUpdater* projectionUpdater)
    {
        NW_NULL_ASSERT(projectionUpdater);
        CameraProjectionUpdater* cameraProjectionUpdater = this->m_ProjectionUpdater.Release();
        this->m_ProjectionUpdater.Reset(projectionUpdater);
        return cameraProjectionUpdater;
    }

    //

    //----------------------------------------
    //
    //

    //
    f32 GetWScale() const { return this->m_WScale; }

    //
    void SetWScale(f32 wScale) { m_WScale = wScale; }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    //
    //
    //
    //
    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    //
    AnimObject* GetAnimObject()
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return m_AnimBinding->GetAnimObject(0);
    }

    //
    const AnimObject* GetAnimObject() const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return m_AnimBinding->GetAnimObject(0);
    }

    //
    //
    //
    void SetAnimObject(AnimObject* animObject)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        NW_FAILSAFE_IF(!ValidateCameraAnimType(animObject))
        {
            NW_LOG("type mismatch between Camera and Animation. Animation did not set.\n");
            return;
        }
        m_AnimBinding->SetAnimObject(0, animObject);
    }

    //

    //----------------------------------------
    //
    //

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    nw::math::VEC3 GetPosition() const
    {
        return Transform().GetTranslate();
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetPosition(const nw::math::VEC3& position)
    {
        Transform().SetTranslate(position);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetPosition(f32 x, f32 y, f32 z)
    {
        SetPosition(nw::math::VEC3(x, y, z));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const nw::math::VEC3& GetTargetPosition() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetTargetPosition(const nw::math::VEC3& targetPosition);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetTargetPosition(f32 x, f32 y, f32 z)
    {
        SetTargetPosition(nw::math::VEC3(x, y, z));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const nw::math::VEC3& GetUpwardVector() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetUpwardVector(const nw::math::VEC3& upwardVector);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetUpwardVector(f32 x, f32 y, f32 z)
    {
        SetUpwardVector(nw::math::VEC3(x, y, z));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    f32 GetTwist() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetTwist(f32 twist);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    const nw::math::VEC3& GetViewRotate() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetViewRotate(const nw::math::VEC3& viewRotate);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetViewRotate(f32 x, f32 y, f32 z){
        SetViewRotate(nw::math::VEC3(x, y, z));
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetPerspective(
        f32* fovy,
        f32* aspectRatio,
        f32* nearClip,
        f32* farClip
    ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetPerspective(
        f32 fovy,
        f32 aspectRatio,
        f32 nearClip,
        f32 farClip
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_DEPRECATED_FUNCTION(void GetFrustum(
        f32* left,
        f32* right,
        f32* bottom,
        f32* top,
        f32* nearClip,
        f32* farClip
    ) const)
    {
        GetFrustumWithYFlip(left, right, bottom, top, nearClip, farClip);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetFrustumWithoutFlip(
        f32* left,
        f32* right,
        f32* bottom,
        f32* top,
        f32* nearClip,
        f32* farClip
    ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetFrustumWithYFlip(
        f32* left,
        f32* right,
        f32* bottom,
        f32* top,
        f32* nearClip,
        f32* farClip
    ) const
    {
        GetFrustumWithoutFlip(left, right, top, bottom, nearClip, farClip);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetFrustum(
        f32 left,
        f32 right,
        f32 bottom,
        f32 top,
        f32 nearClip,
        f32 farClip
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_DEPRECATED_FUNCTION(void GetFrustum(
        ut::Rect* rect,
        f32* nearClip,
        f32* farClip
    ) const)
    {
        GetFrustumWithYFlip(rect, nearClip, farClip);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetFrustumWithoutFlip(
        ut::Rect* rect,
        f32* nearClip,
        f32* farClip
    ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetFrustumWithYFlip(
        ut::Rect* rect,
        f32* nearClip,
        f32* farClip
    ) const
    {
        GetFrustumWithoutFlip(rect, nearClip, farClip);
        f32 temp = rect->top;
        rect->top = rect->bottom;
        rect->bottom = temp;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetFrustum(
        const ut::Rect& rect,
        f32 nearClip,
        f32 farClip
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetFrustum(
        const Viewport& viewport
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_DEPRECATED_FUNCTION(void GetOrtho(
        f32* left,
        f32* right,
        f32* bottom,
        f32* top,
        f32* nearClip,
        f32* farClip
    ) const)
    {
        GetOrthoWithYFlip(left, right, bottom, top, nearClip, farClip);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetOrthoWithoutFlip(
        f32* left,
        f32* right,
        f32* bottom,
        f32* top,
        f32* nearClip,
        f32* farClip
    ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetOrthoWithYFlip(
        f32* left,
        f32* right,
        f32* bottom,
        f32* top,
        f32* nearClip,
        f32* farClip
    ) const
    {
        GetOrthoWithoutFlip(left, right, top, bottom, nearClip, farClip);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetOrtho(
        f32 left,
        f32 right,
        f32 bottom,
        f32 top,
        f32 nearClip,
        f32 farClip
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    NW_DEPRECATED_FUNCTION(void GetOrtho(
        ut::Rect* rect,
        f32* nearClip,
        f32* farClip
    ) const)
    {
        GetOrthoWithYFlip(rect, nearClip, farClip);
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetOrthoWithoutFlip(
        ut::Rect* rect,
        f32* nearClip,
        f32* farClip
    ) const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void GetOrthoWithYFlip(
        ut::Rect* rect,
        f32* nearClip,
        f32* farClip
    ) const
    {
        GetOrthoWithoutFlip(rect, nearClip, farClip);
        f32 temp = rect->top;
        rect->top = rect->bottom;
        rect->bottom = temp;
    }

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetOrtho(
        const ut::Rect& rect,
        f32 nearClip,
        f32 farClip
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetOrtho(
        const Viewport& viewport
    );

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    f32 GetNear() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetNear(f32 near);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    f32 GetFar() const;

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    void SetFar(f32 far);

    //

protected:
    //----------------------------------------
    //
    //

    //
    Camera(
        os::IAllocator* allocator,
        ResTransformNode resObj,
        const Camera::Description& description,
        GfxPtr<CameraViewUpdater> viewUpdater,
        GfxPtr<CameraProjectionUpdater> projectionUpdater,
        f32 wscale,
        bool isDynamic);

    //
    virtual ~Camera();

    //

    struct ResCameraDestroyer : public std::unary_function<ResCamera, void>
    {
        ResCameraDestroyer(os::IAllocator* allocator = 0) : m_Allocator(allocator)
        {}
        result_type operator()(argument_type data)
        {
            DestroyResCamera(m_Allocator, data);
        }

        os::IAllocator* m_Allocator;
    };

    virtual Result Initialize(os::IAllocator* allocator);

private:
    //
    Result StoreOriginal(os::IAllocator* allocator);

    //---------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    static void DestroyResCamera(os::IAllocator* allocator, ResCamera resCamera);

    Result CreateAnimGroup(os::IAllocator* allocator);

    //
    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

    //
    //  Check whether the view and the projection's update match for the animation and the camera.
    bool ValidateCameraAnimType(AnimObject* animObject);

    math::MTX34 m_ViewMatrix;
    math::MTX34 m_InverseViewMatrix;
    math::MTX44 m_ProjectionMatrix;
    math::MTX44 m_InverseProjectionMatrix;
    math::MTX34 m_TextureProjectionMatrix;

    GfxPtr<CameraViewUpdater> m_ViewUpdater;
    GfxPtr<CameraProjectionUpdater> m_ProjectionUpdater;

    AnimGroup* m_AnimGroup;

    ResCamera m_OriginalValue;
    math::Transform3 m_OriginalTransform;
    f32 m_WScale;
    bool m_IsDynamic;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_CAMERA_H_
