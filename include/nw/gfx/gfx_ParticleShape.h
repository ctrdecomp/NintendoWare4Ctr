#pragma once

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>
#include <nw/ut/ut_Float24.h>

namespace nw{
namespace gfx{
class ParticleShape;
class ParticleSet;

enum ParticleBuffer
{
    PARTICLE_BUFFER_FRONT = 0,
    PARTICLE_BUFFER_BACK
};

class ParticleShape : public SceneObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleShape);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct VertexAttribute
    {
        s32 m_Usage;
        u32 m_FormatType;
        u8 m_Dimension;
        bool m_IsStream;
        u8* m_Stream[2];
        void* m_CommandPtr[2];
    };

    static ParticleShape* Create(ResSceneObject resource,int capacity,nw::os::IAllocator* mainAllocator,nw::os::IAllocator* deviceAllocator);

    static size_t GetMemorySize(int capacity,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, capacity);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,int capacity);

    static size_t GetDeviceMemorySize(int capacity,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, capacity);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,int capacity);

    static int AddVertexStreamSize(u32 formatType,int dimension,int capacity,int prevSize);

    static int AddVertexParamSize(u32 formatType,int dimension,int prevSize);

    VertexAttribute* AddVertexStream(s32 usage,u32 formatType,int dimension,int capacity,u8** memory);

    VertexAttribute* AddVertexParam(s32 usage,u32 formatType,int dimension,f32* parameters,u8** memory);

    ResParticleShape GetResParticleShape()
    {
        return nw::ut::ResDynamicCast<ResParticleShape>( this->GetResSceneObject() );
    }

    const ResParticleShape GetResParticleShape() const
    {
        return nw::ut::ResDynamicCast<ResParticleShape>( this->GetResSceneObject() );
    }

    int GetVertexAttributesCount() const
    {
        return this->m_ResVertexAttributeDataCount;
    }

    bool IsVertexStream(int index)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        return this->m_VertexAttribute[index].m_IsStream;
    }

    s32 GetVertexAttributeUsage(int index)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        return this->m_VertexAttribute[index].m_Usage;
    }

    u32 GetVertexAttributeFormatType(int index)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        return this->m_VertexAttribute[index].m_FormatType;
    }

    u8 GetVertexAttributeDimension(int index)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        return this->m_VertexAttribute[index].m_Dimension;
    }

    u8* GetVertexStreamPtr(int index, ParticleBuffer side)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        if (!this->m_VertexAttribute[index].m_IsStream)
        {
            return NULL;
        }

        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        return this->m_VertexAttribute[index].m_Stream[side];
    }

    const u8* GetVertexStreamPtr(int index, ParticleBuffer side) const
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        if (!this->m_VertexAttribute[index].m_IsStream)
        {
            return NULL;
        }

        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        return this->m_VertexAttribute[index].m_Stream[side];
    }

    f32* GetVertexParameter(int index)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        if (this->m_VertexAttribute[index].m_IsStream)
        {
            return NULL;
        }

        return reinterpret_cast<f32*>(this->m_VertexAttribute[index].m_Stream[0]);
    }

    const f32* GetVertexParameter(int index) const
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        if (this->m_VertexAttribute[index].m_IsStream)
        {
            return NULL;
        }

        return reinterpret_cast<const f32*>(this->m_VertexAttribute[index].m_Stream[0]);
    }

    void SetVertexParameter(int index, ParticleBuffer side, const f32* ptr)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->m_VertexAttribute[index].m_IsStream)
        {
            return;
        }

        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }

        int count = this->GetVertexAttributeDimension(index);
        f32* fdata = this->GetVertexParameter(index);

        u32 data[4] = { 0, 0, 0, 0 };

        for (int j = 0; j < count; ++j)
        {
            fdata[j] = ptr[j];
            data[j] = nw::ut::Float24::Float32ToBits24(fdata[j]);
        }

        u32* target = reinterpret_cast<u32*>(this->m_VertexAttribute[index].m_CommandPtr[side]);

        *target++ = (data[3] <<  8) | (data[2] >> 16);
        *target++ = (data[2] << 16) | (data[1] >> 8);
        *target = (data[1] << 24) | (data[0]);
    }

    void* SetVertexAttributeCommandPtr(int index, int side, void* ptr)
    {
        NW_ASSERT(index >= 0 && index < this->m_ResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        return this->m_VertexAttribute[index].m_CommandPtr[side] = ptr;
    }

    u8* GetPrimitiveStreamPtr(ParticleBuffer side)
    {
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }
        return this->m_PrimitiveBuffer[side];
    }

    const u8* GetPrimitiveStreamPtr(ParticleBuffer side) const
    {
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }
        return this->m_PrimitiveBuffer[side];
    }

    u32 GetPrimitiveStreamOffset(ParticleBuffer side) const
    {
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->m_BufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }
        return this->m_PrimitiveBufferOffset[side];
    }

    int GetVertexCapacity() const
    {
        return m_Capacity;
    }

    void SetBufferSide(bool swap)
    {
        this->m_BufferSide = swap;
    }

    int GetBufferSide() const
    {
        return this->m_BufferSide ? 1 : 0;
    }

    void CreateCommandCache(ParticleSet* particleSet);

    void FlushBuffer();

    void* m_CommandCache[2];
    s32   m_CommandCacheSize[2];

    void* m_DeactivateVertexCommandCache;
    s32   m_DeactivateVertexCommandCacheSize;

    void* m_PrimitiveCommandCache;
    s32   m_PrimitiveCommandCacheSize;

protected:
    ParticleShape(int capacity,nw::os::IAllocator* allocator,nw::os::IAllocator* deviceAllocator,void* deviceMemory,ResParticleShape resObj);

    virtual ~ParticleShape();

private:
    VertexAttribute* AddVertexAttribute(s32 usage,u32 formatType,u8 dimension,bool isStream,u8* stream0,u8* stream1)
{
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_Usage = usage;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_FormatType = formatType;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_Dimension = dimension;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_IsStream = isStream;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_Stream[0] = stream0;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_Stream[1] = stream1;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_CommandPtr[0] = NULL;
        this->m_VertexAttribute[this->m_ResVertexAttributeDataCount].m_CommandPtr[1] = NULL;
        ++this->m_ResVertexAttributeDataCount;
        return &this->m_VertexAttribute[this->m_ResVertexAttributeDataCount - 1];
    }

    static const int bufferAlignment = 4;
    int m_Capacity;
    bool m_BufferSide;

    int m_ResVertexAttributeDataCount;
    VertexAttribute m_VertexAttribute[PARTICLEUSAGE_COUNT];

    u8* m_PrimitiveBuffer[2];
    u32 m_PrimitiveBufferOffset[2];

    nw::os::IAllocator* m_DeviceAllocator;
    void* m_DeviceMemory;
};

}
}