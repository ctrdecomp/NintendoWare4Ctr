#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/anim/res/anim_ResAnim.h>

namespace nw {
namespace gfx {

class SharedAnimCache : public GfxObject{
public:
    class Builder{
    public:
        Builder(): 
            mAnimDataPtr(NULL),
            mCacheBufferSize(0)
        {}

        Builder& AnimData(anim::ResAnim animData){
            mAnimDataPtr = animData.ptr();
            return *this;
        }

        Builder& CacheBufferSize(int size){
            mCacheBufferSize = size;
            return *this;
        }

        SharedAnimCache* Create(nw::os::IAllocator* allocator){
            void* memory = allocator->Alloc(sizeof(SharedAnimCache));
            if (memory == NULL){
                return NULL;
            }

            SharedAnimCache* result = new(memory) SharedAnimCache(allocator);
            result->SetAnimData(this->mAnimDataPtr);

            bool cacheBufferAlloced = result->AllocBuffer(this->mCacheBufferSize);
            if (!cacheBufferAlloced){
                result->~SharedAnimCache();
                allocator->Free(result);
                return NULL;
            }

            return result;
        }

    private:
        anim::ResAnimData* mAnimDataPtr;
        int mCacheBufferSize;
    };

    virtual ~SharedAnimCache(){
        this->DestroyCache();
    }

    void SetFrame(f32 frame) { mFrame = frame; }
    f32 GetFrame() const { return mFrame; }
    void SetStepFrame(f32 stepFrame) { mStepFrame = stepFrame; }
    f32 GetStepFrame() const { return mStepFrame; }

    void* GetCacheBuffer() { return mCacheBuf; }
    bool IsDirty() const { return mIsDirty; }
    void SetDirtyFlag(bool isDirty) { mIsDirty = isDirty; }

private:
    void SetAnimData(const anim::ResAnimData* animData){
        mAnimData = anim::ResAnim(animData);
    }

    bool AllocBuffer(int cacheBufferSize){
        if (0 < cacheBufferSize){
            NW_ASSERT(mCacheBuf == NULL);
            mCacheBuf = GetAllocator().Alloc(cacheBufferSize);
            return (mCacheBuf != NULL);
        }

        return false;
    }

    explicit SharedAnimCache(nw::os::IAllocator* allocator): 
        GfxObject(allocator),
        mCacheBuf(NULL),
        mAnimData(NULL),
        mFrame(0.0f),
        mStepFrame(0.0f),
        mIsDirty(true)
    {}

    inline void DestroyCache(){
        if (mCacheBuf != NULL){
            nw::os::SafeFree(this->mCacheBuf, &GetAllocator());
        }        
    }

    void* mCacheBuf;
    anim::ResAnim mAnimData;
    f32 mFrame;
    f32 mStepFrame;
    bool mIsDirty;
};

}
}