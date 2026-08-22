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

#ifndef NW_GFX_PARTICLECONTEXT_H_
#define NW_GFX_PARTICLECONTEXT_H_

#include <nw/math.h>
#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_MoveArray.h>
#include <nw/gfx/gfx_GfxObject.h>
#include <nw/gfx/gfx_ParticleRandom.h>

#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_suppress 1301 // padding inserted in struct.
#pragma diag_suppress 2530 // padding added to end of struct.
#endif
namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleContext : public GfxObject
{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(ParticleContext);

public:
    //
    typedef ut::MoveArray<nw::math::VEC3> VEC3Array;

    //
    typedef ut::MoveArray<f32> F32Array;

    //
    typedef ut::MoveArray<u16> U16Array;

    //----------------------------------------
    //
    //

    //
    //
    //
    class Builder
    {
    public:
        //
        Builder()
        : m_IsFixedSizeMemory(true),
          m_MaxEmission(1000),
          m_MaxStreamLength(1000),
          m_UseDoubleBuffer(false) {}

        //
        //
        //
        //
        //
        Builder& IsFixedSizeMemory(bool isFixedSizeMemory)
        {
            m_IsFixedSizeMemory = isFixedSizeMemory;
            return *this;
        }

        //
        //
        //
        Builder& MaxEmission(int maxEmission) { m_MaxEmission = maxEmission; return *this; }

        //
        //
        //
        Builder& MaxStreamLength(int maxStreamLength) { m_MaxStreamLength = maxStreamLength; return *this; }

        //
        //
        //
        //
        Builder& UseDoubleBuffer(int useDoubleBuffer) { m_UseDoubleBuffer = useDoubleBuffer; return *this; }

        //
        //
        //
        ParticleContext* Create(os::IAllocator* allocator);

    private:
        bool m_IsFixedSizeMemory;
        int m_MaxEmission;
        int m_MaxStreamLength;
        bool m_UseDoubleBuffer;
    };

    //

    //----------------------------------------
    //
    //

    //
    //
    int GetEmissionWorkCapacity() const
    {
        return m_EmissionPositionWork.Capacity();
    }

    //
    //
    //
    //
    VEC3Array::iterator GetEmissionPositionWork(int requireSize = 0)
    {
#ifdef NW_MOVE_ARRAY_VARIABILITY_ENABLED
        if (m_EmissionPositionWork.GetArrayKind() == ut::ARRAY_VARIABILITY)
        {
            if (m_EmissionPositionWork.capacity() < requireSize)
            {
                m_EmissionPositionWork.resize(requireSize);
            }
        }
#endif

        return m_EmissionPositionWork.Begin();
    }

    //
    //
    //
    //
    U16Array::iterator GetEmissionParentWork(int requireSize = 0)
    {
#ifdef NW_MOVE_ARRAY_VARIABILITY_ENABLED
        if (m_EmissionParentWork.GetArrayKind() == ut::ARRAY_VARIABILITY)
        {
            if (m_EmissionParentWork.capacity() < requireSize)
            {
                m_EmissionParentWork.resize(requireSize);
            }
        }
#endif

        return m_EmissionParentWork.Begin();
    }

    //
    //
    F32Array::iterator GetParticleWorkF32()
    {
        return m_ParticleWorkF32.Begin();
    }

    //
    //
    //
    //
    VEC3Array::iterator GetPrevTranslateWork(int requireSize = 0)
    {
#ifdef NW_MOVE_ARRAY_VARIABILITY_ENABLED
        if (m_PrevTranslateWork.GetArrayKind() == ut::ARRAY_VARIABILITY)
        {
            if (m_PrevTranslateWork.capacity() < requireSize)
            {
                m_PrevTranslateWork.resize(requireSize);
            }
        }
#endif

        return m_PrevTranslateWork.Begin();
    }

    //
    //
    //
    int GetPrevTranslateWorkCapacity() const
    {
        return m_PrevTranslateWork.Capacity();
    }

    //
    //
    //
    void Srand(u32 seed)
    {
        m_ParticleRandom.Srand(seed);
    }

    //
    u16 GetRandom()
    {
        return m_ParticleRandom.Next(0xffff);
    }

    //

private:
    ParticleContext(
        os::IAllocator* allocator,
        VEC3Array emissionPositionWork,
        U16Array emissionParentWork,
        F32Array particleWorkF32,
        VEC3Array prevTranslateWork)
    : GfxObject(allocator),
      m_EmissionPositionWork(emissionPositionWork),
      m_EmissionParentWork(emissionParentWork),
      m_ParticleWorkF32(particleWorkF32),
      m_PrevTranslateWork(prevTranslateWork)
    {}
    virtual ~ParticleContext() {}

    VEC3Array m_EmissionPositionWork;
    U16Array m_EmissionParentWork;
    F32Array m_ParticleWorkF32;
    VEC3Array m_PrevTranslateWork;

    ParticleRandom m_ParticleRandom;
};

} // namespace gfx
} // namespace nw
#if NN_CURRENT_VERSION_NUMBER >= NN_VERSION_NUMBER(4,0,0,0)
#pragma diag_default 2530 // padding added to end of struct.
#pragma diag_default 1301 // padding inserted in struct.
#endif

#endif // NW_GFX_PARTICLECONTEXT_H_
