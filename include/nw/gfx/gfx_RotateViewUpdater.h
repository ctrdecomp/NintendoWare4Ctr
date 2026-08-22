#pragma once

#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw{
namespace os{
    class IAllocator;
}

namespace gfx{

class RotateViewUpdater : public CameraViewUpdater{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(RotateViewUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static RotateViewUpdater* Create(nw::os::IAllocator* allocator);

    static RotateViewUpdater* Create(nw::os::IAllocator* allocator,ResRotateViewUpdater resUpdater);

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,bool isDynamicBuild){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(RotateViewUpdater);
        if (isDynamicBuild){
            size += sizeof(ResRotateViewUpdaterData);
        }
    }

    void virtual Update(nw::math::MTX34* viewMatrix,const nw::math::MTX34& worldMatrix,const nw::math::VEC3& cameraPosition);

    virtual ResCameraViewUpdater GetResource() {
        return this->mResource;
    }

    virtual const ResCameraViewUpdater GetResource() const {
        return this->mResource;
    }

    virtual anim::ResCameraAnimData::ViewUpdaterKind Kind() const{
        return anim::ResCameraAnimData::VIEW_UPDATER_ROTATE;
    }
    
private:
    RotateViewUpdater(nw::os::IAllocator* pAllocator,bool isDynamic,ResRotateViewUpdater resUpdater);

    virtual ~RotateViewUpdater();

    ResRotateViewUpdater mResource;
};

}
}