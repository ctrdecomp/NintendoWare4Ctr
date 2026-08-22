#pragma once

#include <nw/ut/ut_Preprocessor.h>
#include <nw/ut/ut_TypeTraits.h>
#include <nw/ut/ut_Flag.h>
#include <nw/os/os_Memory.h>

#include <iterator>
#include <algorithm>

namespace nw {
namespace ut {


namespace detail {

template<typename From, typename To>
struct is_convertible{
    static char test(To*);
    static int test(...);
    static const bool value = sizeof(test(static_cast<From*>(NULL))) == 1;
};

} // namespace detail

enum ArrayKind{
    ARRAY_WRAPPER,
    ARRAY_VARIABILITY,
    ARRAY_FIXED,

    NW_FLAG_VALUE_DECLARE(ARRAY_KIND, 30, 2)
};

template<typename TElement>
class MoveArray{
public:
    static const size_t MEMORY_ALIGNMENT = os::IAllocator::CACHE_LINE_ALIGNMENT;

    typedef TElement& reference;
    typedef TElement difference_type;
    typedef TElement value_type;
    typedef TElement* iterator;
    typedef const TElement* const_iterator;

    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

public:
    MoveArray():
        mAllocator(NULL),
        mElements(NULL),
        mEnd(NULL){
        SetCapacity(0);
        SetArrayKind(ARRAY_WRAPPER);
    }

    template<typename TTElemet>
    MoveArray(TTElemet* elements, size_t capacity, os::IAllocator* allocator = NULL, ArrayKind kind = ARRAY_WRAPPER):
        mAllocator(allocator),
        mElements(reinterpret_cast<TElement*>(elements)),
        mEnd(reinterpret_cast<TElement*>(elements)){
        SetCapacity(capacity);
        SetArrayKind(kind);
    }

    MoveArray(size_t capacity, os::IAllocator* allocator, ArrayKind kind = ARRAY_WRAPPER):
        mAllocator(allocator)
    {
        NW_NULL_ASSERT(allocator);
        if (0 < capacity){
            mElements = static_cast<TElement*>(allocator->Alloc(sizeof(TElement) * capacity, MEMORY_ALIGNMENT));

            NW_NULL_ASSERT(mElements);
        }
        else{
            mElements = NULL;
        }
        mEnd = mElements;
        SetCapacity(capacity);
        SetArrayKind(kind);
    }

    MoveArray(os::IAllocator* allocator):
        mAllocator(allocator),
        mElements(NULL),
        mEnd(NULL){
        NW_NULL_ASSERT(allocator);
        SetCapacity(0);
        SetArrayKind(ARRAY_VARIABILITY);
    }

    MoveArray(const MoveArray& array):
        mAllocator(array.mAllocator),
        mElements(array.mElements),
        mEnd(array.mEnd),
        mCapacity(array.mCapacity){
        const_cast<MoveArray&>(array).release();
    }

    ~MoveArray(){
        clear();
        if (mAllocator && mElements){
            mAllocator->Free(mElements);
        }
    }

public:
    struct SafeBoolHelper { int x; };
    typedef int SafeBoolHelper::* SafeBool;

    operator SafeBool() const{
#ifdef NW_MOVE_ARRAY_VARIABILITY_ENABLED
        return (capacity() == 0 && GetArrayKind() != ARRAY_VARIABILITY)
#else
        return (capacity() == 0)
#endif
            ? 0 : &SafeBoolHelper::x;
    }

    MoveArray& operator=(MoveArray rhs){
        rhs.swap(*this);
        return *this;
    }

    const TElement* Elements() const { return mElements; }

    TElement& operator[](int index){
        return mElements[index];
    }

    const TElement& operator[](int index) const{
        return mElements[index];
    }

    TElement* release(){
        NW_ASSERT(GetArrayKind() != ARRAY_FIXED);
        TElement* result = mElements;
        mElements = 0;
        mEnd = 0;
        SetCapacity(0);
        return result;
    }

    void Reset(void* elements, size_t size, os::IAllocator* allocator = NULL, ArrayKind kind = ARRAY_WRAPPER){
        clear();
        if (mAllocator && mElements){
            mAllocator->Free(mElements);
        }
        release();
        mAllocator = allocator;
        mElements = static_cast<TElement*>(elements);
        mEnd = static_cast<TElement*>(elements);
        SetCapacity(size);
        SetArrayKind(kind);
    }

    int size() const { return mEnd - mElements; }

    iterator begin() { return mElements; }
    const_iterator begin() const { return mElements; }
    iterator end() { return mEnd; }
    const_iterator end() const { return mEnd; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    TElement& front() { return *mElements; }
    const TElement& front() const { return *mElements; }
    TElement& back() { return *(mEnd - 1); }
    const TElement& back() const { return *(mEnd - 1); }

    bool empty() const { return mElements == mEnd; }
    int capacity() const { return GetCapacity(); }

    void CopyFrom(const MoveArray<TElement>& source){
        clear();
        this->resize(source.size());
        std::copy(source.begin(), source.end(), NW_CHECKED_ARRAY_ITERATOR(this->begin(), this->size()));
    }

    bool reserve(int reserveSize);
    bool push_back(const TElement& element);
    void pop_back();
    bool assign(int size, const TElement& element);
    void swap(MoveArray<TElement>& other);
    bool resize(int number);

    template<typename TIterator>
    TIterator erase(TIterator first);

    template<typename TIterator>
    TIterator erase(TIterator first, TIterator last);

    void clear() { erase(begin()); }

    bool erase_find(const TElement& element){
        iterator removed = std::remove(begin(), end(), element);
        bool IsErased = removed != end();
        erase(removed);
        return IsErased;
    }

    template<typename Predicate>
    bool erase_if(Predicate predicate){
        iterator removed = std::remove_if(begin(), end(), predicate);
        bool IsErased = removed != end();
        erase(removed);
        return IsErased;
    }

    void ShrinkToFit(){
        if (GetArrayKind() == ARRAY_VARIABILITY){
            MoveArray clone(size(), mAllocator, GetArrayKind());
            clone.CopyFrom(*this);
            clone.swap(*this);
        }
    }

    ArrayKind GetArrayKind() const{
        return GetFlagValue<ArrayKind>(mCapacity, FLAG_ARRAY_KIND_VALUE_SHIFT, FLAG_ARRAY_KIND_VALUE_MASK);
    }

    os::IAllocator& GetAllocator() { return *mAllocator; }

protected:
    os::IAllocator* mAllocator;
    TElement* mElements;
    TElement* mEnd;

    void SetArrayKind(ArrayKind kind){
        mCapacity = SetFlagValue(mCapacity, FLAG_ARRAY_KIND_VALUE_SHIFT, FLAG_ARRAY_KIND_VALUE_MASK, kind);
    }

    void SetCapacity(int capacity){
        mCapacity = (mCapacity & static_cast<size_t>(FLAG_ARRAY_KIND_VALUE_MASK)) | static_cast<size_t>(capacity);
    }

    int GetCapacity() const{
        return mCapacity & (~FLAG_ARRAY_KIND_VALUE_MASK);
    }

private:
    size_t mCapacity;

    void construct(TElement* element, const TElement& value){
        new(static_cast<void*>(element)) TElement(value);
    }

    void construct(TElement* element){
        new(static_cast<void*>(element)) TElement();
    }

    void destroy(TElement* element){
        NW_UNUSED_VARIABLE(element);
        element->~TElement();
    }

    void move_construct(iterator dest, const TElement& source){
        new(static_cast<void*>(dest)) TElement(source);
    }

    void destroy_range(iterator first, iterator last){
        if (IsClass<TElement>::value){
            for (; first != last; ++first){
                destroy(first);
            }
        }
    }

public:
    int Size() const { return this->size(); }

    iterator Begin() { return this->begin(); }
    const_iterator Begin() const { return this->begin(); }
    iterator End() { return this->end(); }
    const_iterator End() const { return this->end(); }

    reverse_iterator RBegin() { return this->rbegin(); }
    const_reverse_iterator RBegin() const { return this->rbegin(); }
    reverse_iterator REnd() { return this->rend(); }
    const_reverse_iterator REnd() const { return this->rend(); }

    TElement& Front() { return this->front(); }
    const TElement& Front() const { return this->front(); }
    TElement& Back() { return this->back(); }
    const TElement& Back() const { return this->back(); }

    bool Empty() const { return this->empty(); }
    int Capacity() const { return this->capacity(); }

    bool Reserve(int reserveSize) { return this->reserve(reserveSize); }
    bool PushBack(const TElement& element) { return this->push_back(element); }
    void PopBack() { this->pop_back(); }
    bool Assign(int size, const TElement& element) { return this->assign(size, element); }
    void Swap(MoveArray<TElement>& other) { this->swap(other); }
    bool Resize(int number) { return this->resize(number); }

    template<typename TIterator> TIterator Erase(TIterator first) { return this->erase(first); }
    template<typename TIterator> TIterator Erase(TIterator first, TIterator last) { return this->erase(first, last); }

    void Clear() { this->clear(); }
    void EraseFind(const TElement& element) { this->erase_find(element); }
    template<typename Predicate> void EraseIf(Predicate predicate) { this->erase_if(predicate); }

    template<typename TArg0>
    void PushBackFast(TArg0 arg0){
        new(mEnd++) TElement(arg0);
    }

    template<typename TArg0, typename TArg1>
    void PushBackFast(TArg0 arg0, TArg1 arg1){
        new(mEnd++) TElement(arg0, arg1);
    }

    template<typename TArg0, typename TArg1, typename TArg2>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2){
        new(mEnd++) TElement(arg0, arg1, arg2);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3, arg4);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3, arg4, arg5);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3, arg4, arg5, arg6);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8);
    }

    template<typename TArg0, typename TArg1, typename TArg2, typename TArg3, typename TArg4, typename TArg5, typename TArg6, typename TArg7, typename TArg8, typename TArg9>
    void PushBackFast(TArg0 arg0, TArg1 arg1, TArg2 arg2, TArg3 arg3, TArg4 arg4, TArg5 arg5, TArg6 arg6, TArg7 arg7, TArg8 arg8, TArg9 arg9){
        new(mEnd++) TElement(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9);
    }
};

template<typename TElement, size_t TSize>
class FixedSizeArray : public MoveArray<TElement>{
private:
    NW_DISALLOW_COPY_AND_ASSIGN(FixedSizeArray);
public:
    FixedSizeArray():
        MoveArray<TElement>(reinterpret_cast<TElement*>(mFixedSizeElements), TSize, 0, ARRAY_FIXED)
    {}

private:
    void swap(MoveArray<TElement>& array);
    TElement* release();

    u8 mFixedSizeElements[sizeof(TElement) * TSize];
};

template<typename TElement>
inline bool MoveArray<TElement>::reserve(int reserveSize){
#ifdef NW_MOVE_ARRAY_VARIABILITY_ENABLED
    if (reserveSize <= capacity()){
        return true;
    }

    if (mAllocator == 0 || GetArrayKind() != ARRAY_VARIABILITY){
        return false;
    }

#ifdef NW_MOVE_ARRAY_CACHE_LINE_ALIGNMENT_ENABLED
    TElement* elements = static_cast<TElement*>(mAllocator->Alloc(sizeof(TElement) * reserveSize, MEMORY_ALIGNMENT));
#else
    TElement* elements = static_cast<TElement*>(mAllocator->Alloc(sizeof(TElement) * reserveSize));
#endif
    NW_ASSERT(0 != elements);

    size_t elementsCount = 0;
    if (!empty()){
        std::copy(begin(), end(), NW_CHECKED_ARRAY_ITERATOR(elements, reserveSize));
        elementsCount = size();
    }

    if (0 != mElements){
        NW_ASSERT(0 != mAllocator);
        mAllocator->Free(mElements);
    }

    mElements = elements;
    mEnd = elements + elementsCount;
    SetCapacity(reserveSize);
    return true;
#else
    if (reserveSize <= capacity()){
        return true;
    }
    NW_FATAL_ERROR("Can't increase capacity.");
    return false;
#endif
}

template<typename TElement>
inline bool MoveArray<TElement>::push_back(const TElement& element){
    bool result = true;
    int capacity = GetCapacity();
    if (capacity <= size()){
#ifdef NW_MOVE_ARRAY_VARIABILITY_ENABLED
        int newCapacity = (capacity == 0) ? 1 : capacity * 2;
        result = reserve(newCapacity);
#else
        result = false;
#endif
    }
    if (result){
        construct(mEnd, element);
        ++mEnd;
    }
    return result;
}

template<typename TElement>
inline void MoveArray<TElement>::pop_back(){
    if (!empty()){
        destroy(mEnd - 1);
        --mEnd;
    }
}

template<typename TElement>
inline bool MoveArray<TElement>::assign(int size, const TElement& element){
    clear();
    bool result = reserve(size);
    if (!result){
        size = capacity();
    }
    for (int i = 0; i < size; ++i){
        push_back(element);
    }
    return result;
}

template<typename TElement>
inline void MoveArray<TElement>::swap(MoveArray<TElement>& other){
    if (&other == this){
        return;
    }
    TElement* elements = mElements;
    TElement* end = mEnd;
    size_t capacity = mCapacity;
    os::IAllocator* allocator = mAllocator;

    mElements = other.mElements;
    mEnd = other.mEnd;
    mCapacity = other.mCapacity;
    mAllocator = other.mAllocator;

    other.mElements = elements;
    other.mEnd = end;
    other.mCapacity = capacity;
    other.mAllocator = allocator;
}

template<typename TElement>
inline bool MoveArray<TElement>::resize(int number){
    bool result = true;
    if (number < size()){
        int min = number;
        if (min < 0){
            min = 0;
            result = false;
        }
        if (IsClass<TElement>::value){
            for (int i = min; i < size(); ++i){
                destroy(mElements + i);
            }
        }
        mEnd = mElements + min;
    }
    else{
        int max = number;
        if (capacity() < max){
            if (!reserve(max)){
                result = false;
                max = capacity();
            }
        }
        if (IsClass<TElement>::value){
            for (int i = size(); i < max; ++i){
                construct(mElements + i);
            }
        }
        mEnd = mElements + max;
    }
    return result;
}

template<typename TElement> template<typename TIterator>
inline TIterator MoveArray<TElement>::erase(TIterator first){
    destroy_range(first, end());
    mEnd = first;
    return first;
}

template<typename TElement> template<typename TIterator>
inline TIterator MoveArray<TElement>::erase(TIterator first, TIterator last){
    TIterator dest = first;
    TIterator source = last;
    for (; dest != last && source != end(); ++dest, ++source){
        destroy(dest);
        move_construct(dest, *source);
    }

    if (dest != last){
        destroy_range(dest, last);
        destroy_range(last, end());
    }
    else{
        for (; source != end(); ++dest, ++source){
            destroy(dest);
            move_construct(dest, *source);
        }
        destroy_range(dest, end());
    }

    mEnd = dest;
    return first;
}

} // namespace ut
} // namespace nw

template<typename TElement>
inline bool operator==(const nw::ut::MoveArray<TElement>& lhs, const nw::ut::MoveArray<TElement>& rhs){
    if (lhs.size() != rhs.size()){
        return false;
    }
    return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template<typename TElement>
inline bool operator!=(const nw::ut::MoveArray<TElement>& lhs, const nw::ut::MoveArray<TElement>& rhs){
    if (lhs.size() != rhs.size()){
        return false;
    }
    return !(lhs == rhs);
}

template<typename TElement>
inline bool operator<(const nw::ut::MoveArray<TElement>& lhs, const nw::ut::MoveArray<TElement>& rhs){
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template<typename TElement>
inline bool operator>(const nw::ut::MoveArray<TElement>& lhs, const nw::ut::MoveArray<TElement>& rhs){
    return rhs < lhs;
}

template<typename TElement>
inline bool operator<=(const nw::ut::MoveArray<TElement>& lhs, const nw::ut::MoveArray<TElement>& rhs){
    return !(rhs < lhs);
}

template<typename TElement>
inline bool operator>=(const nw::ut::MoveArray<TElement>& lhs, const nw::ut::MoveArray<TElement>& rhs){
    return !(lhs < rhs);
}