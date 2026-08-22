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

#ifndef NW_GFX_ANIMEVALUATOR_H_
#define NW_GFX_ANIMEVALUATOR_H_

#include <nw/gfx/gfx_BaseAnimEvaluator.h>
#include <nw/gfx/gfx_SharedAnimCache.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/ut/ut_RuntimeTypeInfo.h>

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
class AnimEvaluator : public BaseAnimEvaluator
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
        Builder& AnimData(const nw::anim::ResAnim& animData) { m_AnimData = animData; return *this; }

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

            size += sizeof(AnimEvaluator);
            AnimEvaluator::GetMemorySizeForInitialize(pSize, m_AnimData, m_MaxMembers, m_MaxAnimMembers, m_AllocCache);
        }

        //
        //
        //
        //
        //
        //
        AnimEvaluator* Create(os::IAllocator* allocator)
        {
            void* buf = allocator->Alloc(sizeof(AnimEvaluator));

            if (buf == NULL)
            {
                return NULL;
            }

            AnimEvaluator* animEvaluator = new(buf) AnimEvaluator(allocator);

            Result result = animEvaluator->Initialize(m_AnimData, m_MaxMembers, m_MaxAnimMembers, m_AllocCache);
            NW_ASSERT(result.IsSuccess());

            return animEvaluator;
        }

    private:
        nw::anim::ResAnim m_AnimData;
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
    virtual Result ChangeAnim(const nw::anim::ResAnim animData)
    {
        // To find the cache size, you need to reference m_AnimData, so it needs to be updated ahead of time
        // 
        Result result = BaseAnimEvaluator::ChangeAnim(animData);

        m_CachePtrs.Resize(animData.GetMemberAnimSetCount());

        if (!m_IsCacheExternal && m_CacheBuf != NULL)
        {
            os::SafeFree(m_CacheBuf, &GetAllocator());

            // Avoid Alloc with a size 0
            // Can occur when using functions from CreateEmpty to Anim
            if (animData.GetMemberAnimSetCount() != 0)
            {
                m_CacheBuf = GetAllocator().Alloc(GetCacheBufferSizeNeeded());
                NW_NULL_ASSERT(m_CacheBuf);

                SetCacheBufferPointers();
            }
        }

        return result;
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
    //
    Result ForceBindMaterialAnim(AnimGroup* animGroup, const char* materialName);

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
    virtual bool HasMemberAnim(int memberIdx) const
    {
        NW_MINMAXLT_ASSERT(memberIdx, 0, m_BindIndexTable.Size());
        return m_BindIndexTable[memberIdx] != NotFoundIndex;
    }

    //

    //----------------------------------------
    //
    //

    //
    virtual void UpdateCache() { this->UpdateCacheNonVirtual(); }

    //
    void UpdateCacheNonVirtual();

    //
    void UpdateCacheImpl();

    //
    //
    virtual int GetCacheBufferSizeNeeded() const;

    //
    virtual const void* GetCacheBuffer() const { return m_CacheBuf; }

    //
    //
    //
    //
    //
    //
    //
    virtual void SetCacheBuffer(void* buf, int size)
    {
        m_CacheBuf = buf;
        if (buf != NULL)
        {
            NW_ASSERT(size >= GetCacheBufferSizeNeeded());
            (void)size;
            m_IsCacheDirty = true;
            m_IsCacheExternal = true;
            SetCacheBufferPointers();
        }
    }

    //
    void SetSharedCacheBuffer(SharedAnimCache* cache)
    {
        // When the cache was specified with another format, stops on an assert
        NW_ASSERT(m_CacheBuf == NULL);
        NW_ASSERT(!m_IsCacheExternal);

        m_SharedCache = cache;
        if (cache != NULL)
        {
            m_IsCacheDirty = true;
            m_IsCacheExternal = true;
            m_UseSharedCache = true;
            m_CacheBuf = cache->GetCacheBuffer();
            SetCacheBufferPointers();
        }
        else
        {
            m_CacheBuf = NULL;
        }
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    //
    //
    AnimEvaluator(
        os::IAllocator* allocator);

    //
    //
    //
    virtual ~AnimEvaluator()
    {
        if (!m_IsCacheExternal && m_CacheBuf != NULL)
        {
            GetAllocator().Free(m_CacheBuf);
        }
    }

    //

    //
    //
    //
    static void GetMemorySizeForInitialize(
        os::MemorySizeCalculator* pSize,
        const nw::anim::ResAnim& animData,
        const int maxMembers,
        const int maxAnimMembers,
        bool allocCache);

    //
    Result Initialize(
        const nw::anim::ResAnim& animData,
        const int maxMembers,
        const int maxAnimMembers,
        bool allocCache);

    //
    //
    //
    static int GetCacheBufferSizeNeeded(const anim::ResAnim& animData);

    //
    //
    //
    void SetCacheBufferPointers();

    //
    //
    //
    void* m_CacheBuf;

    //
    SharedAnimCache* m_SharedCache;

    //
    //
    //
    ut::MoveArray<anim::AnimResult*> m_CachePtrs;

private:
    //
    //
    //
    class ReplaceMaterialNameIndexGetterFunctor
    {
    private:
        static const int MAX_BUFFER_LENGTH = 256;

    public:
        ReplaceMaterialNameIndexGetterFunctor(const char* materialName)
            : m_MaterialName(materialName),
              m_LenMaterialName(0)
        {
            NW_NULL_ASSERT(materialName);
            m_LenMaterialName = std::strlen(m_MaterialName);
        }

        int operator() (AnimGroup* animGroup, anim::ResMemberAnim member)
        {
            const char MATERIAL_PREFIX[] = "Materials[\"";
            const size_t MATERIAL_PREFIX_LEN = sizeof(MATERIAL_PREFIX) - 1;

            const char* path = member.GetPath();
            size_t lenPath = std::strlen(path);

            NW_ASSERTMSG(std::strncmp(path, MATERIAL_PREFIX, MATERIAL_PREFIX_LEN) == 0, "Not material animation member.");

            NW_FAILSAFE_IF (lenPath <= MATERIAL_PREFIX_LEN)
            {
                // Since the length of the path to be rewritten is abnormal, it is possible that the resource is corrupted.
                // 

                // -1 is the return value when it couldn't be found with a dictionary search
                return -1;
            }

            // "Materials[\"" is confirmed so the pointer advances to the next string.
            path += MATERIAL_PREFIX_LEN;
            lenPath -=  MATERIAL_PREFIX_LEN;

            // Search for next ".
            while ((*path != 0) && (*path != '\"'))
            {
                ++path;
                --lenPath;
            }

            // At this point the path either begins with a '\"' or indicates a NULL terminator.

            NW_FAILSAFE_IF (MATERIAL_PREFIX_LEN + m_LenMaterialName + lenPath + 1 > MAX_BUFFER_LENGTH)
            {
                // The rewritten path cannot fit in the buffer.

                // -1 is the return value when it couldn't be found with a dictionary search
                return -1;
            }

            // Create the path
            char replacedPath[MAX_BUFFER_LENGTH];
            ut::strncpy(replacedPath, MAX_BUFFER_LENGTH, MATERIAL_PREFIX, MATERIAL_PREFIX_LEN);
            ut::strncat(replacedPath, MAX_BUFFER_LENGTH, m_MaterialName, m_LenMaterialName);
            ut::strncat(replacedPath, MAX_BUFFER_LENGTH, path, lenPath);

            return animGroup->GetResAnimGroupMemberIndex(replacedPath);
        }

    private:
        const char* m_MaterialName;
        size_t m_LenMaterialName;
    };
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_ANIMEVALUATOR_H_
