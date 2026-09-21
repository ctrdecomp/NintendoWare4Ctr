// Filename: snd_WaveSoundPlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_WaveSoundPlayer.h>
#include <nw/snd/snd_DisposeCallbackManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

WaveSoundPlayer::WaveSoundPlayer()
{
}

void WaveSoundPlayer::Initialize()
{
    BasicSoundPlayer::Initialize();

    m_ReleasePriorityFixFlag = false;
    m_PanRange = 1.0f;
    m_Priority = DEFAULT_PRIORITY;
    m_pCallback = NULL;
    m_WaveSoundIndex = WAVE_TYPE_INVALID;
    m_pWaveSoundData = NULL;
    m_WaveSoundInfo.pitch = 1.0f;
    m_WaveSoundInfo.pan = 40;
    m_WaveSoundInfo.surroundPan = PAN_CURVE_SQRT;

    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        m_WaveSoundInfo.fxSend[i] = 0;
    }

    m_WaveSoundInfo.mainSend = 0x7F;
    m_LfoParam.Initialize();
    m_WavePlayFlag = false;
    m_pChannel = NULL;
}

void WaveSoundPlayer::Finalize()
{
    if (m_ActiveFlag)
    {
        DisposeCallbackManager::GetInstance().UnregisterDisposeCallback(this);
        m_ActiveFlag = false;
    }

    CloseChannel();

    BasicSoundPlayer::Finalize();
}

void WaveSoundPlayer::Start()
{
    SoundThread::GetInstance().RegisterPlayerCallback(this);
    m_StartedFlag = true;
}

void WaveSoundPlayer::Stop()
{
    FinishPlayer();
}

void WaveSoundPlayer::Pause(bool flag)
{
    m_PauseFlag = flag;
    if (IsChannelActive())
    {
        if (m_pChannel->IsPause() != flag)
        {
            m_pChannel->Pause(flag);
        }
    }
}

void WaveSoundPlayer::InvalidateData(const void* start, const void* end)
{
    if (m_ActiveFlag)
    {
        const void* current = GetWaveSoundDataAddress();
        if (start <= current && current <= end)
        {
            FinishPlayer();
        }
    }
}

bool WaveSoundPlayer::Prepare(const void* waveSoundBase, int index, StartOffsetType startOffsetType, int startOffset, const WaveSoundCallback* callback, u32 callbackData)
{

    if (m_ActiveFlag)
    {
        FinishPlayer();
    }

    m_pCallback = callback;
    m_CallbackData = callbackData;

    m_pWaveSoundData = waveSoundBase;
    m_WaveSoundIndex = index;
    m_StartOffsetType = startOffsetType;
    m_StartOffset = startOffset;

    DisposeCallbackManager::GetInstance().RegisterDisposeCallback(this);

    m_ActiveFlag = true;

    return true;
}

void WaveSoundPlayer::CloseChannel()
{
    if (IsChannelActive())
    {
        UpdateChannel();
        m_pChannel->Release();
    }

    if (m_pChannel != NULL)
    {
        Channel::FreeChannel(m_pChannel);
    }
    m_pChannel = NULL;
}

void WaveSoundPlayer::FinishPlayer()
{
    if (m_StartedFlag)
    {
        SoundThread::GetInstance().UnregisterPlayerCallback( this );
        m_StartedFlag = false;
    }
}

s32 WaveSoundPlayer::GetPlaySamplePosition() const
{
    if (m_pChannel == NULL)
    {
        return -1;
    }

    return static_cast<s32>(m_pChannel->GetCurrentPlayingSample());
}
} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw