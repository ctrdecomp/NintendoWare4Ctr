// Filename: snd_StreamSound.cpp
//
// Project: NintendoWare4Ctr


#include <nw/snd/snd_StreamSound.h>
#include <nw/snd/snd_StreamSoundHandle.h>
#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {
namespace internal {

NW_UT_RUNTIME_TYPEINFO_DEFINITION(StreamSound,BasicSound);

StreamSound::StreamSound(StreamSoundInstanceManager& manager):  
    m_Manager(manager),
    m_InitializeFlag(false)
{
}

void StreamSound::Initialize()
{
    driver::BasicSoundPlayer* basicPlayer = GetBasicSoundPlayerHandle();
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    if (!basicPlayer->TryWaitInstanceFree()) 
    {
        cmdmgr.FlushCommand(true);
        basicPlayer->WaitInstanceFree();
    }
    
    BasicSound::Initialize();
    
    m_pTempSpecialHandle = NULL;
    
    for (int i = 0; i < driver::StreamSoundPlayer::STRM_TRACK_NUM; i++)
    {
        m_TrackVolume[i].InitValue(0.0f);
        m_TrackVolume[i].SetTarget(1.0f, 1);
    }

    m_InitializeFlag = true;
}

void StreamSound::Finalize()
{
    if (!m_InitializeFlag) 
    {
        return;
    }
    m_InitializeFlag = false;
    
    BasicSound::Finalize();

    m_Manager.Free(this);
}

void StreamSound::Setup(driver::StreamBufferPool* pBufferPool, 
    int allocChannelCount, u16 allocTrackFlag)
{
    NW_NULL_ASSERT(pBufferPool);

    m_AllocTrackFlag = allocTrackFlag;
    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandStreamSoundInit* command = cmdmgr.AllocCommand<DriverCommandStreamSoundInit>();
    command->id = DRIVER_COMMAND_STRM_INIT;
    command->player = &m_StreamSoundPlayerInstance;
    command->pBufferPool = pBufferPool;
    command->allocChannelCount = allocChannelCount;
    command->allocTrackFlag = allocTrackFlag;
    cmdmgr.PushCommand(command);
}

void StreamSound::Prepare(
    driver::StreamSoundPlayer::StartOffsetType startOffsetType,
    s32 offset, io::FileStream* pFileStream)
{    
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandStreamSoundSetup* command = cmdmgr.AllocCommand<DriverCommandStreamSoundSetup>();
    command->id = DRIVER_COMMAND_STRM_SETUP;
    command->player = &m_StreamSoundPlayerInstance;
    command->pFileStream = pFileStream;
    command->startOffsetType = startOffsetType;
    command->offset = offset;
    cmdmgr.PushCommand(command);
}

void StreamSound::UpdateMoveValue()
{
    BasicSound::UpdateMoveValue();
    
    u16 bitFlag = m_AllocTrackFlag;
    for (int trackNo = 0; trackNo < driver::StreamSoundPlayer::STRM_TRACK_NUM; trackNo++, bitFlag >>= 1)
    {
        if (bitFlag & 0x01)
        {
            m_TrackVolume[trackNo].Update();
        }
    }
}

void StreamSound::UpdateParam()
{
    BasicSound::UpdateParam();

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    
    u16 bitFlag = m_AllocTrackFlag;
    for (int trackNo = 0; trackNo < driver::StreamSoundPlayer::STRM_TRACK_NUM; trackNo++, bitFlag >>= 1)
    {
        if (bitFlag & 0x01)
        {
            DriverCommandStreamSoundTrackParam* command = cmdmgr.AllocCommand<DriverCommandStreamSoundTrackParam>();
            command->id = DRIVER_COMMAND_STRM_TRACK_VOLUME;
            command->player = &m_StreamSoundPlayerInstance;
            command->trackBitFlag = static_cast<unsigned long>(1 << trackNo);
            command->value = m_TrackVolume[trackNo].GetValue();
            cmdmgr.PushCommand(command);
        }
    }
}

void StreamSound::SetTrackVolume(unsigned long trackBitFlag, float volume, int frames)
{
    if (volume < 0.0f) 
    {
        volume = 0.0f;
    }
    
    u16 bitFlag = (m_AllocTrackFlag & trackBitFlag);
    for(int trackNo = 0; trackNo < driver::StreamSoundPlayer::STRM_TRACK_NUM && trackBitFlag != 0; trackNo++, bitFlag >>=1)
    {
        if (bitFlag & 0x01)
        {
            m_TrackVolume[trackNo].SetTarget(volume, frames);
        }
    }
}

void StreamSound::SetTrackPan(unsigned long trackBitFlag, float pan)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandStreamSoundTrackParam* command = cmdmgr.AllocCommand<DriverCommandStreamSoundTrackParam>();
    command->id = DRIVER_COMMAND_STRM_TRACK_PAN;
    command->player = &m_StreamSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = pan;
    cmdmgr.PushCommand(command);
}

void StreamSound::SetTrackSurroundPan(unsigned long trackBitFlag, float span)
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandStreamSoundTrackParam* command = cmdmgr.AllocCommand<DriverCommandStreamSoundTrackParam>();
    command->id = DRIVER_COMMAND_STRM_TRACK_SPAN;
    command->player = &m_StreamSoundPlayerInstance;
    command->trackBitFlag = trackBitFlag;
    command->value = span;
    cmdmgr.PushCommand(command);
}

void StreamSound::OnUpdatePlayerPriority()
{
    m_Manager.UpdatePriority(this, CalcCurrentPlayerPriority());
}

bool StreamSound::IsAttachedTempSpecialHandle()
{
    return m_pTempSpecialHandle != NULL;
}

void StreamSound::DetachTempSpecialHandle()
{
    m_pTempSpecialHandle->DetachSound();
}

bool StreamSound::ReadStreamDataInfo(StreamDataInfo* info) const
{
    if (!IsPlayerAvailable()) 
    {
        return 0;
    }
    return m_StreamSoundPlayerInstance.ReadStreamDataInfo(info);
}

long StreamSound::GetPlayLoopCount() const
{
    if (!IsPlayerAvailable()) 
    {
        return 0;
    }
    return m_StreamSoundPlayerInstance.GetPlayLoopCount();
}

long StreamSound::GetPlaySamplePosition() const
{
    if (!IsPlayerAvailable()) 
    {
        return 0;
    }
    return m_StreamSoundPlayerInstance.GetPlaySamplePosition();
}

float StreamSound::GetFilledBufferPercentage() const
{
    if (!IsPlayerAvailable()) 
    {
        return 0.0f;
    }
    return m_StreamSoundPlayerInstance.GetFilledBufferPercentage();
}

bool StreamSound::IsPrepared() const
{
    if (!IsPlayerAvailable()) 
    {
        return false;
    }
    return m_StreamSoundPlayerInstance.IsPrepared();
}

bool StreamSound::IsSuspendByLoadingDelay() const
{
    if (!IsPlayerAvailable()) 
    {
        return false;
    }
    return m_StreamSoundPlayerInstance.IsSuspendByLoadingDelay();
}


} // namespace internal
} // namespace snd
} // namespace nw