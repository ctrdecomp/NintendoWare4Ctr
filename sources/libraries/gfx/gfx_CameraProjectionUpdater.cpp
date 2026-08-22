#include <nw/gfx/gfx_CameraProjectionUpdater.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(CameraProjectionUpdater);

const float CameraProjectionUpdater::PROJECTION_NEAR_CLIP = 1.0f;
const float CameraProjectionUpdater::PROJECTION_FAR_CLIP = 1.0f;
const float CameraProjectionUpdater::PROJECTION_FOVY_RADIAN = math::PI / 4.0f;
const float CameraProjectionUpdater::PROJECTION_ASPECT_RATIO = 1.0f;
const math::VEC2 CameraProjectionUpdater::PROJECTION_CENTER = math::VEC2(0.0f,0.0f);
const float CameraProjectionUpdater::PROJECTION_HEIGHT = 1.0f;

}
}