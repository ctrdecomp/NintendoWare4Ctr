#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/anim/res/anim_ResAnim.h>

namespace nw {
namespace gfx {

class SharedAnimCache : public GfxObject
{
public:
    class Builder
    {
    public:
        Builder(): 
            m_AnimDataPtr(NULL),
            m_CacheBufferSize(0) {}

        Builder& AnimData(anim::ResAnim animData)
        {
            m_AnimDataPtr = animData.ptr();
            return *this;
        }

        Builder& CacheBufferSize(int size)
        {
            m_CacheBufferSize = size;
            return *this;
        }

        SharedAnimCache* Create(nw::os::IAllocator* allocator)
        {
            void* memory = allocator->Alloc(sizeof(SharedAnimCache));
            if (memory == NULL)
            {
                return NULL;
            }

            SharedAnimCache* result = new(memory) SharedAnimCache(allocator);
            result->SetAnimData(this->m_AnimDataPtr);

            bool cacheBufferAlloced = result->AllocBuffer(this->m_CacheBufferSize);
            if (!cacheBufferAlloced)
            {
                result->~SharedAnimCache();
                allocator->Free(result);
                return NULL;
            }

            return result;
        }

    private:
        anim::ResAnimData* m_AnimDataPtr;
        int m_CacheBufferSize;
    };

    virtual ~SharedAnimCache()
    {
        this->DestroyCache();
    }

    void SetFrame(f32 frame) { m_Frame = frame; }
    f32 GetFrame() const { return m_Frame; }
    void SetStepFrame(f32 stepFrame) { m_StepFrame = stepFrame; }
    f32 GetStepFrame() const { return m_StepFrame; }

    void* GetCacheBuffer() { return m_CacheBuf; }
    bool IsDirty() const { return m_IsDirty; }
    void SetDirtyFlag(bool isDirty) { m_IsDirty = isDirty; }

private:
    void SetAnimData(const anim::ResAnimData* animData)
    {
        m_AnimData = anim::ResAnim(animData);
    }

    bool AllocBuffer(int cacheBufferSize)
    {
        if (0 < cacheBufferSize)
        {
            NW_ASSERT(m_CacheBuf == NULL);
            m_CacheBuf = GetAllocator().Alloc(cacheBufferSize);
            return (m_CacheBuf != NULL);
        }

        return false;
    }

    explicit SharedAnimCache(nw::os::IAllocator* allocator): 
        GfxObject(allocator),
        m_CacheBuf(NULL),
        m_AnimData(NULL),
        m_Frame(0.0f),
        m_StepFrame(0.0f),
        m_IsDirty(true) {}

    inline void DestroyCache()
    {
        if (m_CacheBuf != NULL)
        {
            nw::os::SafeFree(this->m_CacheBuf, &GetAllocator());
        }        
    }

    void* m_CacheBuf;
    anim::ResAnim m_AnimData;
    f32 m_Frame;
    f32 m_StepFrame;
    bool m_IsDirty;
};

}
}