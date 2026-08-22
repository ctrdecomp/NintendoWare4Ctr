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

#ifndef NW_GFX_PARTICLERANDOM_H_
#define NW_GFX_PARTICLERANDOM_H_

#include <nw/types.h>

namespace nw
{
namespace gfx
{

//---------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleRandom
{
public:
    //
    ParticleRandom()
    {
        m_Seed = 0;
    }

    //
    //
    //
    ParticleRandom(const ParticleRandom& source)
    {
        Set(source);
    }

    //
    //
    //
    void Srand(u32 seed)
    {
        m_Seed = seed;
    }

    //
    //
    //
    //
    u16 Next(u16 maxValue)
    {
        MixRandomSeed();
        return (u16)(m_Seed >> 8) % maxValue;
    }

    //
    //
    //
    f32 NextFloat()
    {
        MixRandomSeed();
        return (f32)((m_Seed >> 16) & 0xffff) / 65536.0f;
    }

    //
    //
    //
    f32 NextFloatSignedOne();

    //
    //
    //
    f32 NextFloatSignedHalf();

    //
    //
    //
    void Set(const ParticleRandom& source)
    {
        m_Seed = source.m_Seed;
    }

private:
    //
    void MixRandomSeed()
    {
        m_Seed = (m_Seed * 214013u) + 2531011u;
    }

    u32 m_Seed;
};

} // namespace gfx
} // namespace nw

#endif // NW_GFX_PARTICLERANDOM_H_
