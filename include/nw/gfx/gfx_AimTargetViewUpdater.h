#pragma once

#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/ut/ut_Preprocessor.h>

namespace nw{
namespace os{
    class IAllocator;
}

namespace gfx{

class AimTargetViewUpdater : public CameraViewUpdater{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(AimTargetViewUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static AimTargetViewUpdater* Create(nw::os::IAllocator* allocator);

    static AimTargetViewUpdater* Create(nw::os::IAllocator* allocator,ResAimTargetViewUpdater resUpdater);

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,bool isDynamicBuild){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(AimTargetViewUpdater);
        if (isDynamicBuild){
            size += sizeof(ResAimTargetViewUpdaterData);
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
        return anim::ResCameraAnimData::VIEW_UPDATER_AIM;
    }
    
private:
    AimTargetViewUpdater(nw::os::IAllocator* pAllocator,bool isDynamic,ResAimTargetViewUpdater resUpdater);
    virtual ~AimTargetViewUpdater();


    ResAimTargetViewUpdater mResource;
};

}
}