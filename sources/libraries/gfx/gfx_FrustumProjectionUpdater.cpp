#include <nw/gfx/gfx_FrustumProjectionUpdater.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_Rect.h>

#include <nn/math/inline/math_Matrix34.ipp>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(FrustumProjectionUpdater,CameraProjectionUpdater);

FrustumProjectionUpdater* FrustumProjectionUpdater::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(FrustumProjectionUpdater));
    NW_NULL_ASSERT(updaterMemory);

    void* dataMemory = AllocateAndFill<ResFrustumProjectionUpdaterData>(allocator, 0);

    ResFrustumProjectionUpdaterData* buffer =
            new(dataMemory) ResFrustumProjectionUpdaterData();

    buffer->typeInfo = ResFrustumProjectionUpdater::TYPE_INFO;
    buffer->m_Near = PROJECTION_NEAR_CLIP;
    buffer->m_Far = PROJECTION_FAR_CLIP;
    buffer->m_Rect.m_AspectRatio = PROJECTION_ASPECT_RATIO;
    buffer->m_Rect.m_Center = PROJECTION_CENTER;
    buffer->m_Rect.m_Height = PROJECTION_HEIGHT;

    ResFrustumProjectionUpdater resUpdater = ResFrustumProjectionUpdater(buffer);

    return new(updaterMemory) FrustumProjectionUpdater(allocator, true, resUpdater);
}

FrustumProjectionUpdater* FrustumProjectionUpdater::Create(os::IAllocator* allocator, ResFrustumProjectionUpdater resUpdater)
{
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(FrustumProjectionUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) FrustumProjectionUpdater(allocator, false, resUpdater);
}

FrustumProjectionUpdater::FrustumProjectionUpdater(os::IAllocator* allocator,bool isDynamic,ResFrustumProjectionUpdater resUpdater): 
    CameraProjectionUpdater(allocator, isDynamic),
    m_Resource(resUpdater) {}

FrustumProjectionUpdater::~FrustumProjectionUpdater()
{
    if (this->IsDynamic() && this->m_Resource.IsValid())
    {
        this->GetAllocator().Free(m_Resource.ptr());
    }
}

void FrustumProjectionUpdater::Update(math::MTX44* projectionMatrix, math::MTX34* textureProjectionMatrix)
{
    NW_ASSERT(this->m_Resource.IsValid());

    float halfWidth = this->m_Resource.GetRect().GetWidth() / 2.0f;
    float halfHeight = this->m_Resource.GetRect().GetHeight() / 2.0f;

    float left = this->m_Resource.GetRect().GetCenter().x - halfWidth;
    float right = this->m_Resource.GetRect().GetCenter().x + halfWidth;
    float bottom = this->m_Resource.GetRect().GetCenter().y - halfHeight;
    float top = this->m_Resource.GetRect().GetCenter().y + halfHeight;
    float near = this->m_Resource.GetNear();
    float far = this->m_Resource.GetFar();

    math::MTX44FrustumPivot(projectionMatrix,left,right,bottom,top,near,far,this->GetPivotDirection());

    math::MTX34TextureProjectionFrustum(textureProjectionMatrix,left,right,bottom,top,near,
        this->TextureScale().x,this->TextureScale().y,
        this->TextureTranslate().x,this->TextureTranslate().y);
}

}
}