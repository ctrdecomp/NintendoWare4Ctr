// Filename: snd_VoiceManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_SoundThread.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

VoiceManager::VoiceManager()
{
}

VoiceManager::~VoiceManager()
{
}

VoiceManager& VoiceManager::GetInstance()
{
    static VoiceManager instance;
    return instance;
}

void VoiceManager::Initialize(void* mem, size_t memSize)
{
    if (m_Initialized)
    {
        return;
    }

    int voiceCount = memSize / sizeof(Voice);

    u8* ptr = reinterpret_cast<u8*>(mem);

    for (int i = 0; i < voiceCount; i++)
    {
        m_FreeVoiceList.PushBack(new(ptr)Voice());
        ptr += sizeof(Voice);
    }

    m_Initialized = true;
}

void VoiceManager::Finalize()
{
    if (!m_Initialized)
    {
        return;
    }

    StopAllVoices();

    while (!m_FreeVoiceList.IsEmpty())
    {
        Voice& voice = m_FreeVoiceList.GetFront();
        m_FreeVoiceList.PopFront();
        voice.~Voice();
    }

    m_Initialized = false;
}

Voice* VoiceManager::AllocVoice(int voiceChannelCount, int priority, Voice::VoiceCallback callback, void* callbackData)
{
    if (m_FreeVoiceList.IsEmpty())
    {
        if (DropLowestPriorityVoice(priority) == 0)
        {
            return NULL;
        }
    }

    Voice& voice = m_FreeVoiceList.GetFront();

    if (!voice.Alloc(voiceChannelCount, priority, callback, callbackData))
    {
        return NULL;
    }

    voice.m_Priority = static_cast<u8>(priority);
    AppendVoiceList(&voice);
    
    return &voice;
}

void VoiceManager::FreeVoice( Voice* voice )
{
    RemoveVoiceList(voice);
}

void VoiceManager::UpdateAllVoices()
{
    NW_UT_LINKLIST_FOREACH_SAFE(it, m_PrioVoiceList, { it->StopFinished(); })
    NW_UT_LINKLIST_FOREACH_SAFE(it, m_PrioVoiceList, { it->Calc(); })
    NW_UT_LINKLIST_FOREACH_SAFE(it, m_PrioVoiceList, { it->Update(); })
}

void VoiceManager::AppendVoiceList( Voice* voice )
{
    m_FreeVoiceList.Erase(voice);

    VoiceList::ReverseIterator itr = m_PrioVoiceList.GetBeginReverseIter();
    while (itr != m_PrioVoiceList.GetEndReverseIter())
    {
        if (itr->m_Priority <= voice->m_Priority)
        {
            break;
        }
        (void)++itr;
    }

    m_PrioVoiceList.Insert(itr.GetBase(), voice);
}

void VoiceManager::RemoveVoiceList( Voice* voice )
{
    m_PrioVoiceList.Erase(voice);
    m_FreeVoiceList.PushBack(voice);
}

void VoiceManager::ChangeVoicePriority( Voice* voice )
{
    RemoveVoiceList(voice);
    AppendVoiceList(voice);
    UpdateEachVoicePriority(m_PrioVoiceList.GetIteratorFromPointer(voice),m_PrioVoiceList.GetEndIter());
}

void VoiceManager::UpdateEachVoicePriority(const VoiceList::Iterator& beginItr, const VoiceList::Iterator& endItr)
{
    for (VoiceList::Iterator itr = beginItr; itr != endItr; (void)++itr)
    {
        if (itr->GetPriority() <= Voice::PRIORITY_RELEASE)
        {
            return;
        }

        if (itr->GetPriority() == Voice::PRIORITY_NODROP)
        {
            continue;
        }

        itr->UpdateVoicesPriority();
    }
}

void VoiceManager::UpdateAllVoicesSync(u32 syncFlag)
{
    for (VoiceList::Iterator itr = m_PrioVoiceList.GetBeginIter(); itr != m_PrioVoiceList.GetEndIter(); )
    {
        VoiceList::Iterator curItr = itr++;
        if (curItr->m_IsActive)
        {
            curItr->m_SyncFlag |= syncFlag;
        }
    }
}

void VoiceManager::StopAllVoices()
{
    while (!m_PrioVoiceList.IsEmpty())
    {
        Voice& voice = m_PrioVoiceList.GetFront();
        voice.Stop();
        if (voice.m_Callback != NULL)
        {
            voice.m_Callback(&voice, Voice::CALLBACK_STATUS_CANCEL, voice.m_pCallbackData);
        }
        voice.Free();
    }
}

int VoiceManager::GetVoiceCount() const
{
    SoundThreadLock lock;
    
    int voiceCount = 0;

    NW_UT_LINKLIST_FOREACH_SAFE(it, m_PrioVoiceList, { it->GetPhysicalVoiceCount(); })

    return voiceCount;
}

unsigned long VoiceManager::GetActiveCount() const
{
    SoundThreadLock lock;
    
    return m_PrioVoiceList.GetSize();
}

unsigned long VoiceManager::GetFreeCount() const
{
    SoundThreadLock lock;
    
    return m_FreeVoiceList.GetSize();
}

const VoiceManager::VoiceList& VoiceManager::GetVoiceList() const
{
    return m_PrioVoiceList;
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw