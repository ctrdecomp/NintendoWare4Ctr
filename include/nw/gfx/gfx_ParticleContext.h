#pragma once

#include <nw/math/math_Types.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ParticleRandom.h>

namespace nw{
namespace gfx{

class ParticleContext : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleContext);

public:
    typedef nw::ut::MoveArray<nw::math::VEC3> VEC3Array;
    typedef nw::ut::MoveArray<f32> F32Array;
    typedef nw::ut::MoveArray<u16> U16Array;

    class Builder
    {
    public:
        Builder(): 
            m_IsFixedSizeMemory(true),
            m_MaxEmission(1000),
            m_MaxStreamLength(1000),
            m_UseDoubleBuffer(false) {}

        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_IsFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        Builder& MaxEmission(int maxEmission) { m_MaxEmission = maxEmission; return *this; }

        Builder& MaxStreamLength(int maxStreamLength) { m_MaxStreamLength = maxStreamLength; return *this; }

        Builder& UseDoubleBuffer(int useDoubleBuffer) { m_UseDoubleBuffer = useDoubleBuffer; return *this; }

        ParticleContext* Create(nw::os::IAllocator* allocator);

    private:
        bool m_IsFixedSizeMemory;
        int m_MaxEmission;
        int m_MaxStreamLength;
        bool m_UseDoubleBuffer;
    };

    int GetEmissionWorkCapacity() const
    {
        return this->m_EmissionPositionWork.Capacity();
    }

    VEC3Array::iterator GetEmissionPositionWork(int requireSize = 0)
    {
        if (this->m_EmissionPositionWork.GetArrayKind() == nw::ut::ARRAY_VARIABILITY)
        {
            if (this->m_EmissionPositionWork.capacity() < requireSize)
            {
                this->m_EmissionPositionWork.resize(requireSize);
            }
        }
        return this->m_EmissionPositionWork.Begin();
    }

    U16Array::iterator GetEmissionParentWork(int requireSize = 0)
    {
        if (this->m_EmissionParentWork.GetArrayKind() == nw::ut::ARRAY_VARIABILITY)
        {
            if (this->m_EmissionParentWork.capacity() < requireSize)
            {
                this->m_EmissionParentWork.resize(requireSize);
            }
        }
        return this->m_EmissionParentWork.Begin();
    }

    F32Array::iterator GetParticleWorkF32()
    {
        return this->m_ParticleWorkF32.Begin();
    }

    VEC3Array::iterator GetPrevTranslateWork(int requireSize = 0)
    {
        if (m_PrevTranslateWork.GetArrayKind() == nw::ut::ARRAY_VARIABILITY)
        {
            if (this->m_PrevTranslateWork.capacity() < requireSize)
            {
                this->m_PrevTranslateWork.resize(requireSize);
            }
        }

        return this->m_PrevTranslateWork.Begin();
    }

    int GetPrevTranslateWorkCapacity() const
    {
        return this->m_PrevTranslateWork.Capacity();
    }

    void Srand(u32 seed)
    {
        this->m_ParticleRandom.Srand(seed);
    }

    u16 GetRandom()
    {
        return this->m_ParticleRandom.Next(0xffff);
    }

private:
    ParticleContext(nw::os::IAllocator* allocator,VEC3Array emissionPositionWork,U16Array emissionParentWork,
        F32Array particleWorkF32, VEC3Array prevTranslateWork): 
        GfxObject(allocator),
        m_EmissionPositionWork(emissionPositionWork),
        m_EmissionParentWork(emissionParentWork),
        m_ParticleWorkF32(particleWorkF32),
        m_PrevTranslateWork(prevTranslateWork) {}
    virtual ~ParticleContext() {}

    VEC3Array m_EmissionPositionWork;
    U16Array m_EmissionParentWork;
    F32Array m_ParticleWorkF32;
    VEC3Array m_PrevTranslateWork;

    ParticleRandom m_ParticleRandom;
};

} // namespace gfx
} // namespace nw