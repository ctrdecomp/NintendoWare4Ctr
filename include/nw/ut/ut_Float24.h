#pragma once

#include <nw/types.h>

namespace nw{
namespace ut{

class Float32{
public:
    static u32 Float32ToBits32(f32 value){
        return *reinterpret_cast<u32*>(&value);
    }

    static f32 Bits32ToFloat32(u32 value){
        return *reinterpret_cast<f32*>(&value);
    }
};

class Float24{
public:
    Float24(): 
        mFloat32(0.0f) 
    {}
    Float24( u32 bits24 ) {
        mFloat32 = Bits24ToFloat32( bits24 );
    }
    Float24(f32 value): 
        mFloat32(value) 
    {}
    
    f32     GetFloat32Value() const { return mFloat32; }
    u32     GetFloat24Value() const { return Float32ToBits24( mFloat32 ); }
    
    Float24& operator =(f32 value) { this->mFloat32 = value; return *this; }
    Float24& operator =(u32 bits24) { this->mFloat32 = Bits24ToFloat32( bits24 ); return *this; }
    
    operator f32() const { return mFloat32; }
    
    f32 operator +(f32 right) const { return this->mFloat32 + right; }
    f32 operator -(f32 right) const { return this->mFloat32 - right; }
    f32 operator *(f32 right) const { return this->mFloat32 * right; }
    f32 operator /(f32 right) const { return this->mFloat32 / right; }
    
    Float24& operator +=(f32 rhs) { this->mFloat32 += rhs; return *this; }
    Float24& operator -=(f32 rhs) { this->mFloat32 -= rhs; return *this; }
    Float24& operator *=(f32 rhs) { this->mFloat32 *= rhs; return *this; }
    Float24& operator /=(f32 rhs) { this->mFloat32 /= rhs; return *this; }
    
    bool operator ==(f32 rhs) const { return (rhs == this->mFloat32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    static f32 Bits24ToFloat32(u32 bits24){
        u32 sign = bits24 & SIGN24;
        int exp = (int)((bits24 & EXP_MASK24) >> FRACTION_WIDTH24);
        u32 fraction = bits24 & FRACTION_MASK24;
        
        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;
        
        if ((bits24 & ~SIGN24) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS24 + EXP_BIAS32;
        }
        
        fraction = fraction << (FRACTION_WIDTH32 - FRACTION_WIDTH24);

        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;
        
        return *reinterpret_cast<f32*>(&bits32);
    }

    static u32 Float32ToBits24(f32 value){
        u32 bits32 = *reinterpret_cast<u32*>(&value);
        
        u32 sign = bits32 & SIGN32;
        int exp = (int)((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;
        
        u32 bits24 = 0;
        bits24 |= (sign != 0) ? SIGN24 : 0;
        
        if ((bits32 & ~SIGN32) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS32 + EXP_BIAS24;
        }
        
        fraction = fraction >> (FRACTION_WIDTH32 - FRACTION_WIDTH24);
        
        if (exp < 0){ }
        else if (exp > 127){
            bits24 = (u32)0x7F << FRACTION_WIDTH24;
        }
        else{
            bits24 |= fraction & FRACTION_MASK24;
            bits24 |= ((u32)exp & 0x7F) << FRACTION_WIDTH24;
        }
        
        return bits24;
    }
    
private:
    f32 mFloat32;
    
    enum{
        SIGN32 = 0x80000000,
        SIGN24 = 0x00800000,
        
        EXP_BIAS32 = 127,
        EXP_BIAS24 = 63,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK24 = 0x007F0000,
        
        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH24 = 16,
        FRACTION_MASK24  = 0x0000FFFF
    };
};

class Float31{
public:
    Float31(): 
        mFloat32(0.0f) 
    {}

    Float31(u32 bits31){
        mFloat32 = Bits31ToFloat32(bits31);
    }

    Float31(f32 value): 
        mFloat32(value) 
    {}
    
    f32     GetFloat32Value() const { return mFloat32; }
    u32     GetFloat31Value() const { return Float32ToBits31( mFloat32 ); }
    
    Float31& operator =(f32 value) { this->mFloat32 = value; return *this; }
    Float31& operator =(u32 bits31) { this->mFloat32 = Bits31ToFloat32( bits31 ); return *this; }
    
    operator f32() const { return mFloat32; }
    
    f32 operator +(f32 right) const { return this->mFloat32 + right; }
    f32 operator -(f32 right) const { return this->mFloat32 - right; }
    f32 operator *(f32 right) const { return this->mFloat32 * right; }
    f32 operator /(f32 right) const { return this->mFloat32 / right; }
    
    Float31& operator +=(f32 rhs) { this->mFloat32 += rhs; return *this; }
    Float31& operator -=(f32 rhs) { this->mFloat32 -= rhs; return *this; }
    Float31& operator *=(f32 rhs) { this->mFloat32 *= rhs; return *this; }
    Float31& operator /=(f32 rhs) { this->mFloat32 /= rhs; return *this; }
    
    bool operator ==(f32 rhs) const { return (rhs == this->mFloat32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    static f32 Bits31ToFloat32(u32 bits31){
        u32 sign = bits31 & SIGN31;
        int exp = (int)((bits31 & EXP_MASK31) >> FRACTION_WIDTH31);
        u32 fraction = bits31 & FRACTION_MASK31;
        
        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;
        
        if ((bits31 & ~SIGN31) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS31 + EXP_BIAS32;
        }

        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;
        
        return *reinterpret_cast<f32*>(&bits32);
    }

    static u32 Float32ToBits31(f32 value){
        u32 bits32 = *reinterpret_cast<u32*>(&value);
        
        u32 sign = bits32 & SIGN32;
        int exp = (int)((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;
        
        u32 bits31 = 0;
        bits31 |= (sign != 0) ? SIGN31 : 0;
        
        if ((bits32 & ~SIGN32) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS32 + EXP_BIAS31;
        }
        
        if (exp < 0){ }
        else if (exp > 127){
            bits31 = (u32)0x7F << FRACTION_WIDTH31;
        }
        else{
            bits31 |= fraction & FRACTION_MASK31;
            bits31 |= ((u32)exp & 0x7F) << FRACTION_WIDTH31;
        }
        
        return bits31;
    }
    
private:
    f32 mFloat32;
    
    enum{
        SIGN32 = 0x80000000,
        SIGN31 = 0x40000000,
        
        EXP_BIAS32 = 127,
        EXP_BIAS31 = 63,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK31 = 0x3F800000,
        
        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH31 = 23,
        FRACTION_MASK31  = 0x007FFFFF
    };
};

class Float20{
public:
    Float20(): 
        mFloat32(0.0f) 
    {}
    Float20(u32 bits20) {
        mFloat32 = Bits20ToFloat32(bits20);
    }
    Float20(f32 value): 
        mFloat32(value) 
    {}
    
    f32     GetFloat32Value() const { return mFloat32; }
    u32     GetFloat20Value() const { return Float32ToBits20(mFloat32); }
    
    Float20& operator =(f32 value) { this->mFloat32 = value; return *this; }
    Float20& operator =(u32 bits20) { this->mFloat32 = Bits20ToFloat32( bits20 ); return *this; }
    
    operator f32() const { return mFloat32; }
    
    f32 operator +(f32 right) const { return this->mFloat32 + right; }
    f32 operator -(f32 right) const { return this->mFloat32 - right; }
    f32 operator *(f32 right) const { return this->mFloat32 * right; }
    f32 operator /(f32 right) const { return this->mFloat32 / right; }
    
    Float20& operator +=(f32 rhs) { this->mFloat32 += rhs; return *this; }
    Float20& operator -=(f32 rhs) { this->mFloat32 -= rhs; return *this; }
    Float20& operator *=(f32 rhs) { this->mFloat32 *= rhs; return *this; }
    Float20& operator /=(f32 rhs) { this->mFloat32 /= rhs; return *this; }
    
    bool operator ==(f32 rhs) const { return (rhs == this->mFloat32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    static f32 Bits20ToFloat32(u32 bits20){
        u32 sign = bits20 & SIGN20;
        int exp = (int)((bits20 & EXP_MASK20) >> FRACTION_WIDTH20);
        u32 fraction = bits20 & FRACTION_MASK20;
        
        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;
        
        if ((bits20 & ~SIGN20) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS20 + EXP_BIAS32;
        }
        
        fraction = fraction << (FRACTION_WIDTH32 - FRACTION_WIDTH20);

        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;
        
        return *reinterpret_cast<f32*>(&bits32);
    }

    static u32 Float32ToBits20(f32 value){
        u32 bits32 = *reinterpret_cast<u32*>(&value);
        
        u32 sign = bits32 & SIGN32;
        int exp = (int)((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;
        
        u32 bits20 = 0;
        bits20 |= (sign != 0) ? SIGN20 : 0;
        
        if ((bits32 & ~SIGN32) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS32 + EXP_BIAS20;
        }
        
        fraction = fraction >> (FRACTION_WIDTH32 - FRACTION_WIDTH20);
        
        if (exp < 0){ }
        else if (exp > 127){
            bits20 = (u32)0x7F << FRACTION_WIDTH20;
        }
        else{
            bits20 |= fraction & FRACTION_MASK20;
            bits20 |= ((u32)exp & 0x7F) << FRACTION_WIDTH20;
        }
        
        return bits20;
    }
    
private:
    f32 mFloat32;
    
    enum{
        SIGN32 = 0x80000000,
        SIGN20 = 0x00080000,
        
        EXP_BIAS32 = 127,
        EXP_BIAS20 = 63,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK20 = 0x0007F000,
        
        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH20 = 12,
        FRACTION_MASK20  = 0x00000FFF
    };
};

class Float16{
public:
    Float16(): 
        mFloat32(0.0f) 
    {}

    Float16(u32 bits16) {
        mFloat32 = Bits16ToFloat32(bits16);
    }

    Float16(f32 value): 
        mFloat32(value) 
    {}
    
    f32     GetFloat32Value() const { return mFloat32; }
    u16     GetFloat16Value() const { return Float32ToBits16( mFloat32 ); }
    
    Float16& operator =(f32 value) { this->mFloat32 = value; return *this; }
    Float16& operator =(u32 bits16) { this->mFloat32 = Bits16ToFloat32( bits16 ); return *this; }
    
    operator f32() const { return mFloat32; }
    operator u16() const { return GetFloat16Value(); }
    
    f32 operator +(f32 right) const { return this->mFloat32 + right; }
    f32 operator -(f32 right) const { return this->mFloat32 - right; }
    f32 operator *(f32 right) const { return this->mFloat32 * right; }
    f32 operator /(f32 right) const { return this->mFloat32 / right; }
    
    Float16& operator +=(f32 rhs) { this->mFloat32 += rhs; return *this; }
    Float16& operator -=(f32 rhs) { this->mFloat32 -= rhs; return *this; }
    Float16& operator *=(f32 rhs) { this->mFloat32 *= rhs; return *this; }
    Float16& operator /=(f32 rhs) { this->mFloat32 /= rhs; return *this; }
    
    bool operator ==(f32 rhs) const { return (rhs == this->mFloat32); }
    bool operator !=(f32 rhs) const { return !(*this == rhs); }

    static f32 Bits16ToFloat32(u32 bits16){
        u32 sign = bits16 & SIGN16;
        int exp = (int)((bits16 & EXP_MASK16) >> FRACTION_WIDTH16);
        u32 fraction = bits16 & FRACTION_MASK16;
        
        u32 bits32 = 0;
        bits32 |= (sign != 0) ? SIGN32 : 0;
        
        if ((bits16 & ~SIGN16) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS16 + EXP_BIAS32;
        }
        
        fraction = fraction << (FRACTION_WIDTH32 - FRACTION_WIDTH16);

        bits32 |= fraction & FRACTION_MASK32;
        bits32 |= ((u32)exp & 0xFF) << FRACTION_WIDTH32;
        
        return *reinterpret_cast<f32*>(&bits32);
    }

    static u16 Float32ToBits16(f32 value){
        u32 bits32 = *reinterpret_cast<u32*>(&value);
        
        u32 sign = bits32 & SIGN32;
        int exp = (int)((bits32 & EXP_MASK32) >> FRACTION_WIDTH32);
        u32 fraction = bits32 & FRACTION_MASK32;
        
        u32 bits16 = 0;
        bits16 |= (sign != 0) ? SIGN16 : 0;
        
        if ((bits32 & ~SIGN32) == 0){
            exp = 0;
        }
        else{
            exp = exp - EXP_BIAS32 + EXP_BIAS16;
        }
        
        fraction = fraction >> (FRACTION_WIDTH32 - FRACTION_WIDTH16);
        
        if (exp < 0){}
        else if (exp > 31){
            bits16 = (u32)0x1F << FRACTION_WIDTH16;
        }
        else{
            bits16 |= fraction & FRACTION_MASK16;
            bits16 |= ((u32)exp & 0x1F) << FRACTION_WIDTH16;
        }
        
        return static_cast<u16>(bits16);
    }
    
private:
    f32 mFloat32;
    
    enum{
        SIGN32 = 0x80000000,
        SIGN16 = 0x00008000,
        
        EXP_BIAS32 = 127,
        EXP_BIAS16 = 15,
        EXP_MASK32 = 0x7F800000,
        EXP_MASK16 = 0x00007C00,
        
        FRACTION_WIDTH32 = 23,
        FRACTION_MASK32  = 0x007FFFFF,
        FRACTION_WIDTH16 = 10,
        FRACTION_MASK16  = 0x000003FF
    };
};

class Fixed13{
public:
    Fixed13(): 
        mFloat32(0.0f) 
    {}

    explicit Fixed13(u32 fixed13) {
        mFloat32 = Fixed13ToFloat32(fixed13);
    }

    explicit Fixed13(f32 fvalue) {
        mFloat32 = fvalue;
    }

    
    f32     GetFloat32Value() const { return mFloat32; }
    u16     GetFixed13Value() const { return Float32ToFixed13( mFloat32 ); }

    static f32 Fixed13ToFloat32(u32 fixed13){
        f32 float32 = static_cast<f32>(fixed13);
        
        if (fixed13 & (0x1 << (TOTAL_WIDTH - 1))){
            float32 -= (0x1 << TOTAL_WIDTH);
        }
        
        return float32 / (0x1 << DECIMAL_WIDTH);
    }

    static u16 Float32ToFixed13(f32 value){
        f32 fixed = value;
        fixed += ((0x1 << INT_WIDTH) / 2);
        fixed *= (0x1 << DECIMAL_WIDTH);
        
        if (fixed < 0){ 
            fixed = 0.0f; 
        }
        else if (fixed >= (0x1 << TOTAL_WIDTH)){
            fixed = (0x1 << TOTAL_WIDTH) - 1;
        }
        
        fixed -= 0x1 << (TOTAL_WIDTH - 1);
        
        return static_cast<u16>(static_cast<s16>(fixed) & MASK);
    }

private:
    f32 mFloat32;
    enum{
        INT_WIDTH = 2,
        DECIMAL_WIDTH = 11,
        TOTAL_WIDTH = 13,
        MASK = (0x1 << TOTAL_WIDTH) - 1
    };
};

class Fixed11{
public:
    Fixed11(): 
        mFloat32(0.0f) 
    {}

    explicit Fixed11( u32 fixed11 ) {
        mFloat32 = Fixed11ToFloat32( fixed11 );
    }

    explicit Fixed11( f32 fvalue ) {
        mFloat32 = fvalue;
    }

    
    f32     GetFloat32Value() const { return mFloat32; }
    u16     GetFixed11Value() const { return Float32ToFixed11(this->mFloat32); }

    static f32 Fixed11ToFloat32(u32 fixed11){
        f32 float32 = static_cast<f32>(fixed11);
        return float32 / MASK;
    }
    
    static u16 Float32ToFixed11(f32 value){
        u32 fixed;
        u32 v_ = *(u32*)&value;

        if (value <= 0 || (v_ & 0x7f800000) == 0x7f800000){
            fixed = 0;
        }
        else{ 
            value *= 1 << (DECIMAL_WIDTH - 0);
            if (value >= (1 << DECIMAL_WIDTH)){
                fixed = (1 << DECIMAL_WIDTH) - 1;
            }
            else{
                fixed = (unsigned)(value);
            }
        }
        
        return static_cast<u16>(static_cast<s16>(fixed) & MASK);
    }

private:
    f32 mFloat32;

    enum{
        INT_WIDTH = 2,
        DECIMAL_WIDTH = 11,
        TOTAL_WIDTH = 11,
        MASK = (0x1 << TOTAL_WIDTH) - 1
    };
};

class FixedU24{
public:
    FixedU24(): 
        mFloat32( 0.0f ) 
    {}
    explicit FixedU24(u32 fixedU24) {
        mFloat32 = FixedU24ToFloat32(fixedU24);
    }
    explicit FixedU24(f32 fvalue) {
        mFloat32 = fvalue;
    }
    
    f32     GetFloat32Value() const { return mFloat32; }
    u32     GetFixedU24Value() const { return Float32ToFixedU24( mFloat32 ); }

    static f32 FixedU24ToFloat32(u32 fixedU24){
        f32 float32 = static_cast<f32>(fixedU24);
        return float32 / MASK;
    }
    
    static u32 Float32ToFixedU24(f32 value){
        u32 fixed;
        u32 v_ = *(u32*)&value;

        if (value <= 0 || (v_ & 0x7f800000) == 0x7f800000){
            fixed = 0;
        }
        else{ 
            value *= 1 << (DECIMAL_WIDTH - 0);
            if (value >= (1 << DECIMAL_WIDTH)){
                fixed = (1 << DECIMAL_WIDTH) - 1;
            }
            else{
                fixed = (unsigned)(value);
            }
        }
        
        return static_cast<u32>(static_cast<s32>(fixed) & MASK);
    }

private:
    f32 mFloat32;

    enum{
        INT_WIDTH = 2,
        DECIMAL_WIDTH = 24,
        TOTAL_WIDTH = 24,
        MASK = (0x1 << TOTAL_WIDTH) - 1
    };
};
}
}