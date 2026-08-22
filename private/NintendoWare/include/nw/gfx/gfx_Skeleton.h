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

#ifndef NW_GFX_SKELETON_H_
#define NW_GFX_SKELETON_H_

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/ut/ut_Signal.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace math
{
struct MTX34;
struct Transform3;
}

namespace gfx
{

class SkeletalModel;
class CalculatedTransform;

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class Skeleton  : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Skeleton);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    //
    //
    //
    typedef ut::Signal2<void, Skeleton*, int> CalculateMatrixSignal;

    //
    typedef CalculateMatrixSignal::SlotType CalculateMatrixSlot;

    //
    class TransformPose
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(TransformPose);

    public:
        //
        typedef CalculatedTransform Transform;
        //
        typedef ut::MoveArray<Transform> TransformArray;
        //
        typedef std::pair<TransformArray::iterator, TransformArray::iterator> TransformRange;
        //
        typedef std::pair<TransformArray::const_iterator, TransformArray::const_iterator> ConstTransformRange;

        //
        TransformPose() {}

        //
        explicit TransformPose(TransformArray& transforms) : m_Transforms(transforms) {}

        //
        int GetBonesCount() const { return m_Transforms.Size(); }

        //
        bool GetTransform(int index, Transform** transform)
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *transform = &this->m_Transforms[index];
            return true;
        }

        //
        bool GetTransform(int index, const Transform** transform) const
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *transform = &this->m_Transforms[index];
            return true;
        }

        //
        Transform* GetTransform(int index)
        {
            return &this->m_Transforms[index];
        }

        //
        const Transform* GetTransform(int index) const
        {
            return &this->m_Transforms[index];
        }

        //
        TransformArray::iterator GetBeginTransform() { return m_Transforms.Begin(); }

        //
        TransformArray::iterator GetEndTransform() { return m_Transforms.End(); }

        //
        TransformRange GetAllTransforms()
        {
            return TransformRange(m_Transforms.Begin(), m_Transforms.End());
        }

        //
        ConstTransformRange GetAllTransforms() const
        {
            return ConstTransformRange(m_Transforms.Begin(), m_Transforms.End());
        }

    private:
        TransformArray m_Transforms;
    };

    //
    class MatrixPose
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(MatrixPose);

    public:
        //
        typedef ut::MoveArray<math::MTX34> MatrixArray;
        //
        typedef std::pair<MatrixArray::iterator, MatrixArray::iterator> MatrixRange;
        //
        typedef std::pair<MatrixArray::const_iterator, MatrixArray::const_iterator> ConstMatrixRange;

        //
        MatrixPose() {}

        //
        explicit MatrixPose(MatrixArray& matrices) : m_Matrices(matrices) {}

        //
        int GetBonesCount() const { return m_Matrices.Size(); }

        //
        bool GetMatrix(int index, math::MTX34** matrix)
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *matrix = &this->m_Matrices[index];
            return true;
        }

        //
        bool GetMatrix(int index, const math::MTX34** matrix) const
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *matrix = &this->m_Matrices[index];
            return true;
        }

        //
        math::MTX34* GetMatrix(int index)
        {
            return &this->m_Matrices[index];
        }

        //
        const math::MTX34* GetMatrix(int index) const
        {
            return &this->m_Matrices[index];
        }

        //
        MatrixArray::iterator GetBeginMatrix() { return m_Matrices.Begin(); }

        //
        MatrixArray::iterator GetEndMatrix() { return m_Matrices.End(); }

        //
        MatrixRange GetAllMatrices()
        {
            return MatrixRange(m_Matrices.Begin(), m_Matrices.End());
        }

        //
        ConstMatrixRange GetAllMatrices() const
        {
            return ConstMatrixRange(m_Matrices.Begin(), m_Matrices.End());
        }

    private:
        MatrixArray m_Matrices;
    };

    //
    class OriginalPose
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(OriginalPose);

    public:
        //
        typedef math::Transform3 Transform;

        //
        explicit OriginalPose(ResSkeleton resource) : m_Resource(resource) {}

        //
        int GetBonesCount() const { return m_Resource.GetBonesCount(); }

        //
        bool GetTransform(int index, Transform** transform)
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            ResBone bone = m_Resource.GetBones(index);
            if (!bone.IsValid()) { return false; }
            *transform = &bone.GetTransform();
            return true;
        }

        //
        bool GetTransform(int index, const Transform** transform) const
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            ResBone bone = m_Resource.GetBones(index);
            if (!bone.IsValid()) { return false; }
            *transform = &bone.GetTransform();
            return true;
        }

        //
        Transform* GetTransform(int index)
        {
            return &m_Resource.GetBones(index).GetTransform();
        }

        //
        const Transform* GetTransform(int index) const
        {
            return &m_Resource.GetBones(index).GetTransform();
        }

    private:
        ResSkeleton m_Resource;
    };

    //----------------------------------------
    //
    //

    //
    ResSkeleton GetResSkeleton()
    {
        return ResStaticCast<ResSkeleton>(this->GetResSceneObject());
    }

    //
    const ResSkeleton GetResSkeleton() const
    {
        return ResStaticCast<ResSkeleton>(this->GetResSceneObject());
    }

    //

    //----------------------------------------
    //
    //

    //
    bool IsUpdated()
    {
        return this->m_IsUpdated;
    }

    //
    void SetUpdated(bool isUpdated)
    {
        this->m_IsUpdated = isUpdated;
    }

    //

    //----------------------------------------
    //
    //

    //
    const SkeletalModel* GetOwnerSkeletalModel() const { return m_OwnerSkeletalModel; }

    //
    SkeletalModel* GetOwnerSkeletalModel() { return m_OwnerSkeletalModel; }

    //
    void SetOwnerSkeletalModel(SkeletalModel* ownerSkeletalModel)
    {
        m_OwnerSkeletalModel = ownerSkeletalModel;
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    //
    virtual TransformPose& LocalTransformPose() = 0;

    //
    //
    //
    //
    virtual const TransformPose& LocalTransformPose() const = 0;

    //
    //
    //
    //
    virtual TransformPose& WorldTransformPose() = 0;

    //
    //
    //
    //
    virtual const TransformPose& WorldTransformPose() const = 0;

    //
    //
    //
    //
    virtual MatrixPose& WorldMatrixPose() = 0;

    //
    //
    //
    //
    virtual const MatrixPose& WorldMatrixPose() const = 0;

    //
    //
    //
    //
    virtual MatrixPose& SkiningMatrixPose() = 0;

    //
    //
    //
    //
    virtual const MatrixPose& SkiningMatrixPose() const = 0;

    //
    //
    //
    //
    virtual OriginalPose& LocalOriginalPose() = 0;

    //
    //
    //
    //
    virtual const OriginalPose& LocalOriginalPose() const = 0;

    //

    //----------------------------------------
    //
    //

    //
    //
    //
    CalculateMatrixSignal& PreCalculateMatrixSignal() { return *m_PreCalculateMatrixSignal; }

    //
    //
    //
    const CalculateMatrixSignal& PreCalculateMatrixSignal() const { return *m_PreCalculateMatrixSignal; }

    //
    //
    //
    CalculateMatrixSignal& PostCalculateMatrixSignal() { return *m_PostCalculateMatrixSignal; }

    //
    //
    //
    const CalculateMatrixSignal& PostCalculateMatrixSignal() const { return *m_PostCalculateMatrixSignal; }

    //

protected:
    //----------------------------------------
    //
    //

    //
    Skeleton(
        os::IAllocator* allocator,
        ResSkeleton resource,
        int maxCallbacks,
        bool isFixedSizeMemory)
    :
    SceneObject(allocator, resource),
    m_PreCalculateMatrixSignal(NULL),
    m_PostCalculateMatrixSignal(NULL),
    m_IsUpdated(false)
    {
        this->SetOwnerSkeletalModel(NULL);
        CreateCallbacks(allocator, maxCallbacks, isFixedSizeMemory);
    }

    //
    virtual ~Skeleton()
    {
        SafeDestroy(this->m_PreCalculateMatrixSignal);
        SafeDestroy(this->m_PostCalculateMatrixSignal);
    }

    //

    //
    //
    //
    static void GetMemorySizeForConstruct(
        os::MemorySizeCalculator* pSize,
        int maxCallbacks)
    {
        // Skeleton::CreateCallbacks
        if (maxCallbacks == 0)
        {
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else
        {
            CalculateMatrixSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, maxCallbacks);
            CalculateMatrixSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, maxCallbacks);
        }
    }

private:
    //
    void CreateCallbacks(os::IAllocator* allocator, int maxCallbacks, bool isFixedSizeMemory);

    SkeletalModel* m_OwnerSkeletalModel;

    CalculateMatrixSignal* m_PreCalculateMatrixSignal;
    CalculateMatrixSignal* m_PostCalculateMatrixSignal;
    bool m_IsUpdated;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_SKELETON_H_
