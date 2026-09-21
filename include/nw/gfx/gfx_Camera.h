#pragma once


#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_Viewport.h>

namespace nw{
namespace gfx{

class Camera : public TransformNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Camera);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description
    {
        Description() {}
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        DynamicBuilder& ViewUpdater(CameraViewUpdater* viewUpdater) { this->m_ViewUpdater.Reset(viewUpdater); return *this; }

        DynamicBuilder& ProjectionUpdater(CameraProjectionUpdater* projectionUpdater) { this->m_ProjectionUpdater.Reset(projectionUpdater); return *this; }

        Camera* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        Camera::Description m_Description;
        GfxPtr<CameraViewUpdater> m_ViewUpdater;
        GfxPtr<CameraProjectionUpdater> m_ProjectionUpdater;
    };
    
    static Camera* Create(SceneNode* parent,ResSceneObject resource,const Camera::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResCamera resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResCamera resource,Description description);

    virtual void Accept(ISceneVisitor* visitor);

    ResCamera GetResCamera() { return ResStaticCast<ResCamera>(GetResSceneObject()); }

    const ResCamera GetResCamera() const { return ResStaticCast<ResCamera>(GetResSceneObject()); }

    void UpdateCameraMatrix();

    nw::math::MTX44& ProjectionMatrix() { return m_ProjectionMatrix; }

    const nw::math::MTX44& ProjectionMatrix() const { return m_ProjectionMatrix; }

    nw::math::MTX44& InverseProjectionMatrix() { return m_InverseProjectionMatrix; }

    const nw::math::MTX44& InverseProjectionMatrix() const { return m_InverseProjectionMatrix; }

    nw::math::MTX34& ViewMatrix() { return m_ViewMatrix; }

    const nw::math::MTX34& ViewMatrix() const { return m_ViewMatrix; }

    nw::math::MTX34& InverseViewMatrix() { return m_InverseViewMatrix; }

    const nw::math::MTX34& InverseViewMatrix() const { return m_InverseViewMatrix; }

    nw::math::MTX34& TextureProjectionMatrix() { return m_TextureProjectionMatrix; }

    const nw::math::MTX34& TextureProjectionMatrix() const { return m_TextureProjectionMatrix; }

    CameraViewUpdater* GetViewUpdater() { return this->m_ViewUpdater.Get(); }

    const CameraViewUpdater* GetViewUpdater() const { return this->m_ViewUpdater.Get(); }

    void SetViewUpdater(CameraViewUpdater* viewUpdater) { this->m_ViewUpdater.Reset(viewUpdater); }

    CameraViewUpdater* SwapViewUpdater(CameraViewUpdater* viewUpdater)
    {
        NW_NULL_ASSERT(viewUpdater);
        CameraViewUpdater* cameraViewUpdater = this->m_ViewUpdater.Release();
        this->m_ViewUpdater.Reset(viewUpdater);
        return cameraViewUpdater;
    }

    CameraProjectionUpdater* GetProjectionUpdater() { return this->m_ProjectionUpdater.Get(); }

    const CameraProjectionUpdater* GetProjectionUpdater() const { return this->m_ProjectionUpdater.Get(); }

    void SetProjectionUpdater(CameraProjectionUpdater* projectionUpdater) { this->m_ProjectionUpdater.Reset(projectionUpdater); }

    CameraProjectionUpdater* SwapProjectionUpdater(CameraProjectionUpdater* projectionUpdater)
    {
        NW_NULL_ASSERT(projectionUpdater);
        CameraProjectionUpdater* cameraProjectionUpdater = this->m_ProjectionUpdater.Release();
        this->m_ProjectionUpdater.Reset(projectionUpdater);
        return cameraProjectionUpdater;
    }

    f32 GetWScale() const { return this->m_WScale; }

    void SetWScale(f32 wScale) { m_WScale = wScale; }

    AnimGroup* GetAnimGroup() { return m_AnimGroup; }

    const AnimGroup* GetAnimGroup() const { return m_AnimGroup; }

    AnimObject* GetAnimObject()
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(0);
    }

    const AnimObject* GetAnimObject() const
    {
        NW_NULL_ASSERT(m_AnimBinding);
        return this->m_AnimBinding->GetAnimObject(0);
    }

    void SetAnimObject(AnimObject* animObject)
    {
        NW_NULL_ASSERT(m_AnimBinding);
        {
            return;
        }
        this->m_AnimBinding->SetAnimObject(0, animObject);
    }

    nw::math::VEC3 GetPosition() const
    {
        return this->Transform().GetTranslate();
    }

    void SetPosition(const nw::math::VEC3& position)
    {
        this->Transform().SetTranslate(position);
    }

    void SetPosition(f32 x, f32 y, f32 z)
    {
        this->SetPosition(nw::math::VEC3(x, y, z));
    }

    const nw::math::VEC3& GetTargetPosition() const;

    void SetTargetPosition(const nw::math::VEC3& targetPosition);

    void SetTargetPosition(f32 x, f32 y, f32 z)
    {
        this->SetTargetPosition(nw::math::VEC3(x, y, z));
    }

    const nw::math::VEC3& GetUpwardVector() const;

    void SetUpwardVector(const nw::math::VEC3& upwardVector);

    void SetUpwardVector(f32 x, f32 y, f32 z)
    {
        this->SetUpwardVector(nw::math::VEC3(x, y, z));
    }

    f32 GetTwist() const;

    void SetTwist(f32 twist);

    const nw::math::VEC3& GetViewRotate() const;

    void SetViewRotate(const nw::math::VEC3& viewRotate);

    void SetViewRotate(f32 x, f32 y, f32 z) {this->SetViewRotate(nw::math::VEC3(x, y, z));}

    void GetPerspective(f32* fovy,f32* aspectRatio,f32* nearClip,f32* farClip) const;

    void SetPerspective(f32 fovy,f32 aspectRatio,f32 nearClip,f32 farClip);

    void GetFrustum(f32* left,f32* right,f32* bottom,f32* top,f32* nearClip,f32* farClip) const;

    void SetFrustum(f32 left,f32 right,f32 bottom,f32 top,f32 nearClip,f32 farClip);

    void GetFrustum(nw::ut::Rect* rect,f32* nearClip,f32* farClip) const;

    void SetFrustum(const nw::ut::Rect& rect,f32 nearClip,f32 farClip);

    void SetFrustum(const Viewport& viewport);

    void GetOrtho(f32* left,f32* right,f32* bottom,f32* top,f32* nearClip,f32* farClip) const;

    void SetOrtho(f32 left,f32 right,f32 bottom,f32 top,f32 nearClip,f32 farClip);

    void GetOrtho(nw::ut::Rect* rect,f32* nearClip,f32* farClip) const;

    void SetOrtho(const nw::ut::Rect& rect,f32 nearClip,f32 farClip);

    void SetOrtho(const Viewport& viewport);

    f32 GetNear() const;

    void SetNear(f32 near);

    f32 GetFar() const;

    void SetFar(f32 far);
protected:

    Camera(nw::os::IAllocator* allocator,ResTransformNode resObj,const Camera::Description& description,GfxPtr<CameraViewUpdater> viewUpdater,
        GfxPtr<CameraProjectionUpdater> projectionUpdater,
        f32 wscale,
        bool isDynamic);

    virtual ~Camera();


    struct ResCameraDestroyer : public std::unary_function<ResCamera, void>
    {
        ResCameraDestroyer(os::IAllocator* allocator = 0) : 
            m_Allocator(allocator) {}
        result_type operator()(argument_type data)
        {
            DestroyResCamera(m_Allocator, data);
        }

        nw::os::IAllocator* m_Allocator;
    };
    
private:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    Result StoreOriginal(nw::os::IAllocator* allocator);

    static void DestroyResCamera(nw::os::IAllocator* allocator, ResCamera resCamera);

    Result CreateAnimGroup(nw::os::IAllocator* allocator);

    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

    bool ValidateCameraAnimType(AnimObject* animObject);

    nw::math::MTX34 m_ViewMatrix;
    nw::math::MTX34 m_InverseViewMatrix;
    nw::math::MTX44 m_ProjectionMatrix;
    nw::math::MTX44 m_InverseProjectionMatrix;
    nw::math::MTX34 m_TextureProjectionMatrix;

    GfxPtr<CameraViewUpdater> m_ViewUpdater;
    GfxPtr<CameraProjectionUpdater> m_ProjectionUpdater;
    AnimGroup* m_AnimGroup;
    ResCamera m_OriginalValue;
    math::Transform3 m_OriginalTransform;
    f32 m_WScale;
    bool m_IsDynamic;
};

}
}