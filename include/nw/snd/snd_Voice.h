#ifndef NW_SND_VOICE_H_
#define NW_SND_VOICE_H_

#include <nn/snd.h>
#include <nn/snd/CTR/Common/snd_Adpcm.h>
#include <nw/snd/snd_Global.h>
#include <nw/ut/ut_LinkList.h>

namespace nw {
namespace snd {
namespace internal {
namespace driver {

typedef nn::snd::CTR::Voice HardwareChannel;

class Voice
{
    friend class VoiceManager;

public:
    static const int UPDATE_START           = 0x0001 << 0;
    static const int UPDATE_PAUSE           = 0x0001 << 1;
    static const int UPDATE_SRC             = 0x0001 << 2;
    static const int UPDATE_MIX             = 0x0001 << 3;
    static const int UPDATE_LPF             = 0x0001 << 4;
    static const int UPDATE_BIQUAD          = 0x0001 << 5;
    static const int UPDATE_VE              = 0x0001 << 6;

    enum VoiceCallbackStatus
    {
        CALLBACK_STATUS_FINISH_WAVE,
        CALLBACK_STATUS_CANCEL,
        CALLBACK_STATUS_DROP_VOICE,
        CALLBACK_STATUS_DROP_DSP
    };

    typedef void (*VoiceCallback)(Voice* voice, VoiceCallbackStatus status, void* callbackData);

    static const int CHANNEL_MAX = nw::snd::internal::WAVE_CHANNEL_MAX;

    static const f32 VOLUME_MIN;
    static const f32 VOLUME_DEFAULT;
    static const f32 VOLUME_MAX;
    static const f32 PAN_LEFT;
    static const f32 PAN_CENTER;
    static const f32 PAN_RIGHT;
    static const f32 SPAN_FRONT;
    static const f32 SPAN_CENTER;
    static const f32 SPAN_REAR;
    static const f32 CUTOFF_FREQ_MIN;
    static const f32 CUTOFF_FREQ_MAX;
    static const f32 BIQUAD_VALUE_MIN;
    static const f32 BIQUAD_VALUE_MAX;
    static const f32 SEND_MIN;
    static const f32 SEND_MAX;

private:
    static const int PRIORITY_MAX = nn::snd::CTR::VOICE_PRIORITY_NODROP;
    static const int PRIORITY_MIN = 0;
    static const u32 VOICE_PRIORITY_RELEASE = 1;

public:
    static const int PRIORITY_NODROP = PRIORITY_MAX;
    static const int PRIORITY_RELEASE = 1;

public:
    Voice();
    ~Voice();

    bool Alloc(int channelCount, int priority, Voice::VoiceCallback callback, void* callbackData);
    void Free();
    void Initialize( const WaveInfo& waveInfo, u32 startOffset );

    void Start();
    void Stop();
    void StopFinished();
    void Pause( bool flag );

    void Calc();
    void Update();

    bool IsActive() const   { return m_pHardwareChannel[0] != NULL; }
    bool IsRun()    const;
    bool IsPause()  const   { return m_IsPause == true; }
    bool IsPlayFinished() const;

    f32 GetVolume() const { return m_Volume; }
    void SetVolume(f32 volume);

    f32 GetPitch() const { return m_Pitch; }
    void SetPitch(f32 pitch);

    void SetPanMode(PanMode panMode);
    void SetPanCurve(PanCurve panCurve);

    f32 GetPan() const { return m_Pan; }
    void SetPan(f32 pan);

    f32 GetSurroundPan() const { return m_SurroundPan; }
    void SetSurroundPan(f32 span);

    f32 GetLpfFreq() const { return m_LpfFreq; }
    void SetLpfFreq(f32 lpfFreq);

    int GetBiquadType() const { return m_BiquadType; }
    f32 GetBiquadValue() const { return m_BiquadValue; }
    void SetBiquadFilter(int type, f32 value);

    int GetPriority() const { return m_Priority; }
    void SetPriority(int priority);

    void SetFrontBypass(bool isFrontBypass);
    void SetInterpolationType(u8 interpolationType);

    int GetPhysicalVoiceCount() const { return m_ChannelCount; }
    int GetChannelCount() const { return m_ChannelCount; }
    const HardwareChannel* GetHardwareChannel( int channelIndex ) const;

    void UpdateVoicesPriority();

    f32 GetMainSend() const { return m_MainSend; }
    void SetMainSend(f32 send);

    f32 GetFxSend(AuxBus bus) const { return m_FxSend[bus]; }
    void SetFxSend(AuxBus bus, f32 send);

    u32 GetCurrentPlayingSample() const;
    nw::snd::SampleFormat GetFormat() const;
private:
    static void SdkVoiceDropCallbackFunc(nn::snd::CTR::Voice* pDropVoice,uptr userArg);
    static void SdkVoiceDropCallbackFuncMulti(nn::snd::CTR::Voice* pDropVoice,uptr userArg);

    void InitParam(int channelCount, Voice::VoiceCallback callback,void* callbackData);

    void CalcSrc(bool initialUpdate);
    void CalcVe();
    void CalcMix();
    void CalcLpf();
    void CalcBiquadFilter();

    void CalcMixParam(int channelIndex,nw::snd::internal::MixParam* mix);

    void RunAllHardwareChannel();
    void StopAllHardwareChannel();
    void PauseAllHardwareChannel();

private:
    HardwareChannel*          m_pHardwareChannel[CHANNEL_MAX];
    int                       m_ChannelCount;
    VoiceCallback             m_Callback;
    void*                     m_pCallbackData;
    bool                      m_IsActive;
    bool                      m_IsStart;
    bool                      m_IsStarted;
    bool                      m_IsPause;
    bool                      m_IsPausing;
    bool                      m_IsInitialized;
    bool                      m_IsAllocating;
    bool                      m_AllocateErrorFlag;
    nn::snd::CTR::WaveBuffer* m_pLastWaveBuffer;
    u16                       m_SyncFlag;
    u8                        m_BiquadType;
    f32                       m_Volume;
    f32                       m_Pitch;
    PanMode                   m_PanMode;
    PanCurve                  m_PanCurve;
    f32                       m_Pan;
    f32                       m_SurroundPan;
    f32                       m_LpfFreq;
    f32                       m_BiquadValue;
    int                       m_Priority;
    f32                       m_MainSend;
    f32                       m_FxSend[AUX_BUS_NUM];

    nw::snd::SampleFormat m_Format;
    uptr m_VoiceUser;

public:
    static u32 FrameToByte(u32 sample, nw::snd::SampleFormat format);

    void AppendWaveBuffer(int channelIndex,nn::snd::CTR::WaveBuffer* pBuffer,bool lastFlag);
    void SetAdpcmParam(int channelIndex,const nn::snd::CTR::AdpcmParam& param);

public:
    ut::LinkListNode    m_LinkNode;
};

} // namespace driver
} // namespace internal
} // namespace snd
} // namespace nw

#endif // NW_SND_VOICE_H_