// Filename: gfx_ResParticleModel.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResParticleSet.h>
#include <nw/gfx/res/gfx_ResParticleModel.h>
#include <nw/gfx/gfx_Common.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2extern.h>

namespace nw {
namespace gfx {
namespace res {

void ResParticleModel::Setup()
{
    NW_ASSERT(this->IsValid());
    NW_ASSERT(internal::ResCheckRevision(*this));


    s32 particleSetNum = this->GetParticleSetsCount();

    for (int i = 0; i < particleSetNum; ++i)
    {
        ResParticleSet particleSet = this->GetParticleSets( s32(i) );
        particleSet.Setup();
    }
}

}
}
}

