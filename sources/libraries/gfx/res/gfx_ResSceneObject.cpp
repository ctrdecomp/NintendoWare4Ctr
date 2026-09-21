// Filename: gfx_ResSceneObject.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/res/gfx_ResSceneObject.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

namespace nw {
namespace gfx {
namespace res {


#if 0

NW_UT_RUNTIME_TYPEINFO_ROOT_DEFINITION(ResSceneObject);
NW_UT_RUNTIME_TYPEINFO_DEFINITION(ResSceneNode, ResSceneObject);
NW_UT_RUNTIME_TYPEINFO_DEFINITION(ResTransformNode, ResSceneNode);

static const nw::ut::internal::RuntimeTypeInfo*
s_GfxTypeInfoTable[] =
{
    &ResSceneObject::s_TypeInfo,
    &ResSceneNode::s_TypeInfo,
    &ResTransformNode::s_TypeInfo
};
#endif

}
}
}