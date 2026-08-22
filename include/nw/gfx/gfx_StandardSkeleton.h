#pragma once

#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

namespace nw{
namespace gfx{

class StandardSkeleton  : public Skeleton{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(StandardSkeleton);

public:
    NW_UT_RUNTIME_TYPEINFO;


    static StandardSkeleton* Create(ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory,Skeleton::TransformPose::TransformArray poseTransforms,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResSkeleton resource,int maxCallbacks, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, maxCallbacks);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResSkeleton resource,int maxCallbacks){
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(StandardSkeleton);

        size.Add(sizeof(Skeleton::TransformPose::Transform) * resource.GetBonesCount(), nw::os::IAllocator::CACHE_LINE_ALIGNMENT);
        size.Add(sizeof(nw::math::MTX34) * resource.GetBonesCount(), nw::os::IAllocator::CACHE_LINE_ALIGNMENT);
        size.Add(sizeof(nw::math::MTX34) * resource.GetBonesCount(), nw::os::IAllocator::CACHE_LINE_ALIGNMENT);

        Skeleton::GetMemorySizeForConstruct(pSize, maxCallbacks);
    }

    virtual TransformPose& LocalTransformPose() { return mPose; }

    virtual const TransformPose& LocalTransformPose() const { return mPose; }

    virtual TransformPose& WorldTransformPose() { return mWorldTransformPose; }

    virtual const TransformPose& WorldTransformPose() const { return mWorldTransformPose; }

    virtual MatrixPose& WorldMatrixPose() { return mWorldMatrixPose; }

    virtual const MatrixPose& WorldMatrixPose() const { return mWorldMatrixPose; }

    virtual MatrixPose& SkiningMatrixPose() { return mSkiningMatrixPose; }

    virtual const MatrixPose& SkiningMatrixPose() const { return mSkiningMatrixPose; }

    virtual OriginalPose& LocalOriginalPose() { return mOriginalPose; }

    virtual const OriginalPose& LocalOriginalPose() const { return mOriginalPose; }

protected:
    StandardSkeleton(nw::os::IAllocator* allocator,ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory,Skeleton::TransformPose::TransformArray poseTransforms,
        Skeleton::TransformPose::TransformArray poseWorldTransforms,
        Skeleton::MatrixPose::MatrixArray poseWorldMatrices,
        Skeleton::MatrixPose::MatrixArray skiningMatrices): 
        
        Skeleton(allocator, resource, maxCallbacks, isFixedSizeMemory),
        mPose(poseTransforms),
        mWorldTransformPose(poseWorldTransforms),
        mWorldMatrixPose(poseWorldMatrices),
        mSkiningMatrixPose(skiningMatrices),
        mOriginalPose(resource)
    {}

    virtual ~StandardSkeleton(){}

private:
    TransformPose mPose;
    TransformPose mWorldTransformPose;
    MatrixPose mWorldMatrixPose;
    MatrixPose mSkiningMatrixPose;
    OriginalPose mOriginalPose;
};

}
}