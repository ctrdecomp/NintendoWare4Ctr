#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResCamera.h>

namespace nw{
namespace ut{
    struct Rect;
}

namespace gfx{

class CameraProjectionUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(CameraProjectionUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual void Update(nw::math::MTX44* projectionMatrix, nw::math::MTX34* textureProjectionMatrix) = 0;

    void SetPivotDirection(nw::math::PivotDirection pivot) { m_Pivot = pivot; }

    nw::math::PivotDirection GetPivotDirection() const { return m_Pivot; }

    bool IsDynamic() { return m_IsDynamic; }

    virtual ResCameraProjectionUpdater GetResource() = 0;

    virtual const ResCameraProjectionUpdater GetResource() const = 0;

    nw::math::VEC2& TextureScale() { return m_TextureScale; }

    const nw::math::VEC2& TextureScale() const { return m_TextureScale; }

    nw::math::VEC2& TextureTranslate() { return m_TextureTranslate; }

    const nw::math::VEC2& TextureTranslate() const { return m_TextureTranslate; }

    virtual anim::ResCameraAnimData::ProjectionUpdaterKind Kind() const = 0;

protected:

    CameraProjectionUpdater(os::IAllocator* allocator, bool isDynamic): 
        GfxObject(allocator),
        m_Pivot(math::PIVOT_NONE),
        m_IsDynamic(isDynamic),
        m_TextureScale(0.5f, 0.5f),
        m_TextureTranslate(0.5f, 0.5f) {}

    virtual ~CameraProjectionUpdater() {}

private:
    PivotDirection m_Pivot;
    bool m_IsDynamic;
    VEC2 m_TextureScale;
    VEC2 m_TextureTranslate;

protected:
    static const float PROJECTION_NEAR_CLIP;
    static const float PROJECTION_FAR_CLIP;
    static const float PROJECTION_FOVY_RADIAN;
    static const float PROJECTION_ASPECT_RATIO;
    static const VEC2 PROJECTION_CENTER;
    static const float PROJECTION_HEIGHT;
    static const ResProjectionRect PROJECTION_RECT;
};

}
}