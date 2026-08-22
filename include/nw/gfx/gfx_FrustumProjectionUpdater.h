#pragma once

#include <nw/gfx/gfx_CameraProjectionUpdater.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class FrustumProjectionUpdater : public CameraProjectionUpdater{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(FrustumProjectionUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static FrustumProjectionUpdater* Create(nw::os::IAllocator* allocator);
    static FrustumProjectionUpdater* Create(nw::os::IAllocator* allocator,ResFrustumProjectionUpdater resUpdater);

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,bool isDynamicBuild){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(FrustumProjectionUpdater);
        if (isDynamicBuild){
            size += sizeof(ResFrustumProjectionUpdaterData);
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
        return anim::ResCameraAnimData::PROJECTION_UPDATER_FRUSTUM;
    }
    
private:
    FrustumProjectionUpdater(nw::os::IAllocator* pAllocator,bool isDynamic,ResFrustumProjectionUpdater resUpdater);

    virtual ~FrustumProjectionUpdater();

    ResFrustumProjectionUpdater mResource;
};

}
}