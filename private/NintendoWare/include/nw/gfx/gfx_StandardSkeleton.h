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

#ifndef NW_GFX_STANDARDSKELETON_H_
#define NW_GFX_STANDARDSKELETON_H_

#include <nw/gfx/gfx_Skeleton.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class StandardSkeleton  : public Skeleton
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(StandardSkeleton);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
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
    static StandardSkeleton* Create(
        ResSkeleton resource,
        int maxCallbacks,
        bool isFixedSizeMemory,
        Skeleton::TransformPose::TransformArray poseTransforms,
        os::IAllocator* allocator
    );


    //
    //
    //
    //
    //
    static size_t GetMemorySize(
        ResSkeleton resource,
        int maxCallbacks,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource, maxCallbacks);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResSkeleton resource,
        int maxCallbacks)
    {
        // StandardSkeleton::Create
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(StandardSkeleton);
#ifdef NW_MOVE_ARRAY_CACHE_LINE_ALIGNMENT_ENABLED
        size.Add(sizeof(Skeleton::TransformPose::Transform) * resource.GetBonesCount(), Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
        size.Add(sizeof(math::MTX34) * resource.GetBonesCount(), Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
        size.Add(sizeof(math::MTX34) * resource.GetBonesCount(), Skeleton::TransformPose::TransformArray::MEMORY_ALIGNMENT);
#else
        size += sizeof(Skeleton::TransformPose::Transform) * resource.GetBonesCount();
        size += sizeof(math::MTX34) * resource.GetBonesCount();
        size += sizeof(math::MTX34) * resource.GetBonesCount();
#endif
        Skeleton::GetMemorySizeForConstruct(pSize, maxCallbacks);
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual TransformPose& LocalTransformPose() { return m_Pose; }

    //
    //
    //
    //
    virtual const TransformPose& LocalTransformPose() const { return m_Pose; }

    //
    //
    //
    //
    virtual TransformPose& WorldTransformPose() { return m_WorldTransformPose; }

    //
    //
    //
    //
    virtual const TransformPose& WorldTransformPose() const { return m_WorldTransformPose; }

    //
    //
    //
    //
    virtual MatrixPose& WorldMatrixPose() { return m_WorldMatrixPose; }

    //
    //
    //
    //
    virtual const MatrixPose& WorldMatrixPose() const { return m_WorldMatrixPose; }

    //
    //
    //
    //
    virtual MatrixPose& SkiningMatrixPose() { return m_SkiningMatrixPose; }

    //
    //
    //
    //
    virtual const MatrixPose& SkiningMatrixPose() const { return m_SkiningMatrixPose; }

    //
    //
    //
    //
    virtual OriginalPose& LocalOriginalPose() { return m_OriginalPose; }

    //
    //
    //
    //
    virtual const OriginalPose& LocalOriginalPose() const { return m_OriginalPose; }

    //

protected:
    //----------------------------------------
    //
    //

    //
    StandardSkeleton(
        os::IAllocator* allocator,
        ResSkeleton resource,
        int maxCallbacks,
        bool isFixedSizeMemory,
        Skeleton::TransformPose::TransformArray poseTransforms,
        Skeleton::TransformPose::TransformArray poseWorldTransforms,
        Skeleton::MatrixPose::MatrixArray poseWorldMatrices,
        Skeleton::MatrixPose::MatrixArray skiningMatrices)
    : Skeleton(allocator, resource, maxCallbacks, isFixedSizeMemory),
      m_Pose(poseTransforms),
      m_WorldTransformPose(poseWorldTransforms),
      m_WorldMatrixPose(poseWorldMatrices),
      m_SkiningMatrixPose(skiningMatrices),
      m_OriginalPose(resource)
    {
    }

    //
    virtual ~StandardSkeleton()
    {}

    //

private:
    TransformPose m_Pose;
    TransformPose m_WorldTransformPose;
    MatrixPose m_WorldMatrixPose;
    MatrixPose m_SkiningMatrixPose;
    OriginalPose m_OriginalPose;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_STANDARDSKELETON_H_
