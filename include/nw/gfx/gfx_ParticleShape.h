#pragma once

#include <nw/gfx/gfx_SceneObject.h>
#include <nw/gfx/res/gfx_ResParticleShape.h>
#include <nw/gfx/res/gfx_ResParticleUpdater.h>
#include <nw/ut/ut_Float24.h>

namespace nw{
namespace gfx{
class ParticleShape;
class ParticleSet;

enum ParticleBuffer{
    PARTICLE_BUFFER_FRONT = 0,
    PARTICLE_BUFFER_BACK
};

class ParticleShape : public SceneObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleShape);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct VertexAttribute{
        s32 mUsage;
        u32 mFormatType;
        u8 mDimension;
        bool mIsStream;
        u8* mStream[2];
        void* mCommandPtr[2];
    };

    static ParticleShape* Create(ResSceneObject resource,int capacity,nw::os::IAllocator* mainAllocator,nw::os::IAllocator* deviceAllocator);

    static size_t GetMemorySize(int capacity,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeInternal(&size, capacity);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,int capacity);

    static size_t GetDeviceMemorySize(int capacity,size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);

        GetDeviceMemorySizeInternal(&size, capacity);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize,int capacity);

    static int AddVertexStreamSize(u32 formatType,int dimension,int capacity,int prevSize);

    static int AddVertexParamSize(u32 formatType,int dimension,int prevSize);

    VertexAttribute* AddVertexStream(s32 usage,u32 formatType,int dimension,int capacity,u8** memory);

    VertexAttribute* AddVertexParam(s32 usage,u32 formatType,int dimension,f32* parameters,u8** memory);

    ResParticleShape GetResParticleShape(){
        return nw::ut::ResDynamicCast<ResParticleShape>( this->GetResSceneObject() );
    }

    const ResParticleShape GetResParticleShape() const{
        return nw::ut::ResDynamicCast<ResParticleShape>( this->GetResSceneObject() );
    }

    int GetVertexAttributesCount() const{
        return this->mResVertexAttributeDataCount;
    }

    bool IsVertexStream(int index){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        return this->mVertexAttribute[index].mIsStream;
    }

    s32 GetVertexAttributeUsage(int index){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        return this->mVertexAttribute[index].mUsage;
    }

    u32 GetVertexAttributeFormatType(int index){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        return this->mVertexAttribute[index].mFormatType;
    }

    u8 GetVertexAttributeDimension(int index){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        return this->mVertexAttribute[index].mDimension;
    }

    u8* GetVertexStreamPtr(int index, ParticleBuffer side){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        if (!this->mVertexAttribute[index].mIsStream){
            return NULL;
        }

        if (this->mBufferSide){
            side = (ParticleBuffer)(1 - side);
        }

        return this->mVertexAttribute[index].mStream[side];
    }

    const u8* GetVertexStreamPtr(int index, ParticleBuffer side) const{
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        if (!this->mVertexAttribute[index].mIsStream){
            return NULL;
        }

        if (this->mBufferSide){
            side = (ParticleBuffer)(1 - side);
        }

        return this->mVertexAttribute[index].mStream[side];
    }

    f32* GetVertexParameter(int index){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        if (this->mVertexAttribute[index].mIsStream){
            return NULL;
        }

        return reinterpret_cast<f32*>(this->mVertexAttribute[index].mStream[0]);
    }

    const f32* GetVertexParameter(int index) const{
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        if (this->mVertexAttribute[index].mIsStream){
            return NULL;
        }

        return reinterpret_cast<const f32*>(this->mVertexAttribute[index].mStream[0]);
    }

    void SetVertexParameter(int index, ParticleBuffer side, const f32* ptr){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->mVertexAttribute[index].mIsStream){
            return;
        }

        if (this->mBufferSide){
            side = (ParticleBuffer)(1 - side);
        }

        int count = this->GetVertexAttributeDimension(index);
        f32* fdata = this->GetVertexParameter(index);

        u32 data[4] = { 0, 0, 0, 0 };

        for (int j = 0; j < count; ++j){
            fdata[j] = ptr[j];
            data[j] = nw::ut::Float24::Float32ToBits24(fdata[j]);
        }

        u32* target = reinterpret_cast<u32*>(this->mVertexAttribute[index].mCommandPtr[side]);

        *target++ = (data[3] <<  8) | (data[2] >> 16);
        *target++ = (data[2] << 16) | (data[1] >> 8);
        *target = (data[1] << 24) | (data[0]);
    }

    void* SetVertexAttributeCommandPtr(int index, int side, void* ptr){
        NW_ASSERT(index >= 0 && index < this->mResVertexAttributeDataCount);
        NW_ASSERT(side >= 0 && side <= 1);
        return this->mVertexAttribute[index].mCommandPtr[side] = ptr;
    }

    u8* GetPrimitiveStreamPtr(ParticleBuffer side){
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->mBufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }
        return this->mPrimitiveBuffer[side];
    }

    const u8* GetPrimitiveStreamPtr(ParticleBuffer side) const{
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->mBufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }
        return this->mPrimitiveBuffer[side];
    }

    u32 GetPrimitiveStreamOffset(ParticleBuffer side) const{
        NW_ASSERT(side >= 0 && side <= 1);
        if (this->mBufferSide)
        {
            side = (ParticleBuffer)(1 - side);
        }
        return this->mPrimitiveBufferOffset[side];
    }

    int GetVertexCapacity() const{
        return mCapacity;
    }

    void SetBufferSide(bool swap){
        this->mBufferSide = swap;
    }

    int GetBufferSide() const{
        return this->mBufferSide ? 1 : 0;
    }

    void CreateCommandCache(ParticleSet* particleSet);

    void FlushBuffer();

    void* mCommandCache[2];
    s32   mCommandCacheSize[2];

    void* mDeactivateVertexCommandCache;
    s32   mDeactivateVertexCommandCacheSize;

    void* mPrimitiveCommandCache;
    s32   mPrimitiveCommandCacheSize;

protected:
    ParticleShape(int capacity,nw::os::IAllocator* allocator,nw::os::IAllocator* deviceAllocator,void* deviceMemory,ResParticleShape resObj);

    virtual ~ParticleShape();

private:
    VertexAttribute* AddVertexAttribute(s32 usage,u32 formatType,u8 dimension,bool isStream,u8* stream0,u8* stream1){
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mUsage = usage;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mFormatType = formatType;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mDimension = dimension;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mIsStream = isStream;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mStream[0] = stream0;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mStream[1] = stream1;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mCommandPtr[0] = NULL;
        this->mVertexAttribute[this->mResVertexAttributeDataCount].mCommandPtr[1] = NULL;
        ++this->mResVertexAttributeDataCount;
        return &this->mVertexAttribute[this->mResVertexAttributeDataCount - 1];
    }

    static const int bufferAlignment = 4;
    int mCapacity;
    bool mBufferSide;

    int mResVertexAttributeDataCount;
    VertexAttribute mVertexAttribute[PARTICLEUSAGE_COUNT];

    u8* mPrimitiveBuffer[2];
    u32 mPrimitiveBufferOffset[2];

    nw::os::IAllocator* mDeviceAllocator;
    void* mDeviceMemory;
};

}
}