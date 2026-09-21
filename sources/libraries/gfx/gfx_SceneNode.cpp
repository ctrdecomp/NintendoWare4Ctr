#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_ISceneVisitor.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>
#include <nw/ut/ut_Foreach.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SceneNode, SceneObject);

SceneNode* SceneNode::DynamicBuilder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);

    void* memory = allocator->Alloc(sizeof(SceneNode));
    NW_NULL_ASSERT(memory);

    SceneNode* node = new(memory) SceneNode(allocator, ResSceneNode(), this->m_Description);
    
    Result result = node->Initialize(allocator);

    NW_ASSERT(result.IsSuccess());

    return node;
    
}

SceneNode* SceneNode::Create(SceneNode* parent,ResSceneObject resource,const SceneNode::Description& description,os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    ResSceneNode resNode = ResStaticCast<ResSceneNode>(resource);
    
    void* memory = allocator->Alloc(sizeof(SceneNode));
    NW_NULL_ASSERT(memory);

    SceneNode* node = new(memory) SceneNode(
        allocator, resNode, description);
    
    Result result = node->Initialize(allocator);

    NW_ASSERT(result.IsSuccess());
    
    if (parent)
    {
        bool isAttached = parent->AttachChild(node);
        NW_ASSERT(isAttached);
    }
    return node;
}

void SceneNode::Accept(ISceneVisitor* visitor)
{    
    visitor->VisitSceneNode(this);
    AcceptChildren(visitor);
}

Result SceneNode::CreateAnimBinding(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    if (!m_Description.isAnimationEnabled)
    {
        return result;
    }

    ResSceneObject resSceneObject = GetResSceneObject();
    ResSceneNode resSceneNode = *reinterpret_cast<ResSceneNode*>(&resSceneObject);
    if (!resSceneNode.IsValid())
    {
        return result;
    }
    
    const int animGroupCount = resSceneNode.GetAnimGroupsCount();
    if (animGroupCount == 0)
    {
        return result;
    }

    bool hasAnimGroupMember = false;
    for (int idx = 0; idx < animGroupCount; ++idx)
    {
        if (0 < resSceneNode.GetAnimGroups(idx).GetMemberInfoSetCount())
        {
            hasAnimGroupMember = true;
            break;
        }
    }
    if (!hasAnimGroupMember)
    {
        return result;
    }

    AnimBinding* animBinding = AnimBinding::Builder()
        .MaxAnimGroups(animGroupCount)
        .MaxAnimObjectsPerGroup(this->m_Description.maxAnimObjectsPerGroup) 
        .Create(allocator);

    if (animBinding == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
    }

    NW_ENSURE_AND_RETURN(result);

    SetAnimBinding(animBinding);

    return result;
}

Result SceneNode::CreateChildren(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    if (m_Description.isFixedSizeMemory)
    {
        if (m_Description.maxChildren == 0)
        {
            m_Children = SceneNodeChildren(NULL, NULL, 0, NULL);
        }
        else{
            void* memory = allocator->Alloc(
                sizeof(SceneNode*) * m_Description.maxChildren, CHILDREN_MEMORY_ALIGNMENT);

            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_Children = SceneNodeChildren(NULL, memory, this->m_Description.maxChildren, allocator);
        }
    }
    else{
        m_Children = SceneNodeChildren(NULL, allocator);
    }

    return result;
}

Result SceneNode::CreateCallbacks(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    if (m_Description.isFixedSizeMemory)
    {
        if (m_Description.maxCallbacks == 0)
        {
            this->m_PreUpdateSignal = UpdateSignal::CreateInvalidateSignal(allocator);
        }
        else{
            this->m_PreUpdateSignal = UpdateSignal::CreateFixedSizedSignal(this->m_Description.maxCallbacks, allocator);
        }
    }
    else{
        this->m_PreUpdateSignal = UpdateSignal::CreateVariableSizeSignal(allocator);
    }

    if (m_PreUpdateSignal == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
    }

    return result;
}

Result SceneNode::Initialize(os::IAllocator* allocator)
{
    Result result = INITIALIZE_RESULT_OK;

    result |= CreateChildren(allocator);
    NW_ENSURE_AND_RETURN(result);

    this->m_Children.SetParent(this);

    result |= CreateCallbacks(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateAnimBinding(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

}
}