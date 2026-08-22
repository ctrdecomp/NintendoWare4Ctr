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

#ifndef NW_GFX_WORLD_MATRIX_UPDATER_H_
#define NW_GFX_WORLD_MATRIX_UPDATER_H_

#include <nw/gfx/gfx_GfxObject.h>

#include <nw/ut/ut_MoveArray.h>
#include <nw/math.h>

namespace nw
{
namespace os
{
class IAllocator;
} // os
namespace gfx
{

class CalculatedTransform;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class WorldMatrixUpdater : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(WorldMatrixUpdater);

public:
    //---------------------------------------------------------------------------
    //
    //---------------------------------------------------------------------------
    enum ScalingRule
    {
        SCALING_RULE_STANDARD, //
        SCALING_RULE_MAYA,     //
        SCALING_RULE_SOFTIMAGE //
    };

public:
    //
    class Builder
    {
    public:
        //
        Builder() {}

        //
        //
        //
        //
        //
        //
        WorldMatrixUpdater* Create(os::IAllocator* allocator);

    };

    //
    //
    //
    //
    //
    //
    //
    //
    //
    NW_INLINE void UpdateMaya(
        math::MTX34* worldMatrix,
        CalculatedTransform* worldTransform,
        const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,
        const CalculatedTransform& parentLocalTransform,
        bool isSSC) const;

    //
    //
    //
    //
    //
    //
    //
    //
    NW_INLINE void UpdateXsi(
        math::MTX34* worldMatrix,
        CalculatedTransform* worldTransform,
        const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,
        const CalculatedTransform& parentLocalTransform) const;

    //
    //
    //
    //
    //
    //
    //
    //
    NW_INLINE void UpdateBasic(
        math::MTX34* worldMatrix,
        CalculatedTransform* worldTransform,
        const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,
        const CalculatedTransform& parentLocalTransform) const;

private:
    //
    WorldMatrixUpdater(
        os::IAllocator* allocator);

    //
    virtual ~WorldMatrixUpdater();

    void CalculateWorldXsi(
        math::MTX34* transformMatrix,
        math::VEC3* scale,
        const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,
        const CalculatedTransform& parentLocalTransform) const;

    void CalculateWorldMayaSsc(
        math::MTX34* transformMatrix,
        math::VEC3* scale,
        const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,
        const CalculatedTransform& parentLocalTransform) const;

    void CalculateWorldBasic(
        math::MTX34* transformMatrix,
        math::VEC3* scale,
        const CalculatedTransform& localTransform,
        const CalculatedTransform& parentWorldTransform,
        const CalculatedTransform& parentLocalTransform) const;

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
        // Restrict scale to over 1 / 1000000
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

//----------------------------------------
NW_INLINE void
WorldMatrixUpdater::UpdateMaya(
    math::MTX34* worldMatrix,
    CalculatedTransform* worldTransform,
    const CalculatedTransform& localTransform,
    const CalculatedTransform& parentWorldTransform,
    const CalculatedTransform& parentLocalTransform,
    bool isSSC) const
{
    NW_NULL_ASSERT(worldMatrix);
    NW_NULL_ASSERT(worldTransform);

    math::VEC3 scale;
    // Update WorldTransform
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

    // Update WorldMatrix
    math::MTX34MultScale(worldMatrix, worldTransform->m_TransformMatrix, localTransform.m_Scale);

#if defined(NW_GFX_WORLD_MATRIX_FLAG_UPDATE_ENABLED)
    worldTransform->UpdateFlags();
#else
    worldTransform->ResetTransformFlags();
    worldTransform->UpdateScaleFlags();
#endif
}

//----------------------------------------
NW_INLINE void
WorldMatrixUpdater::UpdateBasic(
    math::MTX34* worldMatrix,
    CalculatedTransform* worldTransform,
    const CalculatedTransform& localTransform,
    const CalculatedTransform& parentWorldTransform,
    const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(worldMatrix);
    NW_NULL_ASSERT(worldTransform);

    math::VEC3 scale;

    // Update WorldTransform
    CalculateWorldBasic(
        &worldTransform->m_TransformMatrix,
        &scale,
        localTransform,
        parentWorldTransform,
        parentLocalTransform
    );

    this->CompensateScale(scale);

    worldTransform->m_Scale = scale;

    // Update WorldMatrix
    math::MTX34MultScale(worldMatrix, worldTransform->m_TransformMatrix, localTransform.m_Scale);

#if defined(NW_GFX_WORLD_MATRIX_FLAG_UPDATE_ENABLED)
    worldTransform->UpdateFlags();
#else
    worldTransform->ResetTransformFlags();
    worldTransform->UpdateScaleFlags();
#endif
}

//----------------------------------------
NW_INLINE void
WorldMatrixUpdater::UpdateXsi(
    math::MTX34* worldMatrix,
    CalculatedTransform* worldTransform,
    const CalculatedTransform& localTransform,
    const CalculatedTransform& parentWorldTransform,
    const CalculatedTransform& parentLocalTransform) const
{
    NW_NULL_ASSERT(worldMatrix);
    NW_NULL_ASSERT(worldTransform);

    math::VEC3 scale;

    // Update WorldTransform
    CalculateWorldXsi(
        &worldTransform->m_TransformMatrix,
        &scale,
        localTransform,
        parentWorldTransform,
        parentLocalTransform
    );

    this->CompensateScale(scale);

    worldTransform->m_Scale = scale;

    // Update WorldMatrix
    math::MTX34MultScale(worldMatrix, worldTransform->m_TransformMatrix, worldTransform->m_Scale);

#if defined(NW_GFX_WORLD_MATRIX_FLAG_UPDATE_ENABLED)
    worldTransform->UpdateFlags();
#else
    worldTransform->ResetTransformFlags();
    worldTransform->UpdateScaleFlags();
#endif
}

} // gfx
} // nw

#endif // NW_GFX_WORLD_MATRIX_UPDATER_H_
