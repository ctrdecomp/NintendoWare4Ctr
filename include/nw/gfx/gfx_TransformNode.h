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
        else
        {
            const nn::math::MTX34& parentWorldMatrix = TrackbackWorldMatrix();

            inheritingDirection.x =
                parentWorldMatrix.f._00 * direction.x +
                parentWorldMatrix.f._01 * direction.y +
                parentWorldMatrix.f._02 * direction.z;
    
            inheritingDirection.y =
                parentWorldMatrix.f._10 * direction.x +
                parentWorldMatrix.f._11 * direction.y +
                parentWorldMatrix.f._12 * direction.z;
    
            inheritingDirection.z =
                parentWorldMatrix.f._20 * direction.x +
                parentWorldMatrix.f._21 * direction.y +
                parentWorldMatrix.f._22 * direction.z;
        }
    }

    void SetResourceBasedTransform(
        const math::MTX34& transformMatrix,
        const math::VEC3& scale = math::VEC3(1.0f, 1.0f, 1.0f)
        )
    {
        math::Transform3& resTransform = GetResTransformNode().GetTransform();

        math::MTX34 resultMatrix;
        nw::math::MTX34RotXYZRad(&resultMatrix,
            resTransform.rotate.x,
            resTransform.rotate.y,
            resTransform.rotate.z);
        resultMatrix.f._03 = resTransform.translate.x;
        resultMatrix.f._13 = resTransform.translate.y;
        resultMatrix.f._23 = resTransform.translate.z;

        math::MTX34Mult(&resultMatrix, &transformMatrix, &resultMatrix);

        math::VEC3 resultScale;
        math::VEC3Mult(&resultScale, &scale, &resTransform.scale);

        m_Transform.SetTransformMatrix(resultMatrix);
        m_Transform.SetScale(resultScale);
        m_Transform.UpdateFlagsStrictly();
    }

    void SetResourceScaledTransform(const math::MTX34& transformMatrix)
    {
        math::VEC3 scale;
        math::MTX34 matrix;
        nw::math::MTX34DecomposeToColumnScale(&scale, &transformMatrix);

        NW_ASSERT(scale.x > 0 && scale.y > 0 && scale.z > 0);

        matrix.f._00 = transformMatrix.f._00 / scale.x;
        matrix.f._10 = transformMatrix.f._10 / scale.x;
        matrix.f._20 = transformMatrix.f._20 / scale.x;
        matrix.f._01 = transformMatrix.f._01 / scale.y;
        matrix.f._11 = transformMatrix.f._11 / scale.y;
        matrix.f._21 = transformMatrix.f._21 / scale.y;
        matrix.f._02 = transformMatrix.f._02 / scale.z;
        matrix.f._12 = transformMatrix.f._12 / scale.z;
        matrix.f._22 = transformMatrix.f._22 / scale.z;
        matrix.f._03 = transformMatrix.f._03;
        matrix.f._13 = transformMatrix.f._13;
        matrix.f._23 = transformMatrix.f._23;

        SetResourceBasedTransform(matrix, scale);
    }

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, ResTransformNode resTransformNode, Description description)
    {
        NW_ASSERT(description.isFixedSizeMemory);

        SceneNode::GetMemorySizeForInitialize(pSize, resTransformNode, description);

        if (description.maxCallbacks == 0)
        {
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else
        {
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