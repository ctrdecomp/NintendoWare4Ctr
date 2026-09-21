// Filename: ut_FrameHeap.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_FrameHeap.h>
#include <nw/ut/ut_Inlines.h>
#include <new>

namespace nw {
namespace ut {

FrameHeap* FrameHeap::Create(void* startAddress, u32 size, u16 optFlag)
{
    void* endAddress;

    NW_ASSERT(startAddress != NULL);

    endAddress = RoundDown(AddOffsetToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = RoundUp(startAddress, MIN_ALIGNMENT);

    if (ComparePtr(startAddress, endAddress) > 0 || GetOffsetFromPtr(startAddress, endAddress) < sizeof(FrameHeap))
    {
        return NULL;
    }

    {
        FrameHeap* pHeap = new(startAddress) FrameHeap;

        pHeap->Initialize(FRMHEAP_SIGNATURE,AddOffsetToPtr(pHeap, sizeof(FrameHeap)),endAddress,optFlag);

        pHeap->m_HeadAllocator = pHeap->GetHeapStart();
        pHeap->m_TailAllocator = pHeap->GetHeapEnd();
        pHeap->m_pState = NULL;

        return pHeap;
    }
}

void* FrameHeap::Destroy()
{
    Finalize();
    return this;
}

void* FrameHeap::Alloc(u32 size, int alignment)
{
    void* memory = NULL;

    if (size == 0)
    {
        size = 1;
    }
    size = RoundUp( size, MIN_ALIGNMENT );
    
    LockHeap();

    if (alignment >= 0)
    {
        memory = this->AllocFromHead(size, alignment);
    }
    else{
        memory = this->AllocFromTail(size, -alignment);
    }

    UnlockHeap();

    return memory;
}

void* FrameHeap::AllocFromHead(u32 size, int alignment)
{
    void* newBlock = RoundUp(m_HeadAllocator, (u32)alignment);
    void* endAddress = AddOffsetToPtr(newBlock, size);

    if (GetIntPtr(endAddress) > GetIntPtr(this->m_TailAllocator))
    {
        return NULL;
    }

    FillAllocMemory(m_HeadAllocator,(u32)GetOffsetFromPtr(this->m_HeadAllocator, endAddress));

    m_HeadAllocator = endAddress;

    return newBlock;
}

void* FrameHeap::AllocFromTail( u32 size, int alignment )
{
    void* newBlock = RoundDown(AddOffsetToPtr(this->m_TailAllocator, -static_cast<s32>(size) ), static_cast<u32>(alignment));

    if (GetIntPtr(newBlock) < GetIntPtr(m_HeadAllocator))
    {
        return NULL;
    }

    FillAllocMemory(newBlock,(u32)GetOffsetFromPtr(newBlock, m_TailAllocator));

    m_TailAllocator = newBlock;

    return newBlock;
}

void FrameHeap::Free(int mode)
{
    NW_ASSERT(IsValid());

    this->LockHeap();

    if (mode & FREE_HEAD)
    {
        this->FreeHead();
    }

    if (mode & FREE_TAIL)
    {
        this->FreeTail();
    }

    this->UnlockHeap();
}

void FrameHeap::FreeHead()
{
    FillFreeMemory(this->m_HeapStart, (u32)GetOffsetFromPtr(this->m_HeapStart, this->m_HeadAllocator));
    m_HeadAllocator = m_HeapStart;
    m_pState = NULL;
}

void FrameHeap::FreeTail()
{
    FillFreeMemory(m_TailAllocator, (u32)GetOffsetFromPtr(this->m_TailAllocator, this->m_HeapEnd));

    for (HeapState* pState = m_pState; pState; pState = pState->pPrevState)
    {
        pState->tailAllocator = m_HeapEnd;
    }
    m_TailAllocator = m_HeapEnd;
}

bool FrameHeap::RecordState(u32 tagName)
{
    bool retVal;

    this->LockHeap();

    {
        void* oldHeadAllocator = m_HeadAllocator;
        void* stateHeap = AllocFromHead(sizeof(HeapState), MIN_ALIGNMENT);

        if (stateHeap == NULL)
        {
            retVal = false;
        }
        else{
            HeapState* pState = new(stateHeap) HeapState;
            if (pState == NULL)
            {
                retVal = false;
            }
            else{
                pState->tagName         = tagName;
                pState->headAllocator   = oldHeadAllocator;
                pState->tailAllocator   = m_TailAllocator;
                pState->pPrevState      = m_pState;

                m_pState = pState;
                retVal = true;
            }
        }
    }

    this->UnlockHeap();

    return retVal;
}

bool FrameHeap::FreeByState( u32 tagName )
{
    bool retVal;

    this->LockHeap();

    {
        HeapState* pState = m_pState;

        if (tagName != 0)
        {
            for (; pState; pState = pState->pPrevState)
            {
                if (pState->tagName == tagName)
                {
                    break;
                }
            }
        }

        if (pState == NULL)
        {
            retVal = false;
        }
        else{
            void* oldHeadAllocator = m_HeadAllocator;
            void* oldTailAllocator = m_TailAllocator;

            m_HeadAllocator = pState->headAllocator;
            m_TailAllocator = pState->tailAllocator;
            m_pState = pState->pPrevState;

            FillFreeMemory(m_HeadAllocator,(u32)GetOffsetFromPtr(this->m_HeadAllocator, oldHeadAllocator));
            FillFreeMemory(oldTailAllocator,(u32)GetOffsetFromPtr(oldTailAllocator, this->m_TailAllocator));

            retVal = true;
        }
    }

    this->UnlockHeap();

    return retVal;
}

}
}