#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

#include <nw/ut/ut_MoveArray.h>
#include <nw/math/math_Types.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{

class CalculatedTransform;

class WorldMatrixUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(WorldMatrixUpdater);

public:
    enum ScalingRule
    {
        SCALING_RULE_STANDARD,
        SCALING_RULE_MAYA,
        SCALING_RULE_SOFTIMAGE
    };

public:
    class Builder
    {
    public:
        Builder() {}
        WorldMatrixUpdater* Create(nw::os::IAllocator* allocator);
    };

    inline void UpdateMaya(nw::math::MTX34* worldMatrix,CalculatedTransform* worldTransform,const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform,bool isSSC) const;

    inline void UpdateXsi(nw::math::MTX34* worldMatrix,CalculatedTransform* worldTransform,
        const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const;


    inline void UpdateBasic(nw::math::MTX34* worldMatrix,CalculatedTransform* worldTransform,
        const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const;

private:
    WorldMatrixUpdater(nw::os::IAllocator* allocator);
    virtual ~WorldMatrixUpdater();

    void CalculateWorldXsi(nw::math::MTX34* transformMatrix,nw::math::VEC3* scale,
        const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const;

    void CalculateWorldMayaSsc(nw::math::MTX34* transformMatrix,nw::math::VEC3* scale,
        const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const;

    void CalculateWorldBasic(nw::math::MTX34* transformMatrix,nw::math::VEC3* scale,
        const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const;

    template<typename TMatrix, typename UMatrix>
    void MultScale(TMatrix* dstMatrix, const UMatrix* srcMatrix, const math::VEC3& scale) const
    {
        dstMatrix->matrix[0][0] = srcMatrix->matrix[0][0] * scale.x;
        dstMatrix->matrix[1][0] = srcMatrix->matrix[1][0] * scale.x;
        dstMatrix->matrix[2][0] = srcMatrix->matrix[2][0] * scale.x;

        dstMatrix->matrix[0][1] = srcMatrix->matrix[0][1] * scale.y;
        dstMatrix->matrix[1][1] = srcMatrix->matrix[1][1] * scale.y;
        dstMatrix->matrix[2][1] = srcMatrix->matrix[2][1] * scale.y;

        dstMatrix->matrix[0][2] = srcMatrix->matrix[0][2] * scale.z;
        dstMatrix->matrix[1][2] = srcMatrix->matrix[1][2] * scale.z;
        dstMatrix->matrix[2][2] = srcMatrix->matrix[2][2] * scale.z;
    }

    template<typename TMatrix>
    void ScaleMatrix(TMatrix* dstMatrix, const math::VEC3& scale) const
    {
        dstMatrix->matrix[0][0] *= scale.x;
        dstMatrix->matrix[1][0] *= scale.x;
        dstMatrix->matrix[2][0] *= scale.x;

        dstMatrix->matrix[0][1] *= scale.y;
        dstMatrix->matrix[1][1] *= scale.y;
        dstMatrix->matrix[2][1] *= scale.y;

        dstMatrix->matrix[0][2] *= scale.z;
        dstMatrix->matrix[1][2] *= scale.z;
        dstMatrix->matrix[2][2] *= scale.z;
    }

    void CopyTranslate(nw::math::MTX34* dstMatrix, const nw::math::MTX34& srcMatrix) const
    {
        dstMatrix->matrix[0][3] = srcMatrix.matrix[0][3];
        dstMatrix->matrix[1][3] = srcMatrix.matrix[1][3];
        dstMatrix->matrix[2][3] = srcMatrix.matrix[2][3];
    }

    void AddTranslate(nw::math::MTX34* dstMatrix, const nw::math::VEC3& translate) const
    {
        dstMatrix->matrix[0][3] += translate.x;
        dstMatrix->matrix[1][3] += translate.y;
        dstMatrix->matrix[2][3] += translate.z;
    }

    void CompensateScale(math::VEC3& scale) const
    {
        const float MinimumScale = 0.001f * 0.001f;
        float mag = (scale.x * scale.x + scale.y * scale.y + scale.z * scale.z);

        if (mag < MinimumScale)
        {
            scale.x = (scale.x < 0.0f) ? -MinimumScale : MinimumScale;
            scale.y = (scale.y < 0.0f) ? -MinimumScale : MinimumScale;
            scale.z = (scale.z < 0.0f) ? -MinimumScale : MinimumScale;
        }
    }
};

inline void WorldMatrixUpdater::UpdateMaya(math::MTX34* worldMatrix,CalculatedTransform* worldTransform,const CalculatedTransform& localTransform,
    const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform,bool isSSC) const
{
    NW_NULL_ASSERT(worldMatrix);
    NW_NULL_ASSERT(worldTransform);

    math::VEC3 scale;
    if (isSSC)
    {
        CalculateWorldMayaSsc(
            &worldTransform->m_TransformMatrix,
            &scale,
            localTransform,
            parentWorldTransform,
            parentLocalTransform
        );
    }
    else
    {
        CalculateWorldBasic(
            &worldTransform->m_TransformMatrix,
            &scale,
            localTransform,
            parentWorldTransform,
            parentLocalTransform
        );
    }

    this->CompensateScale(scale);

    worldTransform->m_Scale = scale;

    nw::math::MTX34MultScale(worldMatrix, worldTransform->m_TransformMatrix, localTransform.m_Scale);

    worldTransform->ResetTransformFlags();
    worldTransform->UpdateScaleFlags();
}

inline void WorldMatrixUpdater::UpdateBasic(nw::math::MTX34* worldMatrix,CalculatedTransform* worldTransform,
    const CalculatedTransform& localTransform, const CalculatedTransform& parentWorldTransform,const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(worldMatrix);
    NW_NULL_ASSERT(worldTransform);

    math::VEC3 scale;

    CalculateWorldBasic(&worldTransform->m_TransformMatrix,&scale,
        localTransform,parentWorldTransform,parentLocalTransform
    );

    this->CompensateScale(scale);

    worldTransform->m_Scale = scale;

    nw::math::MTX34MultScale(worldMatrix, worldTransform->m_TransformMatrix, localTransform.m_Scale);

    worldTransform->ResetTransformFlags();
    worldTransform->UpdateScaleFlags();
}

inline void WorldMatrixUpdater::UpdateXsi(nw::math::MTX34* worldMatrix,CalculatedTransform* worldTransform,
    const CalculatedTransform& localTransform,const CalculatedTransform& parentWorldTransform,
    const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(worldMatrix);
    NW_NULL_ASSERT(worldTransform);

    nw::math::VEC3 scale;

    CalculateWorldXsi(&worldTransform->m_TransformMatrix,&scale,localTransform,parentWorldTransform,parentLocalTransform);

    this->CompensateScale(scale);

    worldTransform->m_Scale = scale;

    nw::math::MTX34MultScale(worldMatrix, worldTransform->m_TransformMatrix, worldTransform->m_Scale);

    worldTransform->ResetTransformFlags();
    worldTransform->UpdateScaleFlags();
}

}
}