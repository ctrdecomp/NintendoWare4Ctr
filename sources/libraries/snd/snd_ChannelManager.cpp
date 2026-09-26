// Filename: snd_ChannelManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_ChannelManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

ChannelManager& ChannelManager::GetInstance()
{
    static ChannelManager instance;
    return instance;
}

ChannelManager::ChannelManager(): 
    m_IsInitialized(false),
    m_ChannelCount(0)
{
}

size_t ChannelManager::GetRequiredMemSize(int channelCount)
{
    return sizeof(Channel) * (channelCount + 1);
}

void ChannelManager::Initialize(void* mem, unsigned long memSize)
{
    if (m_IsInitialized) 
        return;

    m_ChannelCount = m_Pool.Create(mem, memSize);
    m_pMem = mem;
    m_MemSize = memSize;

    m_IsInitialized = true;
}

void ChannelManager::Finalize()
{
    if (!m_IsInitialized) 
        return;

    for (ChannelList::Iterator itr = m_ChannelList.GetBeginIter(); itr != m_ChannelList.GetEndIter(); )
    {
        ChannelList::Iterator curItr = itr++;
        curItr->Stop();
    }

    NW_ASSERT(m_ChannelList.IsEmpty());

    m_Pool.Destroy(m_pMem, m_MemSize);

    m_IsInitialized = false;
}

Channel* ChannelManager::Alloc()
{
    Channel* channel = m_Pool.Alloc();
    m_ChannelList.PushBack(channel);
    return channel;
}
void ChannelManager::Free(Channel* channel)
{
    m_ChannelList.Erase(channel);
    m_Pool.Free(channel);
}

void ChannelManager::UpdateAllChannel()
{
    for (ChannelList::Iterator itr = m_ChannelList.GetBeginIter(); itr != m_ChannelList.GetEndIter(); )
    {
        ChannelList::Iterator curItr = itr++;
        curItr->Update(true);
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw