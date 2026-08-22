#pragma once

#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/ut/ut_Preprocessor.h>
namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class OrthoProjectionUpdater : public CameraProjectionUpdater{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(OrthoProjectionUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static OrthoProjectionUpdater* Create(nw::os::IAllocator* allocator);

    static OrthoProjectionUpdater* Create(nw::os::IAllocator* allocator,ResOrthoProjectionUpdater resUpdater);

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,bool isDynamicBuild){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(OrthoProjectionUpdater);
        if (isDynamicBuild){
            size += sizeof(ResOrthoProjectionUpdaterData);
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
        return anim::ResCameraAnimData::PROJECTION_UPDATER_ORTHO;
    }

    
private:
    OrthoProjectionUpdater(nw::os::IAllocator* pAllocator,bool isDynamic,ResOrthoProjectionUpdater resUpdater);

    virtual ~OrthoProjectionUpdater();

    ResOrthoProjectionUpdater mResource;
};

}
}