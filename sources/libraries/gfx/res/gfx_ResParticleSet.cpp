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

void ResParticleSet::Setup()
{
    NW_ASSERT(this->IsValid());
}

}
}
}