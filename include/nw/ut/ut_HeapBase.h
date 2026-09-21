#pragma once

#include <nw/ut/ut_LinkList.h>

#include <stddef.h>

namespace nw {
namespace ut {

class HeapNode
{
  public:
    HeapNode() {}
    virtual ~HeapNode() {}

  public:
    LinkListNode m_LinkNode;
};


class HeapBase : public HeapNode
{
public:
    typedef ut::LinkList< HeapBase, offsetof(HeapNode,m_LinkNode)> HeapList;

    static const int DEFAULT_ALIGNMENT  = 4;

    static const u32 EXPHEAP_SIGNATURE  = 'EXPH';
    static const u32 FRMHEAP_SIGNATURE  = 'FRMH';
    static const u32 UNTHEAP_SIGNATURE  = 'UNTH';

    static const int OPT_0_CLEAR        = (1 << 0);
    static const int OPT_DEBUG_FILL     = (1 << 1);
    static const int OPT_THREAD_SAFE    = (1 << 2);

    static const int ERROR_PRINT            = ( 1 << 0 );

    enum FillType
    {
        HEAP_FILL_NOUSE,
        HEAP_FILL_ALLOC,
        HEAP_FILL_FREE,
        HEAP_FILL_MAX
    };

    enum HeapType
    {
        HEAP_TYPE_EXP,
        HEAP_TYPE_FRM,
        HEAP_TYPE_UNIT,
        HEAP_TYPE_UNKNOWN
    };
    
    static HeapBase* FindContainHeap(const void* memBlock);

    void* GetHeapStartAddress() { return this; }
    void* GetHeapEndAddress() { return m_HeapEnd; }

    s32 GetTotalSize() { return ((s32)(m_HeapEnd) - (s32)(this)); }
    s32 GetTotalUsableSize() { return ((s32)(m_HeapEnd) - (s32)(m_HeapStart)); }

    u32 GetFillValue(FillType type);

    HeapType GetHeapType();

  protected:
    static const int MIN_ALIGNMENT = 4;

    void Initialize( u32 signature, void* start, void* end, u16 optFlag );
    void Finalize();

    u32 GetSignature() const { return m_Signature; }
    void* GetHeapStart() const { return m_HeapStart; }
    void* GetHeapEnd() const { return m_HeapEnd; }

    void LockHeap();
    void UnlockHeap();

    void FillFreeMemory(void* address, u32 size);
    void FillNoUseMemory(void* address, u32 size);
    void FillAllocMemory(void* address, u32 size);

    void*       m_HeapStart;
    void*       m_HeapEnd;

  private:
    static HeapBase* FindContainHeap(HeapList* pList, const void* memBlock);
    static HeapList* FindListContainHeap(HeapBase* pHeapBase);

    u16  GetOptionFlag();
    void SetOptionFlag(u16 optFlag);

    u32         m_Signature;
    HeapList    m_ChildList;
    u32         m_Attribute;
};

}
}