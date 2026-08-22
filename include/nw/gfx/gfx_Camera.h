#pragma once


#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_Viewport.h>

namespace nw{
namespace gfx{

class Camera : public TransformNode{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Camera);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct Description : public TransformNode::Description{
        Description() {}
    };

    class DynamicBuilder{
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren){ 
            mDescription.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks){
            mDescription.maxCallbacks = maxCallbacks;
            return *this;
        }

        DynamicBuilder& ViewUpdater(CameraViewUpdater* viewUpdater) { this->mViewUpdater.Reset(viewUpdater); return *this; }

        DynamicBuilder& ProjectionUpdater(CameraProjectionUpdater* projectionUpdater) { this->mProjectionUpdater.Reset(projectionUpdater); return *this; }

        Camera* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const;

    private:
        Camera::Description mDescription;
        GfxPtr<CameraViewUpdater> mViewUpdater;
        GfxPtr<CameraProjectionUpdater> mProjectionUpdater;
    };
    
    static Camera* Create(SceneNode* parent,ResSceneObject resource,const Camera::Description& description,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResCamera resource,Description description,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, description);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResCamera resource,Description description);

    virtual void Accept(ISceneVisitor* visitor);

    ResCamera GetResCamera() { return ResStaticCast<ResCamera>(GetResSceneObject()); }

    const ResCamera GetResCamera() const { return ResStaticCast<ResCamera>(GetResSceneObject()); }

    void UpdateCameraMatrix();

    nw::math::MTX44& ProjectionMatrix() { return mProjectionMatrix; }

    const nw::math::MTX44& ProjectionMatrix() const { return mProjectionMatrix; }

    nw::math::MTX44& InverseProjectionMatrix() { return mInverseProjectionMatrix; }

    const nw::math::MTX44& InverseProjectionMatrix() const { return mInverseProjectionMatrix; }

    nw::math::MTX34& ViewMatrix() { return mViewMatrix; }

    const nw::math::MTX34& ViewMatrix() const { return mViewMatrix; }

    nw::math::MTX34& InverseViewMatrix() { return mInverseViewMatrix; }

    const nw::math::MTX34& InverseViewMatrix() const { return mInverseViewMatrix; }

    nw::math::MTX34& TextureProjectionMatrix() { return mTextureProjectionMatrix; }

    const nw::math::MTX34& TextureProjectionMatrix() const { return mTextureProjectionMatrix; }

    CameraViewUpdater* GetViewUpdater() { return this->mViewUpdater.Get(); }

    const CameraViewUpdater* GetViewUpdater() const { return this->mViewUpdater.Get(); }

    void SetViewUpdater(CameraViewUpdater* viewUpdater) { this->mViewUpdater.Reset(viewUpdater); }

    CameraViewUpdater* SwapViewUpdater(CameraViewUpdater* viewUpdater){
        NW_NULL_ASSERT(viewUpdater);
        CameraViewUpdater* cameraViewUpdater = this->mViewUpdater.Release();
        this->mViewUpdater.Reset(viewUpdater);
        return cameraViewUpdater;
    }

    CameraProjectionUpdater* GetProjectionUpdater() { return this->mProjectionUpdater.Get(); }

    const CameraProjectionUpdater* GetProjectionUpdater() const { return this->mProjectionUpdater.Get(); }

    void SetProjectionUpdater(CameraProjectionUpdater* projectionUpdater) { this->mProjectionUpdater.Reset(projectionUpdater); }

    CameraProjectionUpdater* SwapProjectionUpdater(CameraProjectionUpdater* projectionUpdater){
        NW_NULL_ASSERT(projectionUpdater);
        CameraProjectionUpdater* cameraProjectionUpdater = this->mProjectionUpdater.Release();
        this->mProjectionUpdater.Reset(projectionUpdater);
        return cameraProjectionUpdater;
    }

    f32 GetWScale() const { return this->mWScale; }

    void SetWScale(f32 wScale) { mWScale = wScale; }

    AnimGroup* GetAnimGroup() { return mAnimGroup; }

    const AnimGroup* GetAnimGroup() const { return mAnimGroup; }

    AnimObject* GetAnimObject(){
        NW_NULL_ASSERT(mAnimBinding);
        return this->mAnimBinding->GetAnimObject(0);
    }

    const AnimObject* GetAnimObject() const{
        NW_NULL_ASSERT(mAnimBinding);
        return this->mAnimBinding->GetAnimObject(0);
    }

    void SetAnimObject(AnimObject* animObject){
        NW_NULL_ASSERT(mAnimBinding);{
            return;
        }
        this->mAnimBinding->SetAnimObject(0, animObject);
    }

    nw::math::VEC3 GetPosition() const{
        return this->Transform().GetTranslate();
    }

    void SetPosition(const nw::math::VEC3& position){
        this->Transform().SetTranslate(position);
    }

    void SetPosition(f32 x, f32 y, f32 z){
        this->SetPosition(nw::math::VEC3(x, y, z));
    }

    const nw::math::VEC3& GetTargetPosition() const;

    void SetTargetPosition(const nw::math::VEC3& targetPosition);

    void SetTargetPosition(f32 x, f32 y, f32 z){
        this->SetTargetPosition(nw::math::VEC3(x, y, z));
    }

    const nw::math::VEC3& GetUpwardVector() const;

    void SetUpwardVector(const nw::math::VEC3& upwardVector);

    void SetUpwardVector(f32 x, f32 y, f32 z){
        this->SetUpwardVector(nw::math::VEC3(x, y, z));
    }

    f32 GetTwist() const;

    void SetTwist(f32 twist);

    const nw::math::VEC3& GetViewRotate() const;

    void SetViewRotate(const nw::math::VEC3& viewRotate);

    void SetViewRotate(f32 x, f32 y, f32 z){this->SetViewRotate(nw::math::VEC3(x, y, z));}

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


    struct ResCameraDestroyer : public std::unary_function<ResCamera, void>{
        ResCameraDestroyer(os::IAllocator* allocator = 0) : 
            mAllocator(allocator)
        {}
        result_type operator()(argument_type data){
            DestroyResCamera(mAllocator, data);
        }

        nw::os::IAllocator* mAllocator;
    };
    
private:
    virtual Result Initialize(nw::os::IAllocator* allocator);

    Result StoreOriginal(nw::os::IAllocator* allocator);

    static void DestroyResCamera(nw::os::IAllocator* allocator, ResCamera resCamera);

    Result CreateAnimGroup(nw::os::IAllocator* allocator);

    void* GetAnimTargetObject(const anim::ResAnimGroupMember& anim);

    bool ValidateCameraAnimType(AnimObject* animObject);

    nw::math::MTX34 mViewMatrix;
    nw::math::MTX34 mInverseViewMatrix;
    nw::math::MTX44 mProjectionMatrix;
    nw::math::MTX44 mInverseProjectionMatrix;
    nw::math::MTX34 mTextureProjectionMatrix;

    GfxPtr<CameraViewUpdater> mViewUpdater;
    GfxPtr<CameraProjectionUpdater> mProjectionUpdater;
    AnimGroup* mAnimGroup;
    ResCamera mOriginalValue;
    math::Transform3 mOriginalTransform;
    f32 mWScale;
    bool mIsDynamic;
};

}
}