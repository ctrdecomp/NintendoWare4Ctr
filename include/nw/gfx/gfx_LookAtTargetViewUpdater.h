#pragma once

#include <nw/gfx/gfx_CameraViewUpdater.h>
#include <nw/ut/ut_Preprocessor.h>


namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class LookAtTargetViewUpdater : public CameraViewUpdater
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(LookAtTargetViewUpdater);

public:
    NW_UT_RUNTIME_TYPEINFO;

    static LookAtTargetViewUpdater* Create(nw::os::IAllocator* allocator);

    static LookAtTargetViewUpdater* Create(nw::os::IAllocator* allocator,ResLookAtTargetViewUpdater resUpdater);

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,bool isDynamicBuild)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(LookAtTargetViewUpdater);
        if (isDynamicBuild)
        {
            size += sizeof(ResLookAtTargetViewUpdaterData);
        }
    }

    void virtual Update(nw::math::MTX34* viewMatrix,const nw::math::MTX34& worldMatrix,const nw::math::VEC3& cameraPosition);

    virtual ResCameraViewUpdater GetResource()
    {
        return this->m_Resource;
    }

    virtual const ResCameraViewUpdater GetResource() const
    {
        return this->m_Resource;
    }

    virtual anim::ResCameraAnimData::ViewUpdaterKind Kind() const
    {
        return anim::ResCameraAnimData::VIEW_UPDATER_LOOKAT;
    }
    
private:
    LookAtTargetViewUpdater(nw::os::IAllocator* pAllocator,bool isDynamic,ResLookAtTargetViewUpdater resUpdater);
    virtual ~LookAtTargetViewUpdater();

    ResLookAtTargetViewUpdater m_Resource;
};

}
}