// Filename: ut_FrameHeap.cpp
//
// Project: NintendoWare4Ctr

#include <nw/ut/ut_FrameHeap.h>
#include <nw/ut/ut_Inlines.h>
#include <new>

namespace nw {
namespace ut {

FrameHeap* FrameHeap::Create(void* startAddress, u32 size, u16 optFlag){
    void* endAddress;

    NW_ASSERT(startAddress != NULL);

    endAddress = RoundDown(AddOffsetToPtr(startAddress, size), MIN_ALIGNMENT);
    startAddress = RoundUp(startAddress, MIN_ALIGNMENT);

    if (ComparePtr(startAddress, endAddress) > 0 || GetOffsetFromPtr(startAddress, endAddress) < sizeof(FrameHeap)){
        return NULL;
    }

    {
        FrameHeap* pHeap = new(startAddress) FrameHeap;

        pHeap->Initialize(FRMHEAP_SIGNATURE,AddOffsetToPtr(pHeap, sizeof(FrameHeap)),endAddress,optFlag);

        pHeap->mHeadAllocator = pHeap->GetHeapStart();
        pHeap->mTailAllocator = pHeap->GetHeapEnd();
        pHeap->mpState = NULL;

        return pHeap;
    }
}

void* FrameHeap::Destroy(){
    Finalize();
    return this;
}

void* FrameHeap::Alloc(u32 size, int alignment){
    void* memory = NULL;

    if (size == 0){
        size = 1;
    }
    size = RoundUp( size, MIN_ALIGNMENT );
    
    LockHeap();

    if (alignment >= 0){
        memory = this->AllocFromHead(size, alignment);
    }
    else{
        memory = this->AllocFromTail(size, -alignment);
    }

    UnlockHeap();

    return memory;
}

void* FrameHeap::AllocFromHead(u32 size, int alignment){
    void* newBlock = RoundUp(mHeadAllocator, (u32)alignment);
    void* endAddress = AddOffsetToPtr(newBlock, size);

    if (GetIntPtr(endAddress) > GetIntPtr(this->mTailAllocator)){
        return NULL;
    }

    FillAllocMemory(mHeadAllocator,(u32)GetOffsetFromPtr(this->mHeadAllocator, endAddress));

    mHeadAllocator = endAddress;

    return newBlock;
}

void* FrameHeap::AllocFromTail( u32 size, int alignment ){
    void* newBlock = RoundDown(AddOffsetToPtr(this->mTailAllocator, -static_cast<s32>(size) ), static_cast<u32>(alignment));

    if (GetIntPtr(newBlock) < GetIntPtr(mHeadAllocator)){
        return NULL;
    }

    FillAllocMemory(newBlock,(u32)GetOffsetFromPtr(newBlock, mTailAllocator));

    mTailAllocator = newBlock;

    return newBlock;
}

void FrameHeap::Free(int mode){
    NW_ASSERT(IsValid());

    this->LockHeap();

    if (mode & FREE_HEAD){
        this->FreeHead();
    }

    if (mode & FREE_TAIL){
        this->FreeTail();
    }

    this->UnlockHeap();
}

void FrameHeap::FreeHead(){
    FillFreeMemory(this->mHeapStart, (u32)GetOffsetFromPtr(this->mHeapStart, this->mHeadAllocator));
    mHeadAllocator = mHeapStart;
    mpState = NULL;
}

void FrameHeap::FreeTail(){
    FillFreeMemory(mTailAllocator, (u32)GetOffsetFromPtr(this->mTailAllocator, this->mHeapEnd));

    for (HeapState* pState = mpState; pState; pState = pState->pPrevState){
        pState->tailAllocator = mHeapEnd;
    }
    mTailAllocator = mHeapEnd;
}

bool FrameHeap::RecordState(u32 tagName){
    bool retVal;

    this->LockHeap();

    {
        void* oldHeadAllocator = mHeadAllocator;
        void* stateHeap = AllocFromHead(sizeof(HeapState), MIN_ALIGNMENT);

        if (stateHeap == NULL){
            retVal = false;
        }
        else{
            HeapState* pState = new(stateHeap) HeapState;
            if (pState == NULL){
                retVal = false;
            }
            else{
                pState->tagName         = tagName;
                pState->headAllocator   = oldHeadAllocator;
                pState->tailAllocator   = mTailAllocator;
                pState->pPrevState      = mpState;

                mpState = pState;
                retVal = true;
            }
        }
    }

    this->UnlockHeap();

    return retVal;
}

bool FrameHeap::FreeByState( u32 tagName ){
    bool retVal;

    this->LockHeap();

    {
        HeapState* pState = mpState;

        if (tagName != 0){
            for (; pState; pState = pState->pPrevState){
                if (pState->tagName == tagName){
                    break;
                }
            }
        }

        if (pState == NULL){
            retVal = false;
        }
        else{
            void* oldHeadAllocator = mHeadAllocator;
            void* oldTailAllocator = mTailAllocator;

            mHeadAllocator = pState->headAllocator;
            mTailAllocator = pState->tailAllocator;
            mpState = pState->pPrevState;

            FillFreeMemory(mHeadAllocator,(u32)GetOffsetFromPtr(this->mHeadAllocator, oldHeadAllocator));
            FillFreeMemory(oldTailAllocator,(u32)GetOffsetFromPtr(oldTailAllocator, this->mTailAllocator));

            retVal = true;
        }
    }

    this->UnlockHeap();

    return retVal;
}

}
}