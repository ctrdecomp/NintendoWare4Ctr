#pragma once

#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

namespace nw{
namespace gfx{

class StandardSkeleton  : public Skeleton
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(StandardSkeleton);

public:
    NW_UT_RUNTIME_TYPEINFO;


    static StandardSkeleton* Create(ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory,Skeleton::TransformPose::TransformArray poseTransforms,nw::os::IAllocator* allocator);

    static size_t GetMemorySize(ResSkeleton resource,int maxCallbacks, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, maxCallbacks);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,ResSkeleton resource,int maxCallbacks)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(StandardSkeleton);

        size.Add(sizeof(Skeleton::TransformPose::Transform) * resource.GetBonesCount(), nw::os::IAllocator::CACHE_LINE_ALIGNMENT);
        size.Add(sizeof(nw::math::MTX34) * resource.GetBonesCount(), nw::os::IAllocator::CACHE_LINE_ALIGNMENT);
        size.Add(sizeof(nw::math::MTX34) * resource.GetBonesCount(), nw::os::IAllocator::CACHE_LINE_ALIGNMENT);

        Skeleton::GetMemorySizeForConstruct(pSize, maxCallbacks);
    }

    virtual TransformPose& LocalTransformPose() { return m_Pose; }

    virtual const TransformPose& LocalTransformPose() const { return m_Pose; }

    virtual TransformPose& WorldTransformPose() { return m_WorldTransformPose; }

    virtual const TransformPose& WorldTransformPose() const { return m_WorldTransformPose; }

    virtual MatrixPose& WorldMatrixPose() { return m_WorldMatrixPose; }

    virtual const MatrixPose& WorldMatrixPose() const { return m_WorldMatrixPose; }

    virtual MatrixPose& SkiningMatrixPose() { return m_SkiningMatrixPose; }

    virtual const MatrixPose& SkiningMatrixPose() const { return m_SkiningMatrixPose; }

    virtual OriginalPose& LocalOriginalPose() { return m_OriginalPose; }

    virtual const OriginalPose& LocalOriginalPose() const { return m_OriginalPose; }

protected:
    StandardSkeleton(nw::os::IAllocator* allocator,ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory,Skeleton::TransformPose::TransformArray poseTransforms,
        Skeleton::TransformPose::TransformArray poseWorldTransforms,
        Skeleton::MatrixPose::MatrixArray poseWorldMatrices,
        Skeleton::MatrixPose::MatrixArray skiningMatrices): 
        
        Skeleton(allocator, resource, maxCallbacks, isFixedSizeMemory),
        m_Pose(poseTransforms),
        m_WorldTransformPose(poseWorldTransforms),
        m_WorldMatrixPose(poseWorldMatrices),
        m_SkiningMatrixPose(skiningMatrices),
        m_OriginalPose(resource) {}

    virtual ~StandardSkeleton() {}

private:
    TransformPose m_Pose;
    TransformPose m_WorldTransformPose;
    MatrixPose m_WorldMatrixPose;
    MatrixPose m_SkiningMatrixPose;
    OriginalPose m_OriginalPose;
};

}
}