// Filename: snd_FrameHeap.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_FrameHeap.h>

namespace nw {
namespace snd {
namespace internal {

FrameHeap::FrameHeap(): 
    m_pHeap(NULL) 
{
}

FrameHeap::~FrameHeap() 
{
    if (IsValid()) 
    {
        Destroy();
    }
}

bool FrameHeap::Create(void* pBase, u32 size) 
{
    if (IsValid()) 
    {
        Destroy();
    }

    void* pEnd = static_cast<u8*>(pBase) + size;
    pBase = ut::RoundUp(pBase, 4);
    if (pBase > pEnd) 
    {
        return false;
    }

    m_pHeap = m_pHeap->Create(pBase, ut::GetOffsetFromPtr(pBase, pEnd));
    if (m_pHeap == NULL) 
    {
        return false;
    }

    if (!NewSection()) 
    {
        return false;
    }

    return true;
}

void FrameHeap::Destroy() 
{
    if (!IsValid()) 
    {
        return;
    }

    ClearSection();
    m_pHeap = NULL;
}

void FrameHeap::Clear() 
{
    ClearSection();
    m_pHeap->Free(3);
    NewSection();
}

void* FrameHeap::Alloc(u32 size, DisposeCallback pCallback, void* pCallbackArg) 
{
    void* pBuffer = m_pHeap->Alloc(
        31 + ut::RoundUp(size, HEAP_ALIGN), HEAP_ALIGN);

    if (pBuffer == NULL) 
    {
        return NULL;
    }

    Block* pBlock = new (pBuffer) Block(size, pCallback, pCallbackArg);
    m_SectionList.GetBack().AppendBlock(pBlock);

    return pBlock->GetBufferAddr();
}

int FrameHeap::SaveState() 
{
    if (!m_pHeap->RecordState(m_SectionList.GetSize())) 
    {
        return -1;
    }

    if (!NewSection()) 
    {
        m_pHeap->FreeByState(0);
        return -1;
    }

    return GetCurrentLevel();
}

void FrameHeap::LoadState(int id) {
    if (id == 0) 
    {
        Clear();
        return;
    }

    while (id < static_cast<int>(m_SectionList.GetSize())) 
    {
        Section& rSection = m_SectionList.GetBack();
        rSection.~Section();
        m_SectionList.Erase(&rSection);
    }

    m_pHeap->FreeByState(id);
    m_pHeap->RecordState(m_SectionList.GetSize());

    NewSection();
}

int FrameHeap::GetCurrentLevel() const 
{
    return m_SectionList.GetSize() - 1;
}

bool FrameHeap::NewSection() 
{
    void* pSection = m_pHeap->Alloc(sizeof(Section));
    if (pSection == NULL) 
    {
        return false;
    }

    m_SectionList.PushBack(new (pSection) Section());
    return true;
}

void FrameHeap::ClearSection() 
{
    while (!m_SectionList.IsEmpty()) 
    {
        Section& rSection = m_SectionList.GetBack();
        rSection.~Section();
        m_SectionList.Erase(&rSection);
    }
}

} // namespace internal
} // namespace snd
} // namespace nw