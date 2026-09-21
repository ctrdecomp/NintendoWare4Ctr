#pragma once

#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ParticleSet.h>
#include <nw/gfx/gfx_ParticleShape.h>

#include <nw/gfx/res/gfx_ResParticleCollection.h>

#include <nw/os/os_Memory.h>

namespace nw{
namespace os{
    class IAllocator;
}
namespace gfx{
namespace internal{
    const int PARTICLE_SIMD_WIDTH_MAX = 8;
}

class ParticleSet;
class ParticleCollection;

class ParticleCollection : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleCollection);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct ParticleAttribute
    {
        s32 m_Usage;
        bool m_IsStream;
        f32* m_Stream;
    };

    static ParticleCollection* Create(ParticleSet* parent,ResParticleCollection resource,nw::os::IAllocator* mainAllocator,nw::os::IAllocator* deviceAllocator,ParticleShape* shape);

    static size_t GetMemorySize(ResParticleCollection resource, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);
        GetMemorySizeInternal(&size, resource);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResParticleCollection resource);

    static size_t GetDeviceMemorySize(ResParticleCollection resource, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);
        GetDeviceMemorySizeInternal(&size, resource);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResParticleCollection resource);

    ResParticleCollection GetResParticleCollection()
    {
        return m_ResParticleCollection;
    }

    const ResParticleCollection GetResParticleCollection() const
    {
        return m_ResParticleCollection;
    }

    int GetCapacity() const
    {
        return m_Capacity;
    }

    int GetCount() const
    {
        return m_Count;
    }

    void SetCount(int count)
    {
        m_Count = count;
    }

    u16 GetMinActiveIndex() const
    {
        return m_MinActiveIndex;
    }

    u16 GetMaxActiveIndex() const
    {
        return m_MaxActiveIndex;
    }

    void SetMinActiveIndex(u16 minActiveIndex)
    {
        m_MinActiveIndex = minActiveIndex;
    }

    void SetMaxActiveIndex(u16 maxActiveIndex)
    {
        m_MaxActiveIndex = maxActiveIndex;
    }

    bool IsStream(ParticleUsage usage) const
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);
        return m_IsStream[usage];
    }

    bool GetBufferSide() const
    {
        return m_BufferSide;
    }

    int GetLastBuffer() const
    {
        return m_LastBuffer;
    }

    void SetLastBuffer(int value)
    {
        m_LastBuffer = value;
    }

    void* GetStreamPtr(ParticleUsage usage, ParticleBuffer side)
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (!m_IsStream[usage])
        {
            return NULL;
        }

        if (m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        return m_StreamPtr[usage][side];
    }

    const void* GetStreamPtr(ParticleUsage usage, ParticleBuffer side) const
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (!m_IsStream[usage])
        {
            return NULL;
        }

        if (m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        return m_StreamPtr[usage][side];
    }

    const void* GetParameterPtr(ParticleUsage usage) const
    {
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (m_IsStream[usage])
        {
            return NULL;
        }

        return m_StreamPtr[usage][0];
    }

    void SetParameter(ParticleUsage usage, ParticleBuffer side, const f32* ptr);

    void SetLifeParameter(const ParticleTime* ptr)
    {
        SetParameter(PARTICLEUSAGE_LIFE, PARTICLE_BUFFER_FRONT, (const f32*)ptr);
    }

    template <class T>
    NW_FORCE_INLINE bool GetStreamOrParameter(ParticleUsage usage, T** stream, T* param, ParticleBuffer side)
    {
        if (m_IsStream[usage])
        {
            if (m_BufferSide)
            {
                side = (ParticleBuffer)(1 - side);
            }

            *stream = (T*)m_StreamPtr[usage][side];
        }
        else{
            *stream = NULL;
            if (param != NULL && m_StreamPtr[usage][0] != NULL)
            {
                memcpy(param, m_StreamPtr[usage][0], sizeof(T));
            }
        }

        return true;
    }

    void Clear();

    void SwapBuffer()
    {
        ParticleShape* shape = m_ParticleShape;

        ParticleBuffer side = PARTICLE_BUFFER_FRONT;
        if (m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        m_BufferSide = !m_BufferSide;
        shape->SetBufferSide(m_BufferSide);

        int size = GetMaxActiveIndex() + 1;
        for (int usage = 0; usage < PARTICLEUSAGE_COUNT; ++usage)
        {
            void* src = m_StreamPtr[usage][side];
            void* dst = m_StreamPtr[usage][(ParticleBuffer)(1 - side)];

            if (usage != PARTICLEUSAGE_ACTIVEINDEX)
            {
                if (src != dst)
                {
                    nw::os::MemCpy(dst, src, m_StreamStride[usage] * size);
                }
            }
        }

        m_LastBuffer = 1 - m_LastBuffer;
    }

protected:
    ParticleCollection(nw::os::IAllocator* allocator, nw::os::IAllocator* deviceAllocator, void* deviceMemory, ResParticleCollection resObj);

    virtual ~ParticleCollection();

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

    nw::os::IAllocator* m_DeviceAllocator;
    void* m_DeviceMemory;

    int m_LastBuffer;
};

}
}