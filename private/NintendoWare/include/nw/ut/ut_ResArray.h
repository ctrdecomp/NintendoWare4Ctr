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

#ifndef NW_UT_RESARRAY_H_
#define NW_UT_RESARRAY_H_

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <iterator>

namespace nw {
namespace ut {

namespace internal {

    template <typename T>
    class ResArrayPrimitiveTraits
    {
    public:
        typedef T*       pointer;
        typedef const T* const_pointer;
        typedef T&       reference;
        typedef const T& const_reference;
        typedef T&       iter_pointer;
        typedef const T& const_iter_pointer;
        typedef int      difference_type;
        typedef std::input_iterator_tag iterator_category;

        static reference   GetValue(void* ptr)   { return GetValue( reinterpret_cast<pointer>(ptr) ); }
        static reference   GetValue(pointer ptr) { return *ptr; }
        static const_reference   GetValue(const void* ptr)   { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return *ptr; }

        static iter_pointer        GetPointer(pointer ptr) { return ptr; }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ptr; }

        static pointer       GetNext(pointer ptr) { return ptr + 1; }
        static const_pointer GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t        ValueSize()          { return sizeof(T); }
    };

    template <typename T>
    class ResArrayPrimitiveTraits<const T>
    {
    public:
        typedef T*       pointer;
        typedef const T* const_pointer;
        typedef T&       reference;
        typedef const T& const_reference;
        typedef T&       iter_pointer;
        typedef const T& const_iter_pointer;
        typedef int      difference_type;
        typedef std::input_iterator_tag iterator_category;

        static const_reference   GetValue(const void* ptr)   { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return *ptr; }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ptr; }

        static const_pointer     GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t            ValueSize()          { return sizeof(T); }
    };

#if 0
    // NOTE: Implementation for enabling use of the arrow operator with the Res class iterator.
    // Compared to the (*it) notation, there is a possibility of a slight cost on the performance level, so the alternative has been commented out until the need for the algorithm required for the allow operators.
    // 

    #define NW_UT_RESARRAY_ENABLE_ARROW_OP_

    template <typename TRes>
    class ResPtr
    {
    public:
        ResPtr(void* ptr) : m_Res(ptr) {}
        TRes*   operator->() { return &m_Res; }
    private:
        TRes m_Res;
    };

    template <typename TRes>
    class ResPtr<const TRes>
    {
    public:
        ResPtr(const void* ptr) : m_Res(ptr) {}
        const TRes*   operator->() const { return &m_Res; }
    private:
        TRes m_Res;
    };
#endif

    template<typename T>
    class ResArrayClassTraits
    {
    public:
        typedef Offset*       pointer;
        typedef const Offset* const_pointer;
        typedef T             reference;
        typedef const T       const_reference;
        typedef int           difference_type;
        typedef std::input_iterator_tag iterator_category;

    #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
        typedef ResPtr<T>     iter_pointer;
        typedef ResPtr<const T>     const_iter_pointer;

        static iter_pointer        GetPointer(pointer ptr) { return ResPtr<T>( ptr->to_ptr() ); }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ResPtr<T>( ptr->to_ptr() ); }
    #endif

        static reference   GetValue(void* ptr)   { return GetValue( reinterpret_cast<pointer>(ptr) ); }
        static reference   GetValue(pointer ptr) { return T( ptr->to_ptr() ); }
        static const_reference   GetValue(const void* ptr)   { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return T( ptr->to_ptr() ); }

        static pointer       GetNext(pointer ptr)       { return ptr + 1; }
        static const_pointer GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t        ValueSize()                { return sizeof(Offset); }
    };

    template<typename T>
    class ResArrayClassTraits<const T>
    {
    public:
        typedef Offset*       pointer;
        typedef const Offset* const_pointer;
        typedef T             reference;
        typedef const T       const_reference;
        typedef int           difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;

        static const_reference   GetValue(const void* ptr)   { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return T( ptr->to_ptr() ); }

        static const_pointer     GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t      ValueSize()          { return sizeof(Offset); }
    };


    //==========================================
    // The ResArray class.
    // If IsOffset is true, handle as an nw::ut::Offset array.
    // 
    //==========================================
    template <typename T, template <typename> class TTraits>
    class ResArray
    {
    public:

        typedef T                                       value_type;
        typedef typename TTraits<T>::pointer            pointer;
        typedef typename TTraits<T>::reference          reference;
        typedef typename TTraits<T>::const_pointer      const_pointer;
        typedef typename TTraits<T>::const_reference    const_reference;
        typedef typename TTraits<T>::difference_type    difference_type;
        typedef typename TTraits<T>::iterator_category  iterator_category;

    #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
        typedef typename TTraits<T>::iter_pointer       iter_pointer;
        typedef typename TTraits<T>::const_iter_pointer const_iter_pointer;
    #endif

        /* ctor */  ResArray( void* pBegin, void* pEnd )
          : m_pBegin( static_cast<pointer>(pBegin) ),
            m_pEnd( static_cast<pointer>(pEnd) )
        {}
        /* ctor */  ResArray( void* pBegin, s32 num )
          : m_pBegin( static_cast<pointer>(pBegin) ),
            m_pEnd( m_pBegin + num )
        {}

        operator pointer()             { return m_pBegin; }
        operator const pointer() const { return m_pBegin; }

        reference operator[](int index)
        {
            NW_MINMAXLT_ASSERT(index, 0, size());
            return TTraits<T>::GetValue(m_pBegin + index);
        }

        const_reference operator[](int index) const
        {
            NW_MINMAXLT_ASSERT(index, 0, size());
            return TTraits<T>::GetValue(m_pBegin + index);
        }

        class const_iterator;
        class iterator
        {
        public:
            typedef iterator TIt;
            typedef T value_type;
            typedef typename TTraits<T>::pointer            pointer;
            typedef typename TTraits<T>::reference          reference;
            typedef typename TTraits<T>::const_pointer      const_pointer;
            typedef typename TTraits<T>::const_reference    const_reference;
            typedef typename TTraits<T>::difference_type    difference_type;
            typedef typename TTraits<T>::iterator_category  iterator_category;

            /* ctor */  iterator() : m_pCurrent( NULL ) {}

            operator const_iterator() { return const_iterator(m_pCurrent); }

            reference operator*()  const { NW_NULL_ASSERT(m_pCurrent); return TTraits<T>::GetValue(m_pCurrent); }

        #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
            iter_pointer   operator->() const { return TTraits<T>::GetPointer(m_pCurrent); }
        #endif

            TIt &operator++()   { ++m_pCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--()   { --m_pCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }

            friend bool operator==(TIt it1, TIt it2) { return it1.m_pCurrent == it2.m_pCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }

        private:
            explicit iterator(pointer p) : m_pCurrent(p) {}

            pointer m_pCurrent;

            friend class const_iterator;
            friend class ResArray<T, TTraits>;
        };

        class const_iterator
        {
        public:
            typedef const_iterator  TIt;
            typedef const T value_type;
            typedef typename TTraits<T>::pointer            pointer;
            typedef typename TTraits<T>::reference          reference;
            typedef typename TTraits<T>::const_pointer      const_pointer;
            typedef typename TTraits<T>::const_reference    const_reference;
            typedef typename TTraits<T>::difference_type    difference_type;
            typedef typename TTraits<T>::iterator_category  iterator_category;

            /* ctor */  const_iterator() : m_pCurrent( NULL ) {}

            const_reference operator*()  const { NW_NULL_ASSERT(m_pCurrent); return TTraits<T>::GetValue(m_pCurrent); }

        #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
            const_iter_pointer   operator->() const { return TTraits<T>::GetPointer(m_pCurrent); }
        #endif

            TIt &operator++()   { ++m_pCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--()   { --m_pCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }

            friend bool operator==(TIt it1, TIt it2) { return it1.m_pCurrent == it2.m_pCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }

        private:
            explicit const_iterator(const_pointer p) : m_pCurrent(p) {}

            const_pointer m_pCurrent;

            friend class ResArray<T, TTraits>;
        };

        typedef internal::reverse_iterator<iterator>        reverse_iterator;
        typedef internal::reverse_iterator<const_iterator>  const_reverse_iterator;

        s32                 size() const { return (size_t(m_pEnd) - size_t(m_pBegin)) / TTraits<T>::ValueSize(); }
        bool                empty() const { return (m_pBegin == m_pEnd); }

        iterator            begin()       { return iterator(m_pBegin); }
        const_iterator      begin() const { return const_iterator(m_pBegin); }
        iterator            end()         { return iterator(m_pEnd); }
        const_iterator      end() const   { return const_iterator(m_pEnd); }

        reverse_iterator            rbegin()       { return reverse_iterator(end()); }
        const_reverse_iterator      rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator            rend()         { return reverse_iterator(begin()); }
        const_reverse_iterator      rend() const   { return const_reverse_iterator(begin()); }

    private:
        pointer   m_pBegin;
        pointer   m_pEnd;

        friend class ResArray<const T, TTraits>;
    };

    //==========================================
    // const version
    //==========================================
    template <typename T, template <typename> class TTraits>
    class ResArray<const T, TTraits>
    {
    public:
        typedef T                                       value_type;
        typedef typename TTraits<T>::pointer            pointer;
        typedef typename TTraits<T>::reference          reference;
        typedef typename TTraits<T>::const_pointer      const_pointer;
        typedef typename TTraits<T>::const_reference    const_reference;
        typedef typename TTraits<T>::difference_type    difference_type;
        typedef typename TTraits<T>::iterator_category  iterator_category;

    #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
        typedef typename TTraits<T>::iter_pointer       iter_pointer;
        typedef typename TTraits<T>::const_iter_pointer const_iter_pointer;
    #endif

        /* ctor */  ResArray( const void* pBegin, const void* pEnd )
          : m_pBegin( static_cast<const_pointer>(pBegin) ),
            m_pEnd( static_cast<const_pointer>(pEnd) )
        {}
        /* ctor */  ResArray( const void* pBegin, s32 num )
          : m_pBegin( static_cast<const_pointer>(pBegin) ),
            m_pEnd( m_pBegin + num )
        {}

        /* ctor */  ResArray( const ResArray<T, TTraits>& rhs )
          : m_pBegin( rhs.m_pBegin ),
            m_pEnd( rhs.m_pEnd )
        {
        }

        operator const pointer() const { return m_pBegin; }

        const_reference operator[](int index) const
        {
            NW_MINMAXLT_ASSERT(index, 0, size());
            return TTraits<T>::GetValue(m_pBegin + index);
        }

        class const_iterator
        {
        public:
            typedef const_iterator  TIt;
            typedef T                                       value_type;
            typedef typename TTraits<T>::pointer            pointer;
            typedef typename TTraits<T>::reference          reference;
            typedef typename TTraits<T>::const_pointer      const_pointer;
            typedef typename TTraits<T>::const_reference    const_reference;
            typedef typename TTraits<T>::difference_type    difference_type;
            typedef typename TTraits<T>::iterator_category  iterator_category;

            /* ctor */  const_iterator() : m_pCurrent( NULL ) {}

            const_reference operator*()  const { NW_NULL_ASSERT(m_pCurrent); return TTraits<T>::GetValue(m_pCurrent); }

        #if defined(NW_UT_RESARRAY_ENABLE_ARROW_OP_)
            const_iter_pointer   operator->() const { return TTraits<T>::GetPointer(m_pCurrent); }
        #endif

            TIt &operator++()   { ++m_pCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--()   { --m_pCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }

            friend bool operator==(TIt it1, TIt it2) { return it1.m_pCurrent == it2.m_pCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }

        private:
            explicit const_iterator(const_pointer p) : m_pCurrent(p) {}

            const_pointer m_pCurrent;

            friend class ResArray<const T, TTraits>;
        };

        typedef internal::reverse_iterator<const_iterator>  const_reverse_iterator;

        s32                 size() const { return (size_t(m_pEnd) - size_t(m_pBegin)) / TTraits<T>::ValueSize(); }
        bool                empty() const { return (m_pBegin == m_pEnd); }

        const_iterator      begin() const { return const_iterator(m_pBegin); }
        const_iterator      end() const   { return const_iterator(m_pEnd); }

        const_reverse_iterator      rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator      rend() const   { return const_reverse_iterator(begin()); }

    private:
        const_pointer   m_pBegin;
        const_pointer   m_pEnd;

        friend class ResArray<T, TTraits>;
    };

} // namespace internal

} // namespace ut
} // namespace nw

#endif // NW_UT_RESARRAY_H_

