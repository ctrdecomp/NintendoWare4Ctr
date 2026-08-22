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

class ParticleCollection : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleCollection);

public:
    NW_UT_RUNTIME_TYPEINFO;

    struct ParticleAttribute{
        s32 mUsage;
        bool mIsStream;
        f32* mStream;
    };

    static ParticleCollection* Create(ParticleSet* parent,ResParticleCollection resource,nw::os::IAllocator* mainAllocator,nw::os::IAllocator* deviceAllocator,ParticleShape* shape);

    static size_t GetMemorySize(ResParticleCollection resource, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);
        GetMemorySizeInternal(&size, resource);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResParticleCollection resource);

    static size_t GetDeviceMemorySize(ResParticleCollection resource, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT){
        nw::os::MemorySizeCalculator size(alignment);
        GetDeviceMemorySizeInternal(&size, resource);
        return size.GetSizeWithPadding(alignment);
    }

    static void GetDeviceMemorySizeInternal(nw::os::MemorySizeCalculator* pSize, ResParticleCollection resource);

    ResParticleCollection GetResParticleCollection(){
        return mResParticleCollection;
    }

    const ResParticleCollection GetResParticleCollection() const{
        return mResParticleCollection;
    }

    int GetCapacity() const{
        return mCapacity;
    }

    int GetCount() const{
        return mCount;
    }

    void SetCount(int count){
        mCount = count;
    }

    u16 GetMinActiveIndex() const{
        return mMinActiveIndex;
    }

    u16 GetMaxActiveIndex() const{
        return mMaxActiveIndex;
    }

    void SetMinActiveIndex(u16 minActiveIndex){
        mMinActiveIndex = minActiveIndex;
    }

    void SetMaxActiveIndex(u16 maxActiveIndex){
        mMaxActiveIndex = maxActiveIndex;
    }

    bool IsStream(ParticleUsage usage) const{
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);
        return mIsStream[usage];
    }

    bool GetBufferSide() const{
        return mBufferSide;
    }

    int GetLastBuffer() const{
        return mLastBuffer;
    }

    void SetLastBuffer(int value){
        mLastBuffer = value;
    }

    void* GetStreamPtr(ParticleUsage usage, ParticleBuffer side){
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (!mIsStream[usage]){
            return NULL;
        }

        if (mBufferSide){
            side = (ParticleBuffer)(1 - side);
        }

        return mStreamPtr[usage][side];
    }

    const void* GetStreamPtr(ParticleUsage usage, ParticleBuffer side) const{
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (!mIsStream[usage]){
            return NULL;
        }

        if (mBufferSide){
            side = (ParticleBuffer)(1 - side);
        }

        return mStreamPtr[usage][side];
    }

    const void* GetParameterPtr(ParticleUsage usage) const{
        NW_ASSERT(usage >= 0 && usage < PARTICLEUSAGE_COUNT);

        if (mIsStream[usage]){
            return NULL;
        }

        return mStreamPtr[usage][0];
    }

    void SetParameter(ParticleUsage usage, ParticleBuffer side, const f32* ptr);

    void SetLifeParameter(const ParticleTime* ptr){
        SetParameter(PARTICLEUSAGE_LIFE, PARTICLE_BUFFER_FRONT, (const f32*)ptr);
    }

    template <class T>
    NW_FORCE_INLINE bool GetStreamOrParameter(ParticleUsage usage, T** stream, T* param, ParticleBuffer side){
        if (mIsStream[usage]){
            if (mBufferSide){
                side = (ParticleBuffer)(1 - side);
            }

            *stream = (T*)mStreamPtr[usage][side];
        }
        else{
            *stream = NULL;
            if (param != NULL && mStreamPtr[usage][0] != NULL){
                memcpy(param, mStreamPtr[usage][0], sizeof(T));
            }
        }

        return true;
    }

    void Clear();

    void SwapBuffer(){
        ParticleShape* shape = mParticleShape;

        ParticleBuffer side = PARTICLE_BUFFER_FRONT;
        if (mBufferSide){
            side = (ParticleBuffer)(1 - side);
        }

        mBufferSide = !mBufferSide;
        shape->SetBufferSide(mBufferSide);

        int size = GetMaxActiveIndex() + 1;
        for (int usage = 0; usage < PARTICLEUSAGE_COUNT; ++usage){
            void* src = mStreamPtr[usage][side];
            void* dst = mStreamPtr[usage][(ParticleBuffer)(1 - side)];

            if (usage != PARTICLEUSAGE_ACTIVEINDEX){
                if (src != dst){
                    nw::os::MemCpy(dst, src, mStreamStride[usage] * size);
                }
            }
        }

        mLastBuffer = 1 - mLastBuffer;
    }

protected:
    ParticleCollection(nw::os::IAllocator* allocator, nw::os::IAllocator* deviceAllocator, void* deviceMemory, ResParticleCollection resObj);

    virtual ~ParticleCollection();

private:
    ParticleAttribute mParticleAttribute[PARTICLEUSAGE_COUNT];
    int mCapacity;
    int mCount;
    bool mBufferSide;
    bool mIsStream[PARTICLEUSAGE_COUNT];
    void* mStreamPtr[PARTICLEUSAGE_COUNT][2];
    int mStreamStride[PARTICLEUSAGE_COUNT];

    u16 mMinActiveIndex;
    u16 mMaxActiveIndex;

    ResParticleCollection mResParticleCollection;
    ParticleShape* mParticleShape;
    ParticleSet* mParticleSet;

    nw::os::IAllocator* mDeviceAllocator;
    void* mDeviceMemory;

    int mLastBuffer;
};

}
}