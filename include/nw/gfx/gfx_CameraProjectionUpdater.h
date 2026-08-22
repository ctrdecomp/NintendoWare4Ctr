#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResCamera.h>

namespace nw{
namespace ut{
    struct Rect;
}

namespace gfx{

class CameraProjectionUpdater : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(CameraProjectionUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    virtual void Update(nw::math::MTX44* projectionMatrix, nw::math::MTX34* textureProjectionMatrix) = 0;

    void SetPivotDirection(nw::math::PivotDirection pivot) { mPivot = pivot; }

    nw::math::PivotDirection GetPivotDirection() const { return mPivot; }

    bool IsDynamic() {return IsDynamic;}

    virtual ResCameraProjectionUpdater GetResource() = 0;

    virtual const ResCameraProjectionUpdater GetResource() const = 0;

    nw::math::VEC2& TextureScale() { return mTextureScale; }

    const nw::math::VEC2& TextureScale() const { return mTextureScale; }

    nw::math::VEC2& TextureTranslate() { return mTextureTranslate; }

    const nw::math::VEC2& TextureTranslate() const { return mTextureTranslate; }

    virtual anim::ResCameraAnimData::ProjectionUpdaterKind Kind() const = 0;

protected:

    CameraProjectionUpdater(os::IAllocator* allocator, bool isDynamic): 
        GfxObject(allocator),
        mPivot(math::PIVOT_NONE),
        mIsDynamic(isDynamic),
        mTextureScale(0.5f, 0.5f),
        mTextureTranslate(0.5f, 0.5f)
    {}

    virtual ~CameraProjectionUpdater(){}

private:
    PivotDirection mPivot;
    bool mIsDynamic;
    VEC2 mTextureScale;
    VEC2 mTextureTranslate;

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