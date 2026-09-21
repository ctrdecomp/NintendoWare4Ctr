// Filename: gfx_WorldMatrixUpdater.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_WorldMatrixUpdater.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/math/math_Types.h>
#include <nw/os/os_Memory.h>

namespace nw {
namespace gfx {

WorldMatrixUpdater* WorldMatrixUpdater::Builder::Create(os::IAllocator* allocator)
{
    NW_NULL_ASSERT(allocator);
    
    void* updaterMemory = allocator->Alloc(sizeof(WorldMatrixUpdater));
    NW_NULL_ASSERT(updaterMemory);
    
    return new(updaterMemory) WorldMatrixUpdater(allocator);
}

WorldMatrixUpdater::WorldMatrixUpdater(os::IAllocator* allocator): 
    GfxObject(allocator)
{
    NW_NULL_ASSERT(allocator);
}

WorldMatrixUpdater::~WorldMatrixUpdater() {}

void WorldMatrixUpdater::CalculateWorldXsi(math::MTX34* transformMatrix,math::VEC3* scale,const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(transformMatrix);
    NW_NULL_ASSERT(scale);

    NW_UNUSED_VARIABLE(parentLocalTransform);

    const math::MTX34& parentMatrix = parentWorldTransform.TransformMatrix();

    if (localTransform.IsEnabledFlagsOr(CalculatedTransform::FLAG_IS_IDENTITY | CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO))
    {
        math::MTX34Copy(transformMatrix, parentMatrix);
    }
    else
    {
        bool isParentScaleOne = parentWorldTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE);
        const math::VEC3& parentScale = parentWorldTransform.m_Scale;

        const math::MTX34& localMatrix = localTransform.TransformMatrix();
        math::VEC3 localTranslate = localTransform.TransformMatrix().GetColumn(3);

        if (localTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_ROTATE_ZERO))
        {

            if (isParentScaleOne)
            {
                math::MTX34MultTranslate(transformMatrix, &parentMatrix, &localTranslate);
            }
            else
            {
                math::VEC3Mult(&localTranslate, &localTranslate, &parentScale);
                math::MTX34MultTranslate(transformMatrix, &parentMatrix, &localTranslate);
            }
        }
        else{
            if (isParentScaleOne)
            {
                math::MTX34Mult(transformMatrix, &parentMatrix, &localMatrix);
            }
            else
            {
                math::MTX34 scaledLocalMatrix;
                math::MTX34Copy(&scaledLocalMatrix, &localMatrix);
                scaledLocalMatrix.matrix[0][3] *= parentScale.x;
                scaledLocalMatrix.matrix[1][3] *= parentScale.y;
                scaledLocalMatrix.matrix[2][3] *= parentScale.z;
                math::MTX34Mult(transformMatrix, &parentMatrix, &scaledLocalMatrix);
            }
        }
    }

    if (parentWorldTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
    {
        scale->Set(localTransform.m_Scale);
    }
    else
    {
        math::VEC3Mult(scale, &parentWorldTransform.m_Scale, &localTransform.m_Scale);
    }
}

void WorldMatrixUpdater::CalculateWorldMayaSsc(math::MTX34* transformMatrix,math::VEC3* scale,const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(transformMatrix);
    NW_NULL_ASSERT(scale);

    const math::MTX34& parentMatrix = parentWorldTransform.m_TransformMatrix;

    if (localTransform.IsEnabledFlagsOr(CalculatedTransform::FLAG_IS_IDENTITY | CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO))
    {
        math::MTX34Copy(transformMatrix, parentMatrix);
    }
    else{
        const math::MTX34& localMatrix = localTransform.m_TransformMatrix;
        math::VEC3 localTranslate = localTransform.m_TransformMatrix.GetColumn(3);
        
        if (localTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_ROTATE_ZERO))
        {
            if (parentLocalTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
            {

                math::MTX34MultTranslate(transformMatrix, &parentMatrix, &localTranslate);
            }
            else
            {

                math::MTX34Copy(transformMatrix, parentMatrix);
                math::MTX34 scaledParentRotate(*transformMatrix);
                const math::VEC3& parentScale = parentLocalTransform.m_Scale;
                this->ScaleMatrix(&scaledParentRotate, parentScale);
                math::VEC3Transform(&localTranslate, &scaledParentRotate, &localTranslate);
                this->AddTranslate(transformMatrix, localTranslate);
            }
        }
        else
        {
            if (parentLocalTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
            {
                math::MTX34MultTranslate(transformMatrix, &parentMatrix, &localTranslate);
                math::MTX33Mult(transformMatrix, transformMatrix, &localMatrix);
            }
            else
            {
                math::MTX34Copy(transformMatrix, parentMatrix);
                math::MTX34 scaledParentRotate(*transformMatrix);
                const math::VEC3& parentScale = parentLocalTransform.m_Scale;
                this->ScaleMatrix(&scaledParentRotate, parentScale);
                math::VEC3Transform(&localTranslate, &scaledParentRotate, &localTranslate);
                this->AddTranslate(transformMatrix, localTranslate);
                math::MTX33Mult(transformMatrix, transformMatrix, &localMatrix);
            }
        }
    }

    if (parentWorldTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
    {
        scale->Set(localTransform.m_Scale);
    }
    else
    {
        math::VEC3Mult(scale, &parentWorldTransform.m_Scale, &localTransform.m_Scale);
    }
}

void WorldMatrixUpdater::CalculateWorldBasic(math::MTX34* transformMatrix, math::VEC3* scale,const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(transformMatrix);
    NW_NULL_ASSERT(scale);

    const math::MTX34& parentMatrix = parentWorldTransform.m_TransformMatrix;

    if (localTransform.IsEnabledFlagsOr(CalculatedTransform::FLAG_IS_IDENTITY | CalculatedTransform::FLAG_IS_ROTATE_TRANSLATE_ZERO))
    {
        if (parentLocalTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
        {
            math::MTX34Copy(transformMatrix, parentMatrix);
        }
        else
        {

            this->MultScale(transformMatrix, &parentMatrix, parentLocalTransform.m_Scale);
            this->CopyTranslate(transformMatrix, parentMatrix);
        }
    }
    else
    {
        if (localTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_ROTATE_ZERO))
        {
            if (parentLocalTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
            {
                math::VEC3 localTranslate = localTransform.m_TransformMatrix.GetColumn(3);

                math::MTX34MultTranslate(transformMatrix, &parentMatrix, &localTranslate);
            }
            else
            {
                math::VEC3 localTranslate = localTransform.m_TransformMatrix.GetColumn(3);

                this->MultScale(transformMatrix, &parentMatrix, parentLocalTransform.m_Scale);
                this->CopyTranslate(transformMatrix, parentMatrix);
                math::MTX34MultTranslate(transformMatrix, transformMatrix, &localTranslate);
            }
        }
        else
        {
            if (parentLocalTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
            {
                math::VEC3 localTranslate = localTransform.m_TransformMatrix.GetColumn(3);

                math::MTX34MultTranslate(transformMatrix, &parentMatrix, &localTranslate);
                math::MTX33Mult(transformMatrix, transformMatrix, &localTransform.m_TransformMatrix);
            }
            else
            {
                math::VEC3 localTranslate = localTransform.m_TransformMatrix.GetColumn(3);

                this->MultScale(transformMatrix, &parentMatrix, parentLocalTransform.m_Scale);
                this->CopyTranslate(transformMatrix, parentMatrix);
                math::MTX34MultTranslate(transformMatrix, transformMatrix, &localTranslate);
                math::MTX33Mult(transformMatrix, transformMatrix, &localTransform.m_TransformMatrix);
            }
        }
    }

    if (parentWorldTransform.IsEnabledFlags(CalculatedTransform::FLAG_IS_SCALE_ONE))
    {
        scale->Set(localTransform.m_Scale);
    }
    else
    {
        math::VEC3Mult(scale, &parentWorldTransform.m_Scale, &localTransform.m_Scale);
    }
}

}
}