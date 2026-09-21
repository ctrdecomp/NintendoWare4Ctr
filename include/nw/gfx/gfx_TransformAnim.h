#pragma once

#include <nw/gfx/gfx_AnimObject.h>
#include <nw/gfx/gfx_CalculatedTransform.h>

namespace nw {
namespace gfx {

class TransformAnimBlendOp : public anim::AnimBlendOp
{
public:
    static const float WeightDiscard;

    TransformAnimBlendOp(bool hasBlend, bool hasPostBlend): AnimBlendOp(hasBlend, hasPostBlend) {}
    virtual ~TransformAnimBlendOp() {}

    virtual void Apply(void* target, const anim::AnimResult* result) const;
    virtual void ConvertToAnimResult(anim::AnimResult* result, const void* source) const;

protected:
    enum BasicBlendFlags
    {
        FLAG_ACCURATE_SCALE_SHIFT    = 0,
        FLAG_QUATERNION_ROTATE_SHIFT = 1,
        FLAG_ACCURATE_SCALE          = 0x1 << FLAG_ACCURATE_SCALE_SHIFT,
        FLAG_QUATERNION_ROTATE       = 0x1 << FLAG_QUATERNION_ROTATE_SHIFT
    };

    void BlendScaleStandard(CalculatedTransform* dst, const CalculatedTransform* src, const float weight) const;
    void BlendScaleAccurate(CalculatedTransform* dst, const CalculatedTransform* src, const float weight) const;
    bool PostBlendAccurateScale(CalculatedTransform* transform) const;
    void BlendRotateMatrix(CalculatedTransform* dst, const CalculatedTransform* src, const float weight) const;
    void BlendRotateQuaternion(CalculatedTransform* dst, const CalculatedTransform* src, const float weight) const;
    void BlendTranslate(CalculatedTransform* dst, const CalculatedTransform* src, const float weight) const;
    bool OverrideTransform(CalculatedTransform* dst, const CalculatedTransform* src, const bit32 blendFlags) const;
};

class TransformAnimBlendOpStandard : public TransformAnimBlendOp
{
public:
    TransformAnimBlendOpStandard(): TransformAnimBlendOp(true, true) {}
    virtual ~TransformAnimBlendOpStandard() {}

    virtual bool Blend(anim::AnimResult* dst, float* dstWeights, const anim::AnimResult* src, const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleStandard(dstX, srcX, srcWeights[0]);
        BlendRotateMatrix (dstX, srcX, srcWeights[1]);
        BlendTranslate    (dstX, srcX, srcWeights[2]);
        return true;
    }

    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        return dstX->NormalizeRotateMatrix();
    }

    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, 0);
    }
};

class TransformAnimBlendOpAccScale : public TransformAnimBlendOp
{
public:
    TransformAnimBlendOpAccScale(): TransformAnimBlendOp(true, true) {}
    virtual ~TransformAnimBlendOpAccScale() {}

    virtual bool Blend(anim::AnimResult* dst, float* dstWeights, const anim::AnimResult* src, const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleAccurate(dstX, srcX, srcWeights[0]);
        BlendRotateMatrix (dstX, srcX, srcWeights[1]);
        BlendTranslate    (dstX, srcX, srcWeights[2]);
        return true;
    }

    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        const bool scaleRet = PostBlendAccurateScale(dstX);
        return dstX->NormalizeRotateMatrix() && scaleRet;
    }

    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, FLAG_ACCURATE_SCALE);
    }
};

class TransformAnimBlendOpQuat : public TransformAnimBlendOp
{
public:
    TransformAnimBlendOpQuat(): TransformAnimBlendOp(true, true) {}
    virtual ~TransformAnimBlendOpQuat() {}

    virtual bool Blend(anim::AnimResult* dst, float* dstWeights, const anim::AnimResult* src, const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleStandard   (dstX, srcX, srcWeights[0]);
        BlendRotateQuaternion(dstX, srcX, srcWeights[1]);
        BlendTranslate       (dstX, srcX, srcWeights[2]);
        return true;
    }

    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        return dstX->QuaternionToRotateMatrix();
    }

    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, FLAG_QUATERNION_ROTATE);
    }
};

class TransformAnimBlendOpAccScaleQuat : public TransformAnimBlendOp
{
public:
    TransformAnimBlendOpAccScaleQuat(): TransformAnimBlendOp(true, true) {}
    virtual ~TransformAnimBlendOpAccScaleQuat() {}

    virtual bool Blend(anim::AnimResult* dst, float* dstWeights, const anim::AnimResult* src, const float* srcWeights) const
    {
        (void)dstWeights;
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        BlendScaleAccurate   (dstX, srcX, srcWeights[0]);
        BlendRotateQuaternion(dstX, srcX, srcWeights[1]);
        BlendTranslate       (dstX, srcX, srcWeights[2]);
        return true;
    }

    virtual bool PostBlend(anim::AnimResult* result, const float* weights) const
    {
        (void)weights;
        CalculatedTransform* dstX = reinterpret_cast<CalculatedTransform*>(result);
        const bool scaleRet = PostBlendAccurateScale(dstX);
        return dstX->QuaternionToRotateMatrix() && scaleRet;
    }

    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* dstX       = reinterpret_cast<CalculatedTransform*>(dst);
        const CalculatedTransform* srcX = reinterpret_cast<const CalculatedTransform*>(src);
        return OverrideTransform(dstX, srcX, FLAG_ACCURATE_SCALE | FLAG_QUATERNION_ROTATE);
    }
};

class TransformAnimEvaluator : public BaseAnimEvaluator
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): m_AnimData(NULL), m_MaxMembers(64), m_MaxAnimMembers(64), m_AllocCache(false) {}

        Builder& AnimData(const anim::ResAnim& animData) { m_AnimData = animData; return *this; }

        Builder& MaxMembers(int maxMembers)
        {
            NW_ASSERT(maxMembers > 0);
            m_MaxMembers = maxMembers;
            return *this;
        }

        Builder& MaxAnimMembers(int maxAnimMembers)
        {
            NW_ASSERT(maxAnimMembers > 0);
            m_MaxAnimMembers = maxAnimMembers;
            return *this;
        }

        Builder& AllocCache(bool allocCache) { m_AllocCache = allocCache; return *this; }

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);
            GetMemorySizeInternal(&size);
            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            os::MemorySizeCalculator& size = *pSize;
            size += sizeof(TransformAnimEvaluator);
            BaseAnimEvaluator::GetMemorySizeForInitialize(pSize, m_MaxMembers, m_MaxAnimMembers);
            if (m_AllocCache)
                size += sizeof(CalculatedTransform) * m_MaxAnimMembers;
        }

        TransformAnimEvaluator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimEvaluator));
            if (buf == NULL) return NULL;
            TransformAnimEvaluator* evaluator = new(buf) TransformAnimEvaluator(allocator);
            Result result = evaluator->Initialize(m_AnimData, m_MaxMembers, m_MaxAnimMembers, m_AllocCache);
            NW_ASSERT(result.IsSuccess());
            return evaluator;
        }

    private:
        anim::ResAnim m_AnimData;
        int  m_MaxMembers;
        int  m_MaxAnimMembers;
        bool m_AllocCache;
    };

    virtual Result TryBind(AnimGroup* animGroup);

    virtual void ChangeAnim(const anim::ResAnim animData)
    {
        if (!m_IsCacheExternal && !m_CacheTransforms.Empty())
            m_CacheTransforms.Resize(animData.GetMemberAnimSetCount());
        BaseAnimEvaluator::ChangeAnim(animData);
    }

    virtual const anim::AnimResult* GetResult(void* target, int memberIdx) const;

    bool GetIsScaleDisabled()     const { return m_IsScaleDisabled; }
    void SetIsScaleDisabled(bool v) { m_IsScaleDisabled = v; }
    bool GetIsRotateDisabled()    const { return m_IsRotateDisabled; }
    void SetIsRotateDisabled(bool v) { m_IsRotateDisabled = v; }
    bool GetIsTranslateDisabled() const { return m_IsTranslateDisabled; }
    void SetIsTranslateDisabled(bool v) { m_IsTranslateDisabled = v; }

    virtual bool HasMemberAnim(int memberIdx) const
    {
        if (m_AnimData.ptr() == NULL)
            return (0 <= memberIdx && memberIdx < m_AnimGroup->GetMemberCount());
        else
            return m_BindIndexTable[memberIdx] != NotFoundIndex;
    }

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
                        GetResult(&m_CacheTransforms[animIdx], memberIdx);
                }
            }
            m_IsCacheDirty = false;
        }
    }

    virtual void UpdateCache() { this->UpdateCacheNonVirtual(); }

    virtual int GetCacheBufferSizeNeeded() const
    {
        return m_AnimData.GetMemberAnimSetCount() * sizeof(CalculatedTransform);
    }

    virtual const void* GetCacheBuffer() const { return m_CacheTransforms.Elements(); }

    virtual void SetCacheBuffer(void* buf, int size)
    {
        if (buf != NULL)
        {
            NW_ASSERT(size >= GetCacheBufferSizeNeeded());
            const int maxCalculatedTransforms = size / sizeof(CalculatedTransform);
            m_CacheTransforms = nw::ut::MoveArray<CalculatedTransform>(buf, maxCalculatedTransforms);
            m_CacheTransforms.Resize(maxCalculatedTransforms);
            m_IsCacheDirty    = true;
            m_IsCacheExternal = true;
        }
        else{
            m_CacheTransforms = nw::ut::MoveArray<CalculatedTransform>();
        }
    }

    static void DisableSRTWeightsIfNeeded(float* weights, const AnimObject* animObj)
    {
        const TransformAnimEvaluator* evaluator = nw::ut::DynamicCast<const TransformAnimEvaluator*>(animObj);
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

    static bool CheckWeightsNearlyZero(const float* weights)
    {
        NW_NULL_ASSERT(weights);

        return AnimWeightNearlyEqualZero(weights[0]) &&AnimWeightNearlyEqualZero(weights[1]) &&AnimWeightNearlyEqualZero(weights[2]);
    }

protected:
    TransformAnimEvaluator(nw::os::IAllocator* allocator):
        BaseAnimEvaluator(allocator, ANIMTYPE_TRANSFORM_SIMPLE),
        m_IsScaleDisabled(false),
        m_IsRotateDisabled(false),
        m_IsTranslateDisabled(false) {}

    virtual ~TransformAnimEvaluator() {}

    virtual Result Initialize(const anim::ResAnim& animData, const int maxMembers, const int maxAnimMembers, bool allocCache)
    {
        Result result = BaseAnimEvaluator::Initialize(animData, maxMembers, maxAnimMembers);
        NW_ENSURE_AND_RETURN(result);

        if (allocCache)
        {
            void* memory = GetAllocator().Alloc(sizeof(CalculatedTransform) * maxAnimMembers);
            if (memory == NULL) result |= Result::MASK_FAIL_BIT;
            NW_ENSURE_AND_RETURN(result);
            m_CacheTransforms = ut::MoveArray<CalculatedTransform>(memory, maxAnimMembers, &GetAllocator());
            m_CacheTransforms.Resize(animData.GetMemberAnimSetCount());
        }

        return result;
    }

    bool m_IsScaleDisabled;
    bool m_IsRotateDisabled;
    bool m_IsTranslateDisabled;
    ut::MoveArray<CalculatedTransform> m_CacheTransforms;

private:
    void ResetNoAnimMember(AnimGroup* animGroup, anim::ResAnim animData);
    const anim::AnimResult* GetResultFast(void* target, int memberIdx) const;
    const anim::AnimResult* GetResultCommon(void* target, int memberIdx, bool writeNoAnimMember) const;
    void EvaluateMemberAnim(CalculatedTransform* result, anim::ResTransformAnim transformAnim, float frame, const math::Transform3* originalTransform, bool writeNoAnimMember) const;
    void EvaluateMemberBakedAnim(CalculatedTransform* result, anim::ResBakedTransformAnim transformAnim, float frame, const math::Transform3* originalTransform, bool writeNoAnimMember) const;
    void UpdateFlagsCommon(CalculatedTransform* transform) const;
    void UpdateFlags(CalculatedTransform* transform) const;
    void ApplyBakedFlags(CalculatedTransform* transform, bit32 flags) const;

    friend class AnimBinding;
};

class TransformAnimInterpolator : public AnimInterpolator
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): m_MaxAnimObjects(2), m_IgnoreNoAnimMember(false) {}

        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        Builder& IgnoreNoAnimMember(bool v) { m_IgnoreNoAnimMember = v; return *this; }

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            nw::os::MemorySizeCalculator size(alignment);
            GetMemorySizeInternal(&size);
            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize) const
        {
            nw::os::MemorySizeCalculator& size = *pSize;
            size += sizeof(TransformAnimInterpolator);
            TransformAnimInterpolator::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        TransformAnimInterpolator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimInterpolator));
            if (buf == NULL) return NULL;
            TransformAnimInterpolator* interpolator = new(buf) TransformAnimInterpolator(allocator);
            Result result = interpolator->Initialize(m_MaxAnimObjects, m_IgnoreNoAnimMember);
            NW_ASSERT(result.IsSuccess());
            return interpolator;
        }

    private:
        int  m_MaxAnimObjects;
        bool m_IgnoreNoAnimMember;
    };

    virtual const anim::AnimResult* GetResult(void* target, int memberIdx) const;

protected:
    TransformAnimInterpolator(os::IAllocator* allocator): AnimInterpolator(allocator) {}
    virtual ~TransformAnimInterpolator() {}
};

class TransformAnimAdder : public AnimAdder
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): m_MaxAnimObjects(2) {}

        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);
            GetMemorySizeInternal(&size);
            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            os::MemorySizeCalculator& size = *pSize;
            size += sizeof(TransformAnimAdder);
            TransformAnimAdder::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        TransformAnimAdder* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimAdder));
            if (buf == NULL) return NULL;
            TransformAnimAdder* adder = new(buf) TransformAnimAdder(allocator);
            Result result = adder->Initialize(m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());
            return adder;
        }

    private:
        int m_MaxAnimObjects;
    };

    virtual const anim::AnimResult* GetResult(void* target, int memberIdx) const;

protected:
    TransformAnimAdder(nw::os::IAllocator* allocator): AnimAdder(allocator) {}
    virtual ~TransformAnimAdder() {}
};

class TransformAnimOverrider : public AnimOverrider
{
public:
    NW_UT_RUNTIME_TYPEINFO;

    class Builder
    {
    public:
        Builder(): m_MaxAnimObjects(2) {}

        Builder& MaxAnimObjects(int maxAnimObjects)
        {
            NW_ASSERT(maxAnimObjects > 0);
            m_MaxAnimObjects = maxAnimObjects;
            return *this;
        }

        size_t GetMemorySize(size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT) const
        {
            os::MemorySizeCalculator size(alignment);
            GetMemorySizeInternal(&size);
            return size.GetSizeWithPadding(alignment);
        }

        void GetMemorySizeInternal(os::MemorySizeCalculator* pSize) const
        {
            os::MemorySizeCalculator& size = *pSize;
            size += sizeof(TransformAnimOverrider);
            TransformAnimOverrider::GetMemorySizeForInitialize(pSize, m_MaxAnimObjects);
        }

        TransformAnimOverrider* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(TransformAnimOverrider));
            if (buf == NULL) return NULL;
            TransformAnimOverrider* overrider = new(buf) TransformAnimOverrider(allocator);
            Result result = overrider->Initialize(m_MaxAnimObjects);
            NW_ASSERT(result.IsSuccess());
            return overrider;
        }

    private:
        int m_MaxAnimObjects;
    };

    virtual const anim::AnimResult* GetResult(void* target, int memberIdx) const;

protected:
    TransformAnimOverrider(os::IAllocator* allocator): AnimOverrider(allocator) {}
    virtual ~TransformAnimOverrider() {}
};

class AnimBlendOpTransform : public TransformAnimBlendOp
{
public:

    AnimBlendOpTransform(): 
        TransformAnimBlendOp(true, true) {}

    virtual ~AnimBlendOpTransform() {}

    virtual bool Blend(anim::AnimResult* dst,float* dstWeights,const anim::AnimResult* src,const float* srcWeight) const
    {
        NW_NULL_ASSERT(dst);
        NW_NULL_ASSERT(src);
        NW_NULL_ASSERT(srcWeight);

        if (!dst->IsEnabledFlags(VALID_SINGLE))
        {
            CalculatedTransform* transform = 
                reinterpret_cast<CalculatedTransform*>(dst->GetValueBuffer());
            transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

            dst->EnableFlags(VALID_SINGLE);
        }

        float srcWeights[3] = { *srcWeight, *srcWeight, *srcWeight };

        return blendOp.Blend(reinterpret_cast<anim::AnimResult*>(dst->GetValueBuffer()), dstWeights, reinterpret_cast<const anim::AnimResult*>(src->GetValueBuffer()), srcWeights);
    }

    virtual bool PostBlend(anim::AnimResult* result, const float* weight) const
    {
        NW_NULL_ASSERT(result);

        CalculatedTransform* transform = reinterpret_cast<CalculatedTransform*>(result->GetValueBuffer());
        
        bool resultBlend;
#if 0
        if (weight)
        {
            float weights[3] = { *weight, *weight, *weight };
            resultBlend= blendOp.PostBlend(reinterpret_cast<anim::AnimResult*>(transform), weights);
        }
        else
#endif
{
            NW_UNUSED_VARIABLE(weight);
            resultBlend= blendOp.PostBlend(
                reinterpret_cast<anim::AnimResult*>(transform), 
                NULL);
        }

        transform->UpdateScaleFlags();
        transform->UpdateRotateFlags();
        transform->UpdateTranslateFlags();
        transform->UpdateCompositeFlags();

        transform->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);

        transform->EnableFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED);

        return resultBlend;
    }

    virtual bool Override(anim::AnimResult* dst, const anim::AnimResult* src) const
    {
        CalculatedTransform* transform =  reinterpret_cast<CalculatedTransform*>(dst->GetValueBuffer());

        if (!dst->IsEnabledFlags(VALID_SINGLE))
        {
            transform->EnableFlags(CalculatedTransform::FLAG_IS_IGNORE_ALL);

            dst->EnableFlags(VALID_SINGLE);
        }

        bool result = blendOp.Override(reinterpret_cast<anim::AnimResult*>(dst->GetValueBuffer()), reinterpret_cast<const anim::AnimResult*>(src->GetValueBuffer()));

        transform->UpdateScaleFlags();
        transform->UpdateRotateFlags();
        transform->UpdateTranslateFlags();
        transform->UpdateCompositeFlags();

        transform->EnableFlags(CalculatedTransform::FLAG_IS_DIRTY);
        transform->EnableFlags(CalculatedTransform::FLAG_IS_WORLDMATRIX_CALCULATION_ENABLED);

        return result;
    }

private:
    TransformAnimBlendOpStandard blendOp;
};

}
}