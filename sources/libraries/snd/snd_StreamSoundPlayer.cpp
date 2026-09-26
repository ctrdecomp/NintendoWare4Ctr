// Filename: snd_StreamSoundPlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_StreamSoundPlayer.h>
#include <nw/snd/snd_StreamBufferPool.h>
#include <nw/snd/snd_SoundSystem.h>
#include <nw/snd/snd_TaskManager.h>
#include <nw/snd/snd_WaveFileReader.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_StreamSoundFileLoader.h>
#include <nw/snd/snd_DriverCommandManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

u8 StreamSoundPlayer::s_LoadBuffer[StreamSoundPlayer::LOAD_BUFFER_SIZE] NW_ALIGN(32);

StreamSoundPlayer::StreamSoundPlayer()
{
    u32 taskCount = m_StreamDataLoadTaskPool.Create(
        m_StreamDataLoadTaskArea, sizeof(m_StreamDataLoadTaskArea));
    NW_ASSERT(taskCount == BUFFER_BLOCK_COUNT_MAX);
}

StreamSoundPlayer::~StreamSoundPlayer()
{
    Finalize();

    NW_ASSERT(m_StreamDataLoadTaskPool.Count() == BUFFER_BLOCK_COUNT_MAX);
    m_StreamDataLoadTaskPool.Destroy(m_StreamDataLoadTaskArea, sizeof(m_StreamDataLoadTaskArea));
}

void StreamSoundPlayer::Initialize()
{
    BasicSoundPlayer::Initialize();
    m_IsInitialized = false;
    m_IsPrepared = false;
    m_PauseStatus = false;
    m_LoadWaitFlag = false;
    m_LoadFinishFlag = false;
    m_IsNoRealtimeLoad = false;
    m_PlayFinishFlag = false;
    m_SkipUpdateAdpcmLoop = false;
    m_ValidAdpcmLoop = false;

    for(int tracks = 0; tracks < STRM_TRACK_NUM; tracks++)
    {
        StreamTrack& tr = m_Tracks[tracks];
        tr.m_ActiveFlag = false;
        tr.m_Volume = 1.0f;
        tr.m_Pan = 0.0f;
        tr.m_SurroundPan = 0.0f;
    }

    for (int channel = 0; channel < STRM_CHANNEL_NUM; channel++)
    {
        StreamChannel& ch = m_Channels[channel];
        ch.m_pBufferAddress = NULL;
        ch.m_pVoice = NULL;
    }
}

StreamSoundPlayer::SetupResult StreamSoundPlayer::Setup(StreamBufferPool* pBufferPool,
    u32 allocChannelCount, u16 allocTrackFlag)
{
    NW_NULL_ASSERT(pBufferPool);
    
    m_ChannelCount = ut::Min(allocChannelCount, STRM_CHANNEL_NUM);
    
    u32 bitMask = allocTrackFlag;
    u32 trackIndex = 0;
    while (bitMask != 0)
    {
        if (bitMask & 0x01)
        {
            if (trackIndex >= STRM_TRACK_NUM)
            {
                NW_WARNING(
                    false, "Too large track index (%d). Max track index is %d.",
                    trackIndex, STRM_TRACK_NUM-1);
                break;
            }
            
            m_Tracks[trackIndex].m_ActiveFlag = true;
        }

        bitMask >>= 1;
        trackIndex++;
    }
    m_TrackCount = ut::Min(trackIndex, STRM_TRACK_NUM);
    if (m_TrackCount == 0)
    {
        return SETUP_ERR_UNKNOWN;
    }

    m_pBufferPool = pBufferPool;

    {
        if (!AllocStreamBuffers())
        {
            NW_WARNING(false, "Failed to start stream sound for not enough stream channel instance." );
            return SETUP_ERR_CANNOT_ALLOCATE_BUFFER;
        }
    }

    for (int channel = 0; channel < m_ChannelCount; channel++ )
    {
        StreamChannel& ch = m_Channels[channel];
    }

    m_IsInitialized = true;

    return SETUP_SUCCESS;
}

void StreamSoundPlayer::Finalize() 
{
    if (!m_IsInitialized)
    {
        return;
    }

    TaskManager::GetInstance().CancelTaskById( reinterpret_cast<u32>(this) );

    m_StreamHeaderLoadTask.Wait();
    for (StreamDataLoadTaskList::Iterator itr = m_StreamDataLoadTaskList.GetBeginIter(); itr != m_StreamDataLoadTaskList.GetEndIter();)
    {
        StreamDataLoadTaskList::Iterator curItr = itr++;
        StreamDataLoadTask* task = &*curItr;
        task->Wait();
        m_StreamDataLoadTaskList.Erase(task);
        m_StreamDataLoadTaskPool.Free(task);
    }
    FreeStreamBuffers();
    FreeVoices();

    if (m_pFileStream != NULL) 
    {
        m_pFileStream->Close();
        m_pFileStream = NULL;
    }
    
    m_pBufferPool = NULL;
    

    m_ActiveFlag = false;
    
    m_IsInitialized = false;
    
    BasicSoundPlayer::Finalize();
}

bool StreamSoundPlayer::Prepare(io::FileStream* pFileStream,
    StartOffsetType startOffsetType, int startOffset)
{
    NW_ASSERT(m_IsInitialized);
    NW_NULL_ASSERT(pFileStream);
    NW_ASSERT(pFileStream->CanRead());
    NW_ASSERT(pFileStream->CanSeek());

    m_pFileStream = pFileStream;
    m_StartOffsetType = startOffsetType;
    m_StartOffset = startOffset;

    m_IsTaskError = false;
    m_IsLoadingDelay = false;
    m_ActiveFlag = true;

    m_StreamHeaderLoadTask.m_PlayerHandle = this;
    m_StreamHeaderLoadTask.m_pFileStream = m_pFileStream;
    m_StreamHeaderLoadTask.m_StartOffsetType = m_StartOffsetType;
    m_StreamHeaderLoadTask.m_StartOffset = m_StartOffset;

    m_StreamHeaderLoadTask.SetId(reinterpret_cast<u32>(this));
    internal::TaskManager::GetInstance().AppendTask(&m_StreamHeaderLoadTask,
        internal::TaskManager::PRIORITY_MIDDLE);

    return true;
}

void StreamSoundPlayer::Start()
{
    if (!m_IsPrepared)
    {
        return;
    }
    
    if (!m_StartedFlag)
    {
        s32 blockIndex = 0;
        u32 blockOffset = 0;
        s32 loopCount = 0;
        if (!CalcStartOffset(&blockIndex, &blockOffset, &loopCount))
        {
            NW_ASSERT(false);
            return;
        }
        m_LoopCounter += loopCount;

        const size_t sampleBufferLen =
            static_cast<size_t>(m_DataBlockSize * m_PlayingBufferBlockCount);
        const u32 sampleCount = Util::GetSampleByByte(
            sampleBufferLen, WaveFileReader::GetSampleFormat(m_StreamInfo.encodeMethod));

        for ( int trackIndex = 0; trackIndex < m_TrackCount; trackIndex++ )
        {
            StreamTrack& track = m_Tracks[trackIndex];
            if (!track.m_ActiveFlag) continue;

            WaveInfo waveInfo;
            waveInfo.sampleFormat = WaveFileReader::GetSampleFormat( m_StreamInfo.encodeMethod );
            waveInfo.channelCount = track.m_TrackInfo.channelCount;
            waveInfo.sampleRate = m_StreamInfo.sampleRate;

            for (int ch = 0; ch < track.m_TrackInfo.channelCount; ch++)
            {
                StreamChannel* channel = GetTrackChannel(track, ch);
                if (channel == NULL)
                {
                    continue;
                }

                if (channel->m_pVoice != NULL)
                {
                    channel->m_pVoice->Initialize(waveInfo, blockOffset);
                    channel->m_pVoice->Start();
                }
            }
        }

        UpdatePauseStatus();

        SoundThread::GetInstance().RegisterPlayerCallback(this);
        
        m_StartedFlag = true;
    }
}

bool StreamSoundPlayer::IsDspAdpcm() const
{
    if (WaveFileReader::GetSampleFormat(m_StreamInfo.encodeMethod) == SAMPLE_FORMAT_DSP_ADPCM )
    {
        return true;
    }
    return false;
}

void StreamSoundPlayer::Stop()
{
    for (int ch = 0; ch < m_ChannelCount; ch++)
    {
        Voice* voice = m_Channels[ch].m_pVoice;
        if (voice != NULL)
        {
            voice->Stop();
        }
    }

    if (m_StartedFlag) 
    {
        SoundThread::GetInstance().UnregisterPlayerCallback( this );
        m_StartedFlag = false;
    }

    m_IsPrepared = false;
}

void StreamSoundPlayer::Pause(bool flag)
{
    m_PauseFlag = flag;
    if (flag)
    {
        m_LoadWaitFlag = true;
    }

    UpdatePauseStatus();
}

bool StreamSoundPlayer::ReadStreamDataInfo(StreamDataInfo* info) const
{
    SoundThreadLock lock;

    if (!m_IsPrepared)
    {
        return false;
    }
    info->loopFlag = m_StreamInfo.isLoop != 0;
    info->sampleRate = m_StreamInfo.sampleRate;
    info->loopStart = m_StreamInfo.loopStart;
    info->loopEnd = m_StreamInfo.frameCount;
    return true;
}

long StreamSoundPlayer::GetPlaySamplePosition() const
{
    SoundThreadLock lock;

    if (!m_ActiveFlag)
    {
        return -1;
    }
    if (!m_Tracks[0].m_ActiveFlag)
    {
        return -1;
    }
    if (!m_IsPrepared)
    {
        return 0;
    }

    s32 bufOffset = 0;

    if (m_Channels[0].m_pVoice != NULL)
    {
        bufOffset = static_cast<s32>(m_Channels[0].m_pVoice->GetCurrentPlayingSample());
    }
    s32 pos = (m_PlayingDataBlockIndex) * static_cast<s32>(m_StreamInfo.oneBlockSamples);
    return pos + bufOffset;
}

f32 StreamSoundPlayer::GetFilledBufferPercentage() const
{
    SoundThreadLock lock;

    if (!m_ActiveFlag)
    {
        return 0.0f;
    }
    if (!m_Tracks[0].m_ActiveFlag)
    {
        return 0.0f;
    }
    if (!m_IsPrepared)
    {
        return static_cast<f32>(m_BufferBlockCountBase - m_PrepareCounter)
            * 100.f / m_BufferBlockCountBase;
    }

    s32 bufferPlayPosition = 0;
    if (m_Channels[0].m_pVoice != NULL)
    {
        bufferPlayPosition = static_cast<s32>(m_Channels[0].m_pVoice->GetCurrentPlayingSample());
    }
    int allBufferSamples = m_LoadingBufferBlockCount * m_StreamInfo.oneBlockSamples;
    int prePlayBufferBlocks = m_LoadingBufferBlockCount - m_StreamDataLoadTaskList.GetSize() - 1;
    f32 percentage = ((prePlayBufferBlocks * m_StreamInfo.oneBlockSamples) + 
        (m_StreamInfo.oneBlockSamples - bufferPlayPosition)) /
        static_cast<f32>( allBufferSamples);
    percentage *= 100.f;

    return percentage;
}

bool StreamSoundPlayer::LoadHeader(const StreamSoundFile::StreamSoundInfo& streamInfo,
    const StreamSoundFileReader::TrackInfo trackInfos[],
    const DspAdpcmParam dspAdpcmParam[],
    const DspAdpcmLoopParam dspAdpcmLoopParam[],
    u32 dataBlockOffset, u32 trackCount, u32 channelCount)
{
    (void)trackCount;
    (void)channelCount;
    
    if (!m_IsInitialized)
    {
        return false;
    }
    
    m_StreamInfo = streamInfo;

    m_DataOffsetFromFileHead =
        dataBlockOffset + sizeof(ut::BinaryBlockHeader)
        + m_StreamInfo.sampleDataOffset.offset;

    NW_ASSERT(m_TrackCount == ut::Min(trackCount, STRM_TRACK_NUM));
    NW_ASSERT(m_ChannelCount == ut::Min(channelCount, STRM_CHANNEL_NUM));

    for (int i = 0; i < m_TrackCount; i++)
    {
        m_Tracks[i].m_TrackInfo = trackInfos[i];

        for (int j = 0; j < trackInfos[i].channelCount; j++)
        {
            m_Tracks[i].m_pChannels[j] = &m_Channels[trackInfos[i].globalChannelIndex[j]];
        }
    }

    if (IsDspAdpcm())
    {
        for (int ch = 0; ch < m_ChannelCount; ch++)
        {
            m_Channels[ch].m_AdpcmParam = dspAdpcmParam[ch];
            m_Channels[ch].m_AdpcmLoopParam = dspAdpcmLoopParam[ch];

            nn::snd::CTR::AdpcmContext& context0 = m_Channels[ch].m_AdpcmContext[StreamChannel::ADPCM_CONTEXT_HEAD];
            context0.pred_scale = m_Channels[ch].m_AdpcmParam.predScale;
            context0.yn1 = m_Channels[ch].m_AdpcmParam.yn1;
            context0.yn2 = m_Channels[ch].m_AdpcmParam.yn2;

            if (m_StreamInfo.isLoop)
            {
                nn::snd::CTR::AdpcmContext& context1 = m_Channels[ch].m_AdpcmContext[StreamChannel::ADPCM_CONTEXT_LOOP];
                context1.pred_scale = m_Channels[ch].m_AdpcmLoopParam.loopPredScale;
                context1.yn1 = m_Channels[ch].m_AdpcmLoopParam.loopYn1;
                context1.yn2 = m_Channels[ch].m_AdpcmLoopParam.loopYn2;
            }
        }
    }

    if (!SetupPlayer())
    {
        return false;
    }

    m_PrepareCounter = 0;
    for (int i = 0; i < m_BufferBlockCountBase; i++)
    {
        UpdateLoadingBlockIndex();
        m_PrepareCounter++;

        if (m_LoadFinishFlag) break;
    }

    if (!AllocVoices())
    {
        FreeStreamBuffers();
        return false;
    }
    
    return true;
}

bool StreamSoundPlayer::LoadStreamData(int bufferBlockIndex, int dataBlockIndex,
    u32 blockSamples, bool isDataLoopBlock, bool lastBlockFlag
)
{
    if (!m_IsInitialized)
    {
        return false;
    }
    
    int currentChannel = 0;
    while (currentChannel < m_ChannelCount)
    {
        int loadChannelCount = LOAD_BUFFER_CHANNEL_NUM;
        if (currentChannel + loadChannelCount > m_ChannelCount)
        {
            loadChannelCount = m_ChannelCount - currentChannel;
        }
        
        for (int i = 0; i < loadChannelCount; i++)
        {
            void* dest = ut::AddOffsetToPtr(
                m_Channels[currentChannel].m_pBufferAddress,
                m_DataBlockSize * bufferBlockIndex);

            nn::snd::CTR::WaveBuffer* pBuffer = &m_Channels[currentChannel].m_WaveBuffer[bufferBlockIndex];
            nn::snd::CTR::InitializeWaveBuffer(pBuffer);
            pBuffer->bufferAddress = dest;
            pBuffer->sampleLength = blockSamples;

            if (IsDspAdpcm())
            {
                if (dataBlockIndex == 0)
                {
                    pBuffer->pAdpcmContext = &m_Channels[currentChannel].m_AdpcmContext[StreamChannel::ADPCM_CONTEXT_HEAD];
                }
                else if (isDataLoopBlock)
                {
                    pBuffer->pAdpcmContext = &m_Channels[currentChannel].m_AdpcmContext[StreamChannel::ADPCM_CONTEXT_LOOP];
                }
                else
                {
                    pBuffer->pAdpcmContext = NULL;
                }
            }

            m_Channels[currentChannel].AppendWaveBuffer(pBuffer, lastBlockFlag);

            ++currentChannel;
        }
    }

    if (!m_IsPrepared)
    {
        m_PrepareCounter--;
        if (m_PrepareCounter == 0)
        {
            m_IsPrepared = true;
        }
    }

    --m_LoadWaitCount;
    
    return true;
}

bool StreamSoundPlayer::SetupPlayer()
{
    NW_NULL_ASSERT(m_pBufferPool);
    const size_t strmBufferSize = m_pBufferPool->GetBlockSize();

    s32 blockIndex = 0;
    u32 blockOffset = 0;
    s32 loopCount = 0;
    if (!CalcStartOffset(&blockIndex, &blockOffset, &loopCount))
    {
        return false;
    }

    m_LoopStartBlockIndex = static_cast<int>(m_StreamInfo.loopStart / m_StreamInfo.oneBlockSamples);
    m_LastBlockIndex = static_cast<int>(m_StreamInfo.blockCount) - 1;

    m_DataBlockSize = static_cast<int>(m_StreamInfo.oneBlockBytes);
    if (m_DataBlockSize > DATA_BLOCK_SIZE_MAX)
    {
        NW_WARNING(false, "Too large stream data block size.");
        return false;
    }
    m_BufferBlockCount = strmBufferSize / m_DataBlockSize;
    if (m_BufferBlockCount < 4)
    {
        NW_WARNING( false, "Too small stream buffer size." );
        return false;
    }
    if (m_BufferBlockCount > BUFFER_BLOCK_COUNT_MAX)
    {
        m_BufferBlockCount = BUFFER_BLOCK_COUNT_MAX;
    }
    m_BufferBlockCountBase = m_BufferBlockCount - 1;
    m_ChangeNumBlocks = m_BufferBlockCountBase;

    m_PlayingDataBlockIndex = blockIndex;
    m_LoadingDataBlockIndex = blockIndex;

    m_LoadingBufferBlockIndex = 0;
    m_PlayingBufferBlockIndex = 0;

    if (m_IsNoRealtimeLoad)
    {
        m_LoadingBufferBlockCount = static_cast<s32>( m_StreamInfo.blockCount );
    }
    else
    {
        m_LoadingBufferBlockCount = CalcLoadingBufferBlockCount();
    }

    m_PlayingBufferBlockCount = m_LoadingBufferBlockCount;

    return true;
}

bool StreamSoundPlayer::AllocStreamBuffers()
{
    for(int index = 0; index < m_ChannelCount; index++)
    {
        void* strmBuffer = m_pBufferPool->Alloc();
        if (strmBuffer == NULL)
        {
            for(int i = 0 ; i < index ; i++)
            {
                m_pBufferPool->Free(m_Channels[i].m_pBufferAddress);
                m_Channels[i].m_pBufferAddress = NULL;
            }
            return false;
        }

        m_Channels[index].m_pBufferAddress = strmBuffer;
    }
    return true;
}

void StreamSoundPlayer::FreeStreamBuffers()
{
    for(int index = 0; index < m_ChannelCount; index++)
    {
        if (m_Channels[index].m_pBufferAddress == NULL)
        {
            continue;
        }

        m_pBufferPool->Free(m_Channels[index].m_pBufferAddress);
        m_Channels[index].m_pBufferAddress = NULL;
    }
}

bool StreamSoundPlayer::AllocVoices()
{
    for (int channelIndex = 0; channelIndex < m_ChannelCount; channelIndex++)
    {
        StreamChannel& channel = m_Channels[ channelIndex ];

        Voice* voice = VoiceManager::GetInstance().AllocVoice(1, Voice::PRIORITY_NODROP,
            VoiceCallbackFunc, &channel);

        if (voice == NULL)
        {
            for (int i = 0; i < channelIndex; i++)
            {
                StreamChannel& c = m_Channels[i];
                if (c.m_pVoice != NULL)
                {
                    c.m_pVoice->Free();
                    c.m_pVoice = NULL;
                }
            }
            return false;
        }

        voice->SetFrontBypass(IsFrontBypass());
        channel.m_pVoice = voice;

        if (IsDspAdpcm())
        {
            nn::snd::CTR::AdpcmParam param;
            for (int i = 0; i < 16; i++)
            {
                param.coef[i] = channel.m_AdpcmParam.coef[i];
            }

            channel.m_pVoice->SetAdpcmParam(0, param);
        }
    }
    return true;
}

void StreamSoundPlayer::FreeVoices()
{
    for (int ch = 0; ch < m_ChannelCount; ch++)
    {
        StreamChannel& channel = m_Channels[ch];
        if (channel.m_pVoice != NULL)
        {
            channel.m_pVoice->Free();
            channel.m_pVoice = NULL;
        }
    }
}

void StreamSoundPlayer::UpdateTask()
{
    for (StreamDataLoadTaskList::Iterator itr = m_StreamDataLoadTaskList.GetBeginIter(); itr != m_StreamDataLoadTaskList.GetEndIter(); )
    {
        StreamDataLoadTaskList::Iterator curItr = itr++;
        StreamDataLoadTask* task = &*curItr;
        if (task->GetStatus() != Task::STATUS_DONE)
        {
            break;
        }
        
        m_StreamDataLoadTaskList.Erase( task );
        m_StreamDataLoadTaskPool.Free( task );
    }
}

void StreamSoundPlayer::Update()
{
    if (m_IsTaskError) 
    {
        NW_WARNING(false, "Task error is occured.");
        Stop();
        return;
    }
    
    UpdateTask();

    if (m_StartedFlag)
    {
        for (int ch = 0; ch < m_ChannelCount; ch++)
        {
            StreamChannel& channel = m_Channels[ch];
            if (channel.m_pVoice == NULL)
            {
                Stop();
                m_FinishFlag = true;
                return;
            }
        }

        for (int trackIndex = 0; trackIndex < m_TrackCount; trackIndex++)
        {
            UpdateVoiceParams(&m_Tracks[trackIndex]);
        }
    }

    if (m_LoadWaitFlag)
    {
        if (m_StreamDataLoadTaskList.IsEmpty() &&  (!CheckDiskDriveError()))
        {
            m_LoadWaitFlag = false;
            UpdatePauseStatus();
        }
    }

    if (m_IsLoadingDelay)
    {
        NW_WARNING(false, "Pause stream because of loading delay.");
        m_IsLoadingDelay = false;
    }
}

void StreamSoundPlayer::UpdateBuffer()
{
    if (!m_StartedFlag)
    {
        return;
    }
    if (!m_Tracks[0].m_ActiveFlag)
    {
        return;
    }

    Voice* voice = m_Channels[0].m_pVoice;

    if (voice == NULL)
    {
        return;
    }
    if (!voice->IsRun())
    {
        return;
    }
        
    if (CheckDiskDriveError())
    {
        m_LoadWaitFlag = true;
        UpdatePauseStatus();
    }

    if ((!m_PlayFinishFlag) && (!m_IsNoRealtimeLoad) && (!m_LoadWaitFlag))
    {
        while (m_Channels[0].m_WaveBuffer[m_PlayingBufferBlockIndex].status == nn::snd::CTR::WaveBuffer::STATUS_DONE)
        {
            if (m_PlayFinishFlag) break;

            if (!m_LoadWaitFlag)
            {
                if (!m_StreamDataLoadTaskList.IsEmpty() && m_LoadWaitCount >= m_BufferBlockCountBase - 2)
                {
                    m_IsLoadingDelay = true;
                    m_LoadWaitFlag = true;
                    UpdatePauseStatus();
                    break;
                }
            }

            UpdatePlayingBlockIndex();
            UpdateLoadingBlockIndex();
        }
    }
}

void StreamSoundPlayer::UpdateVoiceParams(StreamTrack* track)
{
    if (!track->m_ActiveFlag)
    {
        return;
    }

    // volume
    f32 volume = 1.0f;
    volume *= GetVolume();
    volume *= static_cast<f32>( track->m_TrackInfo.volume ) / 127.0f;
    volume *= track->m_Volume;

    // pitch
    f32 pitchRatio = 1.0f;
    pitchRatio *= GetPitch();

    // pan
    f32 pan = 0.0f;
    pan += GetPan();

    if (track->m_TrackInfo.pan <= 1) 
    {
        pan += static_cast<f32>(static_cast<int>(track->m_TrackInfo.pan) - 63) / 63.0f;
    }
    else
    {
        pan += static_cast<f32>(static_cast<int>(track->m_TrackInfo.pan) - 64) / 63.0f;
    }
    pan += track->m_Pan;

    // surround pan
    f32 span = 0.0f;
    span += track->m_SurroundPan;
    span += GetSurroundPan();

    // lpf freq
    f32 lpfFreq = 1.0f;
    lpfFreq += GetLpfFreq();

    // biquad
    int biquadType = GetBiquadFilterType();
    f32 biquadValue = GetBiquadFilterValue();

    // mainsend
    f32 mainSend = 0.0f;
    mainSend += GetMainSend();

    f32 fxsend[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        fxsend[i] = 0.0f;
        fxsend[i] += GetFxSend(AuxBus(i));
    }

    for (int ch = 0; ch < track->m_TrackInfo.channelCount; ch++)
    {
        Voice* voice = track->m_pChannels[ch]->m_pVoice;

        if (voice != NULL)
        {
            voice->SetVolume(volume);
            voice->SetPitch(pitchRatio);
            voice->SetLpfFreq(lpfFreq);
            voice->SetBiquadFilter(biquadType, biquadValue);
            voice->SetMainSend(mainSend);
            for (int i = 0; i < AUX_BUS_NUM; i++)
            {
                AuxBus bus = static_cast<AuxBus>(i);
                voice->SetFxSend(bus, fxsend[i]);
            }

            if (track->m_TrackInfo.channelCount == 1)
            {
                voice->SetPan(pan);
            }
            else if (track->m_TrackInfo.channelCount == 2)
            {
                register f32 voicePan = pan;
                if (ch == 0) 
                { 
                    voicePan -= 1.0f; 
                }
                if (ch == 1) 
                { 
                    voicePan += 1.0f; 
                }
                voice->SetPan(voicePan);
            }
            voice->SetSurroundPan(span);
        }
    }
}

void StreamSoundPlayer::UpdatePlayingBlockIndex()
{
    if (m_PlayFinishFlag)
    {
        return;
    }
    
    u32 playDataIdx = m_PlayingDataBlockIndex;
    u32 playBufIdx = m_PlayingBufferBlockIndex;
    
    m_PlayingDataBlockIndex++;

    if (m_PlayingDataBlockIndex > m_LastBlockIndex)
    {
        if (m_StreamInfo.isLoop)
        {
            m_PlayingDataBlockIndex = m_LoopStartBlockIndex;
            if (m_LoopCounter < INT_MAX)
            {
                m_LoopCounter++;
            }
        }
    }

    m_PlayingBufferBlockIndex++;
    if (m_PlayingBufferBlockIndex >= m_PlayingBufferBlockCount)
    {
        m_PlayingBufferBlockIndex = 0;
        m_PlayingBufferBlockCount = m_LoadingBufferBlockCount;
    }

    if (m_PlayingBufferBlockIndex == m_PlayingBufferBlockCount - 1)
    {
        m_ValidAdpcmLoop = false;
        m_SkipUpdateAdpcmLoop = false;
    }

    if (m_PlayingDataBlockIndex >= m_LastBlockIndex - 1)
    {
        if (!m_StreamInfo.isLoop)
        {
            m_PlayFinishFlag = true;
        }
    }
}

void StreamSoundPlayer::UpdateLoadingBlockIndex()
{
    if ( m_LoadFinishFlag )
    {
        return;
    }

    ++m_LoadWaitCount;

    size_t blockSize = 0;
    u32 blockSamples = 0;
    bool lastBlockFlag = false;
    if (m_LoadingDataBlockIndex == m_LastBlockIndex)
    {
        blockSize = m_StreamInfo.lastBlockPaddedBytes;
        blockSamples = m_StreamInfo.lastBlockSamples;
        if (!m_StreamInfo.isLoop) 
        {
            lastBlockFlag = true;
        }
    }
    else
    {
        blockSize = m_StreamInfo.oneBlockBytes;
        blockSamples = m_StreamInfo.oneBlockSamples;
    }
    
    s32 loadOffset = CalcLoadOffset();

    NW_ALIGN32_ASSERT(blockSize);
    NW_ALIGN4_ASSERT(loadOffset);

    StreamDataLoadTask* task = m_StreamDataLoadTaskPool.Alloc();
    NW_NULL_ASSERT(task);

    task->m_PlayerHandle = this;
    task->m_pFileStream = m_pFileStream;
    task->m_ChannelCount = m_StreamInfo.channelCount;
    task->m_Offset = loadOffset;
    task->m_BlockBytes = blockSize;
    task->m_BlockSamples = blockSamples;
    task->m_BufferBlockIndex = m_LoadingBufferBlockIndex;
    task->m_DataBlockSize = m_DataBlockSize;
    task->m_LoadingDataBlockIndex = m_LoadingDataBlockIndex;
    task->m_LastBlockFlag = lastBlockFlag;

    task->m_IsDataLoopBlock = (m_LoadingDataBlockIndex == m_LoopStartBlockIndex) && m_StreamInfo.isLoop;

    task->SetId(reinterpret_cast<u32>(this));

    for(int ch = 0;ch < m_StreamInfo.channelCount; ch++)
    {
        task->m_BufferAddress[ch] = m_Channels[ch].m_pBufferAddress;
    }
    
    m_StreamDataLoadTaskList.PushBack(task);
    internal::TaskManager::GetInstance().AppendTask(task, 
        m_StartedFlag ? internal::TaskManager::PRIORITY_HIGH : internal::TaskManager::PRIORITY_MIDDLE);

    m_LoadingDataBlockIndex++;
    if (m_LoadingDataBlockIndex > m_LastBlockIndex)
    {
        if (m_StreamInfo.isLoop)
        {
            m_LoadingDataBlockIndex = m_LoopStartBlockIndex;
        }
        else
        {
            m_LoadFinishFlag = true;
            return;
        }
    }

    m_LoadingBufferBlockIndex++;
    if (m_LoadingBufferBlockIndex >= m_LoadingBufferBlockCount)
    {
        m_LoadingBufferBlockIndex = 0;
        m_LoadingBufferBlockCount = CalcLoadingBufferBlockCount();
    }
}

void StreamSoundPlayer::UpdatePauseStatus()
{
    bool pauseStatus = false;
    if (m_PauseFlag)
    {
        pauseStatus = true;
    }
    if (m_LoadWaitFlag)
    {
        pauseStatus = true;
    }

    if (pauseStatus != m_PauseStatus)
    {
        for (int ch = 0; ch < m_ChannelCount; ch++)
        {
            Voice* voice = m_Channels[ch].m_pVoice;
            if (voice != NULL)
            {
                voice->Pause(pauseStatus);
            }
        }

        m_PauseStatus = pauseStatus;
    }
}

bool StreamSoundPlayer::CheckDiskDriveError() const
{
    return SoundSystem::detail_IsStreamLoadWait();
}

int StreamSoundPlayer::CalcLoadingBufferBlockCount() const
{
    return m_BufferBlockCount;
}

void StreamSoundPlayer::VoiceCallbackFunc(Voice* voice, 
    Voice::VoiceCallbackStatus status, void* arg)
{
    NW_NULL_ASSERT(arg);
    StreamChannel* channel = reinterpret_cast<StreamChannel*>(arg);

    NW_ASSERT(channel->m_pVoice == voice);

    switch (status)
    {
    case Voice::CALLBACK_STATUS_FINISH_WAVE:
    case Voice::CALLBACK_STATUS_CANCEL:
        voice->Free();
        channel->m_pVoice = NULL;
        break;
    case Voice::CALLBACK_STATUS_DROP_VOICE:
    case Voice::CALLBACK_STATUS_DROP_DSP:
        channel->m_pVoice = NULL;
        break;
    default:
        NW_ASSERTMSG(false, "Unknown Voice callback status %d", status);
        return;
    }
}

StreamChannel* StreamSoundPlayer::GetTrackChannel( 
    const StreamTrack& track, int channelIndex)
{
    if (channelIndex >= STRM_CHANNEL_NUM_PER_TRACK)
    {
        return NULL;
    }

    int index = track.m_TrackInfo.globalChannelIndex[channelIndex];
    if (index >= STRM_CHANNEL_NUM)
    {
        return NULL;
    }

    return &m_Channels[index];
}

void StreamSoundPlayer::SetTrackVolume(u32 trackBitFlag, f32 volume)
{
    for(int trackNo = 0; trackNo < m_TrackCount && trackBitFlag != 0; trackNo++, trackBitFlag >>= 1)
    {
        if ((trackBitFlag & 0x01) == 0)
        {
            continue;
        }
        m_Tracks[trackNo].m_Volume = volume;
    }
}

void StreamSoundPlayer::SetTrackPan(u32 trackBitFlag, f32 pan)
{
    for(int trackNo = 0; trackNo < m_TrackCount && trackBitFlag != 0; trackNo++, trackBitFlag >>= 1)
    {
        if ((trackBitFlag & 0x01) == 0)
        {
            continue;
        }
        m_Tracks[trackNo].m_Pan = pan;
    }
}

void StreamSoundPlayer::SetTrackSurroundPan(u32 trackBitFlag, f32 span)
{
    for(int trackNo = 0; trackNo < m_TrackCount && trackBitFlag != 0; trackNo++, trackBitFlag >>= 1)
    {
        if ((trackBitFlag & 0x01) == 0)
        {
            continue;
        }
        m_Tracks[trackNo].m_SurroundPan = span;
    }
}

StreamTrack* StreamSoundPlayer::GetPlayerTrack(int trackNo)
{
    if (trackNo > STRM_TRACK_NUM - 1)
    {
        return NULL;
    }

    return &m_Tracks[trackNo];
}

const StreamTrack* StreamSoundPlayer::GetPlayerTrack(int trackNo) const
{
    if (trackNo > STRM_TRACK_NUM - 1)
    {
        return NULL;
    }

    return &m_Tracks[trackNo];
}

s32 StreamSoundPlayer::CalcLoadOffset() const
{
    s32 loadOffset = static_cast<s32>(
        m_DataOffsetFromFileHead + m_LoadingDataBlockIndex *
        (m_StreamInfo.oneBlockBytes * m_StreamInfo.channelCount));
    return loadOffset;
}

/* StreamSoundPlayer::StreamHeaderLoadTask */

StreamSoundPlayer::StreamHeaderLoadTask::StreamHeaderLoadTask(): 
    m_PlayerHandle(NULL),
    m_pFileStream(NULL),
    m_StartOffset(0)
{
}

void StreamSoundPlayer::StreamHeaderLoadTask::Execute()
{
    bool result = LoadHeader();
    if (!result)
    {
        m_PlayerHandle->SetTaskErrorFlag();
        return;
    }
}

bool StreamSoundPlayer::StreamHeaderLoadTask::LoadHeader()
{
    internal::StreamSoundFileLoader loader(*m_pFileStream);
    if (!loader.LoadFileHeader(s_LoadBuffer, StreamSoundPlayer::LOAD_BUFFER_SIZE))
    {
        return false;
    }

    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstanceForTaskThread();
    DriverCommandStreamSoundLoadHeader* command = cmdmgr.AllocCommand<DriverCommandStreamSoundLoadHeader>();
    command->id = DRIVER_COMMAND_STRM_LOADHEADER;
    command->player = m_PlayerHandle;
    
    if (!loader.ReadStreamInfo(&command->streamInfo))
    {
        return false;
    }

    command->dataBlockOffset = loader.GetDataBlockOffset();
    command->trackCount = loader.GetTrackCount();
    command->channelCount = loader.GetChannelCount();

    NW_ASSERT(command->trackCount <= DriverCommandStreamSoundLoadHeader::TRACK_MAX);
    for (u32 i = 0; i < command->trackCount; i++)
    {
        if (!loader.ReadStreamTrackInfo(&command->trackInfoArray[i], i))
        {
            return false;
        }
    }
    
    if (WaveFileReader::GetSampleFormat(command->streamInfo.encodeMethod ) == SAMPLE_FORMAT_DSP_ADPCM)
    {
        NW_ASSERT(command->channelCount <= DriverCommandStreamSoundLoadHeader::CHANNEL_MAX);
        for (u32 ch = 0; ch < command->channelCount; ch++)
        {
            if (!loader.ReadDspAdpcmChannelInfo(&command->dspAdpcmParam[ch], &command->dspAdpcmLoopParam[ch], ch))
            {
                return false;
            }
        }

        if (m_StartOffset != 0)
        {
            int startOffsetSamples = 0;
            if (m_StartOffsetType == START_OFFSET_TYPE_SAMPLE)
            {
                startOffsetSamples = m_StartOffset;
            }
            else if (m_StartOffsetType == START_OFFSET_TYPE_MILLISEC)
            {
                startOffsetSamples = m_StartOffset * command->streamInfo.sampleRate / 1000;
            }
            
            s32 blockIndex = startOffsetSamples / static_cast<s32>(command->streamInfo.oneBlockSamples);
            u16 yn1[STRM_CHANNEL_NUM];
            u16 yn2[STRM_CHANNEL_NUM];
            if (!loader.ReadAdpcBlockData(yn1, yn2, blockIndex, command->channelCount))
            {
                return false;
            }
            for (u32 i = 0; i < command->channelCount; i++)
            {
                command->dspAdpcmParam[i].yn1 = yn1[i];
                command->dspAdpcmParam[i].yn2 = yn2[i];
            }
        }
    }

    cmdmgr.PushCommand(command);
    cmdmgr.FlushCommand(true);

    return true;
}

/* StreamSoundPlayer::StreamDataLoadTask */

StreamSoundPlayer::StreamDataLoadTask::StreamDataLoadTask(): 
    m_PlayerHandle(NULL),
    m_pFileStream(NULL),
    m_Offset(0),
    m_BlockBytes(0),
    m_BufferBlockIndex(-1),
    m_BlockSamples(0),
    m_IsDataLoopBlock(false)
{
}

void StreamSoundPlayer::StreamDataLoadTask::Execute()
{
    bool result = LoadStreamData();

    if (!result) 
    {
        m_PlayerHandle->SetTaskErrorFlag();
    }
}

bool StreamSoundPlayer::StreamDataLoadTask::LoadStreamData()
{
    NW_ALIGN4_ASSERT(m_Offset);
    NW_ALIGN32_ASSERT(m_BlockBytes);

    nn::snd::CTR::InvalidateDataCache(reinterpret_cast<uptr>(s_LoadBuffer), sizeof(s_LoadBuffer));

    int loadOffset = m_Offset;

    unsigned int currentChannel = 0;
    while (currentChannel < m_ChannelCount)
    {
        NW_ALIGN4_ASSERT(loadOffset);

        int loadChannelCount = LOAD_BUFFER_CHANNEL_NUM;
        if (currentChannel + loadChannelCount > m_ChannelCount)
        {
            loadChannelCount = m_ChannelCount - currentChannel;
        }
        size_t loadSize = m_BlockBytes * loadChannelCount;
        NW_ASSERT(loadSize <= LOAD_BUFFER_SIZE);

        m_pFileStream->Seek(loadOffset, io::FILE_STREAM_SEEK_BEGIN);
        s32 resultSize = m_pFileStream->Read(s_LoadBuffer, loadSize);
        loadOffset += loadSize;

        if (resultSize != loadSize)
        {
            return false;
        }

        for (int i = 0; i < loadChannelCount; i++)
        {
            u32 len = static_cast<u32>(m_BlockBytes);
            const void* source = ut::AddOffsetToPtr(s_LoadBuffer, m_BlockBytes * i);
            void* dest = ut::AddOffsetToPtr(m_BufferAddress[currentChannel],
                m_DataBlockSize * m_BufferBlockIndex);
            std::memcpy(dest, source, len);

            nn::snd::CTR::FlushDataCache(reinterpret_cast<uptr>(dest), len);

            ++currentChannel;
        }
    }
   
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstanceForTaskThread();
    DriverCommandStreamSoundLoadData* command = cmdmgr.AllocCommand<DriverCommandStreamSoundLoadData>();
    command->id = DRIVER_COMMAND_STRM_LOADDATA;
    command->player = m_PlayerHandle;
    command->bufferBlockIndex = m_BufferBlockIndex;
    command->blockSamples = m_BlockSamples;
    command->dataBlockIndex = m_LoadingDataBlockIndex;
    command->isDataLoopBlock = m_IsDataLoopBlock;
    command->lastBlockFlag = m_LastBlockFlag;
    cmdmgr.PushCommand(command);
    cmdmgr.FlushCommand(true);
    
    
    return true;
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw