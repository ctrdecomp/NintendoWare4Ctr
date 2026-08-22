#include <nw/gfx/gfx_CameraViewUpdater.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(CameraViewUpdater);

const math::VEC3 CameraViewUpdater::VIEW_TARGET_POSITION = math::VEC3(0.0f,0.0f,-1.0f);
const math::VEC3 CameraViewUpdater::VIEW_UPWARD_VECTOR = math::VEC3(0.0f,1.0f,0.0f);
const math::VEC3 CameraViewUpdater::VIEW_VIEW_ROTATE = math::VEC3(0.0f,0.0f,0.0f);
const float CameraViewUpdater::VIEW_TWIST = 0.0f;

}
}