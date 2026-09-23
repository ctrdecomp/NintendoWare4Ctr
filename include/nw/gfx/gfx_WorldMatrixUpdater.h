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
        dstMatrix->f._00 = srcMatrix->f._00 * scale.x;
        dstMatrix->f._10 = srcMatrix->f._10 * scale.x;
        dstMatrix->f._20 = srcMatrix->f._20 * scale.x;

        dstMatrix->f._01 = srcMatrix->f._01 * scale.y;
        dstMatrix->f._11 = srcMatrix->f._11 * scale.y;
        dstMatrix->f._21 = srcMatrix->f._21 * scale.y;

        dstMatrix->f._02 = srcMatrix->f._02 * scale.z;
        dstMatrix->f._12 = srcMatrix->f._12 * scale.z;
        dstMatrix->f._22 = srcMatrix->f._22 * scale.z;
    }

    template<typename TMatrix>
    void ScaleMatrix(TMatrix* dstMatrix, const math::VEC3& scale) const
    {
        dstMatrix->f._00 *= scale.x;
        dstMatrix->f._10 *= scale.x;
        dstMatrix->f._20 *= scale.x;

        dstMatrix->f._01 *= scale.y;
        dstMatrix->f._11 *= scale.y;
        dstMatrix->f._21 *= scale.y;

        dstMatrix->f._02 *= scale.z;
        dstMatrix->f._12 *= scale.z;
        dstMatrix->f._22 *= scale.z;
    }

    void CopyTranslate(math::MTX34* dstMatrix, const math::MTX34& srcMatrix) const
    {
        dstMatrix->f._03 = srcMatrix.f._03;
        dstMatrix->f._13 = srcMatrix.f._13;
        dstMatrix->f._23 = srcMatrix.f._23;
    }

    void AddTranslate(math::MTX34* dstMatrix, const math::VEC3& translate) const
    {
        dstMatrix->f._03 += translate.x;
        dstMatrix->f._13 += translate.y;
        dstMatrix->f._23 += translate.z;
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