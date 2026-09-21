#pragma once

#include <nw/ut/ut_Inlines.h>
#include <nw/math.h>

namespace nw {
namespace ut {

namespace Endian {
    typedef union
    {
        u64 UInt64;
        s64 SInt64;
#if defined(NW_ENABLE_FLOAT64)
        f64 Float64;
#endif
    } Type64;

    typedef union
    {
        u32 UInt32;
        s32 SInt32;
        f32 Float32;
    } Type32;

    typedef union
    {
        u16 UInt16;
        s16 SInt16;
    } Type16;

    static u64 BSwap(u64 val)
    {
        const u64 MASK  = 0xFF00FF00FF00FF00ULL;
        const u64 MASK2 = 0xFFFF0000FFFF0000ULL;
        val = ((val & MASK) >> 8) | ((val << 8) & MASK);
        val = ((val & MASK2) >> 16) | ((val << 16) & MASK2);
        return (val >> 32) | (val << 32);
    }

    static s64 BSwap(s64 val)
    {
        Type64 data;
        data.SInt64 = val;
        data.UInt64 = BSwap(data.UInt64);
        return data.SInt64;
    }

#if defined(NW_ENABLE_FLOAT64)
    static f64 BSwap(f64 val)
    {
        Type64 data;
        data.Float64 = val;
        data.UInt64  = BSwap(data.UInt64);
        return data.Float64;
    }
#endif

    static u32 BSwap(u32 val)
    {
        const u32 MASK = 0xFF00FF00;
        val = ((val & MASK) >> 8) | ((val << 8) & MASK);
        return (val >> 16) | (val << 16);
    }

    static s32 BSwap(s32 val)
    {
        Type32 data;
        data.SInt32 = val;
        data.UInt32 = BSwap(data.UInt32);
        return data.SInt32;
    }

    static f32 BSwap(f32 val)
    {
        Type32 data;
        data.Float32 = val;
        data.UInt32  = BSwap(data.UInt32);
        return data.Float32;
    }

    static u16 BSwap(u16 val) { return (u16)((val >> 8) | (val << 8)); }

    static s16 BSwap(s16 val) { return (s16)(((u16)val >> 8) | ((u16)val << 8)); }
}

typedef u8 ResU8;
typedef s8 ResS8;
typedef u16 ResU16;
typedef s16 ResS16;
typedef u32 ResU32;
typedef s32 ResS32;
typedef f32 ResF32;
typedef u32 ResU64;
typedef s32 ResS64;
typedef f32 ResF64;

typedef ResU32 Size;
typedef ResU32 Length;
typedef ResU32 ResTypeInfo;

typedef VEC3 ResVec3;
typedef MTX33 ResMtx33;
typedef MTX34 ResMtx34;
typedef MTX44 ResMtx44;

typedef struct BinString{
    ResS32 offset;

    const char* to_ptr() const
    {
        const u8* p = reinterpret_cast<const u8*>(this);
        if (offset != 0) { return reinterpret_cast<const char*>(p + offset); }
        else { return NULL; }
    }

    void set_ptr(const char* ptr)
    {
        if (ptr == NULL) { offset = 0; }
        else { offset = GetOffsetFromPtr(this, ptr); }
    }

    operator const char*() const { return to_ptr(); }
} BinString;

typedef struct Offset{
    ResS32 offset;
    
    static Offset s32_to_ofs(s32 ofs) { return *reinterpret_cast<Offset*>(&ofs); }

    Offset      operator=(s32 ofs) { offset = ofs; return *this; }
    operator s32() const { return offset; }
    Offset      operator+(s32 ofs) const { return  s32_to_ofs(this->offset + ofs); }
    Offset      operator-(s32 ofs) const { return  s32_to_ofs(this->offset - ofs); }
    Offset      operator+=(s32 ofs) { this->offset += ofs; return *this; }
    Offset      operator-=(s32 ofs) { this->offset -= ofs; return *this; }

    void* to_ptr()
    {
        u8* p = reinterpret_cast<u8*>(this);
        if (offset != 0) { return p + offset; }
        else { return NULL; }
    }

    void  set_ptr(const void* ptr)
    {
        if (ptr == NULL) { offset = 0; }
        else { offset = GetOffsetFromPtr(this, ptr); }
    }

    const void* to_ptr() const
    {
        const u8* p = reinterpret_cast<const u8*>(this);
        if (offset != 0) { return p + offset; }
        else { return NULL; }
    }
    void* to_table_ptr()
    {
        return to_ptr();
    }

    template<typename T>
    const T* to_ptr() const
    {
        return static_cast<const T*>( to_ptr() );
    }

    template<typename T>
    T* to_ptr()
    {
        return static_cast<T*>( to_ptr() );
    }

    const void* to_table_ptr() const
    {
        return to_ptr();
    }
} Offset;

typedef ResU32 Size;
typedef ResU32 Length;
typedef ResU32 ResTypeInfo;

typedef struct ResBool
{
    ResS8 value;

    operator bool() const { return (value != 0) ? true : false; }
    bool operator =(bool rhs) { value = rhs; return bool(*this); }
} ResBool;

} // namespace ut
} // namespace nw