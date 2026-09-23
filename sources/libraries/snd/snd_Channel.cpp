// Filename: snd_Channel.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Channel.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_HardwareManager.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_ChannelManager.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_DisposeCallbackManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

namespace
{
u8 GetNwInterpolationTypeFromHardwareManager()
{
    u8 result = 0;
    switch (HardwareManager::GetInstance().GetSrcType())
    {
    case SRC_TYPE_NONE:
        result = 2;
        break;
    case SRC_TYPE_LINEAR:
        result = 1;
        break;
    case SRC_TYPE_4TAP:
        // 0 のまま
        break;
    }

    return result;
}
} // namespace

Channel::Channel():
    m_Disposer(this),
    m_PauseFlag(0),
    m_ActiveFlag(false),
    m_AllocFlag(false),
    m_pVoice(NULL)
{
    DisposeCallbackManager::GetInstance().RegisterDisposeCallback( &m_Disposer );
}

Channel::~Channel()
{
    DisposeCallbackManager::GetInstance().UnregisterDisposeCallback(&m_Disposer);
}

void Channel::InitParam(ChannelCallback callback, u32 callbackData)
{
    m_pNextLink = NULL;

    m_Callback = callback;
    m_CallbackData = callbackData;

    m_PauseFlag = false;
    m_AutoSweep = true;
    m_ReleasePriorityFixFlag = false;
    m_IsIgnoreNoteOff = false;

	m_LoopFlag = false;
    m_LoopStartFrame = 0;
    
    m_Length = 0;
    m_Key = KEY_INIT;
    m_OriginalKey = ORIGINAL_KEY_INIT;
    m_InitVolume = 1.0f;
    m_InitPan = 0.0f;
    m_InitSurroundPan = 0.0f;
    m_Tune = 1.0f;

    m_Cent = 0.0f;
    m_CentPitch = 1.0f;
    
    m_UserVolume = 1.0f;
    m_UserPitch = 0.0f;
    m_UserPitchRatio = 1.0f;
    m_UserPan = 0.0f;
    m_UserSurroundPan = 0.0f;
    m_UserLpfFreq = 0.0f;
    m_BiquadType = -1; // invalid type -1
    m_BiquadValue = 0.0f;
    m_MainSend = 0.0f;
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        m_FxSend[i] = 0.0f;
    }

    m_SilenceVolume.InitValue(SILENCE_VOLUME_MAX);

    m_SweepPitch = 0.0f;
    m_SweepLength = 0;
    m_SweepCounter = 0;

    m_CurveAdshr.Initialize();
    m_Lfo.GetParam().Initialize();
    m_LfoTarget = LFO_TARGET_PITCH;

    m_PanMode = PAN_MODE_DUAL;
    m_PanCurve = PAN_CURVE_SQRT;

    m_KeyGroupId = 0;
    m_InterpolationType = GetNwInterpolationTypeFromHardwareManager();
}

void Channel::Update(bool doPeriodicProc)
{
    if (!m_ActiveFlag) 
        return;

    if (m_PauseFlag) 
    {
        doPeriodicProc = false;
    }

    m_SilenceVolume.Update();
    register f32 volume = m_InitVolume * m_UserVolume * m_SilenceVolume.GetValue() / SILENCE_VOLUME_MAX;

#if 0
    if ( m_CurveAdshr.GetStatus() == CurveAdshr::STATUS_RELEASE )
    {
        f32 envelopValue = Util::CalcVolumeRatio(m_CurveAdshr.GetValue());
        if (envelopValue == 0.0f)
        {
            Stop();
            return;
        }
    }
#else
    if (m_CurveAdshr.GetStatus() == CurveAdshr::STATUS_RELEASE && m_CurveAdshr.GetValue() < -90.4f)
    {
        Stop();
        return;
    }
#endif

    f32 cent = m_Key - m_OriginalKey + m_UserPitch + GetSweepValue();
    if (m_LfoTarget == LFO_TARGET_PITCH)
    {
        cent += m_Lfo.GetValue();
    }

    register f32 pitch;
    if (cent == m_Cent)
    {
        pitch = m_CentPitch;
    }
    else
    {
        pitch = Util::CalcPitchRatio(static_cast<s32>(cent * (1 << Util::PITCH_DIVISION_BIT)));
        m_Cent = cent;
        m_CentPitch = pitch;
    }
    
    pitch = pitch * m_Tune * m_UserPitchRatio;

    f32 pan = m_InitPan + m_UserPan;
    if (m_LfoTarget == LFO_TARGET_PAN)
    {
        pan += m_Lfo.GetValue();
    }

    f32 surroundPan = m_InitSurroundPan + m_UserSurroundPan;

    if (doPeriodicProc)
    {
        if (m_AutoSweep)
        {
            UpdateSweep(HardwareManager::SOUND_FRAME_INTERVAL_MSEC);
        }
        m_Lfo.Update(HardwareManager::SOUND_FRAME_INTERVAL_MSEC);
        m_CurveAdshr.Update(HardwareManager::SOUND_FRAME_INTERVAL_MSEC);
    }

    f32 lpfFreq = 1.0f + m_UserLpfFreq;

#if 1
    volume *= Util::CalcVolumeRatio(m_CurveAdshr.GetValue());
    if (m_LfoTarget == LFO_TARGET_VOLUME)
    {
        volume *= Util::CalcVolumeRatio(m_Lfo.GetValue() * 6.0f);
    }
#else
    register f32 decay = m_CurveAdshr.GetValue();
    if (m_LfoTarget == LFO_TARGET_VOLUME)
    {
        decay += m_Lfo.GetValue() * 6.0f;
    }
    volume *= Util::CalcVolumeRatio(decay);
#endif

    if (m_pVoice != NULL)
    {
        m_pVoice->SetVolume(volume);
        m_pVoice->SetPitch(pitch);
        m_pVoice->SetPan(pan);
        m_pVoice->SetSurroundPan(surroundPan);
        
        m_pVoice->SetLpfFreq(lpfFreq);
        m_pVoice->SetBiquadFilter(m_BiquadType, m_BiquadValue);
        
        m_pVoice->SetMainSend(m_MainSend);
        for (int i = 0; i < AUX_BUS_NUM; i++)
        {
            m_pVoice->SetFxSend(static_cast<AuxBus>(i), m_FxSend[i]);
        }
    }
}

void Channel::Start(const WaveInfo& waveInfo, int length, u32 startOffset)
{
    m_Length = length;
    m_Lfo.Reset();
    m_CurveAdshr.Reset();
    m_SweepCounter = 0;

    m_pVoice->Initialize(waveInfo, startOffset);

    AppendWaveBuffer(waveInfo);

    m_pVoice->SetPanMode(m_PanMode);
    m_pVoice->SetPanCurve(m_PanCurve);
    m_pVoice->SetInterpolationType(m_InterpolationType);
    
    m_pVoice->Start();
    m_ActiveFlag = true;
}

void Channel::AppendWaveBuffer(const WaveInfo& waveInfo)
{
    m_LoopFlag = waveInfo.loopFlag;
    m_LoopStartFrame = waveInfo.loopStartFrame;
    
    u32 loopStartByte = Voice::FrameToByte( m_LoopStartFrame, waveInfo.sampleFormat );
    u32 loopEndByte = Voice::FrameToByte( waveInfo.loopEndFrame, waveInfo.sampleFormat );

    const int channelCount = m_pVoice->GetChannelCount();
    for (int ch = 0; ch < channelCount; ch++)
    {
        const void* dataAddress = waveInfo.channelParam[ch].dataAddress;

        nn::snd::CTR::AdpcmContext& adpcmContext = m_AdpcmContext[ch];
        nn::snd::CTR::AdpcmContext& adpcmLoopContext = m_AdpcmLoopContext[ch];

        if (waveInfo.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM)
        {
            const DspAdpcmParam* pParam = &waveInfo.channelParam[ch].adpcmParam;

            adpcmContext.pred_scale = pParam->predScale;
            adpcmContext.yn1 = pParam->yn1;
            adpcmContext.yn2 = pParam->yn2;

            if (waveInfo.loopFlag)
            {
                const DspAdpcmLoopParam* pLoopParam = &waveInfo.channelParam[ch].adpcmLoopParam;
                adpcmLoopContext.pred_scale = pLoopParam->loopPredScale;
                adpcmLoopContext.yn1 = pLoopParam->loopYn1;
                adpcmLoopContext.yn2 = pLoopParam->loopYn2;
            }

            nn::snd::CTR::AdpcmParam param;
            for (int i = 0; i < 16; i++)
            {
                param.coef[i] = pParam->coef[i];
            }
            m_pVoice->SetAdpcmParam(ch, param);
        }

        {
            nn::snd::CTR::WaveBuffer* pBuffer0 = &m_WaveBuffer[ch][0];
            nn::snd::CTR::WaveBuffer* pBuffer1 = &m_WaveBuffer[ch][1];

            nn::snd::CTR::InitializeWaveBuffer(pBuffer0);
            pBuffer0->bufferAddress = dataAddress;
            pBuffer0->sampleLength = waveInfo.loopEndFrame;
            pBuffer0->loopFlag = false;
            if (waveInfo.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM)
            {
                pBuffer0->pAdpcmContext = &adpcmContext;
            }
            m_pVoice->AppendWaveBuffer(ch, pBuffer0, !waveInfo.loopFlag);
            
            if (waveInfo.loopFlag)
            {
                nn::snd::CTR::InitializeWaveBuffer(pBuffer1);
                pBuffer1->bufferAddress = ut::AddOffsetToPtr(dataAddress, loopStartByte);
                pBuffer1->sampleLength = waveInfo.loopEndFrame - m_LoopStartFrame;
                pBuffer1->loopFlag = true;
                if (waveInfo.sampleFormat == SAMPLE_FORMAT_DSP_ADPCM)
                {
                    pBuffer1->pAdpcmContext = &adpcmLoopContext;
                }
                m_pVoice->AppendWaveBuffer( ch, pBuffer1, true );
            }
        }
    }
}

void Channel::Release()
{
    if (m_CurveAdshr.GetStatus() != CurveAdshr::STATUS_RELEASE)
    {
        if (m_pVoice != NULL)
        {
            if (!m_ReleasePriorityFixFlag)
            {
                m_pVoice->SetPriority(Channel::PRIORITY_RELEASE);
            }
        }
        m_CurveAdshr.SetStatus(CurveAdshr::STATUS_RELEASE);
    }
    m_PauseFlag = false;
}

void Channel::NoteOff()
{
    if (m_IsIgnoreNoteOff) 
        return;

    Release();
}

void Channel::Stop()
{
    if (m_pVoice == NULL) 
        return;
    
    m_pVoice->Stop();
    m_pVoice->Free();
    m_pVoice = NULL;

    m_PauseFlag = false;
    m_ActiveFlag = false;
    
    if (m_Callback != NULL)
    {
        m_Callback(this, CALLBACK_STATUS_STOPPED, m_CallbackData);
    }

    if (m_AllocFlag)
    {
        m_AllocFlag = false;
        ChannelManager::GetInstance().Free(this);
    }
}

void Channel::UpdateSweep(int count)
{
    m_SweepCounter += count;
    if (m_SweepCounter > m_SweepLength)
    {
        m_SweepCounter = m_SweepLength;
    }
}

void Channel::SetSweepParam(f32 sweepPitch, int sweepTime, bool autoUpdate)
{
    m_SweepPitch = sweepPitch;
    m_SweepLength = sweepTime;
    m_AutoSweep = autoUpdate;
    m_SweepCounter = 0;
}

f32 Channel::GetSweepValue() const
{
    if (m_SweepPitch == 0.0f) 
        return 0.0f;
    if (m_SweepCounter >= m_SweepLength) 
        return 0.0f;

    f32 sweep = m_SweepPitch;
    sweep *= m_SweepLength - m_SweepCounter;

    NW_ASSERT(m_SweepLength != 0);
    sweep /= m_SweepLength;

    return sweep;
}

void Channel::SetBiquadFilter(int type, f32 value)
{
    m_BiquadType = static_cast<u8>(type);
    m_BiquadValue = value;
}

u32 Channel::GetCurrentPlayingSample() const
{
    if (!m_ActiveFlag)
        return 0;
    
    NW_NULL_ASSERT(m_pVoice);
    
    if (m_LoopFlag && m_WaveBuffer[0][0].status == nn::snd::CTR::WaveBuffer::STATUS_DONE)
    {
        return m_LoopStartFrame + m_pVoice->GetCurrentPlayingSample();
    }
    else
    {
        return m_pVoice->GetCurrentPlayingSample();
    }
}

void Channel::VoiceCallbackFunc(Voice* voice, 
    Voice::VoiceCallbackStatus status, void* arg)
{
    NW_NULL_ASSERT(arg);

    ChannelCallbackStatus chStatus = CALLBACK_STATUS_FINISH;
    switch (status)
    {
    case Voice::CALLBACK_STATUS_FINISH_WAVE:
        chStatus = CALLBACK_STATUS_FINISH;
        voice->Free();
        break;
    case Voice::CALLBACK_STATUS_CANCEL:
        chStatus = CALLBACK_STATUS_CANCEL;
        voice->Free();
        break;
    case Voice::CALLBACK_STATUS_DROP_VOICE:
        chStatus = CALLBACK_STATUS_DROP;
        break;
    case Voice::CALLBACK_STATUS_DROP_DSP:
        chStatus = CALLBACK_STATUS_DROP;
        break;
    }

    Channel*channel = static_cast<Channel*>(arg);

    if (channel->m_Callback != NULL) 
    {
        channel->m_Callback(channel, chStatus, channel->m_CallbackData);
    }

    channel->m_pVoice = NULL;
    channel->m_PauseFlag = false;
    channel->m_ActiveFlag = false;

    channel->m_AllocFlag = false;
    ChannelManager::GetInstance().Free( channel );
}

Channel* Channel::AllocChannel(int voiceChannelCount, int priority, 
    Channel::ChannelCallback callback, u32 callbackData)
{
    NW_MINMAX_ASSERT(priority, 0, 255);

    Channel* channel = ChannelManager::GetInstance().Alloc();
    if (channel == NULL)
    {
        NW_WARNING(channel, "Channel Allocation failed!");
        return NULL;
    }
    channel->m_AllocFlag = true;

    Voice* voice = VoiceManager::GetInstance().AllocVoice(voiceChannelCount, priority,
        VoiceCallbackFunc, channel);

    if (voice == NULL)
    {
        ChannelManager::GetInstance().Free(channel);
        return NULL;
    }

    channel->m_pVoice = voice;
    channel->InitParam(callback, callbackData);

    return channel;
}

void Channel::FreeChannel(Channel* channel)
{
    if (channel == NULL) 
        return;

    channel->m_Callback = NULL;
    channel->m_CallbackData = 0;
}

void Channel::Disposer::InvalidateData(const void* start, const void* end)
{
    if (m_pChannel->m_pVoice == NULL)
        return;

    bool disposeFlag = false;
    int channelCount = m_pChannel->m_pVoice->GetChannelCount();

    for (int channelIndex = 0; channelIndex < channelCount; channelIndex++)
    {
        for (int waveBufferIndex = 0; waveBufferIndex < WAVE_BUFFER_MAX; waveBufferIndex++)
        {
            const nn::snd::CTR::WaveBuffer& waveBuffer =
                m_pChannel->m_WaveBuffer[channelIndex][waveBufferIndex];
            if (waveBuffer.status == nn::snd::CTR::WaveBuffer::STATUS_FREE ||
                waveBuffer.status == nn::snd::CTR::WaveBuffer::STATUS_DONE)
            {
                continue;
            }
            
            const void* bufferEnd = ut::AddOffsetToPtr(
                    waveBuffer.bufferAddress,
                    Util::GetByteBySample(waveBuffer.sampleLength,
                        m_pChannel->m_pVoice->GetFormat()));
            
            if (start <= bufferEnd && end >= waveBuffer.bufferAddress)
            {
                disposeFlag = true;
                break;
            }
        }
    }
    
    if (disposeFlag)
    {
        if (m_pChannel->m_Callback != NULL)
        {
            m_pChannel->m_Callback(m_pChannel, CALLBACK_STATUS_CANCEL, m_pChannel->m_CallbackData);
            m_pChannel->m_Callback = NULL;
        }
        
        m_pChannel->Stop();
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw