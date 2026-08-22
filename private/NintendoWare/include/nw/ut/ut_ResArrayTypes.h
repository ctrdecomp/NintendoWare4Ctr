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

#ifndef NW_UT_RESARRAYTYPES_H_
#define NW_UT_RESARRAYTYPES_H_

#include <nw/types.h>
#include <nw/ut/ut_ResArray.h>
#include <nw/ut/ut_ResDictionary.h>
#include <iterator>

namespace nw {
namespace ut {

namespace internal {

    template <typename TDic>
    class ResArrayDicTraits
    {
    public:

    template <typename T>
    class Traits
    {
    public:
        typedef typename TDic::ResDicNodeData*         pointer;
        typedef const typename TDic::ResDicNodeData*   const_pointer;
        typedef T                                      reference;
        typedef const T                                const_reference;
        typedef int                                    difference_type;
        typedef std::input_iterator_tag iterator_category;

    #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
        typedef ResPtr<T>           iter_pointer;
        typedef ResPtr<const T>     const_iter_pointer;

        static iter_pointer        GetPointer(pointer ptr) { return ResPtr<T>( ptr->to_ptr() ); }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ResPtr<T>( ptr->to_ptr() ); }
    #endif

        static reference    GetValue(void* ptr)   { return GetValue( reinterpret_cast<pointer>(ptr) ); }
        static reference    GetValue(pointer ptr) { return T( ptr->ofsData.to_ptr() ); }
        static const_reference GetValue(const void* ptr) { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference GetValue(const_pointer ptr) { return T( ptr->ofsData.to_ptr() ); }

        static pointer       GetNext(pointer ptr) { return ptr + 1; }
        static const_pointer GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t       ValueSize()          { return sizeof(TDic::ResDicNodeData); }
    };
    };

} /* namespace internal */


//
template <typename T>
class ResArrayPrimitive
{
public:
    //
    typedef internal::ResArray< T, internal::ResArrayPrimitiveTraits >   type;
};

//
template <typename T>
class ResArrayClass
{
public:
    //
    typedef internal::ResArray< T, internal::ResArrayClassTraits >   type;
};

//
template <typename T>
class ResArrayLinear
{
public:
    //
    typedef internal::ResArray< T, internal::ResArrayDicTraits<ResDicLinearData>::Traits > type;
};

//
template <typename T>
class ResArrayPatricia
{
public:
    //
    typedef internal::ResArray< T, internal::ResArrayDicTraits<ResDicPatriciaData>::Traits > type;
};


} // namespace ut
} // namespace nw

#endif // NW_UT_RESARRAYTYPES_H_

