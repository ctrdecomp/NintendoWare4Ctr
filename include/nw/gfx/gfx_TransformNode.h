#pragma once

#include <nw/math.h>
#include <nw/gfx/gfx_SceneNode.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class TransformNode : public SceneNode{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(TransformNode);

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::Signal2<void, TransformNode*, SceneContext*> CalculateMatrixSignal;
    typedef CalculateMatrixSignal::SlotType CalculateMatrixSlot;

    struct Description : public SceneNode::Description{
        Description() {}
    };

    class DynamicBuilder{
    public:
        DynamicBuilder() {}
        ~DynamicBuilder() {}

        DynamicBuilder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mDescription.isFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        DynamicBuilder& MaxChildren(int maxChildren){
            mDescription.maxChildren = maxChildren;
            return *this;
        }

        DynamicBuilder& MaxCallbacks(int maxCallbacks){
            mDescription.maxCallbacks = maxCallbacks;
            return *this;
        }

        TransformNode* Create(nw::os::IAllocator* allocator);

        size_t GetMemorySize(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT) const{
            nw::os::MemorySizeCalculator size(alignment);

            size += sizeof(TransformNode);
            GetMemorySizeForInitialize(&size, ResTransformNode(), mDescription);

            return size.GetSizeWithPadding(alignment);
        }

    private:
        TransformNode::Description mDescription;
    };

    static TransformNode* Create(SceneNode* parent, ResSceneObject resource, const TransformNode::Description& description, nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResTransformNode resTransformNode, Description description, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);
        GetMemorySizeInternal(&size, resTransformNode, description);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResTransformNode resTransformNode, Description description){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(TransformNode);
        GetMemorySizeForInitialize(pSize, resTransformNode, description);
    }

    ResTransformNode GetResTransformNode(){
        return nw::ut::ResStaticCast<ResTransformNode>(GetResSceneObject());
    }

    const ResTransformNode GetResTransformNode() const{
        return ResStaticCast<ResTransformNode>(GetResSceneObject());
    }

    CalculatedTransform& Transform() { return mTransform; }
    const CalculatedTransform& Transform() const { return mTransform; }

    MTX34& WorldMatrix() { return mWorldMatrix; }
    const MTX34& WorldMatrix() const { return mWorldMatrix; }

    CalculatedTransform& WorldTransform() { return mCalculatedTransform; }
    const CalculatedTransform& WorldTransform() const { return mCalculatedTransform; }

    const MTX34& InverseWorldMatrix() const;

    void InvalidateInverseWorldMatrix();

    virtual void UpdateDirection() {}

    virtual void UpdateTransform(WorldMatrixUpdater* worldMatrixUpdater, SceneContext* sceneContext);

    virtual void Accept(ISceneVisitor* visitor);

    virtual const nw::math::MTX34& TrackbackWorldMatrix() const{
        return WorldMatrix();
    }

    virtual const CalculatedTransform& TrackbackWorldTransform() const{
        return WorldTransform();
    }

    virtual const CalculatedTransform& TrackbackLocalTransform() const{
        return Transform();
    }

    inline virtual void InheritTraversalResults();

    CalculateMatrixSignal& PostCalculateWorldMatrixSignal(){
        return *this->mPostCalculateWorldMatrixSignal;
    }

    const CalculateMatrixSignal& PostCalculateWorldMatrixSignal() const{
        return *this->mPostCalculateWorldMatrixSignal;
    }

protected:
    TransformNode(nw::os::IAllocator* allocator, ResTransformNode resObj, const TransformNode::Description& description);
    virtual ~TransformNode(){
        SafeDestroy(this->mPostCalculateWorldMatrixSignal);
    }

    virtual Result Initialize(nw::os::IAllocator* allocator);

    void CalcInheritingDiretion(VEC3& inheritingDirection, const VEC3& direction) const{
        if (GetParent() == NULL){
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

    static void GetMemorySizeForInitialize(nw::os::MemorySizeCalculator* pSize, ResTransformNode resTransformNode, Description description){
        NW_ASSERT(description.isFixedSizeMemory);

        SceneNode::GetMemorySizeForInitialize(pSize, resTransformNode, description);

        if (description.maxCallbacks == 0){
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else{
            CalculateMatrixSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, description.maxCallbacks);
        }
    }

private:
    Result CreateCallbacks(nw::os::IAllocator* allocator);

    CalculatedTransform mTransform;
    nw::math::MTX34 mWorldMatrix;
    CalculatedTransform mCalculatedTransform;

    mutable nw::math::MTX34 mInverseWorldMatrix;
    mutable bool mIsInverseWorldMatrixValid;

    CalculateMatrixSignal* mPostCalculateWorldMatrixSignal;
    bool mIsBranchWorldMatrixCalculationEnabled;
    Description mDescription;
};

inline void TransformNode::InheritTraversalResults(){
    SceneNode::InheritTraversalResults();

    SceneNode* parent = GetParent();
    bit32 results = GetTraversalResults();

    if (Transform().IsEnabledFlags(CalculatedTransform::FLAG_IS_DIRTY)){
        results = nw::ut::EnableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else if (parent == NULL){
        results = nw::ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else if (parent->IsEnabledResults(SceneNode::FLAG_IS_DIRTY)){
        results = nw::ut::EnableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }
    else{
        results = nw::ut::DisableFlag(results, SceneNode::FLAG_IS_DIRTY);
    }

    SetTraversalResults(results);
}

}
}