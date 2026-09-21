// Filename: ut_HeapBase.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_HeapBase.h>
#include <nw/ut/ut_Inlines.h>
#include <cstring>

#define     GetBitValue(data, st, bits) \
    (((data) >>(st)) & ((1 <<(bits)) -1))

#define     SetBitValue(data, st, bits, val)                        \
                do                                                          \
                {                                                           \
                    u32 maskBits = (u32)((1 <<(bits)) -1);                   \
                    u32 newVal = (val) & maskBits; /* 安全のためマスク */    \
                    (void)(maskBits <<= st);                                 \
                    (data) &= ~maskBits; /* セットする領域をクリア */        \
                    (data) |= newVal <<(st);                                 \
                } while (false);

namespace nw {
namespace ut {
namespace{

HeapBase::HeapList s_RootList;

u32 s_FillVals[HeapBase::HEAP_FILL_MAX] ={
    0xC3C3C3C3,
    0xF3F3F3F3,
    0xD3D3D3D3,
};

}

HeapBase::HeapList* HeapBase::FindListContainHeap(HeapBase* pHeapBase)
{
    HeapBase::HeapList* pList = &s_RootList;
    HeapBase* pContainHeapBase = HeapBase::FindContainHeap(&s_RootList, pHeapBase);

    if (pContainHeapBase)
    {
        pList = &pContainHeapBase->m_ChildList;
    }

    return pList;
}

HeapBase* HeapBase::FindContainHeap(HeapBase::HeapList* pList, const void* memBlock)
{
    u32 memBlockAddress = reinterpret_cast<u32>(memBlock);

    for (HeapList::iterator itr = pList->begin(); itr != pList->end();)
    {
        HeapList::iterator curItr = itr++;
        if (reinterpret_cast<u32>(curItr->m_HeapStart) <= memBlockAddress && reinterpret_cast<u32>(curItr->m_HeapEnd) > memBlockAddress )
        {
            HeapBase* pChildHeapBase = FindContainHeap( &curItr->m_ChildList, memBlock );
            if (pChildHeapBase)
            {
                return pChildHeapBase;
            }
            return &(*curItr);
        }
    }
    return NULL;
}

HeapBase* HeapBase::FindContainHeap(const void* memBlock)
{
    return FindContainHeap(&s_RootList, memBlock);
}

u32 HeapBase::GetFillValue(FillType type)
{
    return s_FillVals[type];
}

void HeapBase::Initialize(u32 signature, void* heapStart, void* heapEnd, u16 optFlag)
{
    m_Signature = signature;

    m_HeapStart = heapStart;
    m_HeapEnd = heapEnd;

    m_Attribute = 0;
    SetOptionFlag(optFlag);
    
    FillNoUseMemory(heapStart, (u32)GetOffsetFromPtr(heapStart, heapEnd));

    HeapList* pList = FindListContainHeap(this);
    pList->push_back(this);
}

void HeapBase::Finalize()
{
    HeapList* pList = FindListContainHeap(this);
    pList->erase(this);
    m_Signature = 0;
}

void HeapBase::LockHeap() {}

void HeapBase::UnlockHeap() {}

void HeapBase::FillFreeMemory(void* address, u32 size)
{
    if (this->GetOptionFlag() & OPT_DEBUG_FILL)
    {
        std::memset(address, GetFillValue( HEAP_FILL_FREE), size);
    }
}

void HeapBase::FillNoUseMemory(void* address, u32 size)
{
    if (this->GetOptionFlag() & OPT_DEBUG_FILL)
    {
        std::memset(address, GetFillValue(HEAP_FILL_NOUSE), size);
    }
}

void HeapBase::FillAllocMemory(void* address, u32 size)
{
    if (this->GetOptionFlag() & OPT_0_CLEAR)
    {
        std::memset(address, 0, size);
    }
    else{
        if (this->GetOptionFlag() & OPT_DEBUG_FILL)
        {
            std::memset(address, GetFillValue(HEAP_FILL_ALLOC), size);
        }
    }
}

u16 HeapBase::GetOptionFlag()
{
    return (u16)GetBitValue(this->m_Attribute, 0, 8);
}

void HeapBase::SetOptionFlag(u16 optFlag)
{
    SetBitValue(this->m_Attribute, 0, 8, optFlag);
}

}
}