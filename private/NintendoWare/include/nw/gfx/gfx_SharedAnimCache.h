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

#ifndef NW_GFX_SHAREDANIMCACHE_H_
#define NW_GFX_SHAREDANIMCACHE_H_

#include <nw/gfx/gfx_GfxObject.h>

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
//---------------------------------------------------------------------------
class SharedAnimCache : public GfxObject
{
public:
    //
    class Builder
    {
    public:
        //
        Builder()
        : m_AnimDataPtr(NULL),
          m_CacheBufferSize(0)
        {
        }

        //
        Builder& AnimData(anim::ResAnim animData)
        {
            m_AnimDataPtr = animData.ptr();
            return *this;
        }

        //
        //
        //
        Builder& CacheBufferSize(int size)
        {
            m_CacheBufferSize = size;
            return *this;
        }

        //
        //
        //
        //
        //
        //
        SharedAnimCache* Create(os::IAllocator* allocator)
        {
            void* memory = allocator->Alloc(sizeof(SharedAnimCache));
            if (memory == NULL)
            {
                return NULL;
            }

            SharedAnimCache* result = new(memory) SharedAnimCache(allocator);
            result->SetAnimData(m_AnimDataPtr);

            bool cacheBufferAlloced = result->AllocBuffer(m_CacheBufferSize);
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

    //
    //
    virtual ~SharedAnimCache()
    {
        DestroyCache();
    }

    // The needed interface to update the cache.
    // It is not assumed that the user will directly operate this.
    void SetFrame(f32 frame) { m_Frame = frame; } //
    f32 GetFrame() const { return m_Frame; } //
    void SetStepFrame(f32 stepFrame) { m_StepFrame = stepFrame; } //
    f32 GetStepFrame() const { return m_StepFrame; } //
    // Since the cache must be written to, it is not a const value
    void* GetCacheBuffer() { return m_CacheBuf; } //
    bool IsDirty() const { return m_IsDirty; } //
    void SetDirtyFlag(bool isDirty) { m_IsDirty = isDirty; } //

private:
    void SetAnimData(const anim::ResAnimData* animData)
    {
        m_AnimData = anim::ResAnim(animData);
    }

    bool AllocBuffer(int cacheBufferSize)
    {
        // When the buffer could not be allocated FALSE is returned and the success of generation is determined
        if (0 < cacheBufferSize)
        {
            NW_ASSERT(m_CacheBuf == NULL);
            m_CacheBuf = GetAllocator().Alloc(cacheBufferSize);
            return (m_CacheBuf != NULL);
        }

        return false;
    }

    explicit SharedAnimCache(os::IAllocator* allocator)
    : GfxObject(allocator),
      m_CacheBuf(NULL),
      m_AnimData(NULL),
      m_Frame(0.0f),
      m_StepFrame(0.0f),
      m_IsDirty(true)
    {
    }

    NW_INLINE void DestroyCache()
    {
        if (m_CacheBuf != NULL)
        {
            os::SafeFree(m_CacheBuf, &GetAllocator());
        }
    }

    void* m_CacheBuf;
    // The cache's index uses the CachePtr from AnimEvaluator so it cannot hold it.
    // What this class holds is information that would allow a unique determination on the state of an animation, including the frame, pointer to the data, etc.
    anim::ResAnim m_AnimData;
    f32 m_Frame;
    f32 m_StepFrame;
    bool m_IsDirty;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#endif

#endif //  NW_GFX_SHAREDANIMCACHE_H_
