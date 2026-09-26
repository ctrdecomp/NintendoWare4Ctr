#ifndef NW_SND_HARDWARE_MANAGER_H_
#define NW_SND_HARDWARE_MANAGER_H_

#include <nw/ut/ut_LinkList.h>
#include <nw/snd/snd_FxBase.h>
#include <nw/snd/snd_Global.h>
#include <nw/snd/snd_MoveValue.h>
#include <nw/snd/snd_Util.h>
#include <nw/snd/snd_HardwareManager.h>
#include <nw/snd/snd_BiquadFilterCallback.h>
#include <nw/snd/snd_BiquadFilterPresets.h>

namespace nw { 
namespace snd { 
namespace internal { 
namespace driver {

class HardwareManager : public Util::Singleton<HardwareManager> // achieves the Util::Singleton symbol
{
public:
    typedef void (*HardwareCallback)();
public:
    HardwareManager();
    
    typedef ut::LinkList< FxBase, offsetof(FxBase, m_Link) > FxList;
    
    static const SampleFormat FX_SAMPLE_FORMAT = SAMPLE_FORMAT_PCM_S32;
    static const int FX_SAMPLE_RATE = 32728;
    static const s32 SOUND_BIQAUDS_COUNT = 128;
    static const u32 SOUND_FRAME_INTERVAL_MSEC = 5;
    static const u32 SOUND_VOICE_COUNT = NN_SND_VOICE_NUM;

    /* Initialization */
    void Initialize();
    void Finalize();
    void Update();

    /* Effects */
    bool AppendEffect(AuxBus bus, FxBase* pFx);
    bool AppendEffect(AuxBus bus, nn::snd::CTR::FxDelay* pFx);
    bool AppendEffect(AuxBus bus, nn::snd::CTR::FxReverb* pFx);
    void ClearEffect(AuxBus bus, int fadeTimes);
    bool IsFinishedClearEffect(AuxBus bus) const { return m_AuxFadeVolume[bus].IsFinished(); }
    void FinalizeEffect(AuxBus bus);

    /* Set Modes */
    void SetOutputMode(OutputMode mode);
    void SetMasterVolume(float volume, int fadeTimes);
    void SetBiquadFilterCallback(int type, const BiquadFilterCallback* cb);
    static void AuxCallbackFunc(AuxBusData *data,s32 sampleLength,uptr userData);

    /* Inlines for sub-classes */
    bool IsInitialized() { return m_IsInitialized != 0; }
    OutputMode GetOutputMode() const { return m_OutputMode; }
    nn::os::Tick GetEffectProcessTick(AuxBus bus) const { return m_EffectProcessTick[bus]; }
    f32 GetMasterVolume() const { return m_MasterVolume.GetValue(); }
    SrcType GetSrcType() const { return m_SrcType; }
    const BiquadFilterCallback* GetBiquadFilterCallback(int type) { return m_BiquadFilterCallbackTable[type]; }

    f32 GetOutputVolume() const;

    static const BiquadFilterLpf        BIQUAD_FILTER_LPF;
    static const BiquadFilterHpf        BIQUAD_FILTER_HPF;
    static const BiquadFilterBpf512     BIQUAD_FILTER_BPF_512;
    static const BiquadFilterBpf1024    BIQUAD_FILTER_BPF_1024;
    static const BiquadFilterBpf2048    BIQUAD_FILTER_BPF_2048;
private:
    bool m_IsInitialized;

    OutputMode m_OutputMode;
    SrcType m_SrcType;

    MoveValue<f32, int> m_MasterVolume;
    MoveValue<f32, int> m_VolumeForReset;
    MoveValue<f32, int> m_AuxFadeVolume[AUX_BUS_NUM];
    MoveValue<f32, int> m_AuxUserVolume[AUX_BUS_NUM];
    FxList m_FxList[AUX_BUS_NUM];
    AuxCallback m_AuxCallback[AUX_BUS_NUM];

    const BiquadFilterCallback* m_BiquadFilterCallbackTable[BIQUAD_FILTER_TYPE_USER_MAX+1];
    uptr m_AuxCallbackContext[AUX_BUS_NUM];
    nn::os::Tick m_EffectProcessTick[AUX_BUS_NUM];
    u8 m_AuxCallbackWaitCounter[AUX_BUS_NUM];
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_HARDWARE_MANAGER_H_
