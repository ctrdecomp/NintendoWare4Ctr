#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {


#if 0

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION( ResSceneObject );
NW_UT_RUNTIME_TYPEINFO_DEFINITION( ResSceneNode, ResSceneObject );
NW_UT_RUNTIME_TYPEINFO_DEFINITION( ResTransformNode, ResSceneNode );

static const nw::ut::internal::RuntimeTypeInfo*
sGfxTypeInfoTable[] ={
    &ResSceneObject::sTypeInfo,
    &ResSceneNode::sTypeInfo,
    &ResTransformNode::sTypeInfo
};
#endif

}
}
}