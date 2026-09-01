// Filename: snd_HardwareManager.cpp
//
// Project: NintendoWare4Ctr

#include <nw/snd/snd_HardwareManager.h>
#include <nw/snd/snd_DriverCommandManager.h>
#include <nw/snd/snd_VoiceManager.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

const BiquadFilterLpf     HardwareManager::BIQUAD_FILTER_LPF;
const BiquadFilterHpf     HardwareManager::BIQUAD_FILTER_HPF;
const BiquadFilterBpf512  HardwareManager::BIQUAD_FILTER_BPF_512;
const BiquadFilterBpf1024 HardwareManager::BIQUAD_FILTER_BPF_1024;
const BiquadFilterBpf2048 HardwareManager::BIQUAD_FILTER_BPF_2048;

HardwareManager::HardwareManager():
    m_IsInitialized(false), m_OutputMode(OUTPUT_MODE_STEREO), m_SrcType(SRC_TYPE_4TAP)
{
    m_MasterVolume.InitValue(0);
    m_VolumeForReset.InitValue(0);

    for(s32 i = 0; i < AUX_BUS_NUM; i++)
    {
        m_AuxFadeVolume[i].InitValue(1.0f);
        m_AuxUserVolume[i].InitValue(1.0f);
        m_AuxCallback[i] = NULL;
        m_AuxCallbackContext[i] = NULL;
        m_EffectProcessTick[i] = nn::os::Tick(0);
    }
}

void HardwareManager::Initialize()
{
    if(m_IsInitialized)
    {
        return;
    }

    for(s32 i = 0; i < AUX_BUS_NUM; i++)
    {
        nn::snd::CTR::GetAuxCallback(static_cast<nn::snd::CTR::AuxBusId>(i), &m_AuxCallback[i], &m_AuxCallbackContext[i]);
        nn::snd::CTR::RegisterAuxCallback(static_cast<nn::snd::CTR::AuxBusId>(i), NULL, 0);
    }

    for(s32 j = 0; j < SOUND_BIQAUDS_COUNT; j += AUX_BUS_NUM)
    {
        m_BiquadFilterCallbackTable[j] = NULL;
    }

    SetBiquadFilterCallback(BIQUAD_FILTER_TYPE_LPF, &BIQUAD_FILTER_LPF);
    SetBiquadFilterCallback(BIQUAD_FILTER_TYPE_HPF, &BIQUAD_FILTER_HPF);
    SetBiquadFilterCallback(BIQUAD_FILTER_TYPE_BPF512, &BIQUAD_FILTER_BPF_512);
    SetBiquadFilterCallback(BIQUAD_FILTER_TYPE_BPF1024, &BIQUAD_FILTER_BPF_1024);
    SetBiquadFilterCallback(BIQUAD_FILTER_TYPE_BPF2048, &BIQUAD_FILTER_BPF_2048);

    nn::snd::CTR::SetMasterVolume(1.0f);
    nn::snd::CTR::OutputMode mode = nn::snd::CTR::GetSoundOutputMode();
    switch(mode)
    {
    case OUTPUT_MODE_MONO:
        m_OutputMode = OUTPUT_MODE_MONO;
        break;
    case OUTPUT_MODE_STEREO:
        m_OutputMode = OUTPUT_MODE_STEREO;
        break;
    case OUTPUT_MODE_3DSURROUND:
        m_OutputMode = OUTPUT_MODE_SURROUND;
        break;
    default:
        //NW_ASSERTMSG(false, "you fuckin fool, your mother = gay and output mode is invalid, thats the main error lol lmao lmao lmao lmao lmao.");
        break;
    }

    m_IsInitialized = false;
}

void HardwareManager::Finalize()
{
    if(m_IsInitialized == false)
    {
        return;
    }

    for(s32 i = 0; i < AUX_BUS_NUM; i++)
    {
        FinalizeEffect(static_cast<AuxBus>(i));
        nn::snd::CTR::GetAuxCallback(static_cast<nn::snd::CTR::AuxBusId>(i), &m_AuxCallback[i], &m_AuxCallbackContext[i]);
        m_AuxCallback[i] = NULL;
        m_AuxCallbackContext[i] = NULL;
    }
}

void HardwareManager::Update()
{
    for (s32 i=0; i < AUX_BUS_NUM; i++)
    {
        bool updateFlag = false;
        if (!m_AuxUserVolume[i].IsFinished())
        {
            m_AuxUserVolume[i].Update();
            updateFlag = true;
        }
        if (!m_AuxFadeVolume[i].IsFinished())
        {
            m_AuxFadeVolume[i].Update();
            if (m_AuxFadeVolume[i].IsFinished())
            {
                FinalizeEffect(static_cast<AuxBus>(i));
            }
            updateFlag = true;
        }

        if (updateFlag)
        {
            f32 returnVolume = 1.0f;
            returnVolume *= ut::Clamp(m_AuxUserVolume[i].GetValue(), 0.0f, 1.0f);
            returnVolume *= ut::Clamp(m_AuxFadeVolume[i].GetValue(), 0.0f, 1.0f);
            nn::snd::CTR::SetAuxReturnVolume(static_cast<nn::snd::CTR::AuxBusId>(i), returnVolume);
        }
    }

    if (!m_MasterVolume.IsFinished())
    {
        m_MasterVolume.Update();
        VoiceManager::GetInstance().UpdateAllVoicesSync(Voice::UPDATE_VE);
    }

    if (!m_VolumeForReset.IsFinished())
    {
        m_VolumeForReset.Update();
        VoiceManager::GetInstance().UpdateAllVoicesSync(Voice::UPDATE_VE);
    }

    {
        f32 volume = 1.0f;
        volume *= m_VolumeForReset.GetValue();
        volume = ut::Clamp(volume, 0.0f, 1.0f);
        nn::snd::CTR::SetMasterVolume(volume);
    }
}

void HardwareManager::SetMasterVolume(float volume, int fadeTimes)
{
    m_MasterVolume.SetTarget(0.0f, (fadeTimes + SOUND_FRAME_INTERVAL_MSEC - 1) / SOUND_FRAME_INTERVAL_MSEC);
    if(fadeTimes != 0)
    {
        return;
    }
{
    DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
    DriverCommandAllVoicesSync* command = cmdmgr.AllocCommand<DriverCommandAllVoicesSync>();
    command->id = DRIVER_COMMAND_SET_MASTER_VOLUME;
    command->syncFlag = Voice::UPDATE_VE;
    cmdmgr.PushCommand(command);
}

}

void HardwareManager::SetBiquadFilterCallback(int type, const BiquadFilterCallback* callback)
{
    if(type != BIQUAD_FILTER_TYPE_INHERIT)
    {
        m_BiquadFilterCallbackTable[type] = callback;
    }
}

f32 HardwareManager::GetOutputVolume() const
{
    f32 volume = m_MasterVolume.GetValue();
    volume *= m_VolumeForReset.GetValue();
    return volume;
}

void HardwareManager::SetOutputMode(OutputMode mode)
{
    if (m_OutputMode == mode)
    { 
        return;
    }

    m_OutputMode = mode;

    switch (mode)
    {
    case OUTPUT_MODE_MONO:
        nn::snd::CTR::SetSoundOutputMode(nn::snd::CTR::OUTPUT_MODE_MONO);
        break;
    case OUTPUT_MODE_STEREO:
        nn::snd::CTR::SetSoundOutputMode(nn::snd::CTR::OUTPUT_MODE_STEREO);
        break;
    case OUTPUT_MODE_SURROUND:
        nn::snd::CTR::SetSoundOutputMode(nn::snd::CTR::OUTPUT_MODE_3DSURROUND);
        break;
    default:
        break;
    }

    {
        DriverCommandManager& cmdmgr = DriverCommandManager::GetInstance();
        DriverCommandAllVoicesSync* command = cmdmgr.AllocCommand<DriverCommandAllVoicesSync>();
        command->id = DRIVER_COMMAND_ALLVOICES_SYNC;
        command->syncFlag = Voice::UPDATE_MIX;
        cmdmgr.PushCommand(command);
    }

    for (int bus = 0; bus < AUX_BUS_NUM; bus++)
    {
        FxList& list = m_FxList[bus];

        NW_UT_LINKLIST_FOREACH_SAFE(itr, list, { itr->OnChangeOutputMode(); })
    }
}

bool HardwareManager::AppendEffect(AuxBus bus, FxBase* pFx)
{
    if(m_AuxFadeVolume[bus].IsFinished())
    {
        FinalizeEffect(bus);
    }

    m_AuxFadeVolume[bus].SetTarget(1.0f, 0);
    nn::snd::CTR::SetAuxReturnVolume(static_cast<AuxBusId>(bus), 1.0f);

    if(m_FxList[bus].size() == 0)
    {
        nn::snd::CTR::RegisterAuxCallback(static_cast<AuxBusId>(bus), AuxCallbackFunc, static_cast<uptr>(bus));
    }

    m_FxList[bus].PushBack(pFx);
    return true;
}

bool HardwareManager::AppendEffect(AuxBus bus, nn::snd::CTR::FxDelay* delay)
{
    if(m_AuxFadeVolume[bus].IsFinished())
    {
        FinalizeEffect(bus);
    }

    m_AuxFadeVolume[bus].SetTarget(1.0f, 0);
    nn::snd::CTR::SetAuxReturnVolume(static_cast<AuxBusId>(bus), 1.0f);
    return nn::snd::CTR::SetEffect(static_cast<AuxBusId>(bus), delay);
}

bool HardwareManager::AppendEffect(AuxBus bus, nn::snd::CTR::FxReverb* reverb)
{
    if(m_AuxFadeVolume[bus].IsFinished())
    {
        FinalizeEffect(bus);
    }

    m_AuxFadeVolume[bus].SetTarget(1.0f, 0);
    nn::snd::CTR::SetAuxReturnVolume(static_cast<AuxBusId>(bus), 1.0f);
    return nn::snd::CTR::SetEffect(static_cast<AuxBusId>(bus), reverb);
}

void HardwareManager::ClearEffect(AuxBus bus, int fadeTimes)
{
    if(fadeTimes == 0)
    {
        FinalizeEffect(bus);

        if(m_AuxFadeVolume[bus].IsFinished())
        {
            m_AuxFadeVolume[bus].SetTarget(0.0f, 0);
        }
    }
    else
    {
        m_AuxFadeVolume[bus].SetTarget(0.0f, (fadeTimes + SOUND_FRAME_INTERVAL_MSEC - 1) / SOUND_FRAME_INTERVAL_MSEC);
    }
}

void HardwareManager::FinalizeEffect(AuxBus bus)
{
    nn::snd::CTR::ClearEffect(static_cast<nn::snd::CTR::AuxBusId>(bus));
    FxList& list = m_FxList[bus];
    if (list.IsEmpty())
    {
        return;
    }

    nn::snd::CTR::ClearAuxCallback(static_cast<nn::snd::CTR::AuxBusId>(bus));
    m_EffectProcessTick[bus] = nn::os::Tick(0);

    NW_UT_LINKLIST_FOREACH_SAFE(itr, list, { itr->Finalize(); })
    list.Clear();
}

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw