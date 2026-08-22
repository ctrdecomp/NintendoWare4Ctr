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

#ifndef NW_GFX_PARTICLECOLLECTION_H_
#define NW_GFX_PARTICLECOLLECTION_H_

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleShape.h>

#include <nw/gfx/res/gfx_ResParticleCollection.h>

#include <nw/os/os_Memory.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#endif
namespace nw
{
namespace gfx
{

namespace internal
{
//
const int PARTICLE_SIMD_WIDTH_MAX = 8;
} // namespace internal

class ParticleSet;
class ParticleCollection;

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleCollection : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleCollection);

public:
    NW_UT_RUNTIME_TYPEINFO;

    //
    //
    struct ParticleAttribute
    {
        s32 m_Usage;
        bool m_IsStream;
        f32* m_Stream;
    };

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
    static ParticleCollection* Create(
        ParticleSet* parent,
        ResParticleCollection resource,
        os::IAllocator* mainAllocator,
        os::IAllocator* deviceAllocator,
        ParticleShape* shape);

    //
    //
    //
    //
    static size_t GetMemorySize(
        ResParticleCollection resource,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, resource);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleCollection resource);

    //
    //
    //
    //
    static size_t GetDeviceMemorySize(
        ResParticleCollection resource,
        size_t alignment = os::IAllocator::DEFAULT_ALIGNMENT)
    {
        os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, resource);

        return size.GetSizeWithPadding(alignment);
    }

    //
    static void GetDeviceMemorySizeInternal(
        os::MemorySizeCalculator* pSize,
        ResParticleCollection resource);

    //

    //----------------------------------------
    //
    //

    //
    //
    ResParticleCollection GetResParticleCollection()
    {
        return this->m_ResParticleCollection;
    }

    //
    //
    const ResParticleCollection GetResParticleCollection() const
    {
        return this->m_ResParticleCollection;
    }

    //

    //----------------------------------------
    //
    //

    //
    //
    int GetCapacity() const
    {
        return this->m_Capacity;
    }

    //
    //
    int GetCount() const
    {
        return this->m_Count;
    }

    //
    //
    //
    void SetCount(int count)
    {
        this->m_Count = count;
    }

    //
    //
    u16 GetMinActiveIndex() const
    {
        return this->m_MinActiveIndex;
    }

    //
    //
    u16 GetMaxActiveIndex() const
    {
        return this->m_MaxActiveIndex;
    }

    //
    //
    void SetMinActiveIndex(u16 minActiveIndex)
    {
        this->m_MinActiveIndex = minActiveIndex;
    }

    //
    //
    void SetMaxActiveIndex(u16 maxActiveIndex)
    {
        this->m_MaxActiveIndex = maxActiveIndex;
    }

    //
    //
    //
    bool IsStream(ParticleUsage usage) const
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        return this->m_IsStream[usage];
    }

    //
    bool GetBufferSide() const
    {
        return this->m_BufferSide;
    }

    //
    int GetLastBuffer() const
    {
        return this->m_LastBuffer;
    }

    //
    void SetLastBuffer(int value)
    {
        this->m_LastBuffer = value;
    }

    //
    //
    //
    //
    void* GetStreamPtr(ParticleUsage usage, ParticleBuffer side)
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (!this->m_IsStream[usage])
        {
            return NULL;
        }

        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        return this->m_StreamPtr[usage][side];
    }

    //
    //
    //
    //
    const void* GetStreamPtr(ParticleUsage usage, ParticleBuffer side) const
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (!this->m_IsStream[usage])
        {
            return NULL;
        }

        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        return this->m_StreamPtr[usage][side];
    }

    //
    //
    //
    const void* GetParameterPtr(ParticleUsage usage) const
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (this->m_IsStream[usage])
        {
            return NULL;
        }

        return this->m_StreamPtr[usage][0];
    }

    //
    //
    //
    //
    //
    void SetParameter(ParticleUsage usage, ParticleBuffer side, const f32* ptr);

    //
    //
    //
    void SetLifeParameter(const ParticleTime* ptr)
    {
        SetParameter(PARTICLEUSAGE_LIFE, PARTICLE_BUFFER_FRONT, (const f32*)ptr);
    }

    //
    //
    //
    //
    //
    //
    //
    template <class T>
    NW_FORCE_INLINE bool GetStreamOrParameter(ParticleUsage usage, T** stream, T* param, ParticleBuffer side)
    {
        if (this->m_IsStream[usage])
        {
            if (this->m_BufferSide)
            {
                side = (ParticleBuffer)(1 - side);
            }

            *stream = (T*)this->m_StreamPtr[usage][side];
        }
        else
        {
            *stream = NULL;
            if (param != NULL && m_StreamPtr[usage][0] != NULL)
            {
                memcpy(param, this->m_StreamPtr[usage][0], sizeof(T));
            }
        }
        return true;
    }

    //
    //
    //
    void Clear();

    //
    //
    void SwapBuffer()
    {
        ParticleShape* shape = this->m_ParticleShape;

        ParticleBuffer side = PARTICLE_BUFFER_FRONT;
        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        this->m_BufferSide = !this->m_BufferSide;
        shape->SetBufferSide(this->m_BufferSide);

        int size = this->GetMaxActiveIndex() + 1;
        for (int usage = 0; usage < PARTICLEUSAGE_COUNT; ++usage)
        {
            void* src = this->m_StreamPtr[usage][side];
            void* dst = this->m_StreamPtr[usage][(ParticleBuffer)(1 - side)];

            if (usage != PARTICLEUSAGE_ACTIVEINDEX)
            {
                if (src != dst)
                {
                    nw::os::MemCpy(dst, src, this->m_StreamStride[usage] * size);
                }
            }
        }

        this->m_LastBuffer = 1 - this->m_LastBuffer;
    }

    //
    //
    //
    void KillParticles()
    {
        NW_NULL_ASSERT(m_ParticleSet);
        ParticleModel* model = static_cast<ParticleModel*>(m_ParticleSet->GetParent());
        NW_NULL_ASSERT(model);
        ParticleTime time = model->ParticleAnimFrameController().GetFrame();

        u16* activeIndex = (u16*)this->GetStreamPtr(
            nw::gfx::PARTICLEUSAGE_ACTIVEINDEX,nw::gfx::PARTICLE_BUFFER_FRONT);
        NW_NULL_ASSERT(activeIndex);

        const bool isAscendingOrder = m_ParticleSet->IsAscendingOrder();
        const int startIndex = (isAscendingOrder) ? 0 : this->GetCapacity() - 1;
        const int incrIndex = (isAscendingOrder) ? 1 : -1;
        activeIndex += startIndex;

#ifdef NW_GFX_PARTICLE_COMPAT_1_1
        ParticleTime* negtime =
            (ParticleTime*)this->GetStreamPtr(PARTICLEUSAGE_NEG_TIMELIMIT, PARTICLE_BUFFER_FRONT);
        NW_NULL_ASSERT(negtime);

        for (s32 i = 0; i < m_Count; ++i)
        {
            int convertedIndex = *activeIndex;
            activeIndex += incrIndex;

            // It will disappear in -time, but just in case set up a margin
            negtime[convertedIndex] = -time + 1.0f;
        }
#else
        ParticleTime* birth =
            (ParticleTime*)this->GetStreamPtr(PARTICLEUSAGE_BIRTH, PARTICLE_BUFFER_FRONT);
        NW_NULL_ASSERT(birth);

        ParticleTime lifeParam;
        if (this->IsStream(PARTICLEUSAGE_LIFE))
        {
            ParticleTime* life =
                (ParticleTime*)this->GetStreamPtr(PARTICLEUSAGE_LIFE, PARTICLE_BUFFER_FRONT);

            for (s32 i = 0; i < m_Count; ++i)
            {
                int convertedIndex = *activeIndex;
                activeIndex += incrIndex;

                lifeParam = life[convertedIndex];
                birth[convertedIndex] = time - lifeParam - 1;
            }
        }
        else
        {
            lifeParam = *(ParticleTime*)this->GetParameterPtr(PARTICLEUSAGE_LIFE);
            for (s32 i = 0; i < m_Count; ++i)
            {
                int convertedIndex = *activeIndex;
                activeIndex += incrIndex;

                birth[convertedIndex] = time - lifeParam - 1;
            }
        }
#endif
    }

    //
    //
    //
    //
    void KillParticle(int index)
    {
        NW_NULL_ASSERT(m_ParticleSet);
        ParticleModel* model = static_cast<ParticleModel*>(m_ParticleSet->GetParent());
        NW_NULL_ASSERT(model);
        ParticleTime time = model->ParticleAnimFrameController().GetFrame();

#ifdef NW_GFX_PARTICLE_COMPAT_1_1
        ParticleTime* negtime =
            (ParticleTime*)this->GetStreamPtr(PARTICLEUSAGE_NEG_TIMELIMIT, PARTICLE_BUFFER_FRONT);
        NW_NULL_ASSERT(negtime);

        negtime[index] = -time + 1.0f; // It will disappear in -time, but just in case set up a margin
#else
        ParticleTime* birth =
            (ParticleTime*)this->GetStreamPtr(PARTICLEUSAGE_BIRTH, PARTICLE_BUFFER_FRONT);
        NW_NULL_ASSERT(birth);

        ParticleTime lifeParam;
        if (this->IsStream(PARTICLEUSAGE_LIFE))
        {
            ParticleTime* life =
                (ParticleTime*)this->GetStreamPtr(PARTICLEUSAGE_LIFE, PARTICLE_BUFFER_FRONT);
            lifeParam = life[index];
        }
        else
        {
            lifeParam = *(ParticleTime*)this->GetParameterPtr(PARTICLEUSAGE_LIFE);
        }

        birth[index] = time - lifeParam - 1;
#endif
    }

    //

protected:
    //----------------------------------------
    //
    //

    //
    ParticleCollection(
        os::IAllocator* allocator,
        os::IAllocator* deviceAllocator,
        void* deviceMemory,
        ResParticleCollection resObj);

    //
    virtual ~ParticleCollection();

    //
private:
    ParticleAttribute m_ParticleAttribute[PARTICLEUSAGE_COUNT];
    int m_Capacity;
    int m_Count;
    bool m_BufferSide;
    bool m_IsStream[PARTICLEUSAGE_COUNT];
    void* m_StreamPtr[PARTICLEUSAGE_COUNT][2];
    int m_StreamStride[PARTICLEUSAGE_COUNT];

    u16 m_MinActiveIndex;
    u16 m_MaxActiveIndex;

    ResParticleCollection m_ResParticleCollection;
    ParticleShape* m_ParticleShape;
    ParticleSet* m_ParticleSet;

    os::IAllocator* m_DeviceAllocator;
    void* m_DeviceMemory;

    int m_LastBuffer;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_PARTICLECOLLECTION_H_
