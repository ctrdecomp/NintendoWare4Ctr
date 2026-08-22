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

#ifndef NW_GFX_TRANSFORMANIMEVALUATOR_H_
#define NW_GFX_TRANSFORMANIMEVALUATOR_H_

#include <nw/gfx/gfx_BaseAnimEvaluator.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
class TransformAnimEvaluator : public BaseAnimEvaluator
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    //----------------------------------------
    //
    //

    //
    class Builder
    {
    public:
        //
        Builder()
        : m_AnimData(NULL),
          m_MaxMembers(64),
          m_MaxAnimMembers(64),
          m_AllocCache(false) {}

        //
        Builder& AnimData(const anim::ResAnim& animData) { m_AnimData = animData; return *this; }

        //
        //
        //
        //
        Builder& MaxMembers(int maxMembers)
        {
            NW_ASSERT(maxMembers > 0);
            m_MaxMembers = maxMembers;
            return *this;
        }

        //
        //
        //
        //
        Builder& MaxAnimMembers(int maxAnimMembers)
        {
            NW_ASSERT(maxAnimMembers > 0);
            m_MaxAnimMembers = maxAnimMembers;
            return *this;
        }

        //
        Builder& AllocCache(bool allocCache) { m_AllocCache = allocCache; return *this; }

        //
        //
        //
        //
        //
        //
        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);

            GetMemorySizeInternal(&size);

            return size.GetSizeWithPadding(alignment);
        }

        //
        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            os::MemorySizeCalculator& size = *pSize;

            size += sizeof(TransformAnimEvaluator);
            BaseAnimEvaluator::GetMemorySizeForInitialize(pSize, m_MaxMembers, m_MaxAnimMembers);

            if (m_AllocCache)
            {
                size += sizeof(CalculatedTransform) * m_MaxAnimMembers;
            }
        }

        //
        //
        //
        //
        //
        //
        TransformAnimEvaluator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimEvaluator));

            if (buf == NULL)
            {
                return NULL;
            }

            TransformAnimEvaluator* evaluator = new(buf) TransformAnimEvaluator(allocator);

            Result result = evaluator->Initialize(m_AnimData, m_MaxMembers, m_MaxAnimMembers, m_AllocCache);
            NW_ASSERT(result.IsSuccess());

            return evaluator;
        }

    private:
        anim::ResAnim m_AnimData;
        int m_MaxMembers;
        int m_MaxAnimMembers;
        bool m_AllocCache;
    };

    //

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
    //
    //
    //
    //
    //
    //
    //
    virtual Result TryBind(AnimGroup* animGroup);

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
    //
    //
    //
    //
    virtual Result ChangeAnim(const nw::anim::ResAnim animData)
    {
        // The Bind function is called within the BaseAnimEvaluator::ChangeAnim, and since the cache is rewritten there, the cache should be resized ahead of time
        // 
        if (!m_IsCacheExternal && !m_CacheTransforms.Empty())
        {
            m_CacheTransforms.Resize(animData.GetMemberAnimSetCount());
        }

        return BaseAnimEvaluator::ChangeAnim(animData);
    }

    //

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
    virtual const anim::AnimResult* GetResult(
        void* target,
        int memberIdx) const;

    //

    //----------------------------------------
    //
    //

    //
    bool GetIsScaleDisabled() const { return m_IsScaleDisabled; }

    //
    void SetIsScaleDisabled(bool isDisabled) { m_IsScaleDisabled = isDisabled; }

    //
    bool GetIsRotateDisabled() const { return m_IsRotateDisabled; }

    //
    void SetIsRotateDisabled(bool isDisabled) { m_IsRotateDisabled = isDisabled; }

    //
    bool GetIsTranslateDisabled() const { return m_IsTranslateDisabled; }

    //
    void SetIsTranslateDisabled(bool isDisabled) { m_IsTranslateDisabled = isDisabled; }

    //
    //
    //
    //
    //
    //
    virtual bool HasMemberAnim(int memberIdx) const
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_BindIndexTable.Size());
        if (m_AnimData.ptr() == NULL)
        {
            return (0 <= memberIdx && memberIdx < m_AnimGroup->GetMemberCount());
        }
        else
        {
            return m_BindIndexTable[memberIdx] != NotFoundIndex;
        }
    }

    //

    //----------------------------------------
    //
    //

    //----------------------------------------------------------
    //
    void UpdateCacheNonVirtual()
    {
        if (!m_CacheTransforms.Empty() && m_IsCacheDirty)
        {
            if (m_AnimData.ptr() != NULL)
            {
                for (int memberIdx = 0; memberIdx < m_AnimGroup->GetMemberCount(); ++memberIdx)
                {
                    const int animIdx = m_BindIndexTable[memberIdx];
                    if (animIdx != NotFoundIndex)
                    {
                        GetResult(&m_CacheTransforms[animIdx], memberIdx);
                    }
                }
            }
            m_IsCacheDirty = false;
        }
    }

    //
    virtual void UpdateCache() { this->UpdateCacheNonVirtual(); }

    //
    virtual int GetCacheBufferSizeNeeded() const
    {
        return m_AnimData.GetMemberAnimSetCount() * sizeof(CalculatedTransform);
    }

    //
    virtual const void* GetCacheBuffer() const { return m_CacheTransforms.Elements(); }

    //
    //
    //
    //
    //
    //
    //
    virtual void SetCacheBuffer(void* buf, int size)
    {
        if (buf != NULL)
        {
            NW_ASSERT(size >= GetCacheBufferSizeNeeded());
            const int maxCalculatedTransforms = size / sizeof(CalculatedTransform);
            m_CacheTransforms = ut::MoveArray<CalculatedTransform>(buf, maxCalculatedTransforms);
            m_CacheTransforms.Resize(maxCalculatedTransforms);
            m_IsCacheDirty = true;
            m_IsCacheExternal = true;
        }
        else
        {
            m_CacheTransforms = ut::MoveArray<CalculatedTransform>();
        }
    }

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
    //
    //
    //
    //
    static void DisableSRTWeightsIfNeeded(float* weights, const AnimObject* animObj)
    {
        const TransformAnimEvaluator* evaluator =
            ut::DynamicCast<const TransformAnimEvaluator*>(animObj);
        if (evaluator != NULL)
        {
            if (evaluator->GetIsScaleDisabled())
            {
                weights[0] = TransformAnimBlendOp::WeightDiscard;
            }

            if (evaluator->GetIsRotateDisabled())
            {
                weights[1] = TransformAnimBlendOp::WeightDiscard;
            }

            if (evaluator->GetIsTranslateDisabled())
            {
                weights[2] = TransformAnimBlendOp::WeightDiscard;
            }
        }
    }

    //
    //
    //
    //
    //
    //
    //
    static bool CheckWeightsNearlyZero(const float* weights)
    {
        NW_NULL_ASSERT(weights);

        return
            AnimWeightNearlyEqualZero(weights[0]) &&
            AnimWeightNearlyEqualZero(weights[1]) &&
            AnimWeightNearlyEqualZero(weights[2]);
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    TransformAnimEvaluator(
        os::IAllocator* allocator)
    : BaseAnimEvaluator(allocator, ANIMTYPE_TRANSFORM_SIMPLE),
      m_IsScaleDisabled(false),
      m_IsRotateDisabled(false),
      m_IsTranslateDisabled(false)
    {
    }

    //
    //
    //
    virtual ~TransformAnimEvaluator() {}

    //

    //
    Result Initialize(
        const anim::ResAnim& animData,
        const int maxMembers,
        const int maxAnimMembers,
        bool allocCache)
    {
        Result result = BaseAnimEvaluator::Initialize(animData, maxMembers, maxAnimMembers);
        NW_ENSURE_AND_RETURN(result);

        if (allocCache)
        {
            void* memory = GetAllocator().Alloc(sizeof(CalculatedTransform) * maxAnimMembers);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_CacheTransforms = ut::MoveArray<CalculatedTransform>(memory, maxAnimMembers, &GetAllocator());
            m_CacheTransforms.Resize(animData.GetMemberAnimSetCount());
        }

        return result;
    }

    bool m_IsScaleDisabled; //
    bool m_IsRotateDisabled; //
    bool m_IsTranslateDisabled; //

    ut::MoveArray<CalculatedTransform> m_CacheTransforms; //

private:
    // Initialized members for which an animation curve does not exist using OriginalValue.
    void ResetNoAnimMember(AnimGroup* animGroup, anim::ResAnim animData);

    const anim::AnimResult* GetResultFast(void* target, int memberIdx) const;
    const anim::AnimResult* GetResultCommon(void* target, int memberIdx, bool writeNoAnimMember) const;

    // Evaluate member animations.
    //
    void EvaluateMemberAnim(
        CalculatedTransform* result,
        anim::ResTransformAnim transformAnim,
        float frame,
        const math::Transform3* originalTransform,
        bool writeNoAnimMember) const;

    // Evaluated baked member animations.
    //
    void EvaluateMemberBakedAnim(
        CalculatedTransform* result,
        anim::ResBakedTransformAnim transformAnim,
        float frame,
        const math::Transform3* originalTransform,
        bool writeNoAnimMember) const;

    void UpdateFlagsCommon(CalculatedTransform* transform) const;
    void UpdateFlags(CalculatedTransform* transform) const;

    // Update the CalculatedTransform flag based on baked information.
    void ApplyBakedFlags(CalculatedTransform* transform, bit32 flags) const;

    friend class AnimBinding;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_TRANSFORMANIMEVALUATOR_H_
