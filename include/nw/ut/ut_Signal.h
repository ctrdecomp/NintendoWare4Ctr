#pragma once

#include <nw/types.h>
#include <nw/os/os_Memory.h>
#include <nw/ut/ut_MoveArray.h>

namespace nw{
namespace ut{

template<typename TSlot>
struct SlotDestroyer : public std::unary_function<TSlot&, void>
{
    void operator()(TSlot& slot) const
    {
        slot->Destroy();
    }
};

template<typename TArray>
inline void DestroyAllSlots(TArray& array)
{
    std::for_each(array.begin(), array.end(), SlotDestroyer<typename TArray::value_type>());
    array.clear();
}

template<typename TResult>
struct LastValueResult
{
    typedef TResult ResultType;
    template<typename TInputIterator, typename TInvoker>
    ResultType operator()(TInputIterator first, TInputIterator last, const TInvoker& invoker)
    {
        ResultType result;
        while (first != last)
        {
            result = invoker(*first);
            ++first;
        }
        return result;
    }
};

template<>
struct LastValueResult<void>
{
    template<typename TInputIterator, typename TInvoker>
    void operator()(TInputIterator first, TInputIterator last, const TInvoker& invoker)
    {
        while (first != last)
        {
            invoker(*first);
            ++first;
        }
    }
};

template<typename TResult,typename TArg0, typename TArg1, typename TAllocator = nw::os::IAllocator>
class Slot2
{
public:
    typedef TResult ResultType;
    typedef TAllocator AllocatorType;

    explicit Slot2(AllocatorType* allocator): 
        m_Allocator(allocator) {}

    void Destroy()
    {
        AllocatorType* allocator = this->GetAllocator();
        if (allocator)
        {
            this->~Slot2();
            allocator->Free(this);
        }
    }

    void Destroy(AllocatorType* allocator)
    {
        NW_NULL_ASSERT(allocator);
        this->~Slot2();
        allocator->Free(this);
    }

    virtual ResultType Invoke(TArg0 arg0, TArg1 arg1) = 0;
    
    AllocatorType* GetAllocator() { return m_Allocator; }
    
protected:
    virtual ~Slot2() {}
    
private:
    AllocatorType* m_Allocator;
};

template<typename TResult,typename TArg0, typename TArg1, typename TArg2, typename TAllocator = os::IAllocator>
class Slot3
{
public:
    typedef TResult ResultType;
    typedef TAllocator AllocatorType;

    explicit Slot3(AllocatorType* allocator) : 
        m_Allocator(allocator) {}

    void Destroy()
    {
        AllocatorType* allocator = this->GetAllocator();
        if (allocator)
        {
            this->~Slot3();
            allocator->Free(this);
        }
    }

    void Destroy(AllocatorType* allocator)
    {
        NW_NULL_ASSERT(allocator);
        this->~Slot3();
        allocator->Free(this);
    }

    virtual ResultType Invoke(TArg0 arg0, TArg1 arg1, TArg2 arg2) = 0;
    
    AllocatorType* GetAllocator() { return m_Allocator; }
    
protected:
    virtual ~Slot3() {}
    
private:
    AllocatorType* m_Allocator;
};


template<typename TResult,typename TArg0, typename TArg1, typename TResultCombiner = LastValueResult<TResult>,typename TAllocator = os::IAllocator,typename TSlot = Slot2<TResult, TArg0, TArg1, TAllocator> >
class Signal2
{
    NW_DISALLOW_COPY_AND_ASSIGN(Signal2);
    
public:
    typedef Signal2<TResult, TArg0, TArg1, TResultCombiner, TAllocator, TSlot> SelfType;
    typedef TSlot SlotType;
    typedef TResult ResultType;
    typedef ut::MoveArray<SlotType*> SlotList;
    typedef TResultCombiner ResultCombinerType;
    typedef TAllocator AllocatorType;

    struct Invoker
    {
        Invoker(TArg0 arg0, TArg1 arg1): 
            marg0(arg0), 
            marg1(arg1) {}
        ResultType operator()(SlotType* slot) const
        {
            return slot->Invoke(marg0, marg1);
        }
        TArg0 marg0;
        TArg1 marg1;
    };


    static SelfType* CreateInvalidateSignal(AllocatorType* allocator)
    {
        void* memory = allocator->Alloc(sizeof(SelfType));
        if (memory)
        {
            return new(memory) SelfType(NULL, 0, allocator);
        }
        else{
            return NULL;
        }
    }


    static SelfType* CreateFixedSizedSignal(size_t maxSlots, AllocatorType* allocator)
    {
        void* memory = allocator->Alloc(sizeof(SelfType));
        if (memory)
        {
            void* elements = allocator->Alloc(sizeof(SlotType*) * maxSlots);
            if (elements != NULL)
            {
                return new(memory) SelfType(elements, maxSlots, allocator);
            }
            else
            {
                allocator->Free(memory);
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }

    static SelfType* CreateVariableSizeSignal(AllocatorType* allocator)
    {
        void* memory = allocator->Alloc(sizeof(SelfType));
        if (memory)
        {
            return new(memory) SelfType(allocator);
        }
        else
        {
            return NULL;
        }
    }

    void Destroy() { this->~Signal2(); this->m_Allocator->Free(this); }

    ResultType operator()(TArg0 arg0, TArg1 arg1)
    {
        ResultCombinerType combiner;
        return combiner(this->m_Slots.begin(), this->m_Slots.end(), Invoker(arg0, arg1));
    }

    static size_t GetMemorySizeForInvalidateSignal(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeForInvalidateSignalInternal(&size);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeForInvalidateSignalInternal(nw::os::MemorySizeCalculator* pSize)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(SelfType);
    }

    static size_t GetMemorySizeForFixedSizedSignal(size_t maxSlots, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeForFixedSizedSignalInternal(&size, maxSlots);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeForFixedSizedSignalInternal(nw::os::MemorySizeCalculator* pSize, size_t maxSlots)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(SelfType);

        size += sizeof(SlotType*) * maxSlots;
    }

private:
    Signal2(void* elements, size_t maxSlots, AllocatorType* allocator): 
        m_Allocator(allocator), m_Slots(elements, maxSlots, allocator) {}
    explicit Signal2(AllocatorType* allocator): 
        m_Allocator(allocator), m_Slots(allocator) {}

    ~Signal2() { DestroyAllSlots(this->m_Slots); }

    AllocatorType* m_Allocator;

    SlotList m_Slots;
};

template<typename TResult,typename TArg0, typename TArg1, typename TArg2, typename TResultCombiner = LastValueResult<TResult>,typename TAllocator = os::IAllocator,typename TSlot = Slot3<TResult, TArg0, TArg1, TArg2, TAllocator> >
class Signal3
{
    NW_DISALLOW_COPY_AND_ASSIGN(Signal3);
    
public:
    typedef Signal3<TResult, TArg0, TArg1, TArg2, TResultCombiner, TAllocator, TSlot> SelfType;
    typedef TSlot SlotType;
    typedef TResult ResultType;
    typedef ut::MoveArray<SlotType*> SlotList;
    typedef TResultCombiner ResultCombinerType;
    typedef TAllocator AllocatorType;

    struct Invoker
    {
        Invoker(TArg0 arg0, TArg1 arg1, TArg2 arg2): 
            marg0(arg0),
            marg1(arg1),
            marg2(arg2) {}
        ResultType operator()(SlotType* slot) const
        {
            return slot->Invoke(marg0, marg1, marg2);
        }
        TArg0 marg0;
        TArg1 marg1;
        TArg2 marg2;
    };


    static SelfType* CreateInvalidateSignal(AllocatorType* allocator)
    {
        void* memory = allocator->Alloc(sizeof(SelfType));
        if (memory)
        {
            return new(memory) SelfType(NULL, 0, allocator);
        }
        else
        {
            return NULL;
        }
    }


    static SelfType* CreateFixedSizedSignal(size_t maxSlots, AllocatorType* allocator)
    {
        void* memory = allocator->Alloc(sizeof(SelfType));
        if (memory)
        {
            void* elements = allocator->Alloc(sizeof(SlotType*) * maxSlots);
            if (elements != NULL)
            {
                return new(memory) SelfType(elements, maxSlots, allocator);
            }
            else
            {
                allocator->Free(memory);
                return NULL;
            }
        }
        else
        {
            return NULL;
        }
    }

    static SelfType* CreateVariableSizeSignal(AllocatorType* allocator)
    {
        void* memory = allocator->Alloc(sizeof(SelfType));
        if (memory)
        {
            return new(memory) SelfType(allocator);
        }
        else
        {
            return NULL;
        }
    }

    void Destroy() { this->~Signal3(); this->m_Allocator->Free(this); }

    ResultType operator()(TArg0 arg0, TArg1 arg1)
    {
        ResultCombinerType combiner;
        return combiner(this->m_Slots.begin(), this->m_Slots.end(), Invoker(arg0, arg1));
    }

    static size_t GetMemorySizeForInvalidateSignal(size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeForInvalidateSignalInternal(&size);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeForInvalidateSignalInternal(nw::os::MemorySizeCalculator* pSize)
    {
        nw::os::MemorySizeCalculator& size = *pSize;

        size += sizeof(SelfType);
    }

    static size_t GetMemorySizeForFixedSizedSignal(size_t maxSlots, size_t alignment = nw::os::IAllocator::DEFAULT_ALIGNMENT)
    {
        nw::os::MemorySizeCalculator size(alignment);

        GetMemorySizeForFixedSizedSignalInternal(&size, maxSlots);

        return size.GetSizeWithPadding(alignment);
    }

    static void GetMemorySizeForFixedSizedSignalInternal(nw::os::MemorySizeCalculator* pSize, size_t maxSlots)
    {
        os::MemorySizeCalculator& size = *pSize;

        size += sizeof(SelfType);

        size += sizeof(SlotType*) * maxSlots;
    }

private:
    Signal3(void* elements, size_t maxSlots, AllocatorType* allocator): 
        m_Allocator(allocator), m_Slots(elements, maxSlots, allocator) {}
    explicit Signal3(AllocatorType* allocator): 
        m_Allocator(allocator), m_Slots(allocator) {}

    ~Signal3() { DestroyAllSlots(this->m_Slots); }

    AllocatorType* m_Allocator;
    SlotList m_Slots;
};
}
}