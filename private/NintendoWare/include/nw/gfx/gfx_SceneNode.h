/*---------------------------------------------------------------------------*
  Project:  NintendoWare

  Copyright (C)Nintendo/HAL Laboratory, Inc.  All rights reserved.

  These coded instructions, statements, and computer programs contain proprietary
  information of Nintendo and/or its licensed developers and are protected by
  national and international copyright laws. They may not be disclosed to third
  parties or copied or duplicated in any form, in whole or in part, without the
  prior written consent of Nintendo.

  The content herein is highly confidential and should be handled accordingly.
 *---------------------------------------------------------------------------*/

#ifndef NW_GFX_SCENENODE_H_
#define NW_GFX_SCENENODE_H_

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_AnimBinding.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

#include <nw/ut/ut_Children.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/ut/ut_Signal.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

class ISceneVisitor;
class SceneContext;
class WorldMatrixUpdater;

//---------------------------------------------------------------------------
//
//
//---------------------------------------------------------------------------
class SceneNode : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(SceneNode);
    NW_CHILD_DECLARE_PARENT(SceneNode);

    static const size_t CHILDREN_MEMORY_ALIGNMENT = os::IAllocator::CACHE_LINE_ALIGNMENT;

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    enum TraversalResults
    {
        //
        FLAG_IS_VISIBLE_SHIFT = 0,
        //
        FLAG_IS_DIRTY_SHIFT                           = 1,

        //
        FLAG_IS_VISIBLE                               = 0x1 << FLAG_IS_VISIBLE_SHIFT,

        //
        FLAG_IS_DIRTY                                 = 0x1 << FLAG_IS_DIRTY_SHIFT,

        //
        FLAG_DEFAULT = FLAG_IS_VISIBLE | FLAG_IS_DIRTY
    };

    //
    //
    //
    typedef ut::Signal2<void, SceneNode*, SceneContext*> UpdateSignal;

    //
    typedef UpdateSignal::SlotType UpdateSlot;

    //
    struct Description
    {
        bool isFixedSizeMemory; //
        bool isAnimationEnabled; //
        s32 maxCallbacks;       //
        s32 maxChildren;        //
        s32 maxAnimObjectsPerGroup; //

        //
        Description()
         : isFixedSizeMemory(true),
           isAnimationEnabled(true),
           maxCallbacks(DEFAULT_MAX_CALLBACKS),
           maxChildren(DEFAULT_MAX_CHILDREN),
           maxAnimObjectsPerGroup(DEFAULT_MAX_ANIMOBJECTS)
        {}
    };

    //----------------------------------------
    //
    //

    //
    //
    //
    class DynamicBuilder
    {
    public:
        //
        DynamicBuilder() {}
        //
        ~DynamicBuilder() {}

        //
        //
        //
        //
        //
        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        //
        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        //
        //
        //
        DynamicBuilder& MaxAnimObjectsPerGroup(s32 maxAnimObjects)
        {
            m_Description.maxAnimObjectsPerGroup = maxAnimObjects;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        DynamicBuilder& IsAnimationEnabled(bool isAnimationEnabled)
        {
            m_Description.isAnimationEnabled = isAnimationEnabled;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        SceneNode* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            size += sizeof(SceneNode);
            GetMemorySizeForInitialize(&size, ResSceneNode(), m_Description);

            return size.GetSizeWithPadding(alignment);
        }

    private:
        SceneNode::Description m_Description;
    };

    //
    //
    //
    //
    //
    //
    //
    //
    //
    static SceneNode* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const SceneNode::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResSceneNode resSceneNode,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resSceneNode, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResSceneNode resSceneNode,
        Description description)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(SceneNode);
        GetMemorySizeForInitialize(pSize, resSceneNode, description);
    }

    //
    void DestroyBranch()
    {
        SceneNodeChildren::iterator end = this->m_Children.end();
        for (SceneNodeChildren::iterator child = this->m_Children.begin(); child != end; ++child)
        {
            if (*child)
            {
                (*child)->SetParent(NULL);
                (*child)->DestroyBranch();
                *child = NULL;
            }
        }
        this->Destroy();
    }

    //
    //
    //
    //
    //
    //
    static size_t GetDeviceMemorySize(
        ResSceneObject,
        Description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        NW_UNUSED_VARIABLE(alignment);

        return 0;
    }

    //
    static void GetDeviceMemorySizeInternal(
        os::MemorySizeCalculator*,
        ResSceneObject,
        Description)
    {
    }

    //

    //----------------------------------------
    //
    //

    //
    ResSceneNode GetResSceneNode()
    {
        return ResStaticCast<ResSceneNode>( this->GetResSceneObject() );
    }

    //
    const ResSceneNode GetResSceneNode() const
    {
        return ResStaticCast<ResSceneNode>( this->GetResSceneObject() );
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    virtual void UpdateTransform(
        WorldMatrixUpdater* worldMatrixUpdater,
        SceneContext* sceneContext)
    {
        NW_UNUSED_VARIABLE(worldMatrixUpdater);
        NW_UNUSED_VARIABLE(sceneContext);
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    bool AttachChild(SceneNode* child)
    {
        if (IsCircularReference(child))
        {
            return false;
        }

        return m_Children.Attach(child);
    }

    //
    //
    //
    //
    void DetachChild(SceneNode* child)
    {
        NW_NULL_ASSERT(child);
        NW_ASSERT(child->GetParent() == this);

        m_Children.Detach(child);
    }

    //
    SceneNodeChildren::iterator GetChildBegin() { return m_Children.begin(); }

    //
    SceneNodeChildren::const_iterator GetChildBegin() const { return m_Children.begin(); }

    //
    SceneNodeChildren::iterator GetChildEnd() { return m_Children.end(); }

    //
    SceneNodeChildren::const_iterator GetChildEnd() const { return m_Children.end(); }

    //
    void DetachAllChildren() { m_Children.clear(); }

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //
    virtual const math::MTX34& TrackbackWorldMatrix() const
    {
        const SceneNode* parent = this->GetParent();
        if (parent == NULL)
        {
            return nw::math::MTX34::Identity();
        }

        return parent->TrackbackWorldMatrix();
    }

    //
    virtual const CalculatedTransform& TrackbackWorldTransform() const
    {
        const SceneNode* parent = this->GetParent();
        if (parent == NULL)
        {
            return CalculatedTransform::Identity();
        }

        return parent->TrackbackWorldTransform();
    }

    //
    virtual const CalculatedTransform& TrackbackLocalTransform() const
    {
        const SceneNode* parent = this->GetParent();
        if (parent == NULL)
        {
            return CalculatedTransform::Identity();
        }

        return parent->TrackbackLocalTransform();
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    //
    //
    UpdateSignal& PreUpdateSignal() { return *m_PreUpdateSignal; }

    //
    //
    //
    //
    //
    //
    const UpdateSignal& PreUpdateSignal() const { return *m_PreUpdateSignal; }

    //

    //----------------------------------------
    //
    //

    //
    const AnimBinding* GetAnimBinding() const { return m_AnimBinding.Get(); }

    //
    AnimBinding* GetAnimBinding() { return m_AnimBinding.Get(); }

    //
    void SetAnimBinding(AnimBinding* animBinding) { m_AnimBinding = GfxPtr<AnimBinding>(animBinding); }

    //
    void UpdateFrame()
    {
        AnimBinding* animBinding = GetAnimBinding();
        if (animBinding != NULL)
        {
            animBinding->UpdateFrame();
        }
    }

    //

    //----------------------------------------
    //
    //

    //
    bit32 GetTraversalResults() const
    {
        return this->m_TraversalResults;
    }

    //
    void SetTraversalResults(bit32 results)
    {
        this->m_TraversalResults = results;
    }

    //
    bool IsEnabledResults(bit32 results) const
    {
        return ut::CheckFlag(m_TraversalResults, results);
    }

    //
    void EnableTraversalResults(bit32 results)
    {
        this->m_TraversalResults = ut::EnableFlag(this->m_TraversalResults, results);
    }

    //
    void DisableTraversalResults(bit32 results)
    {
        this->m_TraversalResults = ut::DisableFlag(this->m_TraversalResults, results);
    }

    //
    void ResetTraversalResults()
    {
        this->m_TraversalResults = FLAG_DEFAULT;
    }

    //
    void CopyTraversalResults(const SceneNode* node)
    {
        if (node != NULL)
        {
            this->m_TraversalResults = node->GetTraversalResults();
        }
    }

    //
    NW_INLINE virtual void InheritTraversalResults();

    //
    //
    //
    //
    //
    //
    void SetBranchVisible(bool isBranchVisible)
    {
        m_BranchVisible = isBranchVisible;
    }

    //
    //
    //
    //
    bool IsBranchVisible() const
    {
        return m_BranchVisible;
    }
    //

    //
    //
    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        ResSceneNode resSceneNode,
        Description description)
    {
        NW_ASSERT(description.isFixedSizeMemory);

        // SceneNode::Initialize
        os::MemorySizeCalculator& size = *pSize;

        // SceneNode::CreateChildren
        size.Add(sizeof(SceneNode*) * description.maxChildren, CHILDREN_MEMORY_ALIGNMENT);

        // SceneNode::CreateCallbacks
        if (description.maxCallbacks == 0)
        {
            UpdateSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else
        {
            UpdateSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, description.maxCallbacks);
        }

        // SceneNode::CreateAnimBinding
        if (description.isAnimationEnabled && resSceneNode.IsValid())
        {
            const int animGroupCount = resSceneNode.GetAnimGroupsCount();
            if (animGroupCount)
            {
                AnimBinding::Builder()
                    .MaxAnimGroups(animGroupCount)
                    .MaxAnimObjectsPerGroup(description.maxAnimObjectsPerGroup)
                    .GetMemorySizeInternal(pSize);
            }
        }
    }

protected:
    //----------------------------------------
    //
    //

    //
    SceneNode(
        os::IAllocator* allocator,
        ResSceneNode resObj,
        const SceneNode::Description& description)
    : SceneObject(allocator, resObj),
      m_BranchVisible(true),
      m_PreUpdateSignal(NULL),
      m_TraversalResults(FLAG_DEFAULT),
      m_Description(description)
    {
        this->SetParent(NULL);
        if (resObj.IsValid())
        {
            m_BranchVisible = resObj.IsBranchVisible();
        }
    }

    //
    virtual ~SceneNode()
    {
        SceneNode* parent = this->GetParent();
        if (parent)
        {
            parent->DetachChild(this);
        }

        SafeDestroy(m_PreUpdateSignal);
    }

    //

    //
    //
    //
    //
    bool IsCircularReference(const SceneNode* child) const
    {
        const SceneNode* parent = this->GetParent();
        if (parent == 0)
        {
            return false;
        }

        if (parent != child)
        {
            return parent->IsCircularReference(child);
        }

        return true;
    }

    //
    //
    //
    //
    void AcceptChildren(ISceneVisitor* visitor)
    {
        NW_FOREACH(SceneNode* child, m_Children)
        {
            child->Accept(visitor);
        }
    }

    //
    //
    //
    //
    virtual Result Initialize(os::IAllocator* allocator);

    //
    SceneNodeChildren m_Children;
    //
    GfxPtr<AnimBinding> m_AnimBinding;
    //
    bool m_BranchVisible;

private:
    //
    Result CreateChildren(os::IAllocator* allocator);

    //
    Result CreateCallbacks(os::IAllocator* allocator);

    //
    Result CreateAnimBinding(os::IAllocator* allocator);

    //
    static const int DEFAULT_MAX_ANIMOBJECTS = 1;

    UpdateSignal* m_PreUpdateSignal;
    bit32 m_TraversalResults;
    Description m_Description;
};

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template<typename TNode>
NW_INLINE void
SafeDestroyBranch(
    TNode*& node
)
{
    if (node == NULL) { return; }
    node->DestroyBranch();
    node = NULL;
}

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
template<typename TNode>
struct SafeBranchDestroyer : public std::unary_function<TNode&, void>
{
    //
    void operator()(TNode& node) const
    {
        SafeDestroyBranch(node);
    }
};

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template<typename TArray>
NW_INLINE void
SafeDestroyBranchAll(
    TArray& nodes
)
{
    std::for_each(nodes.begin(), nodes.end(), SafeBranchDestroyer<typename TArray::value_type>());
    nodes.clear();
}

//----------------------------------------
NW_INLINE void
SceneNode::InheritTraversalResults()
{
    bit32 results = this->GetTraversalResults();
    SceneNode* parent = this->GetParent();

    bool isVisible = this->IsBranchVisible();

    if (parent != NULL && !(parent->IsEnabledResults(SceneNode::FLAG_IS_VISIBLE)))
    {
        isVisible = false;
    }

    if (isVisible)
    {
        results = ut::EnableFlag(results, SceneNode::FLAG_IS_VISIBLE);
    }
    else
    {
        results = ut::DisableFlag(results, SceneNode::FLAG_IS_VISIBLE);
    }

    results = ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);

    this->SetTraversalResults(results);
}


} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_SCENENODE_H_
