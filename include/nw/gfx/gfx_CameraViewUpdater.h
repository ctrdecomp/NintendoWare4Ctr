#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/res/gfx_ResCamera.h>

namespace nw{
namespace math{
    struct VEC3;
    struct MTX34;
}
namespace gfx{

class CameraViewUpdater : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(CameraViewUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    void virtual Update(nw::math::MTX34* viewMatrix,const nw::math::MTX34& worldMatrix,const nw::math::VEC3& cameraPosition) = 0;

    bool IsDynamic() {return this->mIsDynamic;}

    virtual ResCameraViewUpdater GetResource() = 0;
    virtual const ResCameraViewUpdater GetResource() const = 0;
    virtual anim::ResCameraAnimData::ViewUpdaterKind Kind() const = 0;


protected:
    CameraViewUpdater(os::IAllocator* allocator, bool isDynamic): 
        GfxObject(allocator),
        mIsDynamic(isDynamic)
    {}

protected:
    static const nw::math::VEC3 VIEW_TARGET_POSITION;
    static const nw::math::VEC3 VIEW_UPWARD_VECTOR;
    static const nw::math::VEC3 VIEW_VIEW_ROTATE;
    static const float VIEW_TWIST;

private:
    bool mIsDynamic;
};

}
}