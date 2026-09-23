// Filename: snd_BasicSoundPlayer.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_BasicSoundPlayer.h>
#include <nw/assert.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

void PlayerParamSet::Initialize()
{
    volume = 1.0f;
    pitch = 1.0f;
    pan = 0.0f;
    surroundPan = 0.0f;
    lpfFreq = 0.0f;

    isFrontBypass = false;
    biquadType = -1; /* TypeInvalid */
    biquadValue = 0.0f;
    
    mainSend = 0.0f;

    panMode = PAN_MODE_DUAL;
    panCurve = PAN_CURVE_SQRT;

    for (int i = 0; i < AUX_BUS_NUM; i++)
    {
        fxSend[i] = 0.0f;
    }
}

BasicSoundPlayer::BasicSoundPlayer():
    m_Event(true),
    m_ActiveFlag(false),
    m_StartedFlag(false),
    m_PauseFlag(false),
    m_FinishFlag(false)
{
    m_Event.Signal();
}

void BasicSoundPlayer::Initialize()
{
    m_Event.ClearSignal();
    
    m_ActiveFlag = false;
    m_StartedFlag = false;
    m_PauseFlag = false;
    m_FinishFlag = false;
    
    m_PlayerParamSet.Initialize();
    m_pPlayerHeapDataManager = NULL;
}

void BasicSoundPlayer::Finalize()
{
    m_Event.Signal();
}

void BasicSoundPlayer::SetFxSend(AuxBus bus, float send)
{
    NW_MINMAXLT_ASSERT(bus, AUX_BUS_A, AUX_BUS_A + AUX_BUS_NUM);
    m_PlayerParamSet.fxSend[bus] = send;
}

float BasicSoundPlayer::GetFxSend(AuxBus bus) const
{
    NW_MINMAXLT_ASSERT(bus, AUX_BUS_A, AUX_BUS_A + AUX_BUS_NUM);
    return m_PlayerParamSet.fxSend[bus];
}

void BasicSoundPlayer::SetBiquadFilter( int type, float value )
{
    NW_MINMAX_ASSERT(type, 0, BIQUAD_FILTER_TYPE_USER_MAX);
    NW_MINMAX_ASSERT(value, 0.0f, 1.0f);

    m_PlayerParamSet.biquadType = static_cast<u8>(type);
    m_PlayerParamSet.biquadValue = value;
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw