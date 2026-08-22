#include <nw/gfx/gfx_UserRenderNode.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(UserRenderNode, TransformNode);

UserRenderNode::UserRenderNode(
    os::IAllocator* allocator,
    const UserRenderNode::Description& description)
    : TransformNode(allocator, ResTransformNode(), description),
    mTranslucencyKind(description.translucencyKind),
    mPriority(description.priority),
    mLayerId(description.layerId),
    mUserRenderCommand(description.userRenderCommand),
    mDescription(description)
{}

UserRenderNode*UserRenderNode::DynamicBuilder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);

    void* memory = allocator->Alloc(sizeof(UserRenderNode));
    NW_NULL_ASSERT(memory);

    UserRenderNode* node = new(memory) UserRenderNode(allocator,this->mDescription);
    
    Result result = node->Initialize(allocator);

    NW_ASSERT(result.IsSuccess());

    return node;
}

void UserRenderNode::Accept(ISceneVisitor* visitor){
    visitor->VisitUserRenderNode(this);
    AcceptChildren(visitor);
}

}
}