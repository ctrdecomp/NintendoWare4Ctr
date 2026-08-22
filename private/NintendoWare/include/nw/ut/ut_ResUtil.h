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

#ifndef NW_UT_RESUTIL_H_
#define NW_UT_RESUTIL_H_

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_Iterator.h>
#include <cstring>
#include <functional>
#include <algorithm>

// Disabling this macro disables some resource-related assertions in the Development build.
// This improves performance but removes some checks, so only disable it if you know what you are doing.
#define NW_STRICT_RESOURCE_ASSERTION_ENABLED


#define NW_VALIDITY_ASSERT \
    NW_ASSERTMSG(IsValid(), "%s::%s: Object not valid.", GetClassName(), __FUNCTION__)
#define NW_INDEX_ASSERT( name, idx ) \
    NW_ASSERT( 0<= (idx) && static_cast<s32>(idx) < static_cast<s32>(Get##name##Count()) )

#if defined(NW_STRICT_RESOURCE_ASSERTION_ENABLED) || defined(NW_DEBUG)
#define NW_RES_STRICT_ASSERT NW_ASSERT
#else
#define NW_RES_STRICT_ASSERT(exp) ((void)0)
#endif

#ifdef NW_LITTLE_ENDIAN
    #define NW_RES_SIGNATURE32(val)     \
        ((((val) & 0x000000FF) << 24) | \
         (((val) & 0x0000FF00) <<  8) | \
         (((val) & 0x00FF0000) >>  8) | \
         (((val) & 0xFF000000) >> 24) )

    #define NW_RES_SIGNATURE16(val)     \
        ((((val) & 0x00FF) << 8) |      \
         (((val) & 0xFF00) >>  8))

    #define NW_RES_TYPE_INFO(val)       (val)
#else
    #define NW_RES_SIGNATURE32(val)     (val)
    #define NW_RES_SIGNATURE16(val)     (val)
    #define NW_RES_TYPE_INFO(val)       \
        ((((val) & 0x000000FF) << 24) | \
         (((val) & 0x0000FF00) <<  8) | \
         (((val) & 0x00FF0000) >>  8) | \
         (((val) & 0xFF000000) >> 24) )

#endif

namespace nw {
namespace ut {

//
//

//---------------------------------------------------------------------------
//
//
//
//
//
//
//
//
//---------------------------------------------------------------------------
template<class TDown, class TBase>
NW_INLINE TDown
ResDynamicCast( TBase res )
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

//---------------------------------------------------------------------------
//
//
//
//
//
//
//
//---------------------------------------------------------------------------
template<class TDest, class TSrc>
NW_INLINE TDest
ResStaticCast( TSrc res )
{
    // If res.ptr is null, it returns a null without doing a check.
    NW_RES_STRICT_ASSERT( (! res.IsValid()) || ResDynamicCast<TDest>( res ).IsValid() );
    return TDest( res.ptr() );
}

//

//
typedef struct DataBlockHeader
{
    ResU32 signature;   //
    ResU32 length;      //
} DataBlockHeader;

/* ------------------------------------------------------------------------
    NW_RES_CTOR

    A macro to define the constructor for the resource accessor class that inherits ResCommon<T>.
    
    The class name of the resource accessor class is assigned to class_name.
    Must be 4-byte aligned.

    Even if operator== and operator!= are null, to allow them to operate ptr() is not used.
    
   ------------------------------------------------------------------------ */
#define NW_RES_CTOR_ALIGN(class_name, align)                                       \
    typedef class_name SelfType;          /*!< @details :private */                \
    typedef class_name##Data ResDataType; /*!< @details :private */                \
                                                                                   \
    /* Please see man pages for details */                                              \
    /* Please see man pages for details */                                  \
    /* ctor */ explicit class_name(const void *p = NULL)                           \
        : nw::ut::ResCommon<class_name##Data>(p) { NW_RES_STRICT_ASSERT(!((u32)p & ((align)-1))); }   \
                                                                                   \
    /* Please see man pages for details */                                           \
    static const char* GetClassName() { return #class_name; }                      \
                                                                                   \
    /* Please see man pages for details */                                           \
    bool operator==(const class_name& rhs) const { return ptr() == rhs.ptr(); }    \
    /* Please see man pages for details */                                 \
    bool operator!=(const class_name& rhs) const { return ptr() != rhs.ptr(); }    \


#define NW_RES_CTOR(class_name) NW_RES_CTOR_ALIGN(class_name, 4)

// NOTE: When using a macro, @return will at times become part of "brief" as a result of doxygen.
#define NW_RES_CTOR_INHERIT(class_name, base_name)                                  \
    typedef class_name SelfType;          /*!< @details :private */                 \
    typedef class_name##Data ResDataType; /*!< @details :private */                 \
                                                                                    \
    /* Please see man pages for details */                                               \
    /* Please see man pages for details */                                   \
    /* ctor */ explicit class_name(const void* p = NULL) : base_name(p) {}          \
                                                                                    \
    /* Please see man pages for details */                                       \
    ResDataType* ptr()                                                              \
    {                                                                               \
        return reinterpret_cast<ResDataType*>(void_ptr());                          \
    }                                                                               \
    /* Please see man pages for details */                                \
    const ResDataType* ptr() const                                                  \
    {                                                                               \
        return reinterpret_cast<const ResDataType*>(void_ptr());                    \
    }                                                                               \
    /* Please see man pages for details */                                           \
    ResDataType& ref()                                                              \
    {                                                                               \
        NW_RES_STRICT_ASSERT(void_ptr() != NULL);                                   \
        return *reinterpret_cast<ResDataType*>(void_ptr());                         \
    }                                                                               \
    /* Please see man pages for details */                                    \
    const ResDataType& ref() const                                                  \
    {                                                                               \
        NW_RES_STRICT_ASSERT(void_ptr() != NULL);                                   \
        return *reinterpret_cast<const ResDataType*>(void_ptr());                   \
    }                                                                               \
                                                                                    \
    /* Please see man pages for details */                                          \
    bool operator==(const class_name& rhs) const { return ptr() == rhs.ptr(); }     \
    /* Please see man pages for details */                                  \
    bool operator!=(const class_name& rhs) const { return ptr() != rhs.ptr(); }     \
                                                                                    \
    /* Please see man pages for details */                                            \
    static const char* GetClassName() { return #class_name; }                       \


/*------------------------------------------------------------------------
    Definitions of common parts

    Inherits and uses the class that instantiated T. Can make it a crude base class of the accessor.
    Except for mpData, data members must not exist.
  ------------------------------------------------------------------------*/
//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResCommonBase
{
private:
    void* mpData;

public:
    //
    explicit ResCommonBase(void *p) : mpData(p) {}
    explicit ResCommonBase(const void* p) : mpData(const_cast<void*>(p)) {}

    //--------------------------------------------------------------------------
    //
    //
    //
    //
    //---------------------------------------------------------------------------
    bool IsValid() const { return (mpData != NULL); }

protected:
    //
    NW_FORCE_INLINE void*       void_ptr()       { return mpData; }
    //
    NW_FORCE_INLINE const void* void_ptr() const { return mpData; }

    // Returns a pointer to the location ofs bytes after the start of the structure. Returns NULL if ofs is 0.
    //
    template<class X>
    X* ofs_to_ptr(Offset ofs)
    {
        // mpData is not checked because often ofs is traced from mpData
        u8* p = reinterpret_cast<u8*>(mpData);
        if (ofs != 0)
        {
            return reinterpret_cast<X*>(p + ofs);
        }
        else
        {
            return NULL;
        }
    }

    // Returns a pointer to the location ofs bytes after the start of the structure. Returns NULL if ofs is 0.
    //
    template<class X>
    const X* ofs_to_ptr(Offset ofs) const
    {
        // mpData is not checked because often ofs is traced from mpData
        const u8* p = reinterpret_cast<const u8*>(mpData);
        if (ofs != 0)
        {
            return reinterpret_cast<const X*>(p + ofs);
        }
        else
        {
            return NULL;
        }
    }

    // Returns an accessor object that takes the location ofs bytes after the start of the structure as an argument.
    // Returns an accessor object that takes NULL as an argument if ofs is 0.
    //
    template<class X>
    X ofs_to_obj(Offset ofs)
    {
        // mpData is not checked because often ofs is traced from mpData
        u8* p = reinterpret_cast<u8*>(mpData);
        if (ofs != 0)
        {
            return X(p + ofs);
        }
        else
        {
            return X(NULL);
        }
    }

    // Returns an accessor object that takes the location ofs bytes after the start of the structure as an argument.
    // Returns an accessor object that takes NULL as an argument if ofs is 0.
    //
    template<class X>
    const X ofs_to_obj(Offset ofs) const
    {
        // mpData is not checked because often ofs is traced from mpData
        const u8* p = reinterpret_cast<const u8*>(mpData);
        if (ofs != 0)
        {
            return X(const_cast<u8*>(p + ofs));
        }
        else
        {
            return X(NULL);
        }
    }

    // Can be used when ofs != 0 can be guaranteed for ofs_to_ptr.
    //
    template<class X>
    X* ofs_to_ptr_raw(Offset ofs)
    {
        NW_ASSERT(ofs != 0);
        return reinterpret_cast<X*>(reinterpret_cast<u8*>(mpData) + ofs);
    }

    // Can be used when ofs != 0 can be guaranteed for ofs_to_ptr.
    //
    template<class X>
    const X* ofs_to_ptr_raw(Offset ofs) const
    {
        NW_ASSERT(ofs != 0);
        return reinterpret_cast<const X*>(reinterpret_cast<const u8*>(mpData) + ofs);
    }

    // Invalidates the status. Does not destroy the entity of the data that was referenced.
    //
    void invalidate() { mpData = NULL; }
};


//--------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
template<class T>
class ResCommon : public ResCommonBase
{
public:
    //
    explicit ResCommon(void *p) : ResCommonBase(p) {}
    explicit ResCommon(const void* p) : ResCommonBase(p) {}

    //
    NW_FORCE_INLINE T* ptr() { return reinterpret_cast<T*>(void_ptr()); }

    //
    NW_FORCE_INLINE const T* ptr() const { return reinterpret_cast<const T*>(void_ptr()); }

    //
    NW_FORCE_INLINE T& ref() { NW_RES_STRICT_ASSERT(this->IsValid()); return *reinterpret_cast<T*>(void_ptr()); }

    //
    NW_FORCE_INLINE const T& ref() const { NW_RES_STRICT_ASSERT(this->IsValid()); return *reinterpret_cast<const T*>(void_ptr()); }
};

//----------------------------------------
//
//

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
template<typename TResource>
NW_INLINE void
SafeCleanup(
    TResource res
)
{
    if (res.IsValid())
    {
        res.Cleanup();
    }
}

//---------------------------------------------------------------------------
//
//
//
//---------------------------------------------------------------------------
template<typename TObject>
struct SafeCleanupper : public std::unary_function<TObject, void>
{
    //
    void operator()(TObject res) const
    {
        SafeCleanup(res);
    }
};

//---------------------------------------------------------------------------
//
//
//
//
//
//---------------------------------------------------------------------------
template<typename TArray>
NW_INLINE void
SafeCleanupAll(
    TArray array
)
{
    std::for_each(array.begin(), array.end(), SafeCleanupper<typename TArray::value_type>());
}

//

// File information class
//
struct ResFileData
{
    ResU32  signature;
    ResU16  byteOrder;
    ResU16  headerSize;
    ResU32  revision;
    ResU32  fileSize;
};

//--------------------------------------------------------------------------
//
//---------------------------------------------------------------------------
class ResFile : public ResCommon<ResFileData>
{
private:
    static const u16 BOM = 0xFEFF;
    static const u32 SIGNATURE = 'NWFL';

public:
    NW_RES_CTOR( ResFile )

    //
    bool IsBigEndian() const
    {
        NW_ASSERT( (ref().byteOrder == BOM) || (ref().byteOrder == BOM) );
    #if defined( NW_SWAP_ENDIAN )
        return *(u8*)(&ref().byteOrder) == 0xFE;
    #else
        return *(u8*)(&ref().byteOrder) == 0xFF;
    #endif
    }

    //
    bool IsLittleEndian() const
    {
        NW_ASSERT( (ref().byteOrder == BOM) || (ref().byteOrder == BOM) );
    #if defined( NW_SWAP_ENDIAN )
        return *(u8*)(&ref().byteOrder) == 0xFF;
    #else
        return *(u8*)(&ref().byteOrder) == 0xFE;
    #endif
    }

    //
    bool  TestByteOrder() const { return ref().byteOrder == BOM;       }
    //
    bool  TestSignature() const { return ref().signature == SIGNATURE; }

    //
    u32   GetRevision()   const { return ref().revision;   }

    //
    u32   GetFileSize()   const { return ref().fileSize;   }

    //
    u16   GetHeaderSize() const { return ref().headerSize; }

    //
    void*       GetDataAddress() { return (reinterpret_cast<u8*>(ptr()) + u32(ref().headerSize)); }

    //
    const void* GetDataAddress() const { return (reinterpret_cast<const u8*>(ptr()) + u32(ref().headerSize)); }
};

} /* namespace ut */
} /* namespace nw */

#endif /* NW_UT_RESUTIL_H_ */
