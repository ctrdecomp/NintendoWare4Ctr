#pragma once

#include <nw/types.h>
#include <nw/ut/ut_ResTypes.h>
#include <nw/ut/ut_Iterator.h>
#include <iterator>

namespace nw { 
namespace ut {
namespace internal {
    template <typename T>
    class ResArrayPrimitiveTraits{
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
    class ResArrayPrimitiveTraits<const T>{
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

    
    template <typename TRes>
    class ResPtr
    {
    public:
        ResPtr(void* ptr) : mRes(ptr) {}
        TRes*   operator->() { return &mRes; }
    private:
        TRes mRes;
    };
    
    template <typename TRes>
    class ResPtr<const TRes>
    {
    public:
        ResPtr(const void* ptr) : mRes(ptr) {}
        const TRes*   operator->() const { return &mRes; }
    private:
        TRes mRes;
    };

    template<typename T>
    class ResArrayClassTraits{
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
        
        static reference   GetValue(void* ptr)   { return GetValue( reinterpret_cast<pointer>(ptr) ); }
        static reference   GetValue(pointer ptr) { return T( ptr->to_ptr() ); }
        static const_reference   GetValue(const void* ptr)   { return GetValue( reinterpret_cast<const_pointer>(ptr) ); }
        static const_reference   GetValue(const_pointer ptr) { return T( ptr->to_ptr() ); }

        static pointer       GetNext(pointer ptr)       { return ptr + 1; }
        static const_pointer GetNext(const_pointer ptr) { return ptr + 1; }
        static size_t        ValueSize()                { return sizeof(Offset); }
    };

    template<typename T>
    class ResArrayClassTraits<const T>{
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

    template <typename T, template <typename> class TTraits>
    class ResArray{
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
            mPBegin(static_cast<pointer>(pBegin)),
            mPEnd(static_cast<pointer>(pEnd)) 
        {}
        ResArray(void* pBegin, s32 num): 
            mPBegin( static_cast<pointer>(pBegin) ), 
            mPEnd( mPBegin + num )
        {}
        
        operator pointer()             { return mPBegin; }
        operator const pointer() const { return mPBegin; }
        
        reference operator[](int index){
            return TTraits<T>::GetValue(mPBegin + index);
        }
        
        const_reference operator[](int index) const{
            return TTraits<T>::GetValue(mPBegin + index);
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
                mPCurrent(NULL)
            {}
            
            operator const_iterator() { return const_iterator(mPCurrent); }
            
            reference operator*()  const { NW_NULL_ASSERT(mPCurrent); return TTraits<T>::GetValue(mPCurrent); }

            iter_pointer   operator->() const { return TTraits<T>::GetPointer(mPCurrent); }
            
            TIt &operator++()   { ++mPCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--()   { --mPCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }
            
            friend bool operator==(TIt it1, TIt it2) { return it1.mPCurrent == it2.mPCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }
            
        private:
            explicit iterator(pointer p) : mPCurrent(p) {}
            
            pointer mPCurrent;
            
            friend class const_iterator;
            friend class ResArray<T, TTraits>;
        };
        
        class const_iterator{
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
                mPCurrent(0)
            {}
            
            const_reference operator*()  const { NW_NULL_ASSERT(mPCurrent); return TTraits<T>::GetValue(mPCurrent); }

            const_iter_pointer   operator->() const { return TTraits<T>::GetPointer(mPCurrent); }

            TIt &operator++()   { ++mPCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--()   { --mPCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }
            
            friend bool operator==(TIt it1, TIt it2) { return it1.mPCurrent == it2.mPCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }
        private:
            explicit const_iterator(const_pointer p) : mPCurrent(p) {}
            
            const_pointer mPCurrent;
            
            friend class ResArray<T, TTraits>;
        };
        typedef typename internal::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef typename internal::reverse_iterator<iterator> reverse_iterator;
        
        s32  size() const { return (size_t(this->mPEnd) - size_t(this->mPBegin)) / TTraits<T>::ValueSize(); }
        bool empty() const { return (this->mPBegin == this->mPEnd); }
        
        iterator begin() { return iterator(this->mPBegin); }
        const_iterator begin() const { return const_iterator(this->mPBegin); }
        iterator end() { return iterator(this->mPEnd); }
        const_iterator end() const { return const_iterator(this->mPEnd); }

        reverse_iterator  rbegin()       { return reverse_iterator(end()); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        reverse_iterator rend()         { return reverse_iterator(begin()); }
        const_reverse_iterator rend() const   { return const_reverse_iterator(begin()); }
        
    private:
        pointer   mPBegin;
        pointer   mPEnd;

        friend class ResArray<const T, TTraits>;
    };

    template <typename T, template <typename> class TTraits>
    class ResArray<const T, TTraits>{
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
          : mPBegin( static_cast<const_pointer>(pBegin) ), 
            mPEnd( static_cast<const_pointer>(pEnd) ) 
        {}
        ResArray( const void* pBegin, s32 num )
          : mPBegin( static_cast<const_pointer>(pBegin) ), 
            mPEnd( mPBegin + num )
        {}

        ResArray( const ResArray<T, TTraits>& rhs ): 
        mPBegin(rhs.mPBegin),
            mPEnd(rhs.mPEnd)
        {}
        
        operator const pointer() const { return mPBegin; }
        
        const_reference operator[](int index) const{
            return TTraits<T>::GetValue(mPBegin + index);
        }
        
        class const_iterator{
        public:
            typedef const_iterator  TIt;
            typedef T                                       value_type;
            typedef typename TTraits<T>::pointer            pointer;
            typedef typename TTraits<T>::reference          reference;
            typedef typename TTraits<T>::const_pointer      const_pointer;
            typedef typename TTraits<T>::const_reference    const_reference;
            typedef typename TTraits<T>::difference_type    difference_type;
            typedef typename TTraits<T>::iterator_category  iterator_category;
            
            const_iterator() : mPCurrent( NULL ) {}
            
            const_reference operator*()  const { NW_NULL_ASSERT(mPCurrent); return TTraits<T>::GetValue(mPCurrent); }

            const_iter_pointer   operator->() const { return TTraits<T>::GetPointer(mPCurrent); }
            
            TIt &operator++()   { ++mPCurrent; return *this; }
            TIt operator++(int) { TIt it(*this); (void)++*this; return it; }
            TIt &operator--()   { --mPCurrent; return *this; }
            TIt operator--(int) { TIt it(*this); (void)--*this; return it; }
            
            friend bool operator==(TIt it1, TIt it2) { return it1.mPCurrent == it2.mPCurrent; }
            friend bool operator!=(TIt it1_, TIt it2_) { return !(it1_ == it2_); }
            
        private:
            explicit const_iterator(const_pointer p) : mPCurrent(p) {}
            const_pointer mPCurrent;
            friend class ResArray<const T, TTraits>;
        };
        
        typedef internal::reverse_iterator<const_iterator>  const_reverse_iterator;
        
        s32 size() const { return (size_t(mPEnd) - size_t(mPBegin)) / TTraits<T>::ValueSize(); }
        bool empty() const { return (mPBegin == mPEnd); }
        const_iterator begin() const { return const_iterator(mPBegin); }
        const_iterator end() const   { return const_iterator(mPEnd); }
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }
    private:
        const_pointer   mPBegin;
        const_pointer   mPEnd;

        friend class ResArray<T, TTraits>;
    };

}

}
}