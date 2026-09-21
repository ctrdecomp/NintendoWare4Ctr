#ifndef NW_SND_SOUND_INSTANCE_MANAGER_H_
#define NW_SND_SOUND_INSTANCE_MANAGER_H_

#include <nw/ut/ut_LinkList.h>
#include <nw/snd/snd_BasicSound.h>

namespace nw { 
namespace snd { 
namespace internal {

template <typename Sound>
class SoundInstanceManager
{
public:
    typedef ut::LinkList<Sound, offsetof(Sound, m_PriorityLink)> PriorityList;

public:
    SoundInstanceManager(): 
        m_pBuffer(NULL), 
        m_BufferSize(0)
        {
    }

    ~SoundInstanceManager()
    {
    }

private:
    void* m_pBuffer;
    u32 m_BufferSize;
    PriorityList m_PriorityList;
    PriorityList m_FreeList;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_INSTANCE_MANAGER_H_
