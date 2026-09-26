#ifndef NW_SND_SOUND_INSTANCE_MANAGER_H_
#define NW_SND_SOUND_INSTANCE_MANAGER_H_

#include <nw/ut/ut_LinkList.h>
#include <nw/snd/snd_BasicSound.h>

namespace nw { 
namespace snd { 
namespace internal {

template <typename Sound, typename Player>
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

    unsigned long GetRequiredMemSize( int instanceCount )
    {
        return sizeof(Sound) * instanceCount;
    }

    unsigned long Create(void* buffer, unsigned long size)
    {
        NW_NULL_ASSERT(buffer);

        char* ptr = static_cast<char*>(buffer);
        const unsigned long numObjects = size / sizeof(Sound);
        for(unsigned long i = 0; i < numObjects; i++)
        {
            Sound* sound = new(ptr) Sound(*this);
            m_FreeList.PushBack(sound);
            ptr += sizeof(Sound);
        }
        return numObjects;
    }

    void Destroy(void* buffer, unsigned long size)
    {
        NW_NULL_ASSERT(buffer);
        NW_ASSERT(m_PriorityList.IsEmpty());

        NW_UNUSED_VARIABLE(buffer);
        NW_UNUSED_VARIABLE(size);

        m_FreeList.Clear();
        m_PriorityList.Clear();
    }

    Sound* Alloc(int priority, int ambientPriority)
    {
        int allocPriority = priority + ambientPriority;
        allocPriority = ut::Clamp(allocPriority, BasicSound::PRIORITY_MIN, BasicSound::PRIORITY_MAX);

        Sound* sound = NULL;
        while(sound == NULL)
        {
            if (!m_FreeList.IsEmpty())
            {
                sound = &m_FreeList.GetFront();
                m_FreeList.PopFront();
            }
            else
            {
                Sound* lowPrioSound = GetLowestPrioritySound();
                if (lowPrioSound == NULL) 
                {
                    return NULL;
                }
                if (allocPriority < lowPrioSound->CalcCurrentPlayerPriority()) 
                {
                    return NULL;
                }

                lowPrioSound->Stop(0);

            }
        }

        sound->Initialize();
        sound->SetPriority(priority, ambientPriority);
        
        InsertPriorityList(sound, allocPriority);
        return sound;
    }

    void UpdatePriority(Sound* sound, int priority)
    {
        RemovePriorityList(sound);
        InsertPriorityList(sound, priority);
    }

    void SortPriorityList()
    {
        if (m_PriorityList.GetSize() < 2)
        {
            return;
        }

        static const int TMP_NUM = internal::BasicSound::PRIORITY_MAX - internal::BasicSound::PRIORITY_MIN + 1;
        PriorityList tmplist[TMP_NUM];

        while (!m_PriorityList.IsEmpty())
        {
            Sound& front = m_PriorityList.GetFront();
            m_PriorityList.PopFront();
            tmplist[front.CalcCurrentPlayerPriority()].PushBack(&front);
        }
        for (int i = 0; i < TMP_NUM; i++)
        {
            while (!tmplist[i].IsEmpty())
            {
                Sound& front = tmplist[i].GetFront();
                tmplist[i].PopFront();
                m_PriorityList.PushBack(&front);
            }
        }
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
