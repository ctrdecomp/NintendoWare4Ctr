#pragma once

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_Iterator.h>
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
        
        static reference   GetValue(void* ptr) { return GetValue( reinterpret_cast<pointer>(ptr) ); }
        static reference   GetValue(pointer ptr) { return *ptr; }
        static const_reference   GetValue(const void* ptr) { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return *ptr; }
        
        static iter_pointer        GetPointer(pointer ptr) { return ptr; }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ptr; }

        static pointer       GetNext(pointer ptr) { return ptr + 1; }
        static const_pointer GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t        ValueSize() { return sizeof(T); }
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
        
        static const_reference   GetValue(const void* ptr) { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return *ptr; }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ptr; }

        static const_pointer     GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t            ValueSize() { return sizeof(T); }
    };

    
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

        typedef ResPtr<T>     iter_pointer;
        typedef ResPtr<const T>     const_iter_pointer;

        static iter_pointer        GetPointer(pointer ptr) { return ResPtr<T>( ptr->to_ptr() ); }
        static const_iter_pointer  GetPointer(const_pointer ptr) { return ResPtr<T>( ptr->to_ptr() ); }
        
        static reference   GetValue(void* ptr) { return GetValue( reinterpret_cast<pointer>(ptr) ); }
        static reference   GetValue(pointer ptr) { return T( ptr->to_ptr() ); }
        static const_reference   GetValue(const void* ptr) { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return T( ptr->to_ptr() ); }

        static pointer       GetNext(pointer ptr) { return ptr + 1; }
        static const_pointer GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t        ValueSize() { return sizeof(Offset); }
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

        static const_reference   GetValue(const void* ptr) { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return T( ptr->to_ptr() ); }

        static const_pointer     GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t      ValueSize() { return sizeof(Offset); }
    };

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

        typedef typename TTraits<T>::iter_pointer       iter_pointer;
        typedef typename TTraits<T>::const_iter_pointer const_iter_pointer;

        ResArray(void* pBegin, void* pEnd) : 
            m_PBegin(static_cast<pointer>(pBegin)),
            m_PEnd(static_cast<pointer>(pEnd)) {}
        ResArray(void* pBegin, s32 num): 
            m_PBegin( static_cast<pointer>(pBegin) ), 
            m_PEnd( m_PBegin + num ) {}
        
        operator pointer() { return m_PBegin; }
        operator const pointer() const { return m_PBegin; }
        
        reference operator[](int index)
        {
            return TTraits<T>::GetValue(m_PBegin + index);
        }
        
        const_reference operator[](int index) const
        {
            return TTraits<T>::GetValue(m_PBegin + index);
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
            
            iterator() : 
                m_PCurrent(NULL) {}
            
            operator const_iterator() { return const_iterator(m_PCurrent); }
            
            reference operator*()  const { NW_NULL_ASSERT(m_PCurrent); return TTraits<T>::GetValue(m_PCurrent); }

            iter_pointer   operator->() const { return TTraits<T>::GetPointer(m_PCurrent); }
            
            TIt &operator++() { ++m_PCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--() { --m_PCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }
            
            friend bool operator==(TIt it1, TIt it2) { return it1.m_PCurrent == it2.m_PCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }
            
        private:
            explicit iterator(pointer p) : m_PCurrent(p) {}
            
            pointer m_PCurrent;
            
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
            
            const_iterator(): 
                m_PCurrent(0) {}
            
            const_reference operator*()  const { NW_NULL_ASSERT(m_PCurrent); return TTraits<T>::GetValue(m_PCurrent); }

            const_iter_pointer   operator->() const { return TTraits<T>::GetPointer(m_PCurrent); }

            TIt &operator++() { ++m_PCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--() { --m_PCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }
            
            friend bool operator==(TIt it1, TIt it2) { return it1.m_PCurrent == it2.m_PCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }
        private:
            explicit const_iterator(const_pointer p) : m_PCurrent(p) {}
            
            const_pointer m_PCurrent;
            
            friend class ResArray<T, TTraits>;
        };
        typedef typename internal::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef typename internal::reverse_iterator<iterator> reverse_iterator;
        
        s32  size() const { return (size_t(this->m_PEnd) - size_t(this->m_PBegin)) / TTraits<T>::ValueSize(); }
        bool empty() const { return (this->m_PBegin == this->m_PEnd); }
        
        iterator begin() { return iterator(this->m_PBegin); }
        const_iterator begin() const { return const_iterator(this->m_PBegin); }
        iterator end() { return iterator(this->m_PEnd); }
        const_iterator end() const { return const_iterator(this->m_PEnd); }

        reverse_iterator  rbegin() { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend() { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
        
    private:
        pointer   m_PBegin;
        pointer   m_PEnd;

        friend class ResArray<const T, TTraits>;
    };

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

        typedef typename TTraits<T>::iter_pointer       iter_pointer;
        typedef typename TTraits<T>::const_iter_pointer const_iter_pointer;
        
        ResArray( const void* pBegin, const void* pEnd ) 
          : m_PBegin( static_cast<const_pointer>(pBegin) ), 
            m_PEnd( static_cast<const_pointer>(pEnd) ) {}
        ResArray( const void* pBegin, s32 num )
          : m_PBegin( static_cast<const_pointer>(pBegin) ), 
            m_PEnd( m_PBegin + num ) {}

        ResArray( const ResArray<T, TTraits>& rhs ): 
        m_PBegin(rhs.m_PBegin),
            m_PEnd(rhs.m_PEnd) {}
        
        operator const pointer() const { return m_PBegin; }
        
        const_reference operator[](int index) const
        {
            return TTraits<T>::GetValue(m_PBegin + index);
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
            
            const_iterator() : m_PCurrent( NULL ) {}
            
            const_reference operator*()  const { NW_NULL_ASSERT(m_PCurrent); return TTraits<T>::GetValue(m_PCurrent); }

            const_iter_pointer   operator->() const { return TTraits<T>::GetPointer(m_PCurrent); }
            
            TIt &operator++() { ++m_PCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--() { --m_PCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }
            
            friend bool operator==(TIt it1, TIt it2) { return it1.m_PCurrent == it2.m_PCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }
            
        private:
            explicit const_iterator(const_pointer p) : m_PCurrent(p) {}
            const_pointer m_PCurrent;
            friend class ResArray<const T, TTraits>;
        };
        
        typedef internal::reverse_iterator<const_iterator>  const_reverse_iterator;
        
        s32 size() const { return (size_t(m_PEnd) - size_t(m_PBegin)) / TTraits<T>::ValueSize(); }
        bool empty() const { return (m_PBegin == m_PEnd); }
        const_iterator begin() const { return const_iterator(m_PBegin); }
        const_iterator end() const { return const_iterator(m_PEnd); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    private:
        const_pointer   m_PBegin;
        const_pointer   m_PEnd;

        friend class ResArray<T, TTraits>;
    };

}

}
}