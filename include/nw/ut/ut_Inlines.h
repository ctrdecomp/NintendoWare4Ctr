#pragma once

#include <nw/Assert.h>

#include <cstring>
#include <cstdio>
#include <cstdarg>
#include <limits>
#include <functional>
#include <algorithm>
#include <string>
#include <iosfwd>

namespace nw{
namespace ut{

template <typename T>
inline T Min(T a, T b);

namespace internal{

template <typename T>
inline u32 MakeBits(T value, s32 width, s32 shift)
{
    return (u32(value) & ((0x1U << width) - 1)) << shift;
}

template <typename T>
inline T GetBits(T bits,int pos,int len)
{
    NW_ASSERT(len <= 32);
    const u32 mask = ~(0xFFFFFFFFU << len);

    return T((bits >> pos) & mask);
}

template <typename T>
inline bool TestBit(T bits,int pos)
{
    const T mask = T(1 << pos);

    return 0 != (bits & mask);
}

template<typename TChar>
inline size_t strncpy_t(TChar* dest,std::size_t destCount,const TChar* src,std::size_t srcCount)
{
    size_t length = 0;
    while (length < ut::Min((destCount - 1), srcCount))
    {
        *dest = *src;
        ++dest;
        ++src;
        ++length;
        
        if (*src == std::char_traits<TChar>::to_char_type('\0'))
        {
            break;
        }
    }

    *dest = std::char_traits<TChar>::to_char_type('\0');

    return length;
}

}

inline size_t strcpy(char* dest, std::size_t destCount, const char* src)
{
    return internal::strncpy_t(dest, destCount, src, destCount - 1);
}

NW_INLINE size_t strncpy(char* dest,std::size_t destCount,const char* src,std::size_t srcCount)
{
    return ut::internal::strncpy_t(dest, destCount, src, srcCount);
}

NW_INLINE size_t strncat(
    char* dest, std::size_t destCount,
    const char* src, std::size_t srcCount)
{
    size_t length = std::strlen(dest);
    if (destCount <= length + srcCount)
    {
        srcCount = destCount - length - 1;
    }

    char* result = std::strncat(dest, src, srcCount);

    dest[destCount-1] = '\0';

    return srcCount;
}

template <typename T>
inline T Abs(T a)
{
    return (a < 0) ? static_cast<T>(-a) : a;
}

template<>
inline float Abs<float>(register float a)
{
    return (a < 0) ? -a : a;
}

template<typename ValueT>
class CloseAtToleranceWeak
{
public:
    CloseAtToleranceWeak(ValueT tolerance): \
        m_Tolerance(tolerance) {}

    CloseAtToleranceWeak(int numberOfRoundingError): 
        m_Tolerance(numberOfRoundingError * std::numeric_limits<ValueT>::epsilon() * 0.5f) {}

    bool operator()(ValueT lhs, ValueT rhs) const
        {
        ValueT diff = Abs(lhs - rhs);
        return diff <= (m_Tolerance * Abs(lhs)) || diff <= (m_Tolerance * Abs(rhs));
    }
    
private:
    ValueT m_Tolerance;
};

template<typename ValueT>
inline bool FloatEqualsWeak(ValueT lhs, ValueT rhs)
{
    return CloseAtToleranceWeak<ValueT>(std::numeric_limits<ValueT>::epsilon())(lhs, rhs);
}

template <typename T>
inline T Min(T a, T b)
{
    return (a > b) ? b: a;
}

template <typename T>
inline T Max(T a, T b)
{
    return (a < b) ? b: a;
}

template <typename ValueT> ValueT RoundUp(ValueT x, unsigned int base);

template <typename ValueT>
inline void*RoundUp(ValueT* x, unsigned int base)
{
    IntPtr value = reinterpret_cast<IntPtr>(x);
    IntPtr rounded = (value + (base - 1)) & ~(base - 1);
    
    return reinterpret_cast<void*>(rounded);
}

template <typename ValueT>
inline const void* RoundUp(const ValueT* x, unsigned int base)
{
    IntPtr value = reinterpret_cast<IntPtr>(x);
    IntPtr rounded = (value + (base - 1)) & ~(base - 1);
    
    return reinterpret_cast<const void*>(rounded);
}

template <typename ValueT>
inline ValueT RoundUp(ValueT x, unsigned int base)
{
    return static_cast<ValueT>( (x + (base - 1)) & ~(base - 1) );
}

template <typename ValueT> ValueT RoundDown(ValueT x, unsigned int base);

template <typename ValueT>
inline void* RoundDown(ValueT* x, unsigned int base)
{
    IntPtr value = reinterpret_cast<IntPtr>(x);
    IntPtr rounded = value  & ~(base - 1);

    return reinterpret_cast<void*>(rounded);
}

template <typename ValueT>
inline const void* RoundDown(const ValueT* x, unsigned int base)
{
    IntPtr value = reinterpret_cast<IntPtr>(x);
    IntPtr rounded = value  & ~(base - 1);

    return reinterpret_cast<const void*>(rounded);
}

template <typename ValueT>
inline ValueT RoundDown(ValueT x, unsigned int base)
{
    return static_cast<ValueT>(x & ~(base - 1));
}

template <typename T>
inline T Clamp(T x, T low, T high)
{
    return (x > high) ? high : ( (x < low) ? low : x );
}

template <typename OutputT>
inline OutputT BitExtract(OutputT bits, int pos, int len=1)
{
    const OutputT mask = static_cast<OutputT>((1UL << len) - 1);
    return static_cast<OutputT>((bits >> pos) & mask );
}

inline s32 ReverseEndian(s32 x)
{
    return static_cast<s32>(
        BitExtract( x,  0, 8 ) << 24 |
        BitExtract( x,  8, 8 ) << 16 |
        BitExtract( x, 16, 8 ) <<  8 |
        BitExtract( x, 24, 8 ) <<  0
    );
}

inline u32 ReverseEndian(u32 x)
{
    return static_cast<u32>(
        BitExtract( x,  0, 8 ) << 24 |
        BitExtract( x,  8, 8 ) << 16 |
        BitExtract( x, 16, 8 ) <<  8 |
        BitExtract( x, 24, 8 ) <<  0
    );
}

inline u16 ReverseEndian(u16 x)
{
    return static_cast<u16>(
        BitExtract( x,  0, 8 ) << 8 |
        BitExtract( x,  8, 8 ) << 0
    );
}

template<typename TObject>
inline void SafeDestroy(TObject*& object)
{
    if (object == NULL) { return; }
    object->Destroy();
    object = NULL;
}

template<typename TObject> 
struct SafeDestroyer : public std::unary_function<TObject&, void>
{
    void operator()(TObject& object) const
    {
        SafeDestroy(object);
    }
};

template<typename TArray>
inline void SafeDestroyAll(TArray& array)
{
    std::for_each(array.begin(), array.end(), SafeDestroyer<typename TArray::value_type>());
    array.clear();
}

inline IntPtr GetIntPtr(const void* ptr)
{
    return reinterpret_cast<IntPtr>(ptr);
}

inline PtrDiff GetOffsetFromPtr(const void* start, const void* end)
{
    return static_cast<PtrDiff>(GetIntPtr(end) - GetIntPtr(start));
}

template <typename T>
inline void* AddOffsetToPtr(void* ptr, T offset)
{
    return reinterpret_cast<void*>(GetIntPtr(ptr) + offset);
}

inline int ComparePtr(const void* a, const void* b)
{
    return static_cast<int>(GetIntPtr(a) - GetIntPtr(b));
}

template <typename T>
inline const void* AddOffsetToPtr(const void* ptr, T offset)
{
    return reinterpret_cast<const void*>(GetIntPtr(ptr) + offset);
}

}
}

using namespace nw::ut::internal;