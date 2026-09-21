// Filename: gfx_AnimEvaluator.cpp
//
// Project: NintendoWare4Ctr

#include <nw/gfx/gfx_AnimObject.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AnimEvaluator    , BaseAnimEvaluator);

int AnimEvaluator::GetCacheBufferSizeNeeded() const
{
    return GetCacheBufferSizeNeeded(this->m_AnimData);
}

int AnimEvaluator::GetCacheBufferSizeNeeded(const anim::ResAnim& animData)
{
    const int headBytes = anim::AnimResult().GetOffsetToValueBuffer();
    int size = 0;
    
    const int memberAnimCount = animData.GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < memberAnimCount; ++animIdx)
    {
        const anim::ResMemberAnim memberAnim = animData.GetMemberAnimSet(animIdx);
        const int primBytes = memberAnim.GetPrimitiveSize();
        size += ut::RoundUp(headBytes + primBytes, sizeof(bit32));
    }
    return size;
}

void AnimEvaluator::SetCacheBufferPointers()
{
    const int headBytes = anim::AnimResult().GetOffsetToValueBuffer();
    u8* cachePtr = reinterpret_cast<u8*>(this->m_CacheBuf);
    const int memberAnimCount = this->m_AnimData.GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < memberAnimCount; ++animIdx)
    {
        m_CachePtrs[animIdx] = reinterpret_cast<anim::AnimResult*>(cachePtr);
        const anim::ResMemberAnim memberAnim = this->m_AnimData.GetMemberAnimSet(animIdx);
        const int primBytes = memberAnim.GetPrimitiveSize();
        cachePtr += ut::RoundUp(headBytes + primBytes, sizeof(bit32));
    }
}

AnimEvaluator::AnimEvaluator(os::IAllocator* allocator): 
    BaseAnimEvaluator(allocator, ANIMTYPE_SIMPLE),
    m_CacheBuf(NULL),
    m_SharedCache(NULL) {}

void AnimEvaluator::GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize,const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache)
    {
    os::MemorySizeCalculator& size = *pSize;

    BaseAnimEvaluator::GetMemorySizeForInitialize(pSize, maxMembers, maxAnimMembers);
    size += sizeof(anim::AnimResult*) * maxAnimMembers;

    if (allocCache)
    {
        size += AnimEvaluator::GetCacheBufferSizeNeeded(animData);
    }
}

Result AnimEvaluator::Initialize(const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache)
{
    Result result = BaseAnimEvaluator::Initialize(animData, maxMembers, maxAnimMembers);
    NW_ENSURE_AND_RETURN(result);

    void* memory = GetAllocator().Alloc(sizeof(anim::AnimResult*) * maxAnimMembers);
    if (memory == NULL)
    {
        result |= Result::MASK_FAIL_BIT;
    }
    NW_ENSURE_AND_RETURN(result);

    m_CachePtrs = ut::MoveArray<anim::AnimResult*>(memory, maxAnimMembers, &GetAllocator());
    this->m_CachePtrs.Resize(animData.GetMemberAnimSetCount());

    if (allocCache)
    {
        if (animData.GetMemberAnimSetCount() != 0)
        {
            m_CacheBuf = GetAllocator().Alloc(GetCacheBufferSizeNeeded());
            if (m_CacheBuf == NULL)
            {
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            SetCacheBufferPointers();
        }
    }

    return result;
}

const anim::AnimResult*AnimEvaluator::GetResult(void* target,int memberIdx) const
{
    if (!HasMemberAnim(memberIdx))
    {
        return NULL;
    }

    const anim::AnimBlendOp* blendOp = this->m_AnimGroup->GetBlendOperation(memberIdx);
    const int animIdx = m_BindIndexTable[memberIdx];
    if (m_CacheBuf != NULL && !m_IsCacheDirty)
    {
        if (blendOp != NULL)
        {
            return m_CachePtrs[animIdx];
        }
        else{
            anim::ResMemberAnim memberAnim = this->m_AnimData.GetMemberAnimSet(animIdx);
            memberAnim.ApplyCacheForType(target, m_CachePtrs[animIdx]);
            return reinterpret_cast<anim::AnimResult*>(target);
        }
    }
    else{
        anim::ResMemberAnim memberAnim = this->m_AnimData.GetMemberAnimSet(animIdx);
        const void* originalValue = this->m_AnimGroup->HasOriginalValue() ? this->m_AnimGroup->GetOriginalValue(memberIdx) : NULL;

        if (blendOp != NULL)
        {
            anim::AnimResult* result = reinterpret_cast<anim::AnimResult*>(target);
            bit32 flags = memberAnim.EvaluateResultForType(
                result->GetValueBuffer(), result->GetFlags(),
                this->m_AnimFrameController.GetFrame(), originalValue);
            result->SetFlags(flags);
            return result;
        }
        else{
            memberAnim.EvaluateResultForType(
                target, 0, this->m_AnimFrameController.GetFrame(),
                originalValue);
            return reinterpret_cast<anim::AnimResult*>(target);
        }
    }
}

void AnimEvaluator::UpdateCacheNonVirtual()
{
    if (m_IsCacheDirty)
    {
        if (m_CacheBuf != NULL && m_IsCacheDirty)
        {
            UpdateCacheImpl();
        }
        else if (m_UseSharedCache && m_SharedCache != NULL)
        {

            if (this->m_SharedCache->IsDirty() || GetFrame() != this->m_SharedCache->GetFrame() || GetStepFrame() != this->m_SharedCache->GetStepFrame())
            {
                this->UpdateCacheImpl();

                this->m_SharedCache->SetFrame(GetFrame());
                this->m_SharedCache->SetStepFrame(GetStepFrame());
                this->m_SharedCache->SetDirtyFlag(false);
            }
        }
    }
    
    m_IsCacheDirty = false;
}

void AnimEvaluator::UpdateCacheImpl()
{
    int animCount = this->m_AnimData.GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < animCount; ++animIdx)
    {
        anim::AnimResult* result = m_CachePtrs[animIdx];
        result->ResetFlags();

        const int memberIdx = m_ReverseBindIndexTable[animIdx];
        if (memberIdx == BaseAnimEvaluator::NotFoundIndex)
        {
            continue;
        }

        this->GetResult(result, memberIdx);
    }
}

}
}