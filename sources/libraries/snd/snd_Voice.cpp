// Filename: snd_Voice.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_Voice.h>

#include <cstring>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_Config.h>
#include <nw/snd/snd_VoiceManager.h>
#include <nw/snd/snd_HardwareManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {
namespace
{

/* User Made Headers to look better, used in Voice::CalcMixParam */

inline void PanCurveToPanInfo(Util::PanInfo& panInfo, PanCurve curve)
{
    switch (curve)
    {
    case PAN_CURVE_SQRT:
        panInfo.curve = Util::PAN_CURVE_SQRT;
        break;
    case PAN_CURVE_SQRT_0DB:
        panInfo.curve = Util::PAN_CURVE_SQRT;
        panInfo.centerZeroFlag = true;
        break;
    case PAN_CURVE_SQRT_0DB_CLAMP:
        panInfo.curve = Util::PAN_CURVE_SQRT;
        panInfo.centerZeroFlag = true;
        panInfo.zeroClampFlag = true;
        break;
    case PAN_CURVE_SINCOS:
        panInfo.curve = Util::PAN_CURVE_SINCOS;
        break;
    case PAN_CURVE_SINCOS_0DB:
        panInfo.curve = Util::PAN_CURVE_SINCOS;
        panInfo.centerZeroFlag = true;
        break;
    case PAN_CURVE_SINCOS_0DB_CLAMP:
        panInfo.curve = Util::PAN_CURVE_SINCOS;
        panInfo.centerZeroFlag = true;
        panInfo.zeroClampFlag = true;
        break;
    case PAN_CURVE_LINEAR:
        panInfo.curve = Util::PAN_CURVE_LINEAR;
        break;
    case PAN_CURVE_LINEAR_0DB:
        panInfo.curve = Util::PAN_CURVE_LINEAR;
        panInfo.centerZeroFlag = true;
        break;
    case PAN_CURVE_LINEAR_0DB_CLAMP:
        panInfo.curve = Util::PAN_CURVE_LINEAR;
        panInfo.centerZeroFlag = true;
        panInfo.zeroClampFlag = true;
        break;
    default:
        panInfo.curve = Util::PAN_CURVE_SQRT;
        break;
    }
}

inline void CalcPanForMono(f32& left, f32& right, const Util::PanInfo& panInfo)
{
    left = right = Util::CalcPanRatio(Voice::PAN_CENTER, panInfo);
}

inline void CalcBarancePanForStereo(
    f32& left, f32& right, const f32& pan, int channelIndex, const Util::PanInfo& panInfo )
{
    if (channelIndex == 0)
    {
        left = Util::CalcPanRatio(pan, panInfo);
        right = 0.0f;
    }
    else if (channelIndex == 1)
    {
        left = 0.0f;
        right = Util::CalcPanRatio(Voice::PAN_CENTER - pan, panInfo);
    }
}

inline void CalcDualPanForStereo(
    f32& left, f32& right, const f32& pan, const Util::PanInfo& panInfo )
{
    left = Util::CalcPanRatio(pan, panInfo);
    right = Util::CalcPanRatio(Voice::PAN_CENTER - pan, panInfo);
}

inline void CalcSurroundPanForMono(
    f32& front, f32& rear, const Util::PanInfo& panInfo )
{
    front = Util::CalcSurroundPanRatio(Voice::SPAN_FRONT, panInfo);
    rear = Util::CalcSurroundPanRatio(Voice::SPAN_REAR, panInfo);
}

inline void CalcSurroundPanForSurround(
    f32& front, f32& rear, const f32& span, const Util::PanInfo& panInfo )
{
    front = Util::CalcSurroundPanRatio(span, panInfo);
    rear = Util::CalcSurroundPanRatio(Voice::SPAN_REAR - span, panInfo);
}

} // namespace

const f32 Voice::VOLUME_MIN = 0.0f;
const f32 Voice::VOLUME_DEFAULT = 1.0f;
const f32 Voice::VOLUME_MAX = 2.0f;

const f32 Voice::PAN_LEFT = -1.0f;
const f32 Voice::PAN_CENTER = 0.0f;
const f32 Voice::PAN_RIGHT = 1.0f;

const f32 Voice::SPAN_FRONT = 0.0f;
const f32 Voice::SPAN_CENTER = 1.0f;
const f32 Voice::SPAN_REAR = 2.0f;

const f32 Voice::CUTOFF_FREQ_MIN = 0.0f;
const f32 Voice::CUTOFF_FREQ_MAX = 1.0f;

const f32 Voice::BIQUAD_VALUE_MIN = 0.0f;
const f32 Voice::BIQUAD_VALUE_MAX = 1.0f;

const f32 Voice::SEND_MIN = 0.0f;
const f32 Voice::SEND_MAX = 1.0f;

/*

Some can be copied from NW4R since NW4C and NW4R are somewhat simliar.

[REFERENCE] https://github.com/doldecomp/ogws/blob/master/src/nw4r/snd/snd_Voice.cpp

*/

Voice::Voice():
    m_Callback(NULL),
    m_IsActive(false),
    m_IsStart(false),
    m_IsStarted(false),
    m_IsPause(false),
    m_SyncFlag(0)
{
    for (int i = 0; i < CHANNEL_MAX; i++)
    {
        m_pHardwareChannel[i] = NULL;
    }
}

Voice::~Voice()
{
    for (int i = 0; i < CHANNEL_MAX; i++)
    {
        if (m_pHardwareChannel[i] != NULL)
        {
            nn::snd::CTR::FreeVoice(m_pHardwareChannel[i]);
        }
    }
}

bool Voice::Alloc(int channelCount, int priority, Voice::VoiceCallback callback, void* callbackData)
{
    NW_MINMAX_ASSERT(channelCount, 1, CHANNEL_MAX);
    channelCount = ut::Clamp(channelCount, 1, CHANNEL_MAX);
    m_ChannelCount = 0;

    NW_ASSERT(!m_IsActive);

    u32 sdkVoicePriority = ut::Clamp(priority, 0, nn::snd::CTR::VOICE_PRIORITY_NODROP);

    m_IsAllocating = true;
    m_AllocateErrorFlag = false;

    if (channelCount == 1)
    {
        HardwareChannel* pSdkVoice = NULL;

        pSdkVoice = nn::snd::CTR::AllocVoice(
            sdkVoicePriority,
            SdkVoiceDropCallbackFunc,
            reinterpret_cast<uptr>(this));

        if (pSdkVoice == NULL)
        {
            return false;
        }
        m_pHardwareChannel[0] = pSdkVoice;
        m_ChannelCount = 1;
    }
    else
    {
        for (int i = 0; i < channelCount; i++)
        {
            HardwareChannel* pSdkVoice = NULL;

            while (pSdkVoice == NULL)
            {
                pSdkVoice = nn::snd::CTR::AllocVoice(
                    sdkVoicePriority,
                    SdkVoiceDropCallbackFuncMulti,
                    reinterpret_cast<uptr>(this));

                if (pSdkVoice == NULL)
                {
                    int dropVoiceCount = VoiceManager::GetInstance().DropLowestPriorityVoice(sdkVoicePriority);
                    
                    if (dropVoiceCount == 0)
                    {
                        m_AllocateErrorFlag = true;
                        break;
                    }
                    continue;
                }
            }
            m_pHardwareChannel[i] = pSdkVoice;
            m_ChannelCount += 1;
        }
        if (m_AllocateErrorFlag)
        {
            for (int i = 0; i < m_ChannelCount; i++)
            {
                if (m_pHardwareChannel[i])
                {
                    nn::snd::CTR::FreeVoice(m_pHardwareChannel[i]);
                }
            }
            return false;
        }
    }
    
    m_IsAllocating = false;

    InitParam(channelCount, callback, callbackData);
    m_IsActive = true;

    return true;
}

//! @note This isnt confirmed. I have no ref for this function.
//
// Copied from NW4R
void Voice::InitParam(int channelCount, VoiceCallback pCallback,
    void* pCallbackArg) 
{
    m_ChannelCount = channelCount;
    m_Callback = pCallback;
    m_pCallbackData = pCallbackArg;

    m_SyncFlag = 0;
    m_IsPause = false;
    m_IsPausing = false;
    m_IsStarted = false;

    m_Volume = 1.0f;
    m_LpfFreq = 1.0f;
    m_Pan = 0.0f;
    m_SurroundPan = 0.0f;
    m_MainSend = 1.0f;

    for (int i = 0; i < AUX_BUS_NUM; i++) 
    {
        m_FxSend[i] = 0.0f;
    }

    m_Pitch = 1.0f;
    m_PanMode = PAN_MODE_DUAL;
    m_PanCurve = PAN_CURVE_SQRT;
}

void Voice::StopFinished() 
{
    if (m_IsActive && m_IsStarted && IsPlayFinished()) 
    {
        if (m_Callback != NULL) 
        {
            m_Callback(this, CALLBACK_STATUS_FINISH_WAVE, m_pCallbackData);
        }

        m_IsStarted = false;
        m_IsStart = false;
    }
}

void Voice::Calc() 
{
    if (!m_IsStart) 
    {
        return;
    }

    if (m_SyncFlag & UPDATE_SRC) 
    {
        CalcSrc(false);
        m_SyncFlag &= ~UPDATE_SRC;
    }

    if (m_SyncFlag & UPDATE_VE) 
    {
        CalcVe();
        m_SyncFlag &= ~UPDATE_VE;
    }

    if (m_SyncFlag & UPDATE_MIX) 
    {
        CalcMix();
        m_SyncFlag &= ~UPDATE_MIX;
    }

    if (m_SyncFlag & UPDATE_LPF) 
    {
        CalcLpf();
        m_SyncFlag &= ~UPDATE_LPF;
    }

    if (m_SyncFlag & UPDATE_BIQUAD) 
    {
        CalcBiquadFilter();
        m_SyncFlag &= ~UPDATE_BIQUAD;
    }
}

void Voice::Update() 
{
    if (!m_IsActive) 
    {
        return;
    }

    if ((m_SyncFlag & SYNC_MODE_LOOSE) && m_IsStart && !m_IsStarted) 
    {
        CalcSrc(true);
        RunAllHardwareChannel();
        CalcMix();
        CalcVe();

        m_IsStarted = true;
        m_SyncFlag &= ~SYNC_MODE_LOOSE;
        m_SyncFlag &= ~UPDATE_SRC;
    }

    if (m_IsStarted) 
    {
        if ((m_SyncFlag & UPDATE_VE) && m_IsStart) 
        {
            if (m_IsPause) 
            {
                StopAllHardwareChannel();
                m_IsPausing = true;
            } 
            else 
            {
                RunAllHardwareChannel();
                m_IsPausing = false;
            }

            m_SyncFlag &= ~UPDATE_VE;
        }

        PauseAllHardwareChannel();
    }
}

void Voice::Free()
{
    if (!m_IsActive) 
        return;

    for (int channelIndex = 0; channelIndex < m_ChannelCount; channelIndex++)
    {
        HardwareChannel* pChannel = m_pHardwareChannel[channelIndex];
        if (pChannel != NULL)
        {
            nn::snd::CTR::FreeVoice(pChannel);
            m_pHardwareChannel[channelIndex] = NULL;
        }
    }
    m_ChannelCount = 0;

    VoiceManager::GetInstance().FreeVoice(this);

    m_IsActive = false;
}

void Voice::Start() 
{
    m_IsStart = true;
    m_IsPause = false;
    m_SyncFlag |= SYNC_MODE_LOOSE;
}


void Voice::Stop() 
{
    if (m_IsStarted) 
    {
        StopAllHardwareChannel();
        m_IsStarted = false;
    }

    m_IsPausing = false;
    m_IsPause = false;
    m_IsStart = false;
}

void Voice::Pause(bool flag) 
{
    if (m_IsPause == flag) 
    {
        return;
    }

    m_IsPause = flag;
    m_SyncFlag |= SYNC_MODE_NUM;
}

SampleFormat Voice::GetFormat() const 
{
    if (IsActive()) 
    {
        return m_Format;
    }

    return SAMPLE_FORMAT_PCM_S16;
}

void Voice::SetVolume(f32 volume) 
{
    if (volume < VOLUME_MIN)
    {
        volume = VOLUME_MIN;
    }

    if (volume != m_Volume) 
    {
        m_Volume = volume;
        m_SyncFlag |= UPDATE_VE;
    }
}

void Voice::SetPitch(f32 pitch) 
{
    if (pitch == m_Pitch) 
    {
        return;
    }

    m_Pitch = pitch;
    m_SyncFlag |= UPDATE_SRC;
}

void Voice::SetPanMode(PanMode mode) 
{
    if (mode == m_PanMode) 
    {
        return;
    }

    m_PanMode = mode;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetPanCurve(PanCurve curve) 
{
    if (curve == m_PanCurve) 
    {
        return;
    }

    m_PanCurve = curve;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetPan(f32 pan) 
{
    if (pan == m_Pan) 
    {
        return;
    }

    m_Pan = pan;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetSurroundPan(f32 pan) 
{
    if (pan == m_SurroundPan) 
    {
        return;
    }

    m_SurroundPan = pan;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetLpfFreq(f32 freq) 
{
    if (freq == m_LpfFreq) 
    {
        return;
    }

    m_LpfFreq = freq;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetMainSend(f32 send) 
{
    send += 1.0f;
    send = ut::Clamp(send, 0.0f, 1.0f);

    if (send == m_MainSend) 
    {
        return;
    }

    m_MainSend = send;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetFxSend(AuxBus bus, f32 send) 
{
    if(send < 0.0f)
    {
        send = 0.0f;
    }

    if (send == m_FxSend[bus]) 
    {
        return;
    }

    m_FxSend[bus] = send;
    m_SyncFlag |= UPDATE_MIX;
}

void Voice::SetPriority(int priority) 
{
    m_Priority = priority;
    VoiceManager::GetInstance().ChangeVoicePriority(this);

    if (m_Priority != PRIORITY_RELEASE) 
    {
        return;
    }

    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if(m_pHardwareChannel[i] != NULL)
        {
            m_pHardwareChannel[i]->SetPriority(PRIORITY_RELEASE);
        }
    }
}

void Voice::UpdateVoicesPriority() 
{
    if (m_Priority == PRIORITY_RELEASE) 
    {
        return;
    }

    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if(m_pHardwareChannel[i] != NULL)
        {
            m_pHardwareChannel[i]->SetPriority(m_Priority);
        }
    }
}

u32 Voice::GetCurrentPlayingSample() const 
{
    if (IsActive()) 
    {
        return m_pHardwareChannel[0]->GetPlayPosition();
    }

    return 0;
}

void Voice::CalcSrc(bool initial) 
{
    // legit does nothing lmao leftover from NW4R
    NW_UNUSED_VARIABLE(initial);
    for(int channel = 0; channel < m_ChannelCount; channel++)
    {
        if(m_pHardwareChannel[channel] != NULL)
        {
            m_pHardwareChannel[channel]->SetPitch(m_Priority);
        }
    }
}

void Voice::CalcVe() 
{
    f32 baseVolume = 1.0f;
    baseVolume *= m_Volume;
    baseVolume *= HardwareManager::GetInstance().GetOutputVolume();

    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if (m_pHardwareChannel[i] != NULL) 
        {
            m_pHardwareChannel[i]->SetVolume(baseVolume);
        }
    }
}

void Voice::CalcMix() 
{
    nn::snd::CTR::MixParam param;

    bool nextUpdate = false;

    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if (m_pHardwareChannel[i] != NULL) 
        {
            CalcMixParam(i, &param);
            m_pHardwareChannel[i]->SetMixParam(param);
        }
    }
}

void Voice::CalcLpf() 
{
    int cutoff = Util::CalcLpfFreq(m_LpfFreq);

    for (int i = 0; i < m_ChannelCount; i++)
    {
        HardwareChannel* ch = m_pHardwareChannel[i];
        if (ch != NULL) 
        {
            if(cutoff < cutoff)
            {
                ch->EnableMonoFilter(true);
                ch->SetMonoFilterCoefficients(cutoff);
            }
            else
            {
                ch->EnableMonoFilter(false);
            }
        }
    }
}

void Voice::CalcMixParam(int channelIndex, nw::snd::internal::MixParam* mix)
{
    NW_NULL_ASSERT(mix);

    f32 main = ut::Clamp(m_MainSend, 0.0f, 1.0f);
    f32 fx[AUX_BUS_NUM];
    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        fx[i] = ut::Clamp(m_FxSend[i], 0.0f, 1.0f);
    }

    register f32 left, right, front, rear;
    left = right = front = rear = 0.0f;

    Util::PanInfo panInfo;
    PanCurveToPanInfo(panInfo, m_PanCurve);

    const OutputMode mode = HardwareManager::GetInstance().GetOutputMode();

    switch (mode)
    {
    case OUTPUT_MODE_MONO:
    {
        CalcPanForMono(left, right, panInfo);
        break;
    }
    case OUTPUT_MODE_STEREO:
    case OUTPUT_MODE_SURROUND:
    {
        if ((m_ChannelCount > 1) && (m_PanMode == PAN_MODE_BALANCE))
        {
            CalcBarancePanForStereo(left, right, m_Pan, channelIndex, panInfo);
        }
        else
        {
            register f32 voicePan = m_Pan;
            if (m_ChannelCount == 2)
            {
                if (channelIndex == 0) 
                { 
                    voicePan -= 1.0f; 
                }
                if (channelIndex == 1) 
                { 
                    voicePan += 1.0f; 
                }
            }
            CalcDualPanForStereo(left, right, voicePan, panInfo);
        }
        break;
    }
    }

    switch (mode)
    {
    case OUTPUT_MODE_MONO:
    case OUTPUT_MODE_STEREO:
    {
        CalcSurroundPanForMono(front, rear, panInfo);
        break;
    }
    case OUTPUT_MODE_SURROUND:
    {
        CalcSurroundPanForSurround(front, rear, m_SurroundPan, panInfo);
        break;
    }
    }

    register f32 tmpVol[nn::snd::CTR::CHANNEL_INDEX_NUM];
    tmpVol[nn::snd::CTR::CHANNEL_INDEX_FRONT_LEFT]  = front * left;
    tmpVol[nn::snd::CTR::CHANNEL_INDEX_FRONT_RIGHT] = front * right;
    tmpVol[nn::snd::CTR::CHANNEL_INDEX_REAR_LEFT]   = rear * left;
    tmpVol[nn::snd::CTR::CHANNEL_INDEX_REAR_RIGHT]  = rear * right;

    for (int i = 0; i < nn::snd::CTR::CHANNEL_INDEX_NUM; i++)
    {
        mix->mainBus[i] = main * tmpVol[i];
        mix->auxBusA[i] = fx[0] * tmpVol[i];
        mix->auxBusB[i] = fx[1] * tmpVol[i];
    }
}

void Voice::RunAllHardwareChannel() 
{
    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if (m_pHardwareChannel[i] != NULL) 
        {
            m_pHardwareChannel[i]->SetState(nn::snd::CTR::Voice::STATE_PLAY);
        }
    }
}

void Voice::StopAllHardwareChannel() 
{
    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if (m_pHardwareChannel[i] != NULL) 
        {
            m_pHardwareChannel[i]->SetState(nn::snd::CTR::Voice::STATE_STOP);
        }
    }
}

void Voice::PauseAllHardwareChannel() 
{
    for (int i = 0; i < m_ChannelCount; i++) 
    {
        if (m_pHardwareChannel[i] != NULL) 
        {
            m_pHardwareChannel[i]->SetState(nn::snd::CTR::Voice::STATE_PAUSE);
        }
    }
}

void Voice::SdkVoiceDropCallbackFunc(nn::snd::CTR::Voice* pDropSdkVoice, uptr userArg)
{
    NW_NULL_ASSERT(pDropSdkVoice);
    Voice* arg = reinterpret_cast<Voice*>(userArg);
    arg->m_pHardwareChannel[0] = NULL;
    arg->m_IsPause = false;
    arg->m_IsStart = false;
    arg->m_ChannelCount = 0;

    VoiceManager::GetInstance().FreeVoice(arg);
    if(arg->m_Callback != NULL)
    {
        arg->m_Callback(arg, CALLBACK_STATUS_DROP_DSP, arg->m_pCallbackData);
    }
}

void Voice::SdkVoiceDropCallbackFuncMulti(nn::snd::CTR::Voice* pDropSdkVoice, uptr userArg)
{
    NW_NULL_ASSERT(pDropSdkVoice);
    Voice* arg = reinterpret_cast<Voice*>(userArg);

    if(arg->m_IsAllocating != false)
    {
        arg->m_AllocateErrorFlag = true;

        for(int i = 0; i < arg->m_ChannelCount; i++)
        {
            if(arg->m_pHardwareChannel[i] == pDropSdkVoice)
            {
                arg->m_pHardwareChannel[i] = NULL;
                return;
            }
        }
    }
    else
    {
        for(int i = 0; i < arg->m_ChannelCount; i++)
        {
            HardwareChannel* ch = arg->m_pHardwareChannel[i];

            if(ch == NULL)
            {
                continue;
            }

            if(ch != pDropSdkVoice)
            {
                ch->SetState(nn::snd::CTR::Voice::STATE_STOP);
                nn::snd::CTR::FreeVoice(ch);
            }

            arg->m_pHardwareChannel[i] = NULL;
        }
    }

    arg->m_IsPause = false;
    arg->m_IsStart = false;
    arg->m_ChannelCount = 0;

    VoiceManager::GetInstance().FreeVoice(arg);
    
    arg->m_IsActive = false;
    if(arg->m_Callback != NULL)
    {
        arg->m_Callback(arg, CALLBACK_STATUS_DROP_DSP, arg->m_pCallbackData);
    }
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw