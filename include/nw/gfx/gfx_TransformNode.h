#pragma once

#include <nw/math.h>
#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class TransformNode : public SceneNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(TransformNode);

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::Signal2<void, TransformNode*, SceneContext*> CalculateMatrixSignal;
    typedef CalculateMatrixSignal::SlotType CalculateMatrixSlot;

    struct Description : public SceneNode::Description
    {
        Description() {}
    };

    class DynamicBuilder
    {
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_Description.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren)
        {
            m_Description.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks)
        {
            m_Description.maxCallbacks = maxCallbacks;
            return *this;
        }

        TransformNode* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);

            size += sizeof(TransformNode);
            GetMemorySizeForInitialize(&size, ResTransformNode(), m_Description);

            return size.GetSizeWithPadding(alignment);
        }

    private:
        TransformNode::Description m_Description;
    };

    static TransformNode* Create(SceneNode* parent, ResSceneObject resource, const TransformNode::Description& description, nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResTransformNode resTransformNode, Description description, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);
        GetMemorySizeInternal(&size, resTransformNode, description);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResTransformNode resTransformNode, Description description)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(TransformNode);
        GetMemorySizeForInitialize(pSize, resTransformNode, description);
    }

    ResTransformNode GetResTransformNode()
    {
        return nw::ut::ResStaticCast<ResTransformNode>(GetResSceneObject());
    }

    const ResTransformNode GetResTransformNode() const
    {
        return ResStaticCast<ResTransformNode>(GetResSceneObject());
    }

    CalculatedTransform& Transform() { return m_Transform; }
    const CalculatedTransform& Transform() const { return m_Transform; }

    MTX34& WorldMatrix() { return m_WorldMatrix; }
    const MTX34& WorldMatrix() const { return m_WorldMatrix; }

    CalculatedTransform& WorldTransform() { return m_CalculatedTransform; }
    const CalculatedTransform& WorldTransform() const { return m_CalculatedTransform; }

    const MTX34& InverseWorldMatrix() const;

    void InvalidateInverseWorldMatrix();

    virtual void UpdateDirection() {}

    virtual void UpdateTransform(WorldMatrixUpdater* worldMatrixUpdater, SceneContext* sceneContext);

    virtual void Accept(ISceneVisitor* visitor);

    virtual const nw::math::MTX34& TrackbackWorldMatrix() const
    {
        return WorldMatrix();
    }

    virtual const CalculatedTransform& TrackbackWorldTransform() const
    {
        return WorldTransform();
    }

    virtual const CalculatedTransform& TrackbackLocalTransform() const
    {
        return Transform();
    }

    inline virtual void InheritTraversalResults();

    CalculateMatrixSignal& PostCalculateWorldMatrixSignal()
    {
        return *this->m_PostCalculateWorldMatrixSignal;
    }

    const CalculateMatrixSignal& PostCalculateWorldMatrixSignal() const
    {
        return *this->m_PostCalculateWorldMatrixSignal;
    }

protected:
    TransformNode(nw::os::IAllocator* allocator, ResTransformNode resObj, const TransformNode::Description& description);
    virtual ~TransformNode()
    {
        SafeDestroy(this->m_PostCalculateWorldMatrixSignal);
    }

    virtual Result Initialize(nw::os::IAllocator* allocator);

    void CalcInheritingDiretion(VEC3& inheritingDirection, const VEC3& direction) const
    {
        if (GetParent() == NULL)
        {
            inheritingDirection = direction;
        }
        else{
            const nn::math::MTX34& parentWorldMatrix = TrackbackWorldMatrix();

            inheritingDirection.x =
                parentWorldMatrix.matrix[0][0] * direction.x +
                parentWorldMatrix.matrix[0][1] * direction.y +
                parentWorldMatrix.matrix[0][2] * direction.z;

            inheritingDirection.y =
                parentWorldMatrix.matrix[1][0] * direction.x +
                parentWorldMatrix.matrix[1][1] * direction.y +
                parentWorldMatrix.matrix[1][2] * direction.z;

            inheritingDirection.z =
                parentWorldMatrix.matrix[2][0] * direction.x +
                parentWorldMatrix.matrix[2][1] * direction.y +
                parentWorldMatrix.matrix[2][2] * direction.z;
        }
    }

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, ResTransformNode resTransformNode, Description description)
    {
        NW_ASSERT(description.isFixedSizeMemory);

        SceneNode::GetMemorySizeForInitialize(pSize, resTransformNode, description);

        if (description.maxCallbacks == 0)
        {
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else{
            CalculateMatrixSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, description.maxCallbacks);
        }
    }

private:
    Result CreateCallbacks(nw::os::IAllocator* allocator);

    CalculatedTransform m_Transform;
    nw::math::MTX34 m_WorldMatrix;
    CalculatedTransform m_CalculatedTransform;

    mutable nw::math::MTX34 m_InverseWorldMatrix;
    mutable bool m_IsInverseWorldMatrixValid;

    CalculateMatrixSignal* m_PostCalculateWorldMatrixSignal;
    bool m_IsBranchWorldMatrixCalculationEnabled;
    Description m_Description;
};

inline void TransformNode::InheritTraversalResults()
{
    SceneNode::InheritTraversalResults();

    SceneNode* parent = GetParent();
    bit32 results = GetTraversalResults();

    if (Transform().IsEnabledFlags(CalculatedTransform::FLAG_IS_DIRTY))
    {
        results = nw::ut::EnableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else if (parent == NULL)
    {
        results = nw::ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else if (parent->IsEnabledResults(SceneNode::FLAG_IS_DIRTY))
    {
        results = nw::ut::EnableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else{
        results = nw::ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }

    SetTraversalResults(results);
}

}
}