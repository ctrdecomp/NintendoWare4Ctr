// Filename: snd_SoundArchivePlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SoundArchivePlayer.h>

#include <nw/snd/snd_SoundSystem.h>
#include <nw/snd/snd_SoundDataManager.h>
#include <nw/snd/snd_SoundPlayer.h>
#include <nw/snd/snd_SoundHandle.h>
#include <nw/snd/snd_SoundActor.h>
#include <nw/snd/snd_SoundDataManager.h>
#include <nw/snd/snd_Bank.h>
#include <nw/snd/snd_SequenceSoundFileReader.h>
#include <nw/snd/snd_SequenceSoundPlayer.h>
#include <nw/snd/snd_WaveSoundFileReader.h>
#include <nw/snd/snd_WaveArchiveFileReader.h>
#include <nw/snd/snd_WaveFileReader.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_PlayerHeap.h>
#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {

SoundArchivePlayer::SoundArchivePlayer(): 
    m_pSoundArchive(NULL),
    m_SequenceCallback(*this),
    m_WaveSoundCallback(*this),
    m_SequenceUserprocCallback(NULL),
    m_pSequenceUserprocCallbackArg(NULL),
    m_SoundPlayerCount(0),
    m_pSoundPlayers(NULL),
    m_MmlSequenceTrackAllocator(&m_MmlParser),
    m_pSetupBufferAddress(NULL),
    m_SetupBufferSize(0)
{
}

SoundArchivePlayer::~SoundArchivePlayer()
{
}

bool SoundArchivePlayer::IsAvailable() const
{
    if (m_pSoundArchive == NULL)
    {
        return false;
    }
    if (!m_pSoundArchive->IsAvailable())
    {
        return false;
    }

    return true;
}

bool SoundArchivePlayer::Initialize(
    const SoundArchive* arc, const SoundDataManager* manager,
    void* buffer, u32 size, void* strmBuffer, u32 strmBufferSize)
{
    NW_ASSERT(SoundSystem::IsInitialized());
    if (!SoundSystem::IsInitialized())
    {
        return false;
    }

    NW_NULL_ASSERT(arc);
    NW_NULL_ASSERT(manager);
    NW_NULL_ASSERT(buffer);
    NW_ALIGN32_ASSERT(buffer);

    if (strmBufferSize > 0) 
    {
        NW_NULL_ASSERT(strmBuffer);
        NW_ALIGN32_ASSERT(strmBuffer);

        if (!internal::Util::IsDeviceMemory(reinterpret_cast<uptr>(strmBuffer), strmBufferSize))
        {
            NW_ASSERTMSG(false, "strmBuffer, strmBuffer + strmBufferSize is not Device Memory.");
            return false;
        }
    }
    NW_ASSERT(strmBufferSize >= GetRequiredStreamBufferSize(arc));

    NW_ASSERT(strmBufferSize <= GetRequiredStreamBufferSize(arc) * internal::driver::StreamChannel::WAVE_BUFFER_MULTIPLYING_FACTOR);

    if (!SetupMram(arc, buffer, size)) 
    {
        return false;
    }

    if (!SetupStreamBuffer(arc, strmBuffer, strmBufferSize)) 
    {
        return false;
    }

    m_pSequenceTrackAllocator = &m_MmlSequenceTrackAllocator;
    m_pSoundDataManager = manager;
    return true;
}

void SoundArchivePlayer::Finalize()
{
    m_pSoundArchive = NULL;
    m_pSequenceTrackAllocator = NULL;

    for (SoundArchive::ItemId playerId = 0; playerId < m_SoundPlayerCount; ++playerId)
    {
        m_pSoundPlayers[playerId].StopAllSound(0);
    }

    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    u32 tag = cmdmgr.FlushCommand(true);
    cmdmgr.WaitCommandReply(tag);

    m_SoundPlayerCount = 0;
    m_pSoundPlayers = NULL;

    m_StreamBufferPool.Finalize();

    if (m_pSetupBufferAddress != NULL)
    {
        m_SequenceSoundInstanceManager.Destroy(m_pSetupBufferAddress, m_SetupBufferSize);
        m_MmlSequenceTrackAllocator.Destroy(m_pSetupBufferAddress, m_SetupBufferSize);
        m_WaveSoundInstanceManager.Destroy(m_pSetupBufferAddress, m_SetupBufferSize);
        m_StreamSoundInstanceManager.Destroy(m_pSetupBufferAddress, m_SetupBufferSize);

        m_pSetupBufferAddress = NULL;
        m_SetupBufferSize = 0;
    }
}

size_t SoundArchivePlayer::GetRequiredMemSize(const SoundArchive* arc)
{
    NW_NULL_ASSERT(arc);

    size_t size = 0;

    {
        u32 playerCount = arc->GetPlayerCount();
        size += ut::RoundUp(playerCount * sizeof(SoundPlayer), 4);

        for (u32 playerIndex = 0; playerIndex < playerCount; ++playerIndex)
        {
            SoundArchive::PlayerInfo playerInfo;
            if (!arc->ReadPlayerInfo(internal::Util::GetMaskedItemId(
                playerIndex, internal::ItemType_Player), &playerInfo))
            {
                continue;
            }

            if (playerInfo.playerHeapSize > 0)
            {
                for (int i = 0; i < playerInfo.playableSoundMax; i++)
                {
                    size += ut::RoundUp(sizeof(internal::PlayerHeap), 4);
                    size = ut::RoundUp(size, 32);
                    size += ut::RoundUp(playerInfo.playerHeapSize, 4);
                }
            }
        }
    }

    SoundArchive::SoundArchivePlayerInfo soundArchivePlayerInfo;
    if (arc->ReadSoundArchivePlayerInfo(&soundArchivePlayerInfo))
    {
        size += ut::RoundUp(m_SequenceSoundInstanceManager.GetRequiredMemSize(soundArchivePlayerInfo.sequenceSoundMax), 4);

        size += ut::RoundUp(m_WaveSoundInstanceManager.GetRequiredMemSize(soundArchivePlayerInfo.waveSoundMax), 4);

        size += ut::RoundUp(m_StreamSoundInstanceManager.GetRequiredMemSize(soundArchivePlayerInfo.streamSoundMax), 4);

        size += ut::RoundUp(soundArchivePlayerInfo.sequenceTrackMax *
            sizeof(internal::driver::MmlSequenceTrack), 4);

    }

    return size;
}

size_t SoundArchivePlayer::GetRequiredStreamBufferSize(const nw::snd::SoundArchive* arc)
{
    NW_NULL_ASSERT(arc);

    int strmChannelCount = 0;

    SoundArchive::SoundArchivePlayerInfo soundArchivePlayerInfo;
    if (arc->ReadSoundArchivePlayerInfo(&soundArchivePlayerInfo))
    {
        strmChannelCount = soundArchivePlayerInfo.streamChannelMax;
    }

    size_t memSize = static_cast<size_t>(
        internal::driver::StreamSoundPlayer::DATA_BLOCK_SIZE_MAX 
        * DEFAULT_STREAM_BLOCK_COUNT * strmChannelCount
    );
    return memSize;
}

bool SoundArchivePlayer::SetupMram(const SoundArchive* arc, void* buffer, unsigned long size)
{
    NW_ASSERT(size >= GetRequiredMemSize(arc));

    void* endp = static_cast<char*>(buffer) + size;

    void* buf = buffer;
    if (!SetupSoundPlayer(arc, &buf, endp))
    {
        return false;
    }

    SoundArchive::SoundArchivePlayerInfo soundArchivePlayerInfo;
    if (arc->ReadSoundArchivePlayerInfo(&soundArchivePlayerInfo))
    {
        if (!SetupSequenceSound(arc, soundArchivePlayerInfo.sequenceSoundMax, &buf, endp))
        {
            return false;
        }

        if (!SetupWaveSound(arc, soundArchivePlayerInfo.waveSoundMax, &buf, endp))
        {
            return false;
        }

        if (!SetupStreamSound(arc, soundArchivePlayerInfo.streamSoundMax, &buf, endp))
        {
            return false;
        }

        if (!SetupSequenceTrack(arc, soundArchivePlayerInfo.sequenceTrackMax, &buf, endp))
        {
            return false;
        }
    }

    NW_ASSERT(
        static_cast<char*>(buf) - static_cast<char*>(buffer) ==
        static_cast<s32>(GetRequiredMemSize(arc)));

    m_pSoundArchive = arc;
    m_pSetupBufferAddress = buffer;
    m_SetupBufferSize = size;

    return true;
}

internal::PlayerHeap* SoundArchivePlayer::CreatePlayerHeap(
    void** buffer, void* endp, size_t heapSize)
{
    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, sizeof(internal::PlayerHeap)), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return NULL;
    }
    void* buf = *buffer;
    *buffer = ep;

    internal::PlayerHeap* playerHeap = new (buf) internal::PlayerHeap();

    *buffer = ut::RoundUp(*buffer, 32);
    ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, heapSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return NULL;
    }
    buf = *buffer;
    *buffer = ep;

    bool result = playerHeap->Create(buf, heapSize);

    if (!result)
    {
        return NULL;
    }

    return playerHeap;
}

bool SoundArchivePlayer::SetupSoundPlayer(
    const SoundArchive* arc,
    void** buffer,
    void* endp
)
{
    u32 playerCount = arc->GetPlayerCount();
    size_t requireSize = playerCount * sizeof(SoundPlayer);

    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, requireSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return false;
    }
    void* buf = *buffer;
    *buffer = ep;

    m_pSoundPlayers = reinterpret_cast<SoundPlayer*>(buf);
    m_SoundPlayerCount = playerCount;

    u8* ptr = static_cast<u8*>(buf);
    for (u32 playerIndex = 0; playerIndex < playerCount;
        ++playerIndex, ptr += sizeof(SoundPlayer))
    {
        SoundPlayer* player = new (ptr) SoundPlayer();

        SoundArchive::PlayerInfo playerInfo;
        if (!arc->ReadPlayerInfo(internal::Util::GetMaskedItemId(
            playerIndex, internal::ItemType_Player), &playerInfo))
        {
            continue;
        }

        player->SetPlayableSoundCount(playerInfo.playableSoundMax);

        if (playerInfo.playerHeapSize > 0)
        {
            for (int i = 0; i < playerInfo.playableSoundMax; i++)
            {
                internal::PlayerHeap* playerHeap = CreatePlayerHeap(
                    buffer, endp, playerInfo.playerHeapSize);

                NW_WARNING(playerHeap != NULL,
                    "failed to create player heap. ( player id = %d )", playerIndex);
                if (playerHeap == NULL)
                {
                    return false;
                }
                player->detail_AppendPlayerHeap(playerHeap);
            }

            player->detail_SetPlayableSoundLimit(playerInfo.playableSoundMax);
        }
    }

    return true;
}

bool SoundArchivePlayer::SetupSequenceSound(const SoundArchive* /*arc*/, 
    int numSounds, void** buffer, void* endp)
{
    size_t requireSize = m_SequenceSoundInstanceManager.GetRequiredMemSize(numSounds);

    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, requireSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return false;
    }

    unsigned long createNum = m_SequenceSoundInstanceManager.Create(*buffer, requireSize);
    NW_ASSERT(createNum == numSounds);
    *buffer = ep;

    return true;
}

bool SoundArchivePlayer::SetupWaveSound(const SoundArchive* /*arc*/,
    int numSounds, void** buffer, void* endp)
{
    size_t requireSize = m_WaveSoundInstanceManager.GetRequiredMemSize(numSounds);

    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, requireSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return false;
    }

    unsigned long createNum = m_WaveSoundInstanceManager.Create(*buffer, requireSize);
    NW_ASSERT(createNum == numSounds);
    *buffer = ep;

    return true;
}

bool SoundArchivePlayer::SetupStreamSound(const SoundArchive* /*arc*/,
    int numSounds, void** buffer, void* endp)
{
    size_t requireSize = m_StreamSoundInstanceManager.GetRequiredMemSize(numSounds);

    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, requireSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return false;
    }

    unsigned long createNum = m_StreamSoundInstanceManager.Create(*buffer, requireSize);
    NW_ASSERT(createNum == numSounds);
    *buffer = ep;

    return true;
}

bool SoundArchivePlayer::SetupSequenceTrack(const SoundArchive* /*arc*/,
    int numTracks, void** buffer, void* endp)
{
    unsigned long requireSize = numTracks * sizeof(internal::driver::MmlSequenceTrack);

    void* ep = ut::RoundUp(ut::AddOffsetToPtr(*buffer, requireSize), 4);
    if (ut::ComparePtr(ep, endp) > 0)
    {
        return false;
    }

    unsigned long createNum = m_MmlSequenceTrackAllocator.Create(*buffer, requireSize);
    NW_ASSERT(createNum == numTracks);
    *buffer = ep;

    return true;
}

bool SoundArchivePlayer::SetupStreamBuffer(const SoundArchive* arc,
    void* buffer, unsigned long size)
{
    if (size < GetRequiredStreamBufferSize(arc)) 
    {
        return false;
    }

    int strmChannelCount = 0;

    SoundArchive::SoundArchivePlayerInfo soundArchivePlayerInfo;
    if (arc->ReadSoundArchivePlayerInfo(&soundArchivePlayerInfo))
    {
        strmChannelCount = soundArchivePlayerInfo.streamChannelMax;
    }

    m_StreamBufferPool.Initialize(buffer, size, strmChannelCount);

    return true;
}

void SoundArchivePlayer::Update()
{
    for (u32 playerIndex = 0;
        playerIndex < m_SoundPlayerCount; ++playerIndex)
    {
        GetSoundPlayer(internal::Util::GetMaskedItemId(
            playerIndex, internal::ItemType_Player)).Update();
    }

    m_SequenceSoundInstanceManager.SortPriorityList();
    m_WaveSoundInstanceManager.SortPriorityList();
    m_StreamSoundInstanceManager.SortPriorityList();

    internal::DriverCommandManager& cmdmgr = internal::DriverCommandManager::GetInstance();
    cmdmgr.RecvCommandReply();
    cmdmgr.FlushCommand(false);
}

const SoundArchive& SoundArchivePlayer::GetSoundArchive() const
{
    NW_ASSERTMSG(m_pSoundArchive != NULL, "Setup is not completed.");
    return *m_pSoundArchive;
}

SoundPlayer& SoundArchivePlayer::GetSoundPlayer(SoundArchive::ItemId playerId)
{
    u32 itemIndex = internal::Util::GetItemIndex(playerId);
    NW_MINMAXLT_ASSERT(itemIndex, 0, m_SoundPlayerCount);
    return m_pSoundPlayers[itemIndex];
}

SoundPlayer& SoundArchivePlayer::GetSoundPlayer(const char* pStr)
{
    NW_NULL_ASSERT(m_pSoundArchive);

    SoundArchive::ItemId playerId = m_pSoundArchive->GetItemId(pStr);
    NW_ASSERT(playerId != SoundArchive::INVALID_ID);
    return GetSoundPlayer(playerId);
}

template< typename Sound, typename Player >
Sound* SoundArchivePlayer::AllocSound(
    internal::SoundInstanceManager<Sound, Player>* manager,
    SoundArchive::ItemId soundId,
    int priority, int ambientPriority,
    internal::BasicSound::AmbientInfo* ambientArgInfo)
{
    NW_NULL_ASSERT(manager);

    Sound* sound = manager->Alloc(priority, ambientPriority);
    if (sound == NULL) return NULL;

    sound->SetId(soundId);

    if (ambientArgInfo != NULL)
    {
        sound->SetAmbientInfo(*ambientArgInfo);
    }

    return sound;
}

const void*
SoundArchivePlayer::detail_GetFileAddress(SoundArchive::FileId fileId) const
{
    if (m_pSoundDataManager == NULL) return NULL;

    return m_pSoundDataManager->detail_GetFileAddress(fileId);
}

SoundStartable::StartResult SoundArchivePlayer::detail_SetupSound(
    SoundHandle* handle,u32 soundId,
    bool holdFlag,const StartInfo* startInfo)
{
    return detail_SetupSoundImpl(handle, soundId,
        NULL, NULL, holdFlag, startInfo);
}

SoundStartable::StartResult SoundArchivePlayer::detail_SetupSoundImpl(
    SoundHandle* handle, u32 soundId,
    internal::BasicSound::AmbientInfo* ambientArgInfo,
    SoundActor* actor, bool holdFlag,
    const StartInfo* startInfo)
{
    NW_NULL_ASSERT(handle);

    if (!IsAvailable())
    {
        SoundStartable::StartResult
            result(SoundStartable::StartResult::START_ERR_NOT_AVAILABLE);
        return result;
    }

    if (handle->IsAttachedSound()) 
    {
        handle->DetachSound();
    }

    SoundArchive::SoundInfo soundInfo;
    if (!m_pSoundArchive->ReadSoundInfo(soundId, &soundInfo))
    {
        SoundStartable::StartResult
            result(SoundStartable::StartResult::START_ERR_INVALID_SOUNDID);
        return result;
    }

    SoundStartable::StartInfo::StartOffsetType startOffsetType =
        SoundStartable::StartInfo::START_OFFSET_TYPE_MILLISEC;
    int startOffset = 0;
    int playerPriority = soundInfo.playerPriority;
    SoundArchive::ItemId playerId = soundInfo.playerId;
    int actorPlayerId = soundInfo.actorPlayerId;
    const SoundStartable::StartInfo::SeqSoundInfo* seqInfo = NULL;
    if (startInfo != NULL)
    {
        if (startInfo->enableFlag & StartInfo::ENABLE_START_OFFSET)
        {
            startOffsetType = startInfo->startOffsetType;
            startOffset = startInfo->startOffset;
        }
        if (startInfo->enableFlag & StartInfo::ENABLE_PLAYER_PRIORITY)
        {
            playerPriority = startInfo->playerPriority;
        }
        if (startInfo->enableFlag & StartInfo::ENABLE_PLAYER_ID)
        {
            playerId = startInfo->playerId;
        }
        if (startInfo->enableFlag & StartInfo::ENABLE_ACTOR_PLAYER_ID)
        {
            actorPlayerId = startInfo->actorPlayerId;
        }
        if (startInfo->enableFlag & StartInfo::ENABLE_SEQ_SOUND_INFO)
        {
            seqInfo = &startInfo->seqSoundInfo;
        }
    }

    int priority = playerPriority;
    if (holdFlag) 
    {
        --priority;
    }

    int ambientPriority = 0;
    if (ambientArgInfo != NULL)
    {
        ambientPriority = internal::BasicSound::GetAmbientPriority(*ambientArgInfo, soundId);
    }
    int allocPriority = priority + ambientPriority;
    allocPriority = ut::Clamp(allocPriority, internal::BasicSound::PRIORITY_MIN, internal::BasicSound::PRIORITY_MAX);

    internal::ExternalSoundPlayer* extPlayer = NULL;
    if (actor != NULL) 
    {
        extPlayer = actor->detail_GetActorPlayer(actorPlayerId);
        if (extPlayer == NULL)
        {
            NW_WARNING(
                false,
                "actorPlayerId(%d) is out of range. (0-%d)",
                actorPlayerId,
                SoundActor::ACTOR_PLAYER_COUNT-1
            );
            return StartResult::START_ERR_INVALID_PARAMETER;
        }
    }

    SoundPlayer& player = GetSoundPlayer(playerId);
    if (!player.detail_CanPlaySound(allocPriority)) 
    {
        return StartResult::START_ERR_LOW_PRIORITY;
    }
    if ((extPlayer != NULL) && (!extPlayer->detail_CanPlaySound(allocPriority))) 
    {
        return StartResult::START_ERR_LOW_PRIORITY;
    }

    internal::BasicSound* sound = NULL;
    internal::SequenceSound* seqSound = NULL;
    internal::StreamSound* strmSound = NULL;
    internal::WaveSound* waveSound = NULL;

    switch (m_pSoundArchive->GetSoundType(soundId))
    {
    case SoundArchive::SOUND_TYPE_SEQ:
        seqSound = AllocSound<internal::SequenceSound>(
            &m_SequenceSoundInstanceManager, soundId,
            priority, ambientPriority, ambientArgInfo);
        if (seqSound == NULL)
        {
            NW_WARNING(
                !internal::Debug_GetWarningFlag(DEBUG_WARNING_NOT_ENOUGH_SEQSOUND),
                "Failed to start sound (id:0x%08x) for not enough SequenceSound instance.",
                soundId);
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_NOT_ENOUGH_INSTANCE);
            return result;
        }
        sound = seqSound;
        break;

    case SoundArchive::SOUND_TYPE_STRM:
        strmSound = AllocSound<internal::StreamSound>(
            &m_StreamSoundInstanceManager, soundId,
            priority, ambientPriority, ambientArgInfo);
        if (strmSound == NULL)
        {
            NW_WARNING(
                !internal::Debug_GetWarningFlag(DEBUG_WARNING_NOT_ENOUGH_STRMSOUND),
                "Failed to start sound (id:0x%08x) for not enough StreamSound instance.",
                soundId);
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_NOT_ENOUGH_INSTANCE);
            return result;
        }
        sound = strmSound;
        break;

    case SoundArchive::SOUND_TYPE_WAVE:
        waveSound = AllocSound<internal::WaveSound>(
            &m_WaveSoundInstanceManager,
            soundId,
            priority,
            ambientPriority,
            ambientArgInfo
        );
        if (waveSound == NULL)
        {
            NW_WARNING(
                !internal::Debug_GetWarningFlag(DEBUG_WARNING_NOT_ENOUGH_WAVESOUND),
                "Failed to start sound (id:0x%08x) for not enough WaveSound instance.",
                soundId);
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_NOT_ENOUGH_INSTANCE);
            return result;
        }
        sound = waveSound;
        break;

    default:
        {
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_INVALID_SOUNDID);
            return result;
        }
    }

    if (!player.detail_AppendSound(sound))
    {
        sound->Finalize();
        SoundStartable::StartResult
            result(SoundStartable::StartResult::START_ERR_UNKNOWN);
        return result;
    }

    sound->SetFrontBypass(soundInfo.isFrontBypass);

    switch (m_pSoundArchive->GetSoundType(soundId))
    {
    case SoundArchive::SOUND_TYPE_SEQ:
    {
        NW_NULL_ASSERT(seqSound);

        (void)player.detail_AllocPlayerHeap(seqSound);

        SoundArchive::SequenceSoundInfo info;
        if (!m_pSoundArchive->ReadSequenceSoundInfo(soundId, &info)) {
            seqSound->Finalize();
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_INVALID_SOUNDID);
            return result;
        }

        if (seqInfo != NULL)
        {
            for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
            {
                SoundArchive::ItemId bankId = seqInfo->bankIds[i];
                if (bankId != SoundArchive::INVALID_ID)
                {
                    info.bankIds[i] = seqInfo->bankIds[i];
                }
            }
        }

        StartResult result = PrepareSequenceSoundImpl(
            seqSound, &soundInfo, &info,
            startOffsetType, startOffset, seqInfo);
        if (!result.IsSuccess())
        {
            seqSound->Finalize();
            return result;
        }
        break;
    }

    case SoundArchive::SOUND_TYPE_STRM:
    {
        NW_NULL_ASSERT(strmSound);

        SoundArchive::StreamSoundInfo info;
        if (!m_pSoundArchive->detail_ReadStreamSoundInfo(soundId, &info)) {
            strmSound->Finalize();
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_INVALID_SOUNDID);
            return result;
        }

        StartResult result = PrepareStreamSoundImpl(
            strmSound, &soundInfo,
            &info, startOffsetType, startOffset);
        if (!result.IsSuccess())
        {
            strmSound->Finalize();
            return result;
        }
        break;
    }

    case SoundArchive::SOUND_TYPE_WAVE:
    {
        NW_NULL_ASSERT(waveSound);

        (void)player.detail_AllocPlayerHeap(waveSound);

        SoundArchive::WaveSoundInfo info;
        if (!m_pSoundArchive->detail_ReadWaveSoundInfo(soundId, &info)) {
            waveSound->Finalize();
            SoundStartable::StartResult
                result(SoundStartable::StartResult::START_ERR_INVALID_SOUNDID);
            return result;
        }

        StartResult result = PrepareWaveSoundImpl(
            waveSound, &soundInfo,
            &info, startOffsetType, startOffset);
        if (!result.IsSuccess())
        {
            waveSound->Finalize();
            return result;
        }
        break;
    }

    default:
        NW_ASSERT(false);
        sound->Finalize();
        {
            return StartResult::START_ERR_INVALID_SOUNDID;
        }
    }

    if (extPlayer != NULL)
    {
        if (!extPlayer->AppendSound(sound))
        {
            sound->Finalize();
            return StartResult::START_ERR_UNKNOWN;
        }
    }

    if (actor != NULL)
    {
        sound->AttachSoundActor(actor);
    }

    if (holdFlag)
    {
        sound->SetPlayerPriority(playerPriority);
    }

    handle->detail_AttachSound(sound);

    SoundStartable::StartResult
        result(SoundStartable::StartResult::START_SUCCESS);
    return result;
}

SoundStartable::StartResult SoundArchivePlayer::PrepareSequenceSoundImpl(
    internal::SequenceSound* sound,
    const SoundArchive::SoundInfo* commonInfo,
    const SoundArchive::SequenceSoundInfo* info,
    SoundStartable::StartInfo::StartOffsetType startOffsetType,
    int startOffset,
    const StartInfo::SeqSoundInfo* externalSeqInfo)
{
    NW_NULL_ASSERT(info);

    const internal::SequenceSoundFile* seqFile =
        reinterpret_cast<const internal::SequenceSoundFile*>(
                m_pSoundDataManager->detail_GetFileAddress(commonInfo->fileId));
    u32 seqOffset = info->startOffset;
    u32 allocTrackBitFlag = info->allocateTrackFlags;

    if (externalSeqInfo != NULL)
    {
        const void* extSeqFile = externalSeqInfo->seqDataAddress;
        if (extSeqFile != NULL)
        {
            seqFile = reinterpret_cast<const internal::SequenceSoundFile*>(extSeqFile);
        }

        internal::SequenceSoundFileReader seqFileReader(seqFile);
        const char* startLabel = externalSeqInfo->startLocationLabel;
        if (startLabel != NULL)
        {
            if (!seqFileReader.GetOffsetByLabel(startLabel, &seqOffset))
            {
                SoundStartable::StartResult result(
                        SoundStartable::StartResult::START_ERR_INVALID_SEQ_START_LOCATION_LABEL);
                return result;
            }
            seqOffset = internal::driver::MmlParser::ParseAllocTrack(
                    seqFileReader.GetSequenceData(),
                    seqOffset,
                    &allocTrackBitFlag);
        }
    }

    internal::PlayerHeap* pPlayerHeap = sound->GetPlayerHeap();
    bool isRegisterDataLoadTask = false;
    internal::LoadItemInfo loadSeq;
    internal::LoadItemInfo loadBanks[SoundArchive::SEQ_BANK_MAX];
    int fileAddressCount = 0;

    loadSeq.itemId = sound->GetId();
    loadSeq.address = seqFile;
    if (seqFile == NULL)
    {
        if (pPlayerHeap == NULL)
        {
            SoundStartable::StartResult result(
                    SoundStartable::StartResult::START_ERR_NOT_SEQ_LOADED);
            return result;
        }
        isRegisterDataLoadTask = true;
    }

    const void* bankFiles[SoundArchive::SEQ_BANK_MAX] = {NULL};

    if (externalSeqInfo != NULL)
    {
        for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
        {
            loadBanks[i].itemId = externalSeqInfo->bankIds[i];
        }
    }
    else
    {
        for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
        {
            loadBanks[i].itemId = info->bankIds[i];
        }
    }

    for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
    {
        SoundArchive::BankInfo bankInfo;
        bool isReadBankInfo = GetSoundArchive().ReadBankInfo(loadBanks[i].itemId, &bankInfo);
        if (isReadBankInfo)
        {
            const void* bankFile =
                m_pSoundDataManager->detail_GetFileAddress(bankInfo.fileId);
            loadBanks[i].address = bankFile;

            if (bankFile == NULL)
            {
                if (pPlayerHeap == NULL)
                {
                    SoundStartable::StartResult result(
                            SoundStartable::StartResult::START_ERR_NOT_BANK_LOADED);
                    return result;
                }
                isRegisterDataLoadTask = true;
                break;
            }
            else
            {
                bool isLoaded = internal::Util::IsLoadedWaveArchive(
                        bankFile,
                        *m_pSoundArchive,
                        *m_pSoundDataManager);
                if (!isLoaded)
                {
                    if (pPlayerHeap == NULL)
                    {
                        SoundStartable::StartResult result(
                                SoundStartable::StartResult::START_ERR_NOT_WARC_LOADED);
                        return result;
                    }
                    isRegisterDataLoadTask = true;
                    break;
                }
            }
        }
    }

    sound->Setup(
            m_pSequenceTrackAllocator,
            allocTrackBitFlag,
            &m_SequenceCallback);

    UpdateCommonSoundParam(sound, commonInfo);

    sound->SetChannelPriority(info->channelPriority);
    sound->SetReleasePriorityFix(info->isReleasePriorityFix);
    sound->SetSequenceUserprocCallback(
            m_SequenceUserprocCallback, m_pSequenceUserprocCallbackArg);

    internal::driver::SequenceSoundPlayer::OffsetType seqOffsetType;
    switch (startOffsetType)
    {
    case SoundStartable::StartInfo::START_OFFSET_TYPE_MILLISEC:
        seqOffsetType = internal::driver::SequenceSoundPlayer::OFFSET_TYPE_MILLISEC;
        break;
    case SoundStartable::StartInfo::START_OFFSET_TYPE_TICK:
        seqOffsetType = internal::driver::SequenceSoundPlayer::OFFSET_TYPE_TICK;
        break;
    case SoundStartable::StartInfo::START_OFFSET_TYPE_SAMPLE:
        seqOffsetType = internal::driver::SequenceSoundPlayer::OFFSET_TYPE_TICK;
        startOffset = 0;
        break;
    default:
        seqOffsetType = internal::driver::SequenceSoundPlayer::OFFSET_TYPE_TICK;
        startOffset = 0;
        break;
    }

    internal::SequenceSound::StartInfo startInfo =
    {
        static_cast<s32>(seqOffset),
        seqOffsetType,
        startOffset
    };

    if (isRegisterDataLoadTask)
    {
        internal::SequenceSound::LoadInfo loadInfo =
        {
            m_pSoundArchive,
            m_pSoundDataManager,
            &loadSeq,
            loadBanks
        };
        bool result = sound->RegisterDataLoadTask(loadInfo, startInfo);
        if (result == false)
        {
            SoundStartable::StartResult startResult(
                    SoundStartable::StartResult::START_ERR_NOT_ENOUGH_PLAYER_HEAP);
            return startResult;
        }
    }
    else
    {
        internal::SequenceSoundFileReader seqFileReader(seqFile);
        sound->Prepare(seqFileReader.GetSequenceData(), startInfo);

        sound->SetBankData(loadBanks);
    }

    SoundStartable::StartResult startResult(
            SoundStartable::StartResult::START_SUCCESS);
    return startResult;
}

SoundStartable::StartResult SoundArchivePlayer::PrepareStreamSoundImpl(
    internal::StreamSound* sound,
    const SoundArchive::SoundInfo* commonInfo,
    const SoundArchive::StreamSoundInfo* info,
    SoundStartable::StartInfo::StartOffsetType startOffsetType,
    int startOffset
)
{

    sound->Setup(
        &m_StreamBufferPool,
        info->allocChannelCount,
        info->allocTrackCount
    );

    internal::driver::StreamSoundPlayer::StartOffsetType strmStartOffsetType;
    switch (startOffsetType)
    {
    case SoundStartable::StartInfo::START_OFFSET_TYPE_MILLISEC:
        strmStartOffsetType = internal::driver::StreamSoundPlayer::START_OFFSET_TYPE_MILLISEC;
        break;
    case SoundStartable::StartInfo::START_OFFSET_TYPE_TICK:
        strmStartOffsetType = internal::driver::StreamSoundPlayer::START_OFFSET_TYPE_SAMPLE;
        startOffset = 0;
        break;
    case SoundStartable::StartInfo::START_OFFSET_TYPE_SAMPLE:
        strmStartOffsetType = internal::driver::StreamSoundPlayer::START_OFFSET_TYPE_SAMPLE;
        break;
    default:
        strmStartOffsetType = internal::driver::StreamSoundPlayer::START_OFFSET_TYPE_SAMPLE;
        startOffset = 0;
        break;
    }

    void* fileStreamBuffer = sound->GetFileStreamBuffer();
    NW_NULL_ASSERT(fileStreamBuffer);
    long fileStreamBufferSize = sound->GetFileStreamBufferSize();
    io::FileStream* fileStream =
        const_cast<SoundArchive*>(m_pSoundArchive)->detail_OpenFileStream(
        commonInfo->fileId,
        fileStreamBuffer,
        fileStreamBufferSize
    );
    if (fileStream == NULL) {
        SoundStartable::StartResult result(
                SoundStartable::StartResult::START_ERR_CANNOT_OPEN_FILE);
        return result;
    }

    sound->Prepare(strmStartOffsetType, startOffset, fileStream);

    UpdateCommonSoundParam(sound, commonInfo);

    SoundStartable::StartResult startResult(
            SoundStartable::StartResult::START_SUCCESS);
    return startResult;
}

SoundStartable::StartResult SoundArchivePlayer::PrepareWaveSoundImpl(
    internal::WaveSound* sound,
    const SoundArchive::SoundInfo* commonInfo,
    const SoundArchive::WaveSoundInfo* info,
    SoundStartable::StartInfo::StartOffsetType startOffsetType,
    int startOffset)
{
    NW_NULL_ASSERT(info);

    internal::PlayerHeap* pPlayerHeap = sound->GetPlayerHeap();
    bool isRegisterDataLoadTask = false;
    internal::LoadItemInfo loadWsd;

    const void* wsdFile = m_pSoundDataManager->detail_GetFileAddress(commonInfo->fileId);
    loadWsd.itemId = sound->GetId();
    loadWsd.address = wsdFile;
    if (wsdFile == NULL)
    {
        if (pPlayerHeap == NULL)
        {
            SoundStartable::StartResult result(
                    SoundStartable::StartResult::START_ERR_NOT_WSD_LOADED);
            return result;
        }
        isRegisterDataLoadTask = true;
    }
    else
    {
        bool isLoaded = internal::Util::IsLoadedWaveArchive(
                wsdFile,
                info->index,
                *m_pSoundArchive,
                *m_pSoundDataManager);
        if (!isLoaded)
        {
            if (pPlayerHeap == NULL)
            {
                SoundStartable::StartResult result(
                        SoundStartable::StartResult::START_ERR_NOT_WARC_LOADED);
                return result;
            }
            isRegisterDataLoadTask = true;
        }
    }

    internal::driver::WaveSoundPlayer::StartOffsetType wsdStartOffsetType;
    switch (startOffsetType)
    {
    case SoundStartable::StartInfo::START_OFFSET_TYPE_MILLISEC:
        wsdStartOffsetType = internal::driver::WaveSoundPlayer::START_OFFSET_TYPE_MILLISEC;
        break;
    case SoundStartable::StartInfo::START_OFFSET_TYPE_TICK:
        wsdStartOffsetType = internal::driver::WaveSoundPlayer::START_OFFSET_TYPE_SAMPLE;
        startOffset = 0;
        break;
    case SoundStartable::StartInfo::START_OFFSET_TYPE_SAMPLE:
        wsdStartOffsetType = internal::driver::WaveSoundPlayer::START_OFFSET_TYPE_SAMPLE;
        break;
    default:
        wsdStartOffsetType = internal::driver::WaveSoundPlayer::START_OFFSET_TYPE_SAMPLE;
        startOffset = 0;
        break;
    }

    internal::WaveSound::StartInfo startInfo =
    {
        static_cast<s32>(info->index),
        wsdStartOffsetType,
        startOffset,
        &m_WaveSoundCallback,
        commonInfo->fileId
    };

    if (isRegisterDataLoadTask)
    {
        internal::WaveSound::LoadInfo loadInfo =
        {
            m_pSoundArchive,
            m_pSoundDataManager,
            &loadWsd
        };
        bool result = sound->RegisterDataLoadTask(loadInfo, startInfo);
    }
    else
    {
        sound->Prepare(wsdFile, startInfo);
    }

    UpdateCommonSoundParam(sound, commonInfo);
    sound->SetChannelPriority(info->channelPriority);
    sound->SetReleasePriorityFix(info->isReleasePriorityFix);

    SoundStartable::StartResult startResult(
            SoundStartable::StartResult::START_SUCCESS);
    return startResult;
}

void SoundArchivePlayer::UpdateCommonSoundParam(
    internal::BasicSound* sound,
    const SoundArchive::SoundInfo* commonInfo)
{
    NW_NULL_ASSERT(sound);
    NW_NULL_ASSERT(commonInfo);

    sound->SetInitialVolume(static_cast<float>(commonInfo->volume) / 127.0f);
    sound->SetPanMode(commonInfo->panMode);
    sound->SetPanCurve(commonInfo->panCurve);
}

void SoundArchivePlayer::SetSequenceUserprocCallback(SequenceUserprocCallback callback, void* callbackArg)
{
    m_SequenceUserprocCallback = callback;
    m_pSequenceUserprocCallbackArg = callbackArg;
}

internal::driver::Channel* SoundArchivePlayer::SequenceNoteOnCallback::NoteOn(
    internal::driver::SequenceSoundPlayer* seqPlayer,
    u8 bankIndex,
    const internal::driver::NoteOnInfo& noteOnInfo)
{
    if (!m_pSoundArchivePlayer.IsAvailable()) return NULL;

    const SoundArchive& sndArc = m_pSoundArchivePlayer.GetSoundArchive();

    internal::driver::Bank bank;
    internal::driver::Channel* channel = bank.NoteOn(
        seqPlayer->GetBankFile(bankIndex),
        noteOnInfo,
        sndArc,
        m_pSoundArchivePlayer,
        seqPlayer->GetPlayerHeapDataManager()
    );

    internal::driver::SoundThread::GetInstance().IncrNoteOnCount();
    return channel;
}

bool SoundArchivePlayer::WaveSoundCallback::GetWaveSoundData(
    internal::WaveSoundInfo* info,
    internal::WaveSoundNoteInfo* noteInfo,
    internal::WaveInfo* waveInfo,
    const internal::driver::WaveSoundPlayer::WaveSoundCallbackArg& arg) const
{
    if (!m_pSoundArchivePlayer.IsAvailable()) {
        return false;
    }

    {
        internal::WaveSoundFileReader reader(arg.wsdFile);
        if (!reader.ReadWaveSoundInfo(info, arg.wsdIndex))
        {
            return false;
        }
        if (!reader.ReadNoteInfo(noteInfo, arg.wsdIndex, arg.noteIndex))
        {
            return false;
        }
    }

    const SoundArchive& sndArc = m_pSoundArchivePlayer.GetSoundArchive();

    const void* waveFile = internal::Util::GetWaveFile(
            noteInfo->waveArchiveId, noteInfo->waveIndex,
            sndArc, m_pSoundArchivePlayer);
    if (waveFile == NULL)
    {
        if (arg.dataMgr != NULL)
        {
            waveFile = internal::Util::GetWaveFile(
                    noteInfo->waveArchiveId, noteInfo->waveIndex, sndArc, arg.dataMgr);
            if (waveFile == NULL)
            {
                return NULL;
            }
        }
        else
        {
            return false;
        }
    }

    {
        internal::WaveFileReader reader(waveFile);
        if (!reader.ReadWaveInfo(waveInfo))
        {
            return false;
        }
    }

    return true;
}

} // namespace snd
} // namespace nw