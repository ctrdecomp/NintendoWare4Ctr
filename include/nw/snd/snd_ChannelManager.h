#ifndef NW_SND_CHANNEL_MANAGER_H_
#define NW_SND_CHANNEL_MANAGER_H_

#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_InstancePool.h>
#include <nw/snd/snd_FxBase.h>
#include <nw/ut/ut_LinkList.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

class ChannelManager
{
public:
    static ChannelManager& GetInstance();

    size_t GetRequiredMemSize(int channelCount);
    void Initialize(void* mem, unsigned long memSize);
    void Finalize();
    Channel* Alloc();
    void Free(Channel* channel);

    void UpdateAllChannel();

private:
    typedef InstancePool<Channel> ChannelPool;
    typedef ut::LinkList< FxBase, offsetof(FxBase, m_Link) > FxList;
    typedef ut::LinkList< Channel, offsetof(Channel,m_Link)> ChannelList;

    ChannelManager();

    ChannelPool m_Pool;
    ChannelList m_ChannelList;
    bool m_IsInitialized;
    unsigned long m_ChannelCount;
    void* m_pMem;
    unsigned long m_MemSize;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw


#endif // NW_SND_CHANNEL_MANAGER_H_