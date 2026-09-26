// Filename: snd_SoundActor.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundActor.h>
#include <nw/snd/snd_SoundArchivePlayer.h>

namespace nw {
namespace snd {

SoundActor::SoundActor(): 
    m_pSoundArchivePlayer(NULL),
    m_IsInitialized(false),
    m_IsFinalized(true)
{
}

SoundActor::SoundActor(SoundArchivePlayer& soundArchivePlayer): 
    m_pSoundArchivePlayer(NULL),
    m_IsInitialized(false),
    m_IsFinalized(true)
{
    Initialize(soundArchivePlayer);
}

SoundActor::~SoundActor()
{
    Finalize();
}

void SoundActor::Initialize(SoundArchivePlayer& soundArchivePlayer)
{
    if (m_IsInitialized) 
    {
        return;
    }

    m_pSoundArchivePlayer = &soundArchivePlayer;

    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++)
    {
        const int playableSoundCount = (i == 0) ? INT_MAX : 1;
        m_ActorPlayer[i].SetPlayableSoundCount(playableSoundCount);
    }

    m_IsInitialized = true;
    m_IsFinalized = false;
}
void SoundActor::Finalize()
{
    if (m_IsFinalized) 
    {
        return;
    }

    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++)
    {
        m_ActorPlayer[i].Finalize(this);
    }

    m_IsFinalized = true;
    m_IsInitialized = false;
    m_pSoundArchivePlayer = NULL;
    m_ActorParam.Reset();
}

void SoundActor::StopAllSound(int fadeFrames)
{
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++)
    {
        m_ActorPlayer[i].StopAllSound(fadeFrames);
    }
}


void SoundActor::PauseAllSound(bool flag, int fadeFrames)
{
    for (int i = 0; i < ACTOR_PLAYER_COUNT; i++)
    {
        m_ActorPlayer[i].PauseAllSound( flag, fadeFrames);
    }
}

int SoundActor::GetPlayingSoundCount(int actorPlayerId) const
{
    NW_MINMAXLT_ASSERT(actorPlayerId, 0, ACTOR_PLAYER_COUNT);

    return m_ActorPlayer[actorPlayerId].GetPlayingSoundCount();
}

void SoundActor::SetPlayableSoundCount(int actorPlayerId, int count )
{
    NW_MINMAXLT_ASSERT(actorPlayerId, 0, ACTOR_PLAYER_COUNT);

    m_ActorPlayer[actorPlayerId].SetPlayableSoundCount(count);
}

int SoundActor::GetPlayableSoundCount(int actorPlayerId) const
{
    NW_MINMAXLT_ASSERT(actorPlayerId, 0, ACTOR_PLAYER_COUNT);

    return m_ActorPlayer[actorPlayerId].GetPlayableSoundCount();
}

SoundStartable::StartResult SoundActor::SetupSound(SoundHandle* handle, u32 soundId,
    const SoundStartable::StartInfo* startInfo, void* setupArg)
{
    NW_NULL_ASSERT(setupArg);
    if (m_IsInitialized == false)
    {
        return SoundStartable::StartResult::START_ERR_ACTOR_NOT_INITIALIZED;
    }

    const SetupInfo* setupInfo = reinterpret_cast<SetupInfo*>(setupArg);

    return m_pSoundArchivePlayer->detail_SetupSoundImpl(handle, soundId, NULL,
        this, setupInfo->holdFlag, startInfo);
}

SoundStartable::StartResult SoundActor::detail_SetupSoundWithAmbientInfo(SoundHandle* handle,
    u32 soundId, const SoundStartable::StartInfo* startInfo,
    internal::BasicSound::AmbientInfo* ambientInfo, void* setupArg)
{
    NW_NULL_ASSERT(setupArg);
    if (m_IsInitialized == false)
    {
        return SoundStartable::StartResult::START_ERR_ACTOR_NOT_INITIALIZED;
    }

    const SetupInfo* setupInfo = reinterpret_cast<SetupInfo*>(setupArg);

    return m_pSoundArchivePlayer->detail_SetupSoundImpl(handle, soundId, ambientInfo,
        this, setupInfo->holdFlag, startInfo);
}


SoundStartable::StartResult SoundActor::detail_SetupSound(SoundHandle* handle,
    u32 soundId, bool holdFlag, const SoundStartable::StartInfo* startInfo)
{
    SetupInfo setupInfo;
    setupInfo.holdFlag = holdFlag;

    return SetupSound(handle, soundId, startInfo, &setupInfo);
}

SoundArchive::ItemId SoundActor::detail_GetItemId(const char* pString)
{
    if (m_IsInitialized == false)
    {
        return SoundArchive::INVALID_ID;
    }
    return m_pSoundArchivePlayer->detail_GetItemId(pString);
}

} // namespace snd
} // namespace nw

