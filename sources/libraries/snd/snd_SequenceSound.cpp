// Filename: snd_SequenceSound.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_SequenceSound.h>

#include <nw/snd/snd_SoundInstanceManager.h>
#include <nw/snd/snd_SequenceTrack.h>
#include <nw/snd/snd_SequenceSoundFile.h>
#include <nw/snd/snd_SequenceSoundFileReader.h>
#include <nw/snd/snd_SequenceSoundHandle.h>
#include <nw/snd/snd_TaskManager.h>

#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_PlayerHeap.h>
#include <nw/snd/snd_SoundDataManager.h>

namespace nw {
namespace snd {
namespace internal {

NW_UT_RUNTIME_TYPEINFO_DEFINITION(SequenceSound, BasicSound);

SequenceSound::SequenceSound(SequenceSoundInstanceManager& manager):
    m_Manager(manager),
    m_InitializeFlag(false)
{
}

void SequenceSound::Initialize()
{
    BasicSound::Initialize();

    m_pTempSpecialHandle = NULL;
    m_LoadingFlag = false;
    m_PreparedFlag = false;
    m_StartInfo.startOffset = 0;

    m_InitializeFlag = true;
}

void SequenceSound::Finalize()
{
    if (!m_InitializeFlag)
    {
        return;
    }
    m_InitializeFlag = false;

    if (m_LoadingFlag)
    {
        TaskManager::GetInstance().CancelTask(&m_DataLoadTask);
        m_DataLoadTask.Wait();
    }
    
    BasicSound::Finalize();

    m_DataManager.Finalize();
    m_Manager.Free(this);
}

void SequenceSound::Setup(driver::SequenceTrackAllocator* trackAllocator,
    u32 allocTracks, driver::NoteOnCallback* noteOnCallback)
{
    NW_NULL_ASSERT(noteOnCallback);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqInit* command = cmdmgr.AllocCommand<DriverCommandSeqInit>();
    command->id = DRIVER_COMMAND_SEQ_INIT;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackAllocator = trackAllocator;
    command->allocTracks = allocTracks;
    command->noteOnCallback = noteOnCallback;
    cmdmgr.PushCommand(command);
}

void SequenceSound::Prepare(const void* seqBase, const StartInfo& startInfo)
{
    NW_NULL_ASSERT(seqBase);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqSetData* command = cmdmgr.AllocCommand<DriverCommandSeqSetData>();
    command->id = DRIVER_COMMAND_SEQ_SEQDATA;
    command->player = &m_SequenceSoundPlayerInstance;
    command->seqBase = seqBase;
    command->seqOffset = startInfo.seqOffset;
    cmdmgr.PushCommand(command);

    if (startInfo.startOffset > 0 )
    {
        Skip(startInfo.startOffsetType, startInfo.startOffset);
    }

    m_PreparedFlag = true;
}

void SequenceSound::NotifyAsyncLoadFinishedFunc(bool result,
    const LoadItemInfo* seq, const LoadItemInfo banks[], void* userData)
{
    NW_NULL_ASSERT(userData);
    SequenceSound* sound = static_cast<SequenceSound*>(userData);

    sound->m_LoadingFlag = false;

    if (!result)
    {
        sound->Stop(0);
        return;
    }

    SequenceSoundFileReader reader(seq->address);
    const void* seqBase = reader.GetSequenceData();
    sound->Prepare(seqBase, sound->m_StartInfo);

    sound->SetBankData(banks);

    sound->m_SequenceSoundPlayerInstance.SetPlayerHeapDataManager(&sound->m_DataManager);
}

void SequenceSound::Skip(driver::SequenceSoundPlayer::OffsetType offsetType, int offset)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqSkip* command = cmdmgr.AllocCommand<DriverCommandSeqSkip>();
    command->id = DRIVER_COMMAND_SEQ_SKIP;
    command->player = &m_SequenceSoundPlayerInstance;
    command->offsetType = offsetType;
    command->offset = offset;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTempoRatio(f32 tempoRatio)
{
    NW_ASSERT(tempoRatio >= 0.0f);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTempoRatio* command = cmdmgr.AllocCommand<DriverCommandSeqTempoRatio>();
    command->id = DRIVER_COMMAND_SEQ_TEMPORATIO;
    command->player = &m_SequenceSoundPlayerInstance;
    command->tempoRatio = tempoRatio;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetChannelPriority(int priority)
{
    NW_MINMAX_ASSERT(priority, 0, 127);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqChannelPrio* command = cmdmgr.AllocCommand<DriverCommandSeqChannelPrio>();
    command->id = DRIVER_COMMAND_SEQ_CHANNELPRIO;
    command->player = &m_SequenceSoundPlayerInstance;
    command->priority = priority;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetReleasePriorityFix(bool fix)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqPrioFix* command = cmdmgr.AllocCommand<DriverCommandSeqPrioFix>();
    command->id = DRIVER_COMMAND_SEQ_PRIOFIX;
    command->player = &m_SequenceSoundPlayerInstance;
    command->priorityFix = fix;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetSequenceUserprocCallback(SequenceUserprocCallback callback, void* arg)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqUserProc* command = cmdmgr.AllocCommand<DriverCommandSeqUserProc>();
    command->id = DRIVER_COMMAND_SEQ_USERPROC;
    command->player = &m_SequenceSoundPlayerInstance;
    command->callback = reinterpret_cast<uptr>(callback);
    command->arg = arg;
    cmdmgr.PushCommand(command);
}

void SequenceSound::OnUpdatePlayerPriority()
{
    int priority = CalcCurrentPlayerPriority();
    m_Manager.UpdatePriority(this, priority);
}

void SequenceSound::SetTrackMute(u32 trackBitFlag, SeqMute mute)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackMute* command = cmdmgr.AllocCommand<DriverCommandSeqTrackMute>();
    command->id = DRIVER_COMMAND_SEQTRACK_MUTE;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->mute = mute;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackMute(u32 trackBitFlag, bool muteFlag)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackMute* command = cmdmgr.AllocCommand<DriverCommandSeqTrackMute>();
    command->id = DRIVER_COMMAND_SEQTRACK_MUTE;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->mute = muteFlag ? SEQ_MUTE_STOP: SEQ_MUTE_OFF;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackSilence(u32 trackBitFlag, bool silenceFlag, int fadeFrames)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackSilence* command = cmdmgr.AllocCommand<DriverCommandSeqTrackSilence>();
    command->id = DRIVER_COMMAND_SEQTRACK_MUTE;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->silenceFlag = silenceFlag;
    command->fadeFrames = fadeFrames;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackVolume(u32 trackBitFlag, f32 volume)
{
    NW_ASSERT(volume >= 0.0f);

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_VOLUME;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = volume;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackPitch(u32 trackBitFlag, f32 pitch)
{
    NW_ASSERT(pitch >= 0.0f);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_PITCH;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = pitch;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackPan(u32 trackBitFlag, f32 pan)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_PAN;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = pan;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackSurroundPan(u32 trackBitFlag, f32 surroundPan)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_SPAN;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = surroundPan;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackLpfFreq(u32 trackBitFlag, f32 lpfFreq)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_LPF;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = lpfFreq;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackBiquadFilter(u32 trackBitFlag, int type, f32 value)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackBiquad* command = cmdmgr.AllocCommand<DriverCommandSeqTrackBiquad>();
    command->id = DRIVER_COMMAND_SEQTRACK_BIQUAD;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->type = type;
    command->value = value;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackPanRange(u32 trackBitFlag, f32 panRange)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_PANRANGE;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = panRange;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackModDepth(u32 trackBitFlag, f32 depth)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_MODDEPTH;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = depth;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackModSpeed(u32 trackBitFlag, f32 speed)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackParam* command = cmdmgr.AllocCommand<DriverCommandSeqTrackParam>();
    command->id = DRIVER_COMMAND_SEQTRACK_MODSPEED;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = speed;
    cmdmgr.PushCommand(command);
}

void SequenceSound::SetTrackBankIndex(u32 trackBitFlag, int bankIndex)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqTrackBankIndex* command = cmdmgr.AllocCommand<DriverCommandSeqTrackBankIndex>();
    command->id = DRIVER_COMMAND_SEQTRACK_BANKINDEX;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->bankIndex = bankIndex;
    cmdmgr.PushCommand(command);
}

bool SequenceSound::ReadVariable(int varNo, s16* var) const
{
    NW_NULL_ASSERT(var);
    NW_MINMAXLT_ASSERT(varNo, 0, driver::SequenceSoundPlayer::PLAYER_VARIABLE_NUM);

    if (!IsPlayerAvailable())
    {
        *var = driver::SequenceSoundPlayer::VARIABLE_DEFAULT_VALUE;
    }
    else
    {
        *var = m_SequenceSoundPlayerInstance.GetLocalVariable( varNo );
    }
    return true;
}

bool SequenceSound::ReadGlobalVariable(int varNo, s16* var)
{
    NW_NULL_ASSERT(var);
    NW_MINMAXLT_ASSERT(varNo, 0, driver::SequenceSoundPlayer::GLOBAL_VARIABLE_NUM);
    
    *var = driver::SequenceSoundPlayer::GetGlobalVariable(varNo);
    return true;
}

bool SequenceSound::ReadTrackVariable(int trackNo, int varNo, s16* var) const
{
    NW_NULL_ASSERT(var);
    NW_MINMAXLT_ASSERT(trackNo, 0, driver::SequenceSoundPlayer::TRACK_NUM_PER_PLAYER);
    NW_MINMAXLT_ASSERT(varNo, 0, driver::SequenceTrack::TRACK_VARIABLE_NUM);

    if (!IsPlayerAvailable())
    {
        *var = driver::SequenceSoundPlayer::VARIABLE_DEFAULT_VALUE;
    }
    else
    {
        const driver::SequenceTrack* track = m_SequenceSoundPlayerInstance.GetPlayerTrack(trackNo);
        if(track == NULL)
        {
            *var = driver::SequenceSoundPlayer::VARIABLE_DEFAULT_VALUE;
        }
        else
        {
            *var = track->GetTrackVariable(varNo);
        }
    }
    return true;
}

void SequenceSound::WriteVariable(int varNo, s16 var)
{
    NW_MINMAXLT_ASSERT(varNo, 0, driver::SequenceSoundPlayer::PLAYER_VARIABLE_NUM);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqSetVar* command = cmdmgr.AllocCommand<DriverCommandSeqSetVar>();
    command->id = DRIVER_COMMAND_SEQ_SETVAR;
    command->player = &m_SequenceSoundPlayerInstance;
    command->varNo = varNo;
    command->var = var;
    cmdmgr.PushCommand(command);
}

void SequenceSound::WriteGlobalVariable(int varNo, s16 var)
{
    NW_MINMAXLT_ASSERT(varNo, 0, driver::SequenceSoundPlayer::GLOBAL_VARIABLE_NUM );

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqSetVar* command = cmdmgr.AllocCommand<DriverCommandSeqSetVar>();
    command->id = DRIVER_COMMAND_SEQ_SETGVAR;
    command->varNo = varNo;
    command->var = var;
    cmdmgr.PushCommand(command);
}

void SequenceSound::WriteTrackVariable(int trackNo, int varNo, s16 var)
{
    NW_MINMAXLT_ASSERT(trackNo, 0, driver::SequenceSoundPlayer::TRACK_NUM_PER_PLAYER);
    NW_MINMAXLT_ASSERT(varNo, 0, driver::SequenceTrack::TRACK_VARIABLE_NUM);
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandSeqSetVar* command = cmdmgr.AllocCommand<DriverCommandSeqSetVar>();
    command->id = DRIVER_COMMAND_SEQ_SETTVAR;
    command->player = &m_SequenceSoundPlayerInstance;
    command->trackNo = trackNo;
    command->varNo = varNo;
    command->var = var;
    cmdmgr.PushCommand(command);
}

u32 SequenceSound::GetTick() const
{
    if (!IsPlayerAvailable()) 
    {
        return 0;
    }

    return m_SequenceSoundPlayerInstance.GetTickCounter();
}

bool SequenceSound::IsAttachedTempSpecialHandle()
{
    return m_pTempSpecialHandle != NULL;
}

void SequenceSound::DetachTempSpecialHandle()
{
    m_pTempSpecialHandle->DetachSound();
}

/* SequenceSound::DataLoadTask */

SequenceSound::DataLoadTask::DataLoadTask(): 
    m_pDataManager(NULL),
    m_pAllocator(NULL),
    m_Callback(NULL),
    m_CallbackData(NULL)
{
}

bool SequenceSound::RegisterDataLoadTask(const LoadInfo& loadInfo, const StartInfo& startInfo)
{
    PlayerHeap* heap = GetPlayerHeap();
    if (heap == NULL)
    {
        return false;
    }
    m_DataManager.Initialize(loadInfo.arc);
    m_DataLoadTask.m_pDataManager = &m_DataManager;
    m_DataLoadTask.m_pAllocator = heap;
    m_DataLoadTask.m_Callback = NotifyAsyncLoadFinishedFunc;
    m_DataLoadTask.m_CallbackData = this;

    m_DataLoadTask.m_pSoundDataManager = loadInfo.mgr;
    m_DataLoadTask.m_pSoundArchive = loadInfo.arc;
    m_DataLoadTask.m_LoadInfoSeq = *(loadInfo.seq);
    for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
    {
        m_DataLoadTask.m_LoadInfoBanks[i] = loadInfo.banks[i];
    }

    m_StartInfo = startInfo;

    m_LoadingFlag = true;
    TaskManager::GetInstance().AppendTask(&m_DataLoadTask, TaskManager::PRIORITY_MIDDLE);
    return true;
}

void SequenceSound::DataLoadTask::Execute()
{
    m_pAllocator->Clear();
    
    if ((m_LoadInfoSeq.address == NULL) &&
        (m_LoadInfoSeq.itemId != SoundArchive::INVALID_ID))
    {
        SoundArchive::ItemId soundId = m_LoadInfoSeq.itemId;

        if (!m_pDataManager->LoadData(soundId, m_pAllocator, SoundArchiveLoader::LOAD_SEQ))
        {
            NW_WARNING(false, "failed to load SEQ(%08x) to PlayerHeap", soundId);
            if (m_Callback)
            {
                m_Callback(false, NULL, NULL, m_CallbackData);
                return;
            }
        }
        m_LoadInfoSeq.address = m_pDataManager->detail_GetFileAddressByItemId( soundId );
    }

    for (int i = 0; i < SoundArchive::SEQ_BANK_MAX; i++)
    {
        if ((m_LoadInfoBanks[i].address == NULL) &&
            (m_LoadInfoBanks[i].itemId != SoundArchive::INVALID_ID))
        {
            SoundArchive::ItemId bankId = m_LoadInfoBanks[i].itemId;
            if (!m_pDataManager->LoadData(bankId, m_pAllocator, SoundArchiveLoader::LOAD_BANK))
            {
                NW_WARNING(false, "failed to load BANK(%08x) to PlayerHeap", bankId);
                if (m_Callback)
                {
                    m_Callback(false, NULL, NULL, m_CallbackData);
                    return;
                }
            }
            m_LoadInfoBanks[i].address = m_pDataManager->detail_GetFileAddressByItemId(bankId);
        }

        if (m_LoadInfoBanks[i].itemId != SoundArchive::INVALID_ID)
        {
            bool result = Util::IsLoadedWaveArchive(m_LoadInfoBanks[i].address, *m_pSoundArchive, *m_pSoundDataManager);
            if (!result)
            {
                if (!m_pDataManager->detail_LoadWaveArchiveByBankFile(m_LoadInfoBanks[i].address, m_pAllocator))
                {
                    NW_WARNING(false,
                        "failed to load BANK(%08x)'s WARC to PlayerHeap",
                        m_LoadInfoBanks[i].itemId);
                    m_Callback(false, NULL, NULL, m_CallbackData);
                    return;
                }
            }
        }
    }

    if (m_Callback)
    {
        m_Callback(true, &m_LoadInfoSeq, m_LoadInfoBanks, m_CallbackData);
    }
}

} // namespace internal
} // namespace snd
} // namespace nw