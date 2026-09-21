// Filename: gfx_StandardSkeleton.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_StandardSkeleton.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/math/math_Types.h>
#include <nw/ut/ut_ResUtil.h>
#include <nw/ut/ut_ResDictionary.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(StandardSkeleton, Skeleton);

StandardSkeleton* StandardSkeleton::Create(ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory,Skeleton::TransformPose::TransformArray poseTransforms,os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    NW_ASSERT(resource.IsValid());

    void* memory = allocator->Alloc(sizeof(StandardSkeleton));
    NW_NULL_ASSERT(memory);
    Skeleton::TransformPose::TransformArray poseWorldTransforms(resource.GetBonesCount(), allocator);
    Skeleton::MatrixPose::MatrixArray poseWorldMatrices(resource.GetBonesCount(), allocator);
    Skeleton::MatrixPose::MatrixArray skiningMatrices(resource.GetBonesCount(), allocator);

    for (int i = 0; i < resource.GetBonesCount(); ++i)
    {
        poseWorldTransforms.PushBackFast(CalculatedTransform::Identity());
        poseWorldMatrices.PushBackFast(nw::math::MTX34::Identity());
        skiningMatrices.PushBackFast(nw::math::MTX34::Identity());
    }

    StandardSkeleton* skeleton = new(memory) StandardSkeleton(allocator, resource,maxCallbacks,isFixedSizeMemory,poseTransforms, poseWorldTransforms, poseWorldMatrices, skiningMatrices);
    
    return skeleton;
}

}
}