#pragma once

#include <nw/math/math_Types.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ParticleRandom.h>

namespace nw{
namespace gfx{

class ParticleContext : public GfxObject{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleContext);

public:
    typedef nw::ut::MoveArray<nw::math::VEC3> VEC3Array;
    typedef nw::ut::MoveArray<f32> F32Array;
    typedef nw::ut::MoveArray<u16> U16Array;

    class Builder{
    public:
        Builder(): 
            mIsFixedSizeMemory(true),
            mMaxEmission(1000),
            mMaxStreamLength(1000),
            mUseDoubleBuffer(false) 
        {}

        Builder& IsFixedSizeMemory(bool isFixedSizeMemory){
            mIsFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        Builder& MaxEmission(int maxEmission) { mMaxEmission = maxEmission; return *this; }

        Builder& MaxStreamLength(int maxStreamLength) { mMaxStreamLength = maxStreamLength; return *this; }

        Builder& UseDoubleBuffer(int useDoubleBuffer) { mUseDoubleBuffer = useDoubleBuffer; return *this; }

        ParticleContext* Create(nw::os::IAllocator* allocator);

    private:
        bool mIsFixedSizeMemory;
        int mMaxEmission;
        int mMaxStreamLength;
        bool mUseDoubleBuffer;
    };

    int GetEmissionWorkCapacity() const{
        return this->mEmissionPositionWork.Capacity();
    }

    VEC3Array::iterator GetEmissionPositionWork(int requireSize = 0){
        if (this->mEmissionPositionWork.GetArrayKind() == nw::ut::ARRAY_VARIABILITY){
            if (this->mEmissionPositionWork.capacity() < requireSize){
                this->mEmissionPositionWork.resize(requireSize);
            }
        }
        return this->mEmissionPositionWork.Begin();
    }

    U16Array::iterator GetEmissionParentWork(int requireSize = 0){
        if (this->mEmissionParentWork.GetArrayKind() == nw::ut::ARRAY_VARIABILITY){
            if (this->mEmissionParentWork.capacity() < requireSize){
                this->mEmissionParentWork.resize(requireSize);
            }
        }
        return this->mEmissionParentWork.Begin();
    }

    F32Array::iterator GetParticleWorkF32(){
        return this->mParticleWorkF32.Begin();
    }

    VEC3Array::iterator GetPrevTranslateWork(int requireSize = 0){
        if (mPrevTranslateWork.GetArrayKind() == nw::ut::ARRAY_VARIABILITY){
            if (this->mPrevTranslateWork.capacity() < requireSize){
                this->mPrevTranslateWork.resize(requireSize);
            }
        }

        return this->mPrevTranslateWork.Begin();
    }

    int GetPrevTranslateWorkCapacity() const{
        return this->mPrevTranslateWork.Capacity();
    }

    void Srand(u32 seed){
        this->mParticleRandom.Srand(seed);
    }

    u16 GetRandom(){
        return this->mParticleRandom.Next(0xffff);
    }

private:
    ParticleContext(nw::os::IAllocator* allocator,VEC3Array emissionPositionWork,U16Array emissionParentWork,
        F32Array particleWorkF32, VEC3Array prevTranslateWork): 
        GfxObject(allocator),
        mEmissionPositionWork(emissionPositionWork),
        mEmissionParentWork(emissionParentWork),
        mParticleWorkF32(particleWorkF32),
        mPrevTranslateWork(prevTranslateWork)
    {}
    virtual ~ParticleContext() {}

    VEC3Array mEmissionPositionWork;
    U16Array mEmissionParentWork;
    F32Array mParticleWorkF32;
    VEC3Array mPrevTranslateWork;

    ParticleRandom mParticleRandom;
};

} // namespace gfx
} // namespace nw