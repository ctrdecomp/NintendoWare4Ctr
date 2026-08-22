#pragma once

#include <nw/types.h>
#include <nw/Assert.h>
#include <nn/os.h>

namespace nw{
namespace os{

__forceinline void MemCpy(void* dstp, const void* srcp, size_t size){
  #if defined(NW_PLATFORM_CTR)
    nnnstdMemCpy(dstp, srcp, size);
  #else
    std::memcpy(dstp, srcp, size);
  #endif
}

#define NW_CHAR_TRAITS_COPY(MType, MDest, MDestSize, MFrom, MCount) std::char_traits<MType>::copy(MDest, MFrom, MCount)

class IAllocator{
public:
    static const int DEFAULT_ALIGNMENT = 4;
    static const int CACHE_LINE_ALIGNMENT = 32;

    virtual ~IAllocator(){}
    virtual void* Alloc(size_t size, u8 alignment) = 0;
    virtual void  Free(void* memory) = 0;

    void* Alloc(size_t size) { return this->Alloc(size, DEFAULT_ALIGNMENT); }

    template<typename TObject>
    TObject* Alloc(int count, u8 alignment = DEFAULT_ALIGNMENT){
        NW_ASSERT(count >= 0);
        return static_cast<TObject*>(this->Alloc(sizeof(TObject) * count, alignment));
    }

    template<typename TObject>
    TObject* AllocAndConstruct(int count, u8 alignment = DEFAULT_ALIGNMENT){
        NW_ASSERT(count >= 0);
        TObject* objects = static_cast<TObject*>(this->Alloc(sizeof(TObject) * count, alignment));

        if (objects != NULL){
            for (int i = 0; i < count; ++i){
                this->Construct(&objects[i]);
            }
        }

        return objects;
    }

    template<typename TObject>
    TObject* AllocAndFill(int count, const TObject& object, u8 alignment = DEFAULT_ALIGNMENT){
        NW_ASSERT(count >= 0);
        TObject* objects = static_cast<TObject*>(this->Alloc(sizeof(TObject) * count, alignment));
        
        if (objects != NULL){
            std::fill_n(objects, count, object);
        }

        return objects;
    }

private:
    template<typename TObject>
    void Construct(TObject* object){
        new(static_cast<void*>(object)) TObject;
    }
};

template<typename TObject>
inline TObject* AllocateAndFill(os::IAllocator* allocator, u8 value){
    NW_NULL_ASSERT(allocator);
    const size_t memorySize = sizeof(TObject);
    u8* memory = static_cast<u8*>(allocator->Alloc(memorySize));
    NW_NULL_ASSERT(memory);
    
    std::fill_n(memory, memorySize, value);
    
    return reinterpret_cast<TObject*>(memory);
}

template<typename TObject>
inline TObject* AllocateAndFillN(os::IAllocator* allocator, size_t size, u8 value){
    NW_NULL_ASSERT(allocator);
    NW_ASSERT(sizeof(TObject) <= size);
    u8* memory = static_cast<u8*>(allocator->Alloc(size));
    NW_NULL_ASSERT(memory);
    
    std::fill_n(memory, size, value);
    
    return reinterpret_cast<TObject*>(memory);
}

template<typename TChar>
inline TChar* AllocateAndCopyString(const TChar* str, os::IAllocator* allocator, size_t maxSize){
    if (str == NULL) { return NULL; }
    
    size_t length = std::char_traits<TChar>::length(str);
    length = (length < maxSize)? length : maxSize;

    size_t bufferSize = length + 1;
    TChar* copyStr = reinterpret_cast<char*>(allocator->Alloc(bufferSize));
    NW_NULL_ASSERT(copyStr);
    NW_CHAR_TRAITS_COPY(TChar, copyStr, bufferSize, str, length);
    copyStr[length] = '\0';
    
    return copyStr;
}

template<typename TMemory>
inline void SafeFree(TMemory*& memory, IAllocator* allocator){
    if (memory == NULL){
        return;
    }
    allocator->Free(static_cast<void*>(memory));
    memory = NULL;
}

template<typename TMemory> 
struct SafeFreeFunctor : public std::unary_function<TMemory&, void>{
    SafeFreeFunctor(IAllocator* allocator) : 
        mAllocator(allocator) 
    {}

    void operator()(TMemory& memory) const{
        SafeFree(memory, this->mAllocator);
    }

    IAllocator* mAllocator;
};

template<typename TArray>
inline void SafeFreeAll(TArray& array, IAllocator* allocator){
    std::for_each(array.begin(), array.end(),SafeFreeFunctor<typename TArray::value_type>(allocator));
    array.clear();
}

inline bool IsDeviceMemory(const void* memory){
    if (nn::os::GetDeviceMemoryAddress() <= (uint)memory && (uint)memory < nn::os::GetDeviceMemoryAddress() + nn::os::GetDeviceMemorySize()){
        return true;
    }
    else{
        return false;
    }
}

class MemorySizeCalculator{
public:
    static const size_t MAX_ALIGNMENT = IAllocator::CACHE_LINE_ALIGNMENT;

    MemorySizeCalculator(size_t alignment): 
        mSize(0),
        mAlignment(alignment),
        mMaxAlignment(alignment){
        NW_ASSERT(alignment <= MAX_ALIGNMENT);
        NW_ASSERTMSG(alignment != 0 && (alignment & alignment - 1) == 0, "alignment must be power of 2");
    }

    MemorySizeCalculator& operator+=(size_t size){
        mSize = ut::RoundUp(this->mSize, this->mAlignment) + size;
        return *this;
    }

    void ChangeAlignment(size_t alignment){
        NW_ASSERT(alignment <= MAX_ALIGNMENT);
        NW_ASSERTMSG(alignment != 0 && (alignment & alignment - 1) == 0, "alignment must be power of 2");
        mAlignment = alignment;
        mMaxAlignment = ut::Max(this->mMaxAlignment, this->mAlignment);
    }

    void Add(size_t size, size_t immediateAlignment){
        size_t oldAlignment = mAlignment;
        
        ChangeAlignment(immediateAlignment);
        *this += size;
        ChangeAlignment(oldAlignment);
    }

    MemorySizeCalculator& operator*=(int operand){
        mSize = ut::RoundUp(this->mSize, this->mAlignment) * operand;
        return *this;
    }

    size_t GetAlignment() const {
        return mAlignment;
    }

    size_t GetMaxAlignment() const {
        return mMaxAlignment;
    }

    size_t GetSizeWithPadding(size_t alignment) const {
        return mSize + mMaxAlignment - alignment;
    }

private:
    size_t mSize;
    size_t mAlignment;

    size_t mMaxAlignment;
};

}
}