#include <nw/gfx/gfx_SceneHelper.h>

namespace nw{
namespace gfx{

void SceneHelper::ResolveReferenceImpl(SceneNode* parent, SceneNode* child){
    NW_NULL_ASSERT(parent);
    NW_NULL_ASSERT(child);

    if (child->GetParent() != NULL){
        return;
    }

    void* resAddr = child->GetResSceneNode().ptr();
    const int childNum = parent->GetResSceneNode().GetChildrenCount();
    for (int i = 0; i < childNum; ++i){
        void* childResAddr = parent->GetResSceneNode().GetChildren(i).ptr();
        if (nw::ut::ComparePtr(resAddr, childResAddr) == 0){

            bool result = parent->AttachChild(child);
            NW_ASSERT(result);

            return;
        }
    }
}

}
}