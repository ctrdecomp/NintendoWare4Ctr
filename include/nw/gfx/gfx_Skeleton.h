#pragma once

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/ut/ut_Signal.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw{
namespace gfx{
class SkeletalModel;

class Skeleton : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Skeleton);

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::Signal2<void, Skeleton*, int> CalculateMatrixSignal;

    typedef CalculateMatrixSignal::SlotType CalculateMatrixSlot;

    class TransformPose
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(TransformPose);

    public:
        typedef CalculatedTransform Transform;
        typedef nw::ut::MoveArray<Transform> TransformArray;
        typedef std::pair<TransformArray::iterator, TransformArray::iterator> TransformRange;
        typedef std::pair<TransformArray::const_iterator, TransformArray::const_iterator> ConstTransformRange;

        TransformPose() {}
        explicit TransformPose(TransformArray& transforms): 
            m_Transforms(transforms) {}
        int GetBonesCount() const { return this->m_Transforms.Size(); }

        bool GetTransform(int index, Transform** transform)
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *transform = &this->m_Transforms[index];
            return true;
        }

        bool GetTransform(int index, const Transform** transform) const
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *transform = &this->m_Transforms[index];
            return true;
        }

        Transform* GetTransform(int index)
        {
            return &this->m_Transforms[index];
        }

        const Transform* GetTransform(int index) const
        {
            return &this->m_Transforms[index];
        }

        TransformArray::iterator GetBeginTransform() { return this->m_Transforms.Begin(); }

        TransformArray::iterator GetEndTransform() { return this->m_Transforms.End(); }

        TransformRange GetAllTransforms()
        {
            return TransformRange(this->m_Transforms.Begin(), this->m_Transforms.End());
        }

        ConstTransformRange GetAllTransforms() const
        {
            return ConstTransformRange(this->m_Transforms.Begin(), this->m_Transforms.End());
        }
        
    private:
        TransformArray m_Transforms;
    };

    class MatrixPose
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(MatrixPose);
        
    public:
        typedef nw::ut::MoveArray<nw::math::MTX34> MatrixArray;
        typedef std::pair<MatrixArray::iterator, MatrixArray::iterator> MatrixRange;
        typedef std::pair<MatrixArray::const_iterator, MatrixArray::const_iterator> ConstMatrixRange;

        MatrixPose() {}
        explicit MatrixPose(MatrixArray& matrices): 
            m_Matrices(matrices) {}

        int GetBonesCount() const { return this->m_Matrices.Size(); }

        bool GetMatrix(int index, nw::math::MTX34** matrix)
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *matrix = &this->m_Matrices[index];
            return true;
        }

        bool GetMatrix(int index, const nw::math::MTX34** matrix) const
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *matrix = &this->m_Matrices[index];
            return true;
        }

        nw::math::MTX34* GetMatrix(int index)
        {
            return &this->m_Matrices[index];
        }

        const nw::math::MTX34* GetMatrix(int index) const
        {
            return &this->m_Matrices[index];
        }

        MatrixArray::iterator GetBeginMatrix() { return this->m_Matrices.Begin(); }

        MatrixArray::iterator GetEndMatrix() { return this->m_Matrices.End(); }

        MatrixRange GetAllMatrices()
        {
            return MatrixRange(this->m_Matrices.Begin(), this->m_Matrices.End());
        }

        ConstMatrixRange GetAllMatrices() const
        {
            return ConstMatrixRange(this->m_Matrices.Begin(), this->m_Matrices.End());
        }
        
    private:
        MatrixArray m_Matrices;
    };

    class OriginalPose
    {
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(OriginalPose);
    public:
        typedef nw::math::Transform3 Transform;

        explicit OriginalPose(ResSkeleton resource): 
            m_Resource(resource) {}

        int GetBonesCount() const { return this->m_Resource.GetBonesCount(); }

        bool GetTransform(int index, Transform** transform)
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            ResBone bone = this->m_Resource.GetBones(index);
            if (!bone.IsValid()) { return false; }
            *transform = &bone.GetTransform();
            return true;
        }

        bool GetTransform(int index, const Transform** transform) const
        {
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            ResBone bone = this->m_Resource.GetBones(index);
            if (!bone.IsValid()) { return false; }
            *transform = &bone.GetTransform();
            return true;
        }

        Transform* GetTransform(int index)
        {
            return &this->m_Resource.GetBones(index).GetTransform();
        }

        const Transform* GetTransform(int index) const
        {
            return &this->m_Resource.GetBones(index).GetTransform();
        }
        
    private:
        ResSkeleton m_Resource;
    };

    ResSkeleton GetResSkeleton()
    {
        return ResStaticCast<ResSkeleton>(this->GetResSceneObject());
    }

    const ResSkeleton GetResSkeleton() const
    {
        return ResStaticCast<ResSkeleton>(this->GetResSceneObject());
    }


    bool IsUpdated()
    {
        return this->m_IsUpdated;
    }

    void SetUpdated(bool isUpdated)
    {
        this->m_IsUpdated = isUpdated;
    }

    const SkeletalModel* GetOwnerSkeletalModel() const { return m_OwnerSkeletalModel; }

    SkeletalModel* GetOwnerSkeletalModel() { return m_OwnerSkeletalModel; }

    void SetOwnerSkeletalModel(SkeletalModel* ownerSkeletalModel)
    {
        m_OwnerSkeletalModel = ownerSkeletalModel;
    }

    virtual TransformPose& LocalTransformPose() = 0;
    virtual const TransformPose& LocalTransformPose() const = 0;
    virtual TransformPose& WorldTransformPose() = 0;
    virtual const TransformPose& WorldTransformPose() const = 0;
    virtual MatrixPose& WorldMatrixPose() = 0;
    virtual const MatrixPose& WorldMatrixPose() const = 0;
    virtual MatrixPose& SkiningMatrixPose() = 0;
    virtual const MatrixPose& SkiningMatrixPose() const = 0;
    virtual OriginalPose& LocalOriginalPose() = 0;
    virtual const OriginalPose& LocalOriginalPose() const = 0;


    CalculateMatrixSignal& PreCalculateMatrixSignal() { return *this->m_PreCalculateMatrixSignal; }

    const CalculateMatrixSignal& PreCalculateMatrixSignal() const { return *this->m_PreCalculateMatrixSignal; }

    CalculateMatrixSignal& PostCalculateMatrixSignal() { return *this->m_PostCalculateMatrixSignal; }

    const CalculateMatrixSignal& PostCalculateMatrixSignal() const { return *this->m_PostCalculateMatrixSignal; }

protected:
    Skeleton(nw::os::IAllocator* allocator,ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory): 
        SceneObject(allocator, resource),
        m_PreCalculateMatrixSignal(NULL),
        m_PostCalculateMatrixSignal(NULL),
        m_IsUpdated(false)
        {
        this->SetOwnerSkeletalModel(NULL);
        CreateCallbacks(allocator, maxCallbacks, isFixedSizeMemory);
    }

    virtual ~Skeleton()
    {
        SafeDestroy(this->m_PreCalculateMatrixSignal);
        SafeDestroy(this->m_PostCalculateMatrixSignal);
    }
    
    static void GetMemorySizeForConstruct(nw::os::MemorySizeCalculator* pSize,int maxCallbacks)
    {
        if (maxCallbacks == 0)
        {
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
            CalculateMatrixSignal::GetMemorySizeForInvalidateSignalInternal(pSize);
        }
        else{
            CalculateMatrixSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, maxCallbacks);
            CalculateMatrixSignal::GetMemorySizeForFixedSizedSignalInternal(pSize, maxCallbacks);
        }
    }

private:

    void CreateCallbacks(nw::os::IAllocator* allocator, int maxCallbacks, bool isFixedSizeMemory);
    
    SkeletalModel* m_OwnerSkeletalModel;
    
    CalculateMatrixSignal* m_PreCalculateMatrixSignal;
    CalculateMatrixSignal* m_PostCalculateMatrixSignal;
    bool m_IsUpdated;
};
}
}