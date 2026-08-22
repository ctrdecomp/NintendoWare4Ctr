#pragma once

#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class PerspectiveProjectionUpdater : public CameraProjectionUpdater{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(PerspectiveProjectionUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static PerspectiveProjectionUpdater* Create(nw::os::IAllocator* allocator);
    static PerspectiveProjectionUpdater* Create(nw::os::IAllocator* allocator,ResPerspectiveProjectionUpdater resUpdater);

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,bool isDynamicBuild){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(PerspectiveProjectionUpdater);
        if (isDynamicBuild)
        {
            size += sizeof(ResPerspectiveProjectionUpdaterData);
        }
    }

    void virtual Update(nw::math::MTX44* projectionMatrix, nw::math::MTX34* textureProjectionMatrix);

    virtual ResCameraProjectionUpdater GetResource() {
        return this->mResource;
    }

    virtual const ResCameraProjectionUpdater GetResource() const {
        return this->mResource;
    }

    virtual anim::ResCameraAnimData::ProjectionUpdaterKind Kind() const{
        return anim::ResCameraAnimData::PROJECTION_UPDATER_PERSPECTIVE;
    }
    
private:

    PerspectiveProjectionUpdater(nw::os::IAllocator* pAllocator,bool isDynamic,ResPerspectiveProjectionUpdater resUpdater);

    virtual ~PerspectiveProjectionUpdater();

    ResPerspectiveProjectionUpdater mResource;
};

}
}