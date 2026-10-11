#pragma once

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/gfx/res/gfx_ResModel.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResRevision.h>
#include <nw/gfx/res/gfx_ResTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {

struct ResParticleModelData : public ResModelData
{
    nw::ut::ResS32 m_ParticleSetsTableCount;
    nw::ut::Offset toParticleSetsTable;
};

class ResParticleModel : public ResModel
{
public:
    enum
{ TYPE_INFO = NW_GFX_RES_TYPE_INFO(ResParticleModel) };
    enum
{ SIGNATURE = NW_RES_SIGNATURE32('PMDL') };
    enum
{ BINARY_REVISION = REVISION_RES_EMITTER };

    NW_RES_CTOR_INHERIT(ResParticleModel, ResModel)


    s32 GetParticleSetsCount() const
        { return ref().m_ParticleSetsTableCount; }
    ResParticleSet GetParticleSets(int idx)
        {
            nw::ut::Offset* tbl_ptr = static_cast<nw::ut::Offset*>(ref().toParticleSetsTable.to_table_ptr());
            return ResParticleSet(tbl_ptr[idx].to_ptr());
        }
    const ResParticleSet GetParticleSets(int idx) const
        {
            const nw::ut::Offset* tbl_ptr = static_cast<const nw::ut::Offset*>(ref().toParticleSetsTable.to_table_ptr());
            return ResParticleSet(tbl_ptr[idx].to_ptr());
        }
    nw::ut::internal::ResArray<ResParticleSet, nw::ut::internal::ResArrayClassTraits> GetParticleSets()
        {
            return nw::ut::internal::ResArray<ResParticleSet, nw::ut::internal::ResArrayClassTraits>(ref().toParticleSetsTable.to_table_ptr(), GetParticleSetsCount());
        }
    const nw::ut::internal::ResArray<const ResParticleSet, nw::ut::internal::ResArrayClassTraits> GetParticleSets() const
        {
            return nw::ut::internal::ResArray<const ResParticleSet, nw::ut::internal::ResArrayClassTraits>(ref().toParticleSetsTable.to_table_ptr(), GetParticleSetsCount());
        }

    void Setup();

    template<typename TFunction>
    void ForeachParticleSet(TFunction function)
    {
        if (this->IsValid())
        {
            for (s32 i = 0; i < this->GetParticleSetsCount(); ++i)
            {
                ResParticleSet resParticleSet = this->GetParticleSets(i);
                function(resParticleSet);
            }
        }
    }
};

}
}
}

