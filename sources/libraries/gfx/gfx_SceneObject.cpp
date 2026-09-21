// Filename: gfx_SceneObject.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_Model.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_ParticleModel.h>
#include <nw/gfx/gfx_ParticleEmitter.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_FragmentLight.h>
#include <nw/gfx/gfx_VertexLight.h>
#include <nw/gfx/gfx_AmbientLight.h>
#include <nw/gfx/gfx_HemiSphereLight.h>
#include <nw/gfx/gfx_Fog.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_SceneBuilder.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw {
namespace gfx {

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(SceneObject);

}
}