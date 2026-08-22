#pragma once

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResSkeleton.h>
#include <nw/gfx/gfx_CalculatedTransform.h>
#include <nw/ut/ut_Signal.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw{
namespace gfx{
class SkeletalModel;

class Skeleton : public SceneObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(Skeleton);

public:
    NW_UT_RUNTIME_TYPEINFO;

    typedef nw::ut::Signal2<void, Skeleton*, int> CalculateMatrixSignal;

    typedef CalculateMatrixSignal::SlotType CalculateMatrixSlot;

    class TransformPose{
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(TransformPose);

    public:
        typedef CalculatedTransform Transform;
        typedef nw::ut::MoveArray<Transform> TransformArray;
        typedef std::pair<TransformArray::iterator, TransformArray::iterator> TransformRange;
        typedef std::pair<TransformArray::const_iterator, TransformArray::const_iterator> ConstTransformRange;

        TransformPose() {}
        explicit TransformPose(TransformArray& transforms): 
            mTransforms(transforms) 
        {}
        int GetBonesCount() const { return this->mTransforms.Size(); }

        bool GetTransform(int index, Transform** transform){
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *transform = &this->mTransforms[index];
            return true;
        }

        bool GetTransform(int index, const Transform** transform) const{
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *transform = &this->mTransforms[index];
            return true;
        }

        Transform* GetTransform(int index){
            return &this->mTransforms[index];
        }

        const Transform* GetTransform(int index) const{
            return &this->mTransforms[index];
        }

        TransformArray::iterator GetBeginTransform() { return this->mTransforms.Begin(); }

        TransformArray::iterator GetEndTransform() { return this->mTransforms.End(); }

        TransformRange GetAllTransforms(){
            return TransformRange(this->mTransforms.Begin(), this->mTransforms.End());
        }

        ConstTransformRange GetAllTransforms() const{
            return ConstTransformRange(this->mTransforms.Begin(), this->mTransforms.End());
        }
        
    private:
        TransformArray mTransforms;
    };

    class MatrixPose{
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(MatrixPose);
        
    public:
        typedef nw::ut::MoveArray<nw::math::MTX34> MatrixArray;
        typedef std::pair<MatrixArray::iterator, MatrixArray::iterator> MatrixRange;
        typedef std::pair<MatrixArray::const_iterator, MatrixArray::const_iterator> ConstMatrixRange;

        MatrixPose() {}
        explicit MatrixPose(MatrixArray& matrices): 
            mMatrices(matrices) 
        {}

        int GetBonesCount() const { return this->mMatrices.Size(); }

        bool GetMatrix(int index, nw::math::MTX34** matrix){
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *matrix = &this->mMatrices[index];
            return true;
        }

        bool GetMatrix(int index, const nw::math::MTX34** matrix) const{
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            *matrix = &this->mMatrices[index];
            return true;
        }

        nw::math::MTX34* GetMatrix(int index){
            return &this->mMatrices[index];
        }

        const nw::math::MTX34* GetMatrix(int index) const{
            return &this->mMatrices[index];
        }

        MatrixArray::iterator GetBeginMatrix() { return this->mMatrices.Begin(); }

        MatrixArray::iterator GetEndMatrix() { return this->mMatrices.End(); }

        MatrixRange GetAllMatrices(){
            return MatrixRange(this->mMatrices.Begin(), this->mMatrices.End());
        }

        ConstMatrixRange GetAllMatrices() const{
            return ConstMatrixRange(this->mMatrices.Begin(), this->mMatrices.End());
        }
        
    private:
        MatrixArray mMatrices;
    };

    class OriginalPose{
    private:
        NW_DISALLOW_COPY_AND_ASSIGN(OriginalPose);
    public:
        typedef nw::math::Transform3 Transform;

        explicit OriginalPose(ResSkeleton resource): 
            mResource(resource)
        {}

        int GetBonesCount() const { return this->mResource.GetBonesCount(); }

        bool GetTransform(int index, Transform** transform){
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            ResBone bone = this->mResource.GetBones(index);
            if (!bone.IsValid()) { return false; }
            *transform = &bone.GetTransform();
            return true;
        }

        bool GetTransform(int index, const Transform** transform) const{
            if (index < 0 || this->GetBonesCount() <= index) { return false; }
            ResBone bone = this->mResource.GetBones(index);
            if (!bone.IsValid()) { return false; }
            *transform = &bone.GetTransform();
            return true;
        }

        Transform* GetTransform(int index){
            return &this->mResource.GetBones(index).GetTransform();
        }

        const Transform* GetTransform(int index) const{
            return &this->mResource.GetBones(index).GetTransform();
        }
        
    private:
        ResSkeleton mResource;
    };

    ResSkeleton GetResSkeleton(){
        return ResStaticCast<ResSkeleton>(this->GetResSceneObject());
    }

    const ResSkeleton GetResSkeleton() const{
        return ResStaticCast<ResSkeleton>(this->GetResSceneObject());
    }


    bool IsUpdated(){
        return this->mIsUpdated;
    }

    void SetUpdated(bool isUpdated){
        this->mIsUpdated = isUpdated;
    }

    const SkeletalModel* GetOwnerSkeletalModel() const { return mOwnerSkeletalModel; }

    SkeletalModel* GetOwnerSkeletalModel() { return mOwnerSkeletalModel; }

    void SetOwnerSkeletalModel(SkeletalModel* ownerSkeletalModel){
        mOwnerSkeletalModel = ownerSkeletalModel;
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


    CalculateMatrixSignal& PreCalculateMatrixSignal() { return *this->mPreCalculateMatrixSignal; }

    const CalculateMatrixSignal& PreCalculateMatrixSignal() const { return *this->mPreCalculateMatrixSignal; }

    CalculateMatrixSignal& PostCalculateMatrixSignal() { return *this->mPostCalculateMatrixSignal; }

    const CalculateMatrixSignal& PostCalculateMatrixSignal() const { return *this->mPostCalculateMatrixSignal; }

protected:
    Skeleton(nw::os::IAllocator* allocator,ResSkeleton resource,int maxCallbacks,bool isFixedSizeMemory): 
        SceneObject(allocator, resource),
        mPreCalculateMatrixSignal(NULL),
        mPostCalculateMatrixSignal(NULL),
        mIsUpdated(false){
        this->SetOwnerSkeletalModel(NULL);
        CreateCallbacks(allocator, maxCallbacks, isFixedSizeMemory);
    }

    virtual ~Skeleton(){
        SafeDestroy(this->mPreCalculateMatrixSignal);
        SafeDestroy(this->mPostCalculateMatrixSignal);
    }
    
    static void GetMemorySizeForConstruct(nw::os::MemorySizeCalculator* pSize,int maxCallbacks){
        if (maxCallbacks == 0){
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
    
    SkeletalModel* mOwnerSkeletalModel;
    
    CalculateMatrixSignal* mPreCalculateMatrixSignal;
    CalculateMatrixSignal* mPostCalculateMatrixSignal;
    bool mIsUpdated;
};
}
}