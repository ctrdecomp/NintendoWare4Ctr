#include <nw/gfx/gfx_PerspectiveProjectionUpdater.h>
#include <nw/os/os_Memory.h>
#include <nn/math/inline/math_Matrix34.ipp>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(PerspectiveProjectionUpdater,CameraProjectionUpdater);

PerspectiveProjectionUpdater* PerspectiveProjectionUpdater::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(PerspectiveProjectionUpdater));
    NW_NULL_ASSERT(updaterMemory);

    void* dataMemory = AllocateAndFill<ResPerspectiveProjectionUpdaterData>(allocator, 0);

    ResPerspectiveProjectionUpdaterData* buffer = new(dataMemory) ResPerspectiveProjectionUpdaterData();

    buffer->typeInfo = ResPerspectiveProjectionUpdater::TYPE_INFO;
    buffer->mNear = PROJECTION_NEAR_CLIP;
    buffer->mFar = PROJECTION_FAR_CLIP;
    buffer->mFovy = PROJECTION_FOVY_RADIAN;
    buffer->mAspectRatio = PROJECTION_ASPECT_RATIO;

    ResPerspectiveProjectionUpdater resUpdater = ResPerspectiveProjectionUpdater(buffer);

    return new(updaterMemory) PerspectiveProjectionUpdater(allocator, true, resUpdater);
}

PerspectiveProjectionUpdater* PerspectiveProjectionUpdater::Create(os::IAllocator* allocator,ResPerspectiveProjectionUpdater resUpdater){
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(PerspectiveProjectionUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) PerspectiveProjectionUpdater(allocator, false, resUpdater);
}

PerspectiveProjectionUpdater::PerspectiveProjectionUpdater(os::IAllocator* allocator,bool isDynamic,ResPerspectiveProjectionUpdater resUpdater): 
    CameraProjectionUpdater(allocator, isDynamic),
    mResource(resUpdater)
{}

PerspectiveProjectionUpdater::~PerspectiveProjectionUpdater(){
    if (this->IsDynamic() && this->mResource.IsValid()){
        this->GetAllocator().Free(this->mResource.ptr());
    }
}

void PerspectiveProjectionUpdater::Update(math::MTX44* projectionMatrix, math::MTX34* textureProjectionMatrix){
    NW_ASSERT(m_Resource.IsValid());

    float fovy = this->mResource.GetFovy();
    NW_ASSERT(0 < fovy && fovy < nw::math::F_PI);

    float aspect = this->mResource.GetAspectRatio();
    NW_ASSERT(aspect != 0);

    float near = this->mResource.GetNear();
    float far = this->mResource.GetFar();
    NW_ASSERT(near != far);

    math::MTX44PerspectivePivotRad(projectionMatrix,fovy,aspect,near,far,this->GetPivotDirection());

    math::MTX34TextureProjectionPerspective(textureProjectionMatrix,fovy,aspect,this->TextureScale().x,this->TextureScale().y,this->TextureTranslate().x,this->TextureTranslate().y);
}

}
}