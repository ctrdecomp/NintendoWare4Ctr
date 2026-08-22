#include <nw/gfx/gfx_TransformNode.h>
#include <nw/gfx/gfx_ISceneVisitor.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>

#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(TransformNode, SceneNode);

TransformNode::TransformNode(os::IAllocator* allocator,ResTransformNode resObj,const TransformNode::Description& description): 
    SceneNode(allocator, resObj, description),
    mInverseWorldMatrix(math::MTX34::Identity()),
    mIsInverseWorldMatrixValid(false),
    mPostCalculateWorldMatrixSignal(NULL),
    mIsBranchWorldMatrixCalculationEnabled(true),
    mDescription(description){
    if (resObj.IsValid()){
        this->mTransform.SetTransform(resObj.GetTransform());
        this->mWorldMatrix = resObj.GetWorldMatrix();
    }
    else{
        math::MTX34Identity(&this->mWorldMatrix);
    }
}

TransformNode* TransformNode::DynamicBuilder::Create(os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);

    void* memory = allocator->Alloc(sizeof(TransformNode));
    NW_NULL_ASSERT(memory);

    TransformNode* node = new(memory) TransformNode(allocator,ResTransformNode(),this->mDescription);
    
    Result result = node->Initialize(allocator);

    NW_ASSERT(result.IsSuccess());

    return node;
}

TransformNode* TransformNode::Create(SceneNode* parent,ResSceneObject resource,const TransformNode::Description& description,os::IAllocator* allocator){
    NW_NULL_ASSERT(allocator);
    
    ResTransformNode resNode = ResStaticCast<ResTransformNode>(resource);
    
    void* memory = allocator->Alloc(sizeof(TransformNode));
    NW_NULL_ASSERT(memory);

    TransformNode* node = new(memory) TransformNode(allocator,resNode,description);
    
    Result result = node->Initialize(allocator);

    NW_ASSERT(result.IsSuccess());

    if (parent){
        bool isAttached = parent->AttachChild(node);
        NW_ASSERT(isAttached);
    }
    return node;
}

Result TransformNode::CreateCallbacks(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    if (mDescription.isFixedSizeMemory){
        if (mDescription.maxCallbacks == 0){
            mPostCalculateWorldMatrixSignal = 
                CalculateMatrixSignal::CreateInvalidateSignal(allocator);
        }
        else{
            mPostCalculateWorldMatrixSignal =  CalculateMatrixSignal::CreateFixedSizedSignal(this->mDescription.maxCallbacks, allocator);
        }
    }
    else{
        mPostCalculateWorldMatrixSignal =  CalculateMatrixSignal::CreateVariableSizeSignal(allocator);
    }

    if (mPostCalculateWorldMatrixSignal == NULL){
        result |= Result::MASK_FAIL_BIT;
    }

    return result;
}

void TransformNode::Accept(ISceneVisitor* visitor){
    visitor->VisitTransformNode(this);
    AcceptChildren(visitor);
}

const math::MTX34& TransformNode::InverseWorldMatrix() const{
    if (!mIsInverseWorldMatrixValid){
        NW_FAILSAFE_IF(math::MTX34Inverse(&this->mInverseWorldMatrix, this->mWorldMatrix) == 0){
            mInverseWorldMatrix = math::MTX34::Identity();
        }

        mIsInverseWorldMatrixValid = true;
    }

    return mInverseWorldMatrix;
}

void TransformNode::InvalidateInverseWorldMatrix(){
    mIsInverseWorldMatrixValid = false;
}

void TransformNode::UpdateTransform(WorldMatrixUpdater* worldMatrixUpdater,SceneContext* sceneContext){
    if (this->Transform().IsEnabledFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED)){

        if (this->IsEnabledResults(SceneNode::FLAG_IS_DIRTY)){
            const CalculatedTransform* parentWorldTransform;
            const CalculatedTransform* parentLocalTransform;

            if (this->GetParent() == NULL){
                parentWorldTransform = &CalculatedTransform::Identity();
                parentLocalTransform = &CalculatedTransform::Identity();
            }
            else{
                parentWorldTransform = &this->GetParent()->TrackbackWorldTransform();
                parentLocalTransform = &this->GetParent()->TrackbackLocalTransform();
            }

            worldMatrixUpdater->UpdateBasic(&this->WorldMatrix(),&this->WorldTransform(),this->Transform(),*parentWorldTransform,*parentLocalTransform);
            this->InvalidateInverseWorldMatrix();
        }
    }
    else{
        this->DisableTraversalResults(SceneNode::FLAG_IS_DIRTY);
    }
    this->UpdateDirection();
    this->Transform().DisableFlags(CalculatedTransform::FLAG_IS_DIRTY);
    this->PostCalculateWorldMatrixSignal()(this, sceneContext);
}

Result TransformNode::Initialize(os::IAllocator* allocator){
    Result result = INITIALIZE_RESULT_OK;

    result |= SceneNode::Initialize(allocator);
    NW_ENSURE_AND_RETURN(result);

    result |= CreateCallbacks(allocator);
    NW_ENSURE_AND_RETURN(result);

    return result;
}

}
}