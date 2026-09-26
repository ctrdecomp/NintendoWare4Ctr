// Filename: snd_SoundPlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundPlayer.h>

#include <nw/snd/snd_SoundThread.h>

namespace nw {
namespace snd {

SoundPlayer::SoundPlayer(): 
    m_PlayableCount(1),
    m_PlayableLimit(INT_MAX),
    m_Volume(1.0f),
    m_LpfFreq(0.0f),
    m_BiquadType(-1),
    m_BiquadValue(0.0f),
    m_MainSend(0.0f)
{
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        m_FxSend[i] = 0.0f;
    }
}

SoundPlayer::~SoundPlayer()
{
    StopAllSound(0);
}

void SoundPlayer::Update()
{
    NW_UT_LINKLIST_FOREACH_SAFE(itr, m_SoundList,
        { itr->Update(); });

    detail_SortPriorityList();
}

void SoundPlayer::StopAllSound(int fadeFrames)
{
    NW_UT_LINKLIST_FOREACH_SAFE(itr, m_SoundList,
        { itr->Stop(fadeFrames); });
}

void SoundPlayer::PauseAllSound( bool flag, int fadeFrames )
{
    NW_UT_LINKLIST_FOREACH_SAFE(itr, m_SoundList,
        { itr->Pause(flag, fadeFrames); });
}

void SoundPlayer::SetVolume(float volume)
{
    NW_ASSERT(volume >= 0.0f);
    if (volume < 0.0f) 
    {
        volume = 0.0f;
    }
    m_Volume = volume;
}

void SoundPlayer::SetLpfFreq(float lpfFreq)
{
    m_LpfFreq = lpfFreq;
}

void SoundPlayer::SetBiquadFilter(int type, float value)
{
    m_BiquadType = type;
    m_BiquadValue = value;
}

void SoundPlayer::SetMainSend(float send)
{
    m_MainSend = send;
}

void SoundPlayer::SetFxSend(AuxBus bus, float send)
{
    NW_MINMAXLT_ASSERT(bus, 0, AUX_BUS_NUM);
    m_FxSend[bus] = send;
}

void SoundPlayer::RemoveSoundList(internal::BasicSound* pSound)
{
    m_SoundList.Erase(pSound);
    pSound->DetachSoundPlayer(this);
}

void SoundPlayer::InsertPriorityList(internal::BasicSound* pSound)
{
    PriorityList::Iterator itr = m_PriorityList.GetBeginIter();
    while (itr != m_PriorityList.GetEndIter())
    {
        if (pSound->CalcCurrentPlayerPriority() < itr->CalcCurrentPlayerPriority()) break;
        (void)++itr;
    }

    m_PriorityList.Insert(itr, pSound);
}

void SoundPlayer::RemovePriorityList(internal::BasicSound* pSound)
{
    m_PriorityList.Erase(pSound);
}

void SoundPlayer::detail_SortPriorityList(internal::BasicSound* pSound)
{
    RemovePriorityList(pSound);
    InsertPriorityList(pSound);
}

void SoundPlayer::detail_SortPriorityList()
{
    if (m_PriorityList.GetSize() < 2) 
    {
        return;
    }
    
    static const int TMP_NUM = internal::BasicSound::PRIORITY_MAX - internal::BasicSound::PRIORITY_MIN + 1;
    static PriorityList tmplist[TMP_NUM];

    while (!m_PriorityList.IsEmpty())
    {
        internal::BasicSound& front = m_PriorityList.GetFront();
        m_PriorityList.PopFront();
        tmplist[front.CalcCurrentPlayerPriority()].PushBack(&front);
    }

    for (int i = 0; i < TMP_NUM; i++)
    {
        while (!tmplist[i].IsEmpty())
        {
            internal::BasicSound& front = tmplist[i].GetFront();
            tmplist[i].PopFront();
            m_PriorityList.PushBack(&front);
        }
    }
}

bool SoundPlayer::detail_AppendSound(internal::BasicSound* pSound)
{
    NW_NULL_ASSERT( pSound );

    int allocPriority = pSound->CalcCurrentPlayerPriority();

    if (GetPlayableSoundCount() == 0 )
    {
        return false;
    }
    while (GetPlayingSoundCount() >= GetPlayableSoundCount())
    {
        internal::BasicSound* dropSound = GetLowestPrioritySound();
        if (dropSound == NULL) 
        {
            return false;
        }
        if (allocPriority < dropSound->CalcCurrentPlayerPriority()) 
        {
            return false;
        }
        dropSound->Finalize();
    }

    m_SoundList.PushBack(pSound);
    InsertPriorityList(pSound);

    pSound->AttachSoundPlayer(this);

    return true;
}

void SoundPlayer::detail_RemoveSound(internal::BasicSound* pSound)
{
    RemovePriorityList(pSound);
    RemoveSoundList(pSound);
}

void SoundPlayer::SetPlayableSoundCount(int count)
{
    NW_ASSERT(count >= 0);

    NW_WARNING(count <= m_PlayableLimit, "playable sound count is over limit.");
    count = ut::Clamp(count, 0, m_PlayableLimit);

    m_PlayableCount = count;

    while (GetPlayingSoundCount() > GetPlayableSoundCount())
    {
        internal::BasicSound* dropSound = GetLowestPrioritySound();
        NW_NULL_ASSERT(dropSound);
        dropSound->Finalize();
    }
}

void SoundPlayer::detail_SetPlayableSoundLimit(int limit)
{
    NW_ASSERT(limit >= 0);

    m_PlayableLimit = limit;
}

bool SoundPlayer::detail_CanPlaySound(int startPriority)
{
    if (GetPlayableSoundCount() == 0) 
    {
        return false;
    }
    if (GetPlayingSoundCount() >= GetPlayableSoundCount())
    {
        internal::BasicSound* dropSound = GetLowestPrioritySound();
        if (dropSound == NULL) 
        {
            return false;
        }
        if (startPriority < dropSound->CalcCurrentPlayerPriority()) 
        {
            return false;
        }
    }

    return true;
}

void SoundPlayer::detail_AppendPlayerHeap(internal::PlayerHeap* pHeap)
{
    NW_NULL_ASSERT(pHeap);

    pHeap->AttachSoundPlayer(this);
    m_PlayerHeapList.PushBack(pHeap);
}

void SoundPlayer::detail_RemovePlayerHeap(internal::PlayerHeap* pHeap)
{
    m_PlayerHeapList.Erase(pHeap);
}

internal::PlayerHeap*
SoundPlayer::detail_AllocPlayerHeap(internal::BasicSound* pSound)
{
    NW_NULL_ASSERT(pSound);

    if (m_PlayerHeapList.IsEmpty()) 
    {
        return NULL;
    }
    internal::PlayerHeap& playerHeap = m_PlayerHeapList.GetFront();
    m_PlayerHeapList.PopFront();

    playerHeap.AttachSound(pSound);
    pSound->AttachPlayerHeap(&playerHeap);

    return &playerHeap;
}

void SoundPlayer::detail_FreePlayerHeap(internal::BasicSound* pSound)
{
    NW_NULL_ASSERT(pSound);

    internal::PlayerHeap* pHeap = pSound->GetPlayerHeap();
    if (pHeap == NULL) 
    {
        return;
    }

    pHeap->DetachSound(pSound);
    pSound->DetachPlayerHeap(pHeap);

    m_PlayerHeapList.PushBack(pHeap);
}

} // namespace snd
} // namespace nw