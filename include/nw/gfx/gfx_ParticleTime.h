#pragma once

#include <nw/math/math_Types.h>
#include <nw/gfx/gfx_Common.h>

namespace nw{
namespace gfx{

#ifdef NW_GFX_PARTICLE_COMPAT_1_1

typedef f32 ParticleTime;

#else

#ifndef NW_GFX_NO_USE_PARTICLETIME

class ParticleTime{
    friend ParticleTime operator -(const ParticleTime& rhs);
public:
    ParticleTime(): 
        mParticleTime(0) 
    {}
    ParticleTime(const ParticleTime& value){
        mParticleTime = value.mParticleTime;
    }

    ParticleTime(s32 value){
        mParticleTime = S32ToParticleTime(value);
    }

    ParticleTime(f32 value){
        mParticleTime = Float32ToParticleTime(value);
    }

    f32     GetFloat32Value() const { return ParticleTimeToFloat32(mParticleTime); }
    s32     GetS32Value() const { return mParticleTime / 0x100; }
    s32     GetParticleTimeValue() const { return mParticleTime; }

    ParticleTime operator +(ParticleTime right) const{
        ParticleTime result;
        result.mParticleTime = this->mParticleTime + right.mParticleTime;
        return result;
    }

    ParticleTime operator -(ParticleTime right) const{
        ParticleTime result;
        result.mParticleTime = this->mParticleTime - right.mParticleTime;
        return result;
    }

    ParticleTime operator *(ParticleTime right) const{
        ParticleTime result;
        result.mParticleTime = (this->mParticleTime / right.mParticleTime) / 0x100;
        return result;
    }

    ParticleTime operator /(ParticleTime right) const{
        ParticleTime result;
        result.mParticleTime = 0x100 * this->mParticleTime / right.mParticleTime;
        return result;
    }

    s32 GetIntegralParts() const{
        return mParticleTime / 0x100;
    }

    f32 GetFractionalParts() const{
        return (mParticleTime & 0xff) / 0x100;
    }

    s32 Floor() const{
        return mParticleTime / 0x100;
    }

    s32 Ceil() const{
        return (mParticleTime + 0xff) / 0x100;
    }

    ParticleTime& FMod(s32 value){
        this->mParticleTime %= S32ToParticleTime(value);
        return *this;
    }

    ParticleTime& operator =(f32 value) { this->mParticleTime = Float32ToParticleTime(value); return *this; }
    ParticleTime& operator =(s32 value) { this->mParticleTime = S32ToParticleTime(value); return *this; }
    ParticleTime& operator =(ParticleTime value) { this->mParticleTime = value.mParticleTime; return *this; }
    ParticleTime& operator +=(f32 value) { this->mParticleTime += Float32ToParticleTime(value); return *this; }
    ParticleTime& operator +=(s32 value) { this->mParticleTime += S32ToParticleTime(value); return *this; }
    ParticleTime& operator +=(ParticleTime value) { this->mParticleTime += value.mParticleTime; return *this; }
    ParticleTime& operator -=(f32 value) { this->mParticleTime -= Float32ToParticleTime(value); return *this; }
    ParticleTime& operator -=(s32 value) { this->mParticleTime -= S32ToParticleTime(value); return *this; }
    ParticleTime& operator -=(ParticleTime value) { this->mParticleTime -= value.mParticleTime; return *this; }

    bool operator ==(const ParticleTime& rhs) const { return this->mParticleTime == rhs.mParticleTime; }
    bool operator !=(const ParticleTime& rhs) const { return this->mParticleTime == rhs.mParticleTime; }
    bool operator >=(const ParticleTime& rhs) const { return this->mParticleTime >= rhs.mParticleTime; }
    bool operator <=(const ParticleTime& rhs) const { return this->mParticleTime <= rhs.mParticleTime; }
    bool operator >(const ParticleTime& rhs) const { return this->mParticleTime > rhs.mParticleTime; }
    bool operator <(const ParticleTime& rhs) const { return this->mParticleTime < rhs.mParticleTime; }

    bool operator ==(s32 rhs) const { return this->mParticleTime == S32ToParticleTime(rhs); }
    bool operator !=(s32 rhs) const { return this->mParticleTime == S32ToParticleTime(rhs); }
    bool operator >=(s32 rhs) const { return this->mParticleTime >= S32ToParticleTime(rhs); }
    bool operator <=(s32 rhs) const { return this->mParticleTime <= S32ToParticleTime(rhs); }
    bool operator >(s32 rhs) const { return this->mParticleTime > S32ToParticleTime(rhs); }
    bool operator <(s32 rhs) const { return this->mParticleTime < S32ToParticleTime(rhs); }

    ParticleTime Interp(const ParticleTime& length, s32 factor) const{
        s32 work = this->mParticleTime * factor * 0x100;
        work /= length.mParticleTime;

        ParticleTime result;
        result.mParticleTime = work;
        return result;
    }

    static f32 ParticleTimeToFloat32(s32 particleTime){
        f32 float32 = static_cast<f32>(particleTime);
        return float32 / 0x100;
    }

    static s32 Float32ToParticleTime(f32 value){
        s32 fixed = (int)(value * 0x100);
        return fixed;
    }

    static s32 S32ToParticleTime(s32 value){
        return value * 0x100;
    }

private:
    s32 mParticleTime;
};

NW_FORCE_INLINE ParticleTime operator -(const ParticleTime& rhs){
    ParticleTime result;
    result.mParticleTime = -rhs.mParticleTime;
    return result;
}

#else

class ParticleTime{
    friend ParticleTime operator -(const ParticleTime& rhs);
public:
    ParticleTime(): 
        mParticleTime(0)
    {}

    ParticleTime(const ParticleTime& value){
        mParticleTime = value.mParticleTime;
    }

    ParticleTime(s32 ivalue){
        mParticleTime = ivalue;
    }

    ParticleTime(f32 fvalue){
        mParticleTime = Float32ToParticleTime(fvalue);
    }

    f32     GetFloat32Value() const { return mParticleTime; }
    s32     GetS32Value() const { return mParticleTime; }
    f32     GetParticleTimeValue() const { return mParticleTime; }

    ParticleTime operator +(ParticleTime right) const{
        return ParticleTime(this->mParticleTime + right.mParticleTime);
    }

    ParticleTime operator -(ParticleTime right) const{
        return ParticleTime(this->mParticleTime - right.mParticleTime);
    }

    ParticleTime operator *(ParticleTime right) const{
        return ParticleTime(this->mParticleTime * right.mParticleTime);
    }

    ParticleTime operator /(ParticleTime right) const{
        return ParticleTime(this->mParticleTime / right.mParticleTime);
    }

    s32 GetIntegralParts() const{
        return (int)mParticleTime;
    }

    f32 GetFractionalParts() const{
        return mParticleTime - (int)mParticleTime;
    }

    s32 Floor() const{
        return math::FFloor(mParticleTime);
    }

    s32 Ceil() const{
        return math::FCeil(mParticleTime);
    }

    ParticleTime& FMod(s32 value){
        this->mParticleTime = nw::math::FMod(this->mParticleTime, static_cast<f32>(value));
        return *this;
    }

    ParticleTime& operator =(f32 value) { this->mParticleTime = Float32ToParticleTime(value); return *this; }
    ParticleTime& operator =(s32 value) { this->mParticleTime = value; return *this; }
    ParticleTime& operator =(ParticleTime value) { this->mParticleTime = value.mParticleTime; return *this; }
    ParticleTime& operator +=(f32 value) { this->mParticleTime += value; return *this; }
    ParticleTime& operator +=(s32 value) { this->mParticleTime += value; return *this; }
    ParticleTime& operator +=(ParticleTime value) { this->mParticleTime += value.mParticleTime; return *this; }
    ParticleTime& operator -=(f32 value) { this->mParticleTime -= value; return *this; }
    ParticleTime& operator -=(s32 value) { this->mParticleTime -= value; return *this; }
    ParticleTime& operator -=(ParticleTime value) { this->mParticleTime -= value.mParticleTime; return *this; }

    bool operator ==(const ParticleTime& rhs) const { return this->mParticleTime == rhs.mParticleTime; }
    bool operator !=(const ParticleTime& rhs) const { return this->mParticleTime == rhs.mParticleTime; }
    bool operator >=(const ParticleTime& rhs) const { return this->mParticleTime >= rhs.mParticleTime; }
    bool operator <=(const ParticleTime& rhs) const { return this->mParticleTime <= rhs.mParticleTime; }
    bool operator >(const ParticleTime& rhs) const { return this->mParticleTime > rhs.mParticleTime; }
    bool operator <(const ParticleTime& rhs) const { return this->mParticleTime < rhs.mParticleTime; }

    bool operator ==(s32 rhs) const { return this->mParticleTime == rhs; }
    bool operator !=(s32 rhs) const { return this->mParticleTime == rhs; }
    bool operator >=(s32 rhs) const { return this->mParticleTime >= rhs; }
    bool operator <=(s32 rhs) const { return this->mParticleTime <= rhs; }
    bool operator >(s32 rhs) const { return this->mParticleTime > rhs; }
    bool operator <(s32 rhs) const { return this->mParticleTime < rhs; }

    ParticleTime Interp(const ParticleTime& length, s32 factor) const{
        return ParticleTime(this->GetFloat32Value() / length.GetFloat32Value() * factor);
    }

    static f32 ParticleTimeToFloat32(f32 particleTime){
        return particleTime;
    }

    static f32 Float32ToParticleTime(f32 value){
        return value;
    }

private:
    f32 mParticleTime;
};

NW_FORCE_INLINE ParticleTime operator -(const ParticleTime& rhs){
    return ParticleTime(-rhs.mParticleTime);
}

#endif
#endif

}
}