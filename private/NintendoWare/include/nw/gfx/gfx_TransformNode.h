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

#ifndef NW_GFX_TRANSFORMNODE_H_
#define NW_GFX_TRANSFORMNODE_H_

#include <nw/gfx/gfx_SceneNode.h>

#include <nw/gfx/gfx_CalculatedTransform.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class TransformNode : public SceneNode
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(TransformNode);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    //
    //
    typedef ut::Signal2<void, TransformNode*, SceneContext*> CalculateMatrixSignal;

    //
    typedef CalculateMatrixSignal::SlotType CalculateMatrixSlot;

    //
    struct Description : public SceneNode::Description
    {
        //
        Description(){}
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
        //
        //
        //
        TransformNode* Create(os::IAllocator* allocator);

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            size += sizeof(TransformNode);
            GetMemorySizeForInitialize(&size, ResTransformNode(), m_Description);

            return size.GetSizeWithPadding(alignment);
        }

    private:
        TransformNode::Description m_Description;
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
    static TransformNode* Create(
        SceneNode* parent,
        ResSceneObject resource,
        const TransformNode::Description& description,
        os::IAllocator* allocator);

    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResTransformNode resTransformNode,
        Description description,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resTransformNode, description);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResTransformNode resTransformNode,
        Description description)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(TransformNode);
        GetMemorySizeForInitialize(pSize, resTransformNode, description);
    }

    //

    //----------------------------------------
    //
    //

    //
    ResTransformNode GetResTransformNode()
    {
        return ResStaticCast<ResTransformNode>( this->GetResSceneObject() );
    }

    //
    const ResTransformNode GetResTransformNode() const
    {
        return ResStaticCast<ResTransformNode>( this->GetResSceneObject() );
    }

    //

    //----------------------------------------
    //
    //

    //
    CalculatedTransform& Transform() { return m_Transform; }

    //
    const CalculatedTransform& Transform() const { return m_Transform; }

    //
    math::MTX34& WorldMatrix() { return m_WorldMatrix; }

    //
    const math::MTX34& WorldMatrix() const { return m_WorldMatrix; }

    //
    //
    CalculatedTransform& WorldTransform() { return m_CalculatedTransform; }

    //
    //
    //
    const CalculatedTransform& WorldTransform() const { return m_CalculatedTransform; }

    //
    const math::MTX34& InverseWorldMatrix() const;

    //
    void InvalidateInverseWorldMatrix();

    //
    virtual void UpdateDirection() {}

    //
    //
    //
    //
    //
    virtual void UpdateTransform(
        WorldMatrixUpdater* worldMatrixUpdater,
        SceneContext* sceneContext);

    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
    //
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

    //
    //
    //
    //
    //
    //
    //
    //
    //
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

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual void Accept(ISceneVisitor* visitor);

    //
    virtual const math::MTX34& TrackbackWorldMatrix() const
    {
        return this->WorldMatrix();
    }

    //
    virtual const CalculatedTransform& TrackbackWorldTransform() const
    {
        return this->WorldTransform();
    }

    //
    virtual const CalculatedTransform& TrackbackLocalTransform() const
    {
        return this->Transform();
    }

    //
    NW_INLINE virtual void InheritTraversalResults();

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    CalculateMatrixSignal& PostCalculateWorldMatrixSignal()
    {
        return *m_PostCalculateWorldMatrixSignal;
    }

    //
    //
    //
    const CalculateMatrixSignal& PostCalculateWorldMatrixSignal() const
    {
        return *m_PostCalculateWorldMatrixSignal;
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    TransformNode(
        os::IAllocator* allocator,
        ResTransformNode resObj,
        const TransformNode::Description& description);

    //
    virtual ~TransformNode()
    {
        SafeDestroy(m_PostCalculateWorldMatrixSignal);
    }

    virtual Result Initialize(os::IAllocator* allocator);

    //

    //
    void CalcInheritingDiretion(
        math::VEC3& inheritingDirection,
        const math::VEC3 &direction) const
    {
        if (this->GetParent() == NULL)
        {
            inheritingDirection = direction;
        }
        else
        {
            const math::MTX34& parentWorldMatrix =  this->TrackbackWorldMatrix();
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


    //
    //
    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        ResTransformNode resTransformNode,
        Description description)
    {
        NW_ASSERT(description.isFixedSizeMemory);

        SceneNode::GetMemorySizeForInitialize(pSize, resTransformNode, description);

        // TransformNode::CreateCallbacks
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
    //
    Result CreateCallbacks(os::IAllocator* allocator);

    CalculatedTransform m_Transform;
    math::MTX34 m_WorldMatrix;
    CalculatedTransform m_CalculatedTransform;

    mutable math::MTX34 m_InverseWorldMatrix;
    mutable bool m_IsInverseWorldMatrixValid;

    CalculateMatrixSignal* m_PostCalculateWorldMatrixSignal;
    bool m_IsBranchWorldMatrixCalculationEnabled;
    Description m_Description;
};

//----------------------------------------
NW_INLINE void
TransformNode::InheritTraversalResults()
{
    // TODO: Eliminate duplication becasue FLAG_IS_DIRTY operations are performed even at the following call destination.
    SceneNode::InheritTraversalResults();

    SceneNode* parent = this->GetParent();
    bit32 results = this->GetTraversalResults();

    if (this->Transform().IsEnabledFlags(CalculatedTransform::FLAG_IS_DIRTY))
    {
        results = ut::EnableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else if (parent== NULL)
    {
        results = ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else if (parent->IsEnabledResults(SceneNode::FLAG_IS_DIRTY))
    {
        results = ut::EnableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else
    {
        results = ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }

    this->SetTraversalResults(results);
}

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_TRANSFORMNODE_H_
