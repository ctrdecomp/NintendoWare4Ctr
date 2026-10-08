// Filename: snd_ExternalSoundPlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_ExternalSoundPlayer.h>

namespace nw {
namespace snd {
namespace internal {

ExternalSoundPlayer::ExternalSoundPlayer():
    m_PlayableCount(1)
{
}

/*

Some can be copied from NW4R since NW4C and NW4R are somewhat simliar.

[REFERENCE] https://github.com/doldecomp/ogws/blob/master/src/nw4r/snd/snd_ExternalSoundPlayer.cpp

*/

ExternalSoundPlayer::~ExternalSoundPlayer()
{
    for (SoundList::Iterator itr = m_SoundList.GetBeginIter(); itr != m_SoundList.GetEndIter(); )
    {
        SoundList::Iterator curItr = itr++;
        curItr->DetachExternalSoundPlayer(this);
    }
}

void ExternalSoundPlayer::StopAllSound(int fadeFrames)
{
    for (SoundList::Iterator itr = m_SoundList.GetBeginIter(); itr != m_SoundList.GetEndIter();)
    {
        SoundList::Iterator curItr = itr++;
        curItr->Stop(fadeFrames);
    }
}

void ExternalSoundPlayer::PauseAllSound(bool flag, int fadeFrames)
{
    for (SoundList::Iterator itr = m_SoundList.GetBeginIter(); itr != m_SoundList.GetEndIter(); )
    {
        SoundList::Iterator curItr = itr++;
        curItr->Pause(flag, fadeFrames);
    }
}

void ExternalSoundPlayer::SetPlayableSoundCount(int count) 
{
    m_PlayableCount = count;

    while (GetPlayingSoundCount() > GetPlayableSoundCount()) 
    {
        GetLowestPrioritySound()->Finalize();
    }
}

bool ExternalSoundPlayer::detail_CanPlaySound(int startPriority)
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

bool ExternalSoundPlayer::AppendSound(internal::BasicSound* sound)
{
    NW_NULL_ASSERT(sound);
    
    int allocPriority = sound->CalcCurrentPlayerPriority();

    if (GetPlayableSoundCount() == 0) 
        return false;
    while (GetPlayingSoundCount() >= GetPlayableSoundCount())
    {
        internal::BasicSound* dropSound = GetLowestPrioritySound();
        if (dropSound == NULL) 
            return false;
        if (allocPriority < dropSound->CalcCurrentPlayerPriority())
        {
            return false;
        }
        dropSound->Finalize();
    }
    
    m_SoundList.PushBack(sound);
    
    sound->AttachExternalSoundPlayer(this);
    
    return true;
}

void ExternalSoundPlayer::RemoveSound(internal::BasicSound* sound)
{
    m_SoundList.Erase(sound);
    sound->DetachExternalSoundPlayer(this);
}

void ExternalSoundPlayer::Finalize(SoundActor* actor)
{
    for (SoundList::Iterator itr = m_SoundList.GetBeginIter(); itr != m_SoundList.GetEndIter(); )
    {
        SoundList::Iterator curItr = itr++;
        curItr->DetachSoundActor(actor);
        RemoveSound(&*curItr);
    }
}

internal::BasicSound* ExternalSoundPlayer::GetLowestPrioritySound()
{
    int lowestPrio = BasicSound::PRIORITY_MAX + 1;
    BasicSound* pLowest = NULL;

    for (SoundList::Iterator it = m_SoundList.GetBeginIter(); it != m_SoundList.GetEndIter(); ++it) 
    {
        int priority = it->CalcCurrentPlayerPriority();

        if (lowestPrio > priority) 
        {
            pLowest = &*it;
            lowestPrio = priority;
        }
    }

    return pLowest;
}

} // namespace internal
} // namespace snd
} // namespace nw

