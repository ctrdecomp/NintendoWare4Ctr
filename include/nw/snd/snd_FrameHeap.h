#ifndef NW_SND_FRAME_HEAP_H_
#define NW_SND_FRAME_HEAP_H_

#include <nw/ut/ut_LinkList.h>
#include <nw/ut/ut_FrameHeap.h>

namespace nw { 
namespace snd {

class SoundArchive;
class SoundDataManager;

namespace internal {

class FrameHeap
{
public:
    typedef void (*DisposeCallback)(void* mem, unsigned long size, void* userArg);

    struct Block
    {
    public:
        Block(void* buffer, u32 size, DisposeCallback callback, void* callbackArg): 
            m_pBuffer(buffer), 
            m_Size(size), 
            m_Callback(callback), 
            m_pCallbackArg(callbackArg)
            {
        }

        ~Block()
        {
            if (m_Callback != nullptr)
            {
                m_Callback(m_pBuffer, m_Size, m_pCallbackArg);
            }
        }

        void* GetBufferAddr() { return m_pBuffer; }
        const void* GetBufferAddr() const { return m_pBuffer; }
        u32 GetBufferSize() const { return m_Size; }
        DisposeCallback GetDisposeCallback() const { return m_Callback; }
        const void* GetDisposeCallbackArg() const { return m_pCallbackArg; }

    public:
        ut::LinkListNode m_Link;
    
    private:
        void* m_pBuffer;
        u32 m_Size;
        DisposeCallback m_Callback;
        void* m_pCallbackArg;
    };

    typedef ut::LinkList<Block, offsetof(Block, m_Link)> BlockList;

    class Section
    {
    public:
        Section();
        ~Section();

        void AppendBlock(Block* block);
        const BlockList& GetBlockList() const { return m_BlockList; }
        BlockList& GetBlockList() { return m_BlockList; }

        void* GetAddr() { return this; }
        void SetUseCallback(bool use) { m_UseCallback = use; }

        void Dump(nw::snd::SoundDataManager& mgr, nw::snd::SoundArchive& arc) const;

    public:
        ut::LinkListNode m_Link;
    
    private:
        BlockList m_BlockList;
        bool m_UseCallback;
    };

    typedef ut::LinkList<Section, offsetof(Section, m_Link)> SectionList;

private:
    static const int HEAP_ALIGN = 256;

public:
    FrameHeap();
    ~FrameHeap();

    bool Create(void* startAddress, u32 size);
    void Destroy();

    void* Alloc(u32 size, FrameHeap::DisposeCallback callback, void* callbackArg);
    void Clear();

    int SaveState();
    void LoadState(int level);
    int GetCurrentLevel() const;

    u32 GetSize() const;
    u32 GetFreeSize() const;

    bool IsValid() const { return m_pHeap != nullptr; }

    void Dump(SoundDataManager& mgr, SoundArchive& arc) const;
    bool ProcessCallback(int level);

private:
    ut::FrameHeap* m_pHeap;
    SectionList m_SectionList;
};

} // namespace nw
} // namespace snd
} // namespace internal

#endif // NW_SND_FRAME_HEAP_H_