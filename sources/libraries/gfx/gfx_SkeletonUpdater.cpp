#include <nw/gfx/gfx_SkeletonUpdater.h>
#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/gfx/gfx_BillboardUpdater.h>
#include <nw/gfx/gfx_Camera.h>
#include <nw/gfx/gfx_SkeletalModel.h>
#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/ut/ut_Foreach.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/math/inlines/math_Matrix34.ipp>

namespace nw{
namespace gfx{

SkeletonUpdater* SkeletonUpdater::Builder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(SkeletonUpdater));
    NW_NULL_ASSERT(updaterMemory);

    return new(updaterMemory) SkeletonUpdater(allocator);
}

SkeletonUpdater::SkeletonUpdater(os::IAllocator* allocator): 
    GfxObject(allocator) {}

SkeletonUpdater::~SkeletonUpdater() { }

void SkeletonUpdater::UpdateWorld(Skeleton* skeleton,const WorldMatrixUpdater& worldMatrixUpdater) const
{
    NW_NULL_ASSERT(skeleton);

    ResSkeleton resSkeleton = skeleton->GetResSkeleton();
    NW_ASSERT(resSkeleton.IsValid());
    bool isModelCoordinate = ut::CheckFlag(resSkeleton.GetFlags(), ResSkeletonData::FLAG_MODEL_COORDINATE);
    Skeleton::TransformPose& pose = skeleton->LocalTransformPose();
    Skeleton::TransformPose& worldPose = skeleton->WorldTransformPose();
    
    int index = 0;
    Skeleton::MatrixPose::MatrixRange range = skeleton->WorldMatrixPose().GetAllMatrices();
    WorldMatrixUpdater::ScalingRule scalingRule = static_cast<WorldMatrixUpdater::ScalingRule>(resSkeleton.GetScalingRule());

    if (scalingRule == WorldMatrixUpdater::SCALING_RULE_MAYA)
    {
        for (Skeleton::MatrixPose::MatrixArray::iterator matrix = range.first; matrix != range.second; ++matrix, ++index)
    {
            skeleton->PreCalculateMatrixSignal()(skeleton, index);

            if (pose.GetTransform(index)->IsEnabledFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED) )
            {

                ResBone bone = resSkeleton.GetBones(index);
                int parentBoneIndex = bone.GetParentBoneIndex();
                const CalculatedTransform* parentWorldTransform;
                const CalculatedTransform* parentLocalTransform;
                
                if (parentBoneIndex == -1)
                {
                    if (isModelCoordinate)
                    {
                        parentWorldTransform = &CalculatedTransform::Identity();
                        parentLocalTransform = &CalculatedTransform::Identity();
                    }
                    else{
                        parentWorldTransform = &(skeleton->GetOwnerSkeletalModel()->WorldTransform());
                        parentLocalTransform = &(skeleton->GetOwnerSkeletalModel()->Transform());
                    }
                }
                else{
                    parentLocalTransform = pose.GetTransform(parentBoneIndex);
                    parentWorldTransform = worldPose.GetTransform(parentBoneIndex);
                }

                worldMatrixUpdater.UpdateMaya(matrix,worldPose.GetTransform(index),*pose.GetTransform(index),*parentWorldTransform,
                    *parentLocalTransform,
                    ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_IS_SEGMENTSCALE_COMPENSATE));
            }
            skeleton->PostCalculateMatrixSignal()(skeleton, index);
        }
    }
    else if (scalingRule == WorldMatrixUpdater::SCALING_RULE_STANDARD)
    {
        for (Skeleton::MatrixPose::MatrixArray::iterator matrix = range.first; matrix != range.second; ++matrix, ++index)
    {
            skeleton->PreCalculateMatrixSignal()(skeleton, index);

            if (pose.GetTransform(index)->IsEnabledFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED))
            {
                ResBone bone = resSkeleton.GetBones(index);
                int parentBoneIndex = bone.GetParentBoneIndex();
                const CalculatedTransform* parentWorldTransform;
                const CalculatedTransform* parentLocalTransform;
                
                if (parentBoneIndex == -1)
                {
                    if (isModelCoordinate)
                    {
                        parentWorldTransform = &CalculatedTransform::Identity();
                        parentLocalTransform = &CalculatedTransform::Identity();
                    }
                    else{
                        parentWorldTransform = &(skeleton->GetOwnerSkeletalModel()->WorldTransform());
                        parentLocalTransform = &(skeleton->GetOwnerSkeletalModel()->Transform());
                    }
                }
                else
                {
                    parentLocalTransform = pose.GetTransform(parentBoneIndex);
                    parentWorldTransform = worldPose.GetTransform(parentBoneIndex);
                }

                worldMatrixUpdater.UpdateBasic(matrix,worldPose.GetTransform(index),
                    *pose.GetTransform(index),
                    *parentWorldTransform,
                    *parentLocalTransform);
            }

            skeleton->PostCalculateMatrixSignal()(skeleton, index);
        }
    }
    else{
        for (Skeleton::MatrixPose::MatrixArray::iterator matrix = range.first; matrix != range.second; ++matrix, ++index)
    {
            skeleton->PreCalculateMatrixSignal()(skeleton, index);

            if ( pose.GetTransform(index)->IsEnabledFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED) )
            {
                ResBone bone = resSkeleton.GetBones(index);
                int parentBoneIndex = bone.GetParentBoneIndex();
                const CalculatedTransform* parentWorldTransform;
                const CalculatedTransform* parentLocalTransform;
                
                if (parentBoneIndex == -1)
                {
                    if (isModelCoordinate)
                    {
                        parentWorldTransform = &CalculatedTransform::Identity();
                        parentLocalTransform = &CalculatedTransform::Identity();
                    }
                    else{
                        parentWorldTransform = &(skeleton->GetOwnerSkeletalModel()->WorldTransform());
                        parentLocalTransform = &(skeleton->GetOwnerSkeletalModel()->Transform());
                    }
                }
                else{
                    parentLocalTransform = pose.GetTransform(parentBoneIndex);
                    parentWorldTransform = worldPose.GetTransform(parentBoneIndex);
                }

                worldMatrixUpdater.UpdateXsi(matrix,worldPose.GetTransform(index),
                    *pose.GetTransform(index),
                    *parentWorldTransform,
                    *parentLocalTransform);
            }

            skeleton->PostCalculateMatrixSignal()(skeleton, index);
        }
    }
}

void SkeletonUpdater::UpdateView(Skeleton* skeleton,const BillboardUpdater& billboardUpdater,const Camera& camera) const
{
    NW_NULL_ASSERT(skeleton);

    ResSkeleton resSkeleton = skeleton->GetResSkeleton();
    NW_ASSERT(resSkeleton.IsValid());
    Skeleton::TransformPose& pose = skeleton->LocalTransformPose();
    Skeleton::TransformPose& worldPose = skeleton->WorldTransformPose();
    SkeletalModel* model = skeleton->GetOwnerSkeletalModel();
    
    math::VEC3 cameraPos = camera.WorldMatrix().GetColumn(3);

    int index = 0;
    Skeleton::MatrixPose::MatrixRange range = skeleton->WorldMatrixPose().GetAllMatrices();
    for (Skeleton::MatrixPose::MatrixArray::iterator matrix = range.first; matrix != range.second; ++matrix, ++index)
    {
        if ( !pose.GetTransform(index)->IsEnabledFlagsOr(
            CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED |
            CalculatedTransform::FLAG_FORCE_VIEW_CALCULATION_ENABLED))
            {
            continue;
        }

        ResBone bone = resSkeleton.GetBones(index);

        ResBone::BillboardMode billboardMode = bone.GetBillboardMode();
        if (billboardMode != ResBone::BILLBOARD_MODE_OFF)
        {
            NW_ASSERTMSG(!ut::CheckFlag(resSkeleton.GetFlags(), ResSkeletonData::FLAG_MODEL_COORDINATE),"The bone for billboard needs to be world-coordinate.");

            CalculatedTransform* worldTransform = worldPose.GetTransform(index);
            CalculatedTransform* localTransform = pose.GetTransform(index);
            
            billboardUpdater.Update(&(*matrix),camera.ViewMatrix(),camera.InverseViewMatrix(),
                cameraPos,
                *worldTransform,
                *localTransform,
                billboardMode);

            bool isScaleOne = worldTransform->IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE);

            if (!isScaleOne)
            {
                math::MTX34 worldMatrix;
                math::VEC3 rotateScale;
                math::MTX34MultScale(&worldMatrix, &worldTransform->TransformMatrix(), &localTransform->Scale());
                math::MTX34DecomposeToColumnScale(&rotateScale, &worldMatrix);
                math::MTX33Mult(&(*matrix),*matrix, math::MTX34().SetupScale(rotateScale));
            }
        }

        bool isCalcSkiningMatrix = ut::CheckFlag(bone.GetFlags(), ResBoneData::FLAG_HAS_SKINNING_MATRIX | ResBoneData::FLAG_IS_NEED_RENDERING);

        if (isCalcSkiningMatrix)
        {
            math::MTX34Mult(skeleton->SkiningMatrixPose().GetMatrix(index),matrix,&bone.GetInverseBaseMatrix());
        }
    }
}

}
}