#pragma once

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_Iterator.h>
#include <cstring>
#include <functional>
#include <algorithm>

#define NW_VALIDITY_ASSERT \
    NW_ASSERTMSG(IsValid(), "%s::%s: Object not valid.", GetClassName(), __FUNCTION__)
#define NW_INDEX_ASSERT(name, idx) \
    NW_ASSERT(0 <= (idx) && static_cast<s32>(idx) < static_cast<s32>(Get##name##Count()))

#ifdef NW_LITTLE_ENDIAN
    #define NW_RES_SIGNATURE32(val)     \
        ((((val) & 0x000000FF) << 24) | \
         (((val) & 0x0000FF00) <<  8) | \
         (((val) & 0x00FF0000) >>  8) | \
         (((val) & 0xFF000000) >> 24))

    #define NW_RES_SIGNATURE16(val)    \
        ((((val) & 0x00FF) << 8) |     \
         (((val) & 0xFF00) >>  8))

    #define NW_RES_TYPE_INFO(val) (val)
#else
    #define NW_RES_SIGNATURE32(val) (val)
    #define NW_RES_SIGNATURE16(val) (val)
    #define NW_RES_TYPE_INFO(val)       \
        ((((val) & 0x000000FF) << 24) | \
         (((val) & 0x0000FF00) <<  8) | \
         (((val) & 0x00FF0000) >>  8) | \
         (((val) & 0xFF000000) >> 24))
#endif

namespace nw {
namespace ut {

template<class TDown, class TBase>
inline TDown ResDynamicCast(TBase res)
{
    if (!res.IsValid())
    {
        return TDown(NULL);
    }
    if ((res.GetTypeInfo() & TDown::TYPE_INFO) == TDown::TYPE_INFO)
    {
        return TDown(res.ptr());
    }
    return TDown(NULL);
}

template<class TDest, class TSrc>
inline TDest ResStaticCast(TSrc res)
{
    NW_ASSERT((!res.IsValid()) || ResDynamicCast<TDest>(res).IsValid());
    return TDest(res.ptr());
}

typedef struct DataBlockHeader{
    ResU32 signature;
    ResU32 length;
} DataBlockHeader;

#define NW_RES_CTOR_ALIGN(class_name, align)                                              \
    typedef class_name SelfType;                                                          \
    typedef class_name##Data ResDataType;                                                 \
    explicit class_name(const void* p = NULL)                                             \
        : nw::ut::ResCommon<class_name##Data>(p) { NW_ASSERT(!((u32)p & ((align)-1))); } \
    class_name##Data& ref() { NW_VALIDITY_ASSERT; return *ptr(); }                        \
    const class_name##Data& ref() const { NW_VALIDITY_ASSERT; return *ptr(); }           \
    static const char* GetClassName() { return #class_name; }                             \
    bool operator==(const class_name& rhs) const { return ptr() == rhs.ptr(); }          \
    bool operator!=(const class_name& rhs) const { return ptr() != rhs.ptr(); }

#define NW_RES_CTOR(class_name) NW_RES_CTOR_ALIGN(class_name, 4)

#define NW_RES_CTOR_INHERIT(class_name, base_name)                                        \
    typedef class_name SelfType;                                                          \
    typedef class_name##Data ResDataType;                                                 \
    explicit class_name(const void* p = NULL): base_name(p) {}                           \
    ResDataType* ptr() { return reinterpret_cast<ResDataType*>(void_ptr()); }              \
    const ResDataType* ptr() const { return reinterpret_cast<const ResDataType*>(void_ptr()); } \
    ResDataType& ref() { NW_NULL_ASSERT(void_ptr()); return *reinterpret_cast<ResDataType*>(void_ptr()); } \
    const ResDataType& ref() const { NW_NULL_ASSERT(void_ptr()); return *reinterpret_cast<const ResDataType*>(void_ptr()); } \
    static const char* GetClassName() { return #class_name; }                            \
    bool operator==(const class_name& rhs) const { return ptr() == rhs.ptr(); }          \
    bool operator!=(const class_name& rhs) const { return ptr() != rhs.ptr(); }

class ResCommonBase
{
private:
    void* m_pData;

public:
    explicit ResCommonBase(void* p): m_pData(p) {}
    explicit ResCommonBase(const void* p): m_pData(const_cast<void*>(p)) {}

    bool IsValid() const { return (m_pData != NULL); }

protected:
    inline void* void_ptr() { return m_pData; }
    inline const void* void_ptr() const { return m_pData; }

    template<class X>
    X* ofs_to_ptr(Offset ofs)
    {
        u8* p = reinterpret_cast<u8*>(m_pData);
        if (ofs != 0)
        {
            return reinterpret_cast<X*>(p + ofs);
        }
        else
        {
            return NULL;
        }
    }

    template<class X>
    const X* ofs_to_ptr(Offset ofs) const
    {
        const u8* p = reinterpret_cast<const u8*>(m_pData);
        if (ofs != 0)
        {
            return reinterpret_cast<const X*>(p + ofs);
        }
        else
        {
            return NULL;
        }
    }

    template<class X>
    X ofs_to_obj(Offset ofs)
    {
        u8* p = reinterpret_cast<u8*>(m_pData);
        if (ofs != 0)
        {
            return X(p + ofs);
        }
        else
        {
            return X(NULL);
        }
    }

    template<class X>
    const X ofs_to_obj(Offset ofs) const
    {
        const u8* p = reinterpret_cast<const u8*>(m_pData);
        if (ofs != 0)
        {
            return X(const_cast<u8*>(p + ofs));
        }
        else
        {
            return X(NULL);
        }
    }

    template<class X>
    X* ofs_to_ptr_raw(Offset ofs)
    {
        NW_ASSERT(ofs != 0);
        return reinterpret_cast<X*>(reinterpret_cast<u8*>(m_pData) + ofs);
    }

    template<class X>
    const X* ofs_to_ptr_raw(Offset ofs) const
    {
        NW_ASSERT(ofs != 0);
        return reinterpret_cast<const X*>(reinterpret_cast<const u8*>(m_pData) + ofs);
    }

    void invalidate() { m_pData = NULL; }
};

template<class T>
class ResCommon : public ResCommonBase
{
public:
    explicit ResCommon(void* p): ResCommonBase(p) {}
    explicit ResCommon(const void* p): ResCommonBase(p) {}

    inline T* ptr() { return reinterpret_cast<T*>(void_ptr()); }
    inline const T* ptr() const { return reinterpret_cast<const T*>(void_ptr()); }
    inline T& ref() { NW_ASSERT(this->IsValid()); return *reinterpret_cast<T*>(void_ptr()); }
    inline const T& ref() const { NW_ASSERT(this->IsValid()); return *reinterpret_cast<const T*>(void_ptr()); }
};

struct ResNameData
{
    ResS32 len;
    char   str[4];
};

class ResName : public ResCommon<ResNameData>
{
public:
    NW_RES_CTOR(ResName)

    s32 GetLength() const { return ref().len; }

    const char* GetName() const { return &ref().str[0]; }

    bool Equals(const char* str, size_t len) const { return (GetLength() == len) && (::std::strcmp(GetName(), str) == 0); }
};


template<typename TResource>
inline void SafeCleanup(TResource res)
{
    if (res.IsValid())
    {
        res.Cleanup();
    }
}

template<typename TObject>
struct SafeCleanupper : public std::unary_function<TObject, void>
{
    void operator()(TObject res) const
    {
        SafeCleanup(res);
    }
};

template<typename TArray>
inline void SafeCleanupAll(TArray array)
{
    std::for_each(array.begin(), array.end(), SafeCleanupper<typename TArray::value_type>());
}

}
}