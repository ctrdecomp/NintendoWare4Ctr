#pragma once

#include <nw/types.h>

#define NW_FLAG_DECLARE(mflag, mbit) \
    FLAG_##mflag##_SHIFT = (mbit),   \
    FLAG_##mflag = 0x1 << (mbit)

#define NW_FLAG_VALUE_DECLARE(mflag, mbit, msize)           \
    FLAG_##mflag##_VALUE_SHIFT = (mbit),                    \
    FLAG_##mflag##_VALUE_MASK = (((0x1UL << (msize)) - 0x1) << (mbit))

namespace nw {
namespace ut {

template<typename TFlags, typename TMask>
inline bool CheckFlag(const TFlags& flags, const TMask& mask)
{
    return (flags & static_cast<TFlags>(mask)) == static_cast<TFlags>(mask);
}

template<typename TFlags, typename TMask>
inline bool CheckFlagOr(const TFlags& flags, const TMask& mask)
{
    return (flags & static_cast<TFlags>(mask)) != 0;
}

template<typename TFlags, typename TValue>
inline TFlags EnableFlag(const TFlags& flags, const TValue& value)
{
    return flags | static_cast<TFlags>(value);
}

template<typename TFlags, typename TValue>
inline TFlags DisableFlag(const TFlags& flags, const TValue& value)
{
    return flags & ~(static_cast<TFlags>(value));
}

template<typename TResult, typename TFlags, typename TMask>
inline TResult GetFlagValue(const TFlags& flags, int shift, const TMask& mask)
{
    return static_cast<TResult>((flags & static_cast<TFlags>(mask)) >> shift);
}

template<typename TFlags, typename TMask, typename TValue>
inline TFlags SetFlagValue(const TFlags& flags, int shift, const TMask& mask, const TValue& value)
{
    return (flags & ~(static_cast<TFlags>(mask))) | ((value << shift) & static_cast<TFlags>(mask));
}

} // namespace ut
} // namespace nw