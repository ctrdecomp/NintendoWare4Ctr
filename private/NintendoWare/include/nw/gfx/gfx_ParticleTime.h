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

#ifndef NW_GFX_PARTICLETIME_H_
#define NW_GFX_PARTICLETIME_H_

#include <nw/math.h>
#include <nw/gfx/gfx_Common.h>

// When this definition is enabled, particle time management becomes f32.
//#define NW_GFX_NO_USE_PARTICLETIME

namespace nw
{
namespace gfx
{

#ifdef NW_GFX_PARTICLE_COMPAT_1_1

//
typedef f32 ParticleTime;

#else

#ifndef NW_GFX_NO_USE_PARTICLETIME
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleTime
{
    friend ParticleTime operator -(const ParticleTime& rhs);

public:
    //
    ParticleTime() : m_ParticleTime(0) {}

    //
    //
    //
    ParticleTime(const ParticleTime& value)
    {
        m_ParticleTime = value.m_ParticleTime;
    }

    //
    //
    //
    ParticleTime(s32 value)
    {
        m_ParticleTime = S32ToParticleTime(value);
    }

    //
    //
    //
    ParticleTime(f32 value)
    {
        m_ParticleTime = Float32ToParticleTime(value);
    }

    //
    //
    f32     GetFloat32Value() const { return ParticleTimeToFloat32(m_ParticleTime); }

    //
    //
    s32     GetS32Value() const { return m_ParticleTime / 0x100; }

    //
    //
    //
    s32     GetParticleTimeValue() const { return m_ParticleTime; }

    ParticleTime operator +(ParticleTime right) const
    {
        ParticleTime result;
        result.m_ParticleTime = this->m_ParticleTime + right.m_ParticleTime;
        return result;
    }

    ParticleTime operator -(ParticleTime right) const
    {
        ParticleTime result;
        result.m_ParticleTime = this->m_ParticleTime - right.m_ParticleTime;
        return result;
    }

    ParticleTime operator *(ParticleTime right) const
    {
        ParticleTime result;
        result.m_ParticleTime = (this->m_ParticleTime / right.m_ParticleTime) / 0x100;
        return result;
    }

    ParticleTime operator /(ParticleTime right) const
    {
        ParticleTime result;
        result.m_ParticleTime = 0x100 * this->m_ParticleTime / right.m_ParticleTime;
        return result;
    }

    //
    //
    s32 GetIntegralParts() const
    {
        return m_ParticleTime / 0x100;
    }

    //
    //
    f32 GetFractionalParts() const
    {
        return (m_ParticleTime & 0xff) / 0x100;
    }

    //
    //
    s32 Floor() const
    {
        return m_ParticleTime / 0x100;
    }

    //
    //
    s32 Ceil() const
    {
        return (m_ParticleTime + 0xff) / 0x100;
    }

    //
    //
    //
    ParticleTime& FMod(s32 value)
    {
        this->m_ParticleTime %= S32ToParticleTime(value);
        return *this;
    }

    ParticleTime& operator =(f32 value) { this->m_ParticleTime = Float32ToParticleTime(value); return *this; }
    ParticleTime& operator =(s32 value) { this->m_ParticleTime = S32ToParticleTime(value); return *this; }
    ParticleTime& operator =(ParticleTime value) { this->m_ParticleTime = value.m_ParticleTime; return *this; }
    ParticleTime& operator +=(f32 value) { this->m_ParticleTime += Float32ToParticleTime(value); return *this; }
    ParticleTime& operator +=(s32 value) { this->m_ParticleTime += S32ToParticleTime(value); return *this; }
    ParticleTime& operator +=(ParticleTime value) { this->m_ParticleTime += value.m_ParticleTime; return *this; }
    ParticleTime& operator -=(f32 value) { this->m_ParticleTime -= Float32ToParticleTime(value); return *this; }
    ParticleTime& operator -=(s32 value) { this->m_ParticleTime -= S32ToParticleTime(value); return *this; }
    ParticleTime& operator -=(ParticleTime value) { this->m_ParticleTime -= value.m_ParticleTime; return *this; }

    bool operator ==(const ParticleTime& rhs) const { return this->m_ParticleTime == rhs.m_ParticleTime; }
    bool operator !=(const ParticleTime& rhs) const { return this->m_ParticleTime == rhs.m_ParticleTime; }
    bool operator >=(const ParticleTime& rhs) const { return this->m_ParticleTime >= rhs.m_ParticleTime; }
    bool operator <=(const ParticleTime& rhs) const { return this->m_ParticleTime <= rhs.m_ParticleTime; }
    bool operator >(const ParticleTime& rhs) const { return this->m_ParticleTime > rhs.m_ParticleTime; }
    bool operator <(const ParticleTime& rhs) const { return this->m_ParticleTime < rhs.m_ParticleTime; }

    bool operator ==(s32 rhs) const { return this->m_ParticleTime == S32ToParticleTime(rhs); }
    bool operator !=(s32 rhs) const { return this->m_ParticleTime == S32ToParticleTime(rhs); }
    bool operator >=(s32 rhs) const { return this->m_ParticleTime >= S32ToParticleTime(rhs); }
    bool operator <=(s32 rhs) const { return this->m_ParticleTime <= S32ToParticleTime(rhs); }
    bool operator >(s32 rhs) const { return this->m_ParticleTime > S32ToParticleTime(rhs); }
    bool operator <(s32 rhs) const { return this->m_ParticleTime < S32ToParticleTime(rhs); }

    // When we implicitly convert to f32, it gets slower, so no conversion to f32 is prepared

    //
    //
    //
    ParticleTime Interp(const ParticleTime& length, s32 factor) const
    {
        s32 work = this->m_ParticleTime * factor * 0x100;
        work /= length.m_ParticleTime;

        ParticleTime result;
        result.m_ParticleTime = work;
        return result;
    }

    //
    //
    //
    static f32 ParticleTimeToFloat32(s32 particleTime)
    {
        f32 float32 = static_cast<f32>(particleTime);
        return float32 / 0x100;
    }

    //
    //
    //
    static s32 Float32ToParticleTime(f32 value)
    {
        s32 fixed = (int)(value * 0x100);
        return fixed;
    }

    //
    //
    //
    static s32 S32ToParticleTime(s32 value)
    {
        return value * 0x100;
    }

private:
    s32 m_ParticleTime;
};

NW_FORCE_INLINE ParticleTime operator -(const ParticleTime& rhs)
{
    ParticleTime result;
    result.m_ParticleTime = -rhs.m_ParticleTime;
    return result;
}

#else // Made compatible with f32

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ParticleTime
{
    friend ParticleTime operator -(const ParticleTime& rhs);

public:
    //
    ParticleTime() : m_ParticleTime(0) {}

    //
    //
    ParticleTime(const ParticleTime& value)
    {
        m_ParticleTime = value.m_ParticleTime;
    }

    //
    //
    ParticleTime(s32 ivalue)
    {
        m_ParticleTime = ivalue;
    }

    //
    //
    ParticleTime(f32 fvalue)
    {
        m_ParticleTime = Float32ToParticleTime(fvalue);
    }

    //
    //
    f32     GetFloat32Value() const { return m_ParticleTime; }

    //
    //
    s32     GetS32Value() const { return m_ParticleTime; }

    //
    //
    //
    f32     GetParticleTimeValue() const { return m_ParticleTime; }

    ParticleTime operator +(ParticleTime right) const
    {
        return ParticleTime(this->m_ParticleTime + right.m_ParticleTime);
    }

    ParticleTime operator -(ParticleTime right) const
    {
        return ParticleTime(this->m_ParticleTime - right.m_ParticleTime);
    }

    ParticleTime operator *(ParticleTime right) const
    {
        return ParticleTime(this->m_ParticleTime * right.m_ParticleTime);
    }

    ParticleTime operator /(ParticleTime right) const
    {
        return ParticleTime(this->m_ParticleTime / right.m_ParticleTime);
    }

    //
    //
    s32 GetIntegralParts() const
    {
        return (int)m_ParticleTime;
    }

    //
    //
    f32 GetFractionalParts() const
    {
        return m_ParticleTime - (int)m_ParticleTime;
    }

    //
    //
    s32 Floor() const
    {
        return math::FFloor(m_ParticleTime);
    }

    //
    //
    s32 Ceil() const
    {
        return math::FCeil(m_ParticleTime);
    }

    //
    //
    //
    ParticleTime& FMod(s32 value)
    {
        this->m_ParticleTime = nw::math::FMod(this->m_ParticleTime, static_cast<f32>(value));
        return *this;
    }

    ParticleTime& operator =(f32 value) { this->m_ParticleTime = Float32ToParticleTime(value); return *this; }
    ParticleTime& operator =(s32 value) { this->m_ParticleTime = value; return *this; }
    ParticleTime& operator =(ParticleTime value) { this->m_ParticleTime = value.m_ParticleTime; return *this; }
    ParticleTime& operator +=(f32 value) { this->m_ParticleTime += value; return *this; }
    ParticleTime& operator +=(s32 value) { this->m_ParticleTime += value; return *this; }
    ParticleTime& operator +=(ParticleTime value) { this->m_ParticleTime += value.m_ParticleTime; return *this; }
    ParticleTime& operator -=(f32 value) { this->m_ParticleTime -= value; return *this; }
    ParticleTime& operator -=(s32 value) { this->m_ParticleTime -= value; return *this; }
    ParticleTime& operator -=(ParticleTime value) { this->m_ParticleTime -= value.m_ParticleTime; return *this; }

    bool operator ==(const ParticleTime& rhs) const { return this->m_ParticleTime == rhs.m_ParticleTime; }
    bool operator !=(const ParticleTime& rhs) const { return this->m_ParticleTime == rhs.m_ParticleTime; }
    bool operator >=(const ParticleTime& rhs) const { return this->m_ParticleTime >= rhs.m_ParticleTime; }
    bool operator <=(const ParticleTime& rhs) const { return this->m_ParticleTime <= rhs.m_ParticleTime; }
    bool operator >(const ParticleTime& rhs) const { return this->m_ParticleTime > rhs.m_ParticleTime; }
    bool operator <(const ParticleTime& rhs) const { return this->m_ParticleTime < rhs.m_ParticleTime; }

    bool operator ==(s32 rhs) const { return this->m_ParticleTime == rhs; }
    bool operator !=(s32 rhs) const { return this->m_ParticleTime == rhs; }
    bool operator >=(s32 rhs) const { return this->m_ParticleTime >= rhs; }
    bool operator <=(s32 rhs) const { return this->m_ParticleTime <= rhs; }
    bool operator >(s32 rhs) const { return this->m_ParticleTime > rhs; }
    bool operator <(s32 rhs) const { return this->m_ParticleTime < rhs; }

    // When we implicitly convert to f32, it gets slower, so no conversion to f32 is prepared

    ParticleTime Interp(const ParticleTime& length, s32 factor) const
    {
        return ParticleTime(this->GetFloat32Value() / length.GetFloat32Value() * factor);
    }

    //
    //
    //
    static f32 ParticleTimeToFloat32(f32 particleTime)
    {
        return particleTime;
    }

    //
    //
    //
    static f32 Float32ToParticleTime(f32 value)
    {
        return value;
    }

private:
    f32 m_ParticleTime;
};

NW_FORCE_INLINE ParticleTime operator -(const ParticleTime& rhs)
{
    return ParticleTime(-rhs.m_ParticleTime);
}


#endif
#endif

} // namespace gfx
} // namespace nw

#endif // NW_GFX_PARTICLETIME_H_
