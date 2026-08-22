#include <nw/gfx/gfx_AnimObject.h>

namespace nw{
namespace gfx{

NW_UT_RUNTIME_TYPEINFO_DEFINITION(AnimEvaluator    , BaseAnimEvaluator);

int AnimEvaluator::GetCacheBufferSizeNeeded() const{
    return GetCacheBufferSizeNeeded(this->mAnimData);
}

int AnimEvaluator::GetCacheBufferSizeNeeded(const anim::ResAnim& animData){
    const int headBytes = anim::AnimResult().GetOffsetToValueBuffer();
    int size = 0;
    
    const int memberAnimCount = animData.GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < memberAnimCount; ++animIdx){
        const anim::ResMemberAnim memberAnim = animData.GetMemberAnimSet(animIdx);
        const int primBytes = memberAnim.GetPrimitiveSize();
        size += ut::RoundUp(headBytes + primBytes, sizeof(bit32));
    }
    return size;
}

void AnimEvaluator::SetCacheBufferPointers(){
    const int headBytes = anim::AnimResult().GetOffsetToValueBuffer();
    u8* cachePtr = reinterpret_cast<u8*>(this->mCacheBuf);
    const int memberAnimCount = this->mAnimData.GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < memberAnimCount; ++animIdx){
        mCachePtrs[animIdx] = reinterpret_cast<anim::AnimResult*>(cachePtr);
        const anim::ResMemberAnim memberAnim = this->mAnimData.GetMemberAnimSet(animIdx);
        const int primBytes = memberAnim.GetPrimitiveSize();
        cachePtr += ut::RoundUp(headBytes + primBytes, sizeof(bit32));
    }
}

AnimEvaluator::AnimEvaluator(os::IAllocator* allocator): 
    BaseAnimEvaluator(allocator, ANIMTYPE_SIMPLE),
    mCacheBuf(NULL),
    mSharedCache(NULL)
{}

void AnimEvaluator::GetMemorySizeForInitialize(os::MemorySizeCalculator* pSize,const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache){
    os::MemorySizeCalculator& size = *pSize;

    BaseAnimEvaluator::GetMemorySizeForInitialize(pSize, maxMembers, maxAnimMembers);
    size += sizeof(anim::AnimResult*) * maxAnimMembers;

    if (allocCache){
        size += AnimEvaluator::GetCacheBufferSizeNeeded(animData);
    }
}

Result AnimEvaluator::Initialize(const anim::ResAnim& animData,const int maxMembers,const int maxAnimMembers,bool allocCache){
    Result result = BaseAnimEvaluator::Initialize(animData, maxMembers, maxAnimMembers);
    NW_ENSURE_AND_RETURN(result);

    void* memory = GetAllocator().Alloc(sizeof(anim::AnimResult*) * maxAnimMembers);
    if (memory == NULL){
        result |= Result::MASK_FAIL_BIT;
    }
    NW_ENSURE_AND_RETURN(result);

    mCachePtrs = ut::MoveArray<anim::AnimResult*>(memory, maxAnimMembers, &GetAllocator());
    this->mCachePtrs.Resize(animData.GetMemberAnimSetCount());

    if (allocCache){
        if (animData.GetMemberAnimSetCount() != 0){
            mCacheBuf = GetAllocator().Alloc(GetCacheBufferSizeNeeded());
            if (mCacheBuf == NULL){
                result |= Result::MASK_FAIL_BIT;
            }
            NW_ENSURE_AND_RETURN(result);

            SetCacheBufferPointers();
        }
    }

    return result;
}

const anim::AnimResult*AnimEvaluator::GetResult(void* target,int memberIdx) const{
    if (!HasMemberAnim(memberIdx)){
        return NULL;
    }

    const anim::AnimBlendOp* blendOp = this->mAnimGroup->GetBlendOperation(memberIdx);
    const int animIdx = mBindIndexTable[memberIdx];
    if (mCacheBuf != NULL && !mIsCacheDirty){
        if (blendOp != NULL){
            return mCachePtrs[animIdx];
        }
        else{
            anim::ResMemberAnim memberAnim = this->mAnimData.GetMemberAnimSet(animIdx);
            memberAnim.ApplyCacheForType(target, mCachePtrs[animIdx]);
            return reinterpret_cast<anim::AnimResult*>(target);
        }
    }
    else{
        anim::ResMemberAnim memberAnim = this->mAnimData.GetMemberAnimSet(animIdx);
        const void* originalValue = this->mAnimGroup->HasOriginalValue() ? this->mAnimGroup->GetOriginalValue(memberIdx) : NULL;

        if (blendOp != NULL){
            anim::AnimResult* result = reinterpret_cast<anim::AnimResult*>(target);
            bit32 flags = memberAnim.EvaluateResultForType(
                result->GetValueBuffer(), result->GetFlags(),
                this->mAnimFrameController.GetFrame(), originalValue);
            result->SetFlags(flags);
            return result;
        }
        else{
            memberAnim.EvaluateResultForType(
                target, 0, this->mAnimFrameController.GetFrame(),
                originalValue);
            return reinterpret_cast<anim::AnimResult*>(target);
        }
    }
}

void AnimEvaluator::UpdateCacheNonVirtual(){
    if (mIsCacheDirty){
        if (mCacheBuf != NULL && mIsCacheDirty){
            UpdateCacheImpl();
        }
        else if (mUseSharedCache && mSharedCache != NULL){

            if (this->mSharedCache->IsDirty() || GetFrame() != this->mSharedCache->GetFrame() || GetStepFrame() != this->mSharedCache->GetStepFrame()){
                this->UpdateCacheImpl();

                this->mSharedCache->SetFrame(GetFrame());
                this->mSharedCache->SetStepFrame(GetStepFrame());
                this->mSharedCache->SetDirtyFlag(false);
            }
        }
    }
    
    mIsCacheDirty = false;
}

void AnimEvaluator::UpdateCacheImpl(){
    int animCount = this->mAnimData.GetMemberAnimSetCount();
    for (int animIdx = 0; animIdx < animCount; ++animIdx){
        anim::AnimResult* result = mCachePtrs[animIdx];
        result->ResetFlags();

        const int memberIdx = mReverseBindIndexTable[animIdx];
        if (memberIdx == BaseAnimEvaluator::NotFoundIndex){
            continue;
        }

        this->GetResult(result, memberIdx);
    }
}

}
}