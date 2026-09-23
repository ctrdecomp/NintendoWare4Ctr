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
    SoundInstanceManager()
    {
    }

    ~SoundInstanceManager()
    {
    }

    void UpdatePriority(Sound* sound, int priority)
    {
        RemovePriorityList(sound);
        InsertPriorityList(sound, priority);
    }

    void Free(Sound* sound)
    {
        NW_NULL_ASSERT(sound);

        RemovePriorityList(sound);

        sound->Finalize();
        m_FreeList.PushBack(sound);
    }

private:
    void InsertPriorityList(Sound* sound, int priority)
    {
        Iterator itr = m_PriorityList.GetBeginIter();
        while (itr != m_PriorityList.GetEndIter())
        {
            if (priority < itr->CalcCurrentPlayerPriority()) break;
            (void)++itr;
        }
        m_PriorityList.Insert(itr, sound);
    }

    void RemovePriorityList(Sound* sound) 
    { 
        m_PriorityList.Erase(sound);
    }

    PriorityList m_PriorityList;
    PriorityList m_FreeList;
};

} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_SOUND_INSTANCE_MANAGER_H_
