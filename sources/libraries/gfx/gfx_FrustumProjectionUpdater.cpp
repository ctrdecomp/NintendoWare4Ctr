#include <nw/gfx/gfx_FrustumProjectionUpdater.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Rect.h>

#include <nn/math/inline/math_Matrix34.ipp>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(FrustumProjectionUpdater,CameraProjectionUpdater);

FrustumProjectionUpdater* FrustumProjectionUpdater::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(FrustumProjectionUpdater));
    NW_NULL_ASSERT(updaterMemory);

    void* dataMemory = AllocateAndFill<ResFrustumProjectionUpdaterData>(allocator, 0);

    ResFrustumProjectionUpdaterData* buffer =
            new(dataMemory) ResFrustumProjectionUpdaterData();

    buffer->typeInfo = ResFrustumProjectionUpdater::TYPE_INFO;
    buffer->mNear = PROJECTION_NEAR_CLIP;
    buffer->mFar = PROJECTION_FAR_CLIP;
    buffer->mRect.mAspectRatio = PROJECTION_ASPECT_RATIO;
    buffer->mRect.mCenter = PROJECTION_CENTER;
    buffer->mRect.mHeight = PROJECTION_HEIGHT;

    ResFrustumProjectionUpdater resUpdater = ResFrustumProjectionUpdater(buffer);

    return new(updaterMemory) FrustumProjectionUpdater(allocator, true, resUpdater);
}

FrustumProjectionUpdater* FrustumProjectionUpdater::Create(os::IAllocator* allocator, ResFrustumProjectionUpdater resUpdater){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(FrustumProjectionUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) FrustumProjectionUpdater(allocator, false, resUpdater);
}

FrustumProjectionUpdater::FrustumProjectionUpdater(os::IAllocator* allocator,bool isDynamic,ResFrustumProjectionUpdater resUpdater): 
    CameraProjectionUpdater(allocator, isDynamic),
    mResource(resUpdater)
{}

FrustumProjectionUpdater::~FrustumProjectionUpdater(){
    if (this->IsDynamic() && this->mResource.IsValid()){
        this->GetAllocator().Free(mResource.ptr());
    }
}

void FrustumProjectionUpdater::Update(math::MTX44* projectionMatrix, math::MTX34* textureProjectionMatrix){
    NW_ASSERT(this->mResource.IsValid());

    float halfWidth = this->mResource.GetRect().GetWidth() / 2.0f;
    float halfHeight = this->mResource.GetRect().GetHeight() / 2.0f;

    float left = this->mResource.GetRect().GetCenter().x - halfWidth;
    float right = this->mResource.GetRect().GetCenter().x + halfWidth;
    float bottom = this->mResource.GetRect().GetCenter().y - halfHeight;
    float top = this->mResource.GetRect().GetCenter().y + halfHeight;
    float near = this->mResource.GetNear();
    float far = this->mResource.GetFar();

    math::MTX44FrustumPivot(projectionMatrix,left,right,bottom,top,near,far,this->GetPivotDirection());

    math::MTX34TextureProjectionFrustum(textureProjectionMatrix,left,right,bottom,top,near,
        this->TextureScale().x,this->TextureScale().y,
        this->TextureTranslate().x,this->TextureTranslate().y);
}

}
}