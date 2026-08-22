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

#ifndef NW_GFX_ANIMINTERPOLATOR_H_
#define NW_GFX_ANIMINTERPOLATOR_H_

#include <nw/gfx/gfx_AnimBlender.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw {
namespace gfx {

//---------------------------------------------------------------------------
//
//
//
//
//
//
//
//---------------------------------------------------------------------------
class AnimInterpolator : public AnimBlender
{
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
    class Builder
    {
    public:
        //
        Builder()
        : m_MaxAnimObjects(2),
          m_IgnoreNoAnimMember(false) {}

        //
        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        //
        //
        Builder& IgnoreNoAnimMember(bool ignoreNoAnimMember) { m_IgnoreNoAnimMember = ignoreNoAnimMember; return *this; }

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

            size += sizeof(AnimInterpolator);
            AnimInterpolator::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        //
        //
        //
        //
        //
        //
        AnimInterpolator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimInterpolator));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimInterpolator* animInterpolator = new(buf) AnimInterpolator(allocator);

            Result result = animInterpolator->Initialize(m_MaxAnimObjects, m_IgnoreNoAnimMember);
            NW_ASSERT(result.IsSuccess());

            return animInterpolator;
        }

    private:
        int m_MaxAnimObjects;
        bool m_IgnoreNoAnimMember;
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
    virtual const anim::AnimResult* GetResult(
        void* target,
        int memberIdx) const;

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
    float GetWeight(int animObjIdx) const
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_Weights.Size());
        return m_Weights[animObjIdx];
    }

    //
    //
    //
    //
    //
    //
    //
    //
    void SetWeight(int animObjIdx, float weight)
    {
        NW_MINMAXLT_ASSERT(animObjIdx, 0, m_Weights.Size());
        m_Weights[animObjIdx] = weight;
        m_NormalizedWeights[animObjIdx] = weight;
        m_IsWeightDirty = true;
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
    void SetNormalizationEnabled(bool enabled){ m_IsWeightNormalizationEnabled = enabled; }

    //
    bool GetNormalizationEnabled() const { return m_IsWeightNormalizationEnabled; }

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    AnimInterpolator(
        os::IAllocator* allocator)
    : AnimBlender(allocator),
      m_IsOldMethod(false),
      m_IsWeightDirty(false),
      m_IsWeightNormalizationEnabled(true)
    {
    }

    //
    //
    //
    virtual ~AnimInterpolator() {}

    //

    //
    //
    //
    static void GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize, int maxAnimObjects)
    {
        os::MemorySizeCalculator& size = *pSize;

        AnimBlender::GetMemorySizeForInitialize(pSize, maxAnimObjects);
        size += sizeof(float) * maxAnimObjects;
        size += sizeof(float) * maxAnimObjects;
    }

    //
    Result Initialize(int maxAnimObjects, bool ignoreNoAnimMember)
    {
        Result result = AnimBlender::Initialize(maxAnimObjects);
        NW_ENSURE_AND_RETURN(result);

        {
            void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_Weights = ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
            for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx)
            {
                m_Weights.PushBackFast(animObjIdx == 0 ? 1.0f : 0.0f);
            }
        }

        {
            void* memory = GetAllocator().Alloc(sizeof(float) * maxAnimObjects);
            if (memory == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            m_NormalizedWeights = ut::MoveArray<float>(memory, maxAnimObjects, &GetAllocator());
            for (int animObjIdx = 0; animObjIdx < maxAnimObjects; ++animObjIdx)
            {
                m_NormalizedWeights.PushBackFast(animObjIdx == 0 ? 1.0f : 0.0f);
            }
        }

        // TODO: Change variable names, etc.
        m_IsOldMethod = ignoreNoAnimMember;

        return result;
    }

    //
    //
    //
    void NormalizeWeight() const
    {
        float weightSum = 0.0f;
        float normalizeScale;
        for (int i = 0; i < m_Weights.Size(); ++i)
        {
            weightSum += m_Weights[i];
        }
        normalizeScale = GetAnimWeightNormalizeScale(weightSum);
        for (int i = 0; i < m_Weights.Size(); ++i)
        {
            m_NormalizedWeights[i] = m_Weights[i] * normalizeScale;
        }
        m_IsWeightDirty = false;
    }

    ut::MoveArray<float> m_Weights; //
    mutable ut::MoveArray<float> m_NormalizedWeights; //

    bool m_IsOldMethod; //
    mutable bool m_IsWeightDirty; //
    bool m_IsWeightNormalizationEnabled; //
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif // NW_GFX_ANIMINTERPOLATOR_H_
